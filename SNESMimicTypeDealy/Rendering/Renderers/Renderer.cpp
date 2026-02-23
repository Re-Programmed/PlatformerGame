#include "Renderer.h"
#include <iostream>
#include "../../Utils/CollisionDetection.h"
#include <thread>
#include "../../Settings/SettingsGlobals.h"

#include <string>
#include <fstream>

#include "../../Resources/base64.h"

#if _DEBUG
#include "../../Debug/DebugLog.h"
using namespace DEBUG;
#endif

namespace GAME_NAME
{
	namespace Rendering
	{
		constexpr int chunkSize = 0x40;

		constexpr int levelSizeX = DEFAULT_LEVEL_SIZE_X;
		constexpr int levelSizeY = DEFAULT_LEVEL_SIZE_Y;
		unsigned int Renderer::spriteCount = 0, Renderer::bgCount, Renderer::imageCount = 1;

		int Renderer::lastFileOff = -1;

		Chunk Renderer::m_chunks[levelSizeX * levelSizeY];

		bool Renderer::UpdateObjects = true;

		std::mutex Renderer_activeGameObjectsLock;
		std::vector<GameObject*> Renderer::m_activeGameObjects[MICRO_RENDER_LAYER_COUNT];
		std::vector<GUI::IGUIElement*> Renderer::m_guiGameObjects[MICRO_GUI_LAYER_COUNT];

		std::vector<GLuint> Renderer::m_textureIDs;

		GameObject* Renderer::m_lastLoadedObject;

		RENDER_LAYER Renderer::CurrentlyRenderingLayer;

		/*
	 ---RENDER ORDER---
		 
		[BACKGROUND] {Static.}
		[RENDER LAYER 0] {Bound to the chunk spawned in.}
		[RENDER LAYER 1]	|
		[RENDER LAYER 2]	|
		[RENDER LAYER 3]	|
		[ACTIVE RENDER LAYER 0] {Rendered if visible by camera.}
		[ACTIVE RENDER LAYER 1]		|
		[ACTIVE RENDER LAYER 2]		|	(PLAYER)
		[ACTIVE RENDER LAYER 3]		|
		[PRIORITY OBJECTS (FRONT OBJECTS)] {Bound to the chunk spawned in.}
		[GUI LAYER 0] {Always rendered.}
		[GUI LAYER 1]	|
		[GUI LAYER 2]	|
		*/

		std::vector<GameObject*> Renderer::m_destroyQueue;
		std::vector<GameObject*> Renderer::m_destroyActiveQueue;

		std::vector<Renderer::InstantiateGameObject> Renderer::m_instantiations;

		GLuint Renderer::LoadSprite(const char* file)
		{
			spriteCount++;
			return loadImage(file);
		}

		GLuint Renderer::LoadSpriteFromData(unsigned char* data, const int& width, const int& height)
		{
			spriteCount++;
			GLuint val = loadImageData(data, width, height);
			return val;
		}

		GLuint Renderer::LoadBG(const char* file)
		{
			///DEBUGGING
#if _DEBUG
			if (spriteCount > 0) { std::cout << "LOAD ALL BACKGROUND CONTENT BEFORE LOADING SPRITES." << std::endl; return NULL; }
#endif
			bgCount++;
			return loadImage(file);
		}

		const Renderer::TextureData Renderer::GetTextureData(GLuint textureID, GLuint mipmapLevel)
		{
			glBindTexture(GL_TEXTURE_2D, textureID);

			GLint w = 0, h = 0;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, mipmapLevel, GL_TEXTURE_WIDTH, &w);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, mipmapLevel, GL_TEXTURE_HEIGHT, &h);

			unsigned char* imageData = static_cast<unsigned char*>(malloc(w * h * 4 * sizeof(unsigned char)));
			glGetTexImage(GL_TEXTURE_2D, mipmapLevel, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
			

			return { imageData, w, h };
		}

		void Renderer::ClearGUIObjects(uint16_t startIndex, uint8_t layer)
		{
			startIndex = (uint16_t)m_guiGameObjects[layer].size() - startIndex;
			size_t size = m_guiGameObjects[layer].size();
			do {
				size--;
				delete m_guiGameObjects[layer][size];
				m_guiGameObjects[layer].erase(m_guiGameObjects[layer].begin() + size);
			}
			while (size > startIndex);
		}

		Sprite* const Renderer::GetSprite(const int spriteTexture)
		{
			return new Sprite(GetSpriteIdFromTextureId(spriteTexture));
		}

		DynamicSprite* const Renderer::GetDynamicSprite(const int spriteTexture)
		{
			return new DynamicSprite(GetSpriteIdFromTextureId(spriteTexture));
		}

		Sprite* const Renderer::getBackground(const unsigned int bgTexture)
		{
			return new Sprite(bgTexture);
		}

		void Renderer::updateObjectQueues()
		{
			//Check if any obejects need to be created.
			for (InstantiateGameObject obj : m_instantiations)
			{
				if (obj.Active)
				{
					LoadActiveObject(obj.MyObject, obj.Layer);
					continue;
				}

				LoadObject(obj.MyObject, obj.Layer, obj.Front);
			}

			m_instantiations.clear();

			for (GameObject* des : m_destroyActiveQueue)
			{
				for (int layer = 0; layer < MICRO_RENDER_LAYER_COUNT; layer++)
				{
					int index = 0;
					std::vector<GameObject*>* objs = &m_activeGameObjects[layer];
					for (GameObject* obj : *objs)
					{
						if (obj == des)
						{
							objs->erase(objs->begin() + index);
							delete des;
							goto foundDesActive;
						}

						index++;
					}
				}

			foundDesActive:
				continue;
			}

			m_destroyActiveQueue.clear();

			//Check if any objects need to be destroyed.
			for (GameObject* des : m_destroyQueue)
			{
				for (int cI = 0; cI < levelSizeX * levelSizeY; cI++)	//Loop over each chunk.
				{
					for (int i = 0; i < CHUNK_OBJECT_RENDER_LAYER_COUNT; i++)	//Loop over layers in each chunk.
					{
						int index = 0;
						std::vector<GameObject*>* objs = &m_chunks[cI].GetObjects()[i];
						for (GameObject* obj : *objs)	//Loop over objects in each layer.
						{
							if (obj == des)	//If obj (current object to check) and des (target to destroy) point to the same GameObject, delete it.
							{
								objs->erase(objs->begin() + index);	//Remove the object from the chunk.
								delete des;
								goto foundDes;	//Break from loops apart from the destroyQueue loop.
							}
							index++;
						}

						//delete objs;
					}
				}

			foundDes:
				continue;
			}

			m_destroyQueue.clear();
		}

		void Renderer::DestroyActiveObjectImmediate(GameObject* des)
		{
			for (int layer = 0; layer < MICRO_RENDER_LAYER_COUNT; layer++)
			{
				int index = 0;
				std::vector<GameObject*>& objs = m_activeGameObjects[layer];
				for (GameObject* obj : objs)
				{
					if (obj == des)
					{
						objs.erase(objs.begin() + index);
						return;
					}

					index++;
				}
			}
		}

		void Renderer::ClearTextures(const unsigned int startIndex)
		{
#if _DEBUG
			uint16_t removedSprites = 0;
#endif

			//Update last file off if any of the first sprites loaded were cleared.
			lastFileOff = startIndex == 0 ? -1 : (startIndex > static_cast<unsigned int>(lastFileOff < 0 ? 0 : lastFileOff)) ? lastFileOff : startIndex;
			for (unsigned int i = startIndex; i < spriteCount + bgCount; i++)
			{
#if _DEBUG
				removedSprites++;
#endif

				const GLuint textureID = GetTextureIDFromIndex(i + 1);
				glDeleteTextures(1, &textureID);
			}
			
			//Reset all counters or decrease them by (count - startIndex).
			bgCount = startIndex == 0 ? 0 : ((spriteCount < startIndex) ? startIndex - spriteCount : bgCount);
			spriteCount = startIndex == 0 ? 0 : (spriteCount < startIndex) ? 0 : startIndex - bgCount;
			imageCount = startIndex == 0 ? 1 : startIndex;

			m_textureIDs.clear();

#if _DEBUG
			DebugLog::Log("Cleared " + std::to_string(removedSprites) + " sprites.", true);
#endif
		}

		void Renderer::ClearObjects()
		{
			for (int i = 0; i < levelSizeX * levelSizeY; i++)
			{
				m_chunks[i].ClearObjects();
			}


			for (int i = 0; i < MICRO_GUI_LAYER_COUNT; i++)
			{
				//for (GUI::IGUIElement* obj : m_guiGameObjects[i])
				//{
					//delete obj;
				//}

				m_guiGameObjects[i].clear();
			}

			for (int i = 0; i < 4; i++)
			{
				/*for (GameObject* obj : m_activeGameObjects[i])
				{
					delete obj;
				}*/

				m_activeGameObjects[i].clear();
			}
		}

		/*
			B64 files should contain %width%:%height%:%imageData%
			i_b64: FILE EXTENSION FOR B64 PNG DATA.
		*/
		GLuint Renderer::loadImage(const char* file)
		{
			//Load B64 file.
			if (std::string(file).ends_with(".i_b64"))
			{
				std::ifstream fileData(file);
				std::string line;

				//Read B64 image file data if this file is a .b64
				while (std::getline(fileData, line, '\n'))
				{
					//Comments start with "!" 
					if (line.starts_with("!")) { continue; }
					
					int width = 0, height = 0;
					std::string lineSegment; //Current iterated ":" segment.

					//Concatenate line until it starts with a ":" and find width/height.
					while ((!line.starts_with(":") || width == 0 || height == 0) && line.length() > 0)
					{
						std::string substrval = line.substr(0, 1);

						if (substrval != ":")
						{
							//Create segment.
							lineSegment.append(line.substr(0, 1));
						}

						line = line.substr(1);

						//What each segment should do.
						if (line.starts_with(":"))
						{
							if (width == 0)
							{
								width = std::stof(lineSegment);
							}
							else if (height == 0)
							{
								height = std::stof(lineSegment);
							}

							//Reset segment.
							lineSegment = "";
						}

					}

					//Remove ":" at the beginning.
					line = line.substr(1);

					//Found file data, load current line image via reading B64.
					std::vector<unsigned char> decode = Resources::B64::DecodeToBytes(line);

					int x, y, channelsInFile;
					unsigned char* data = stbi_load_from_memory(&decode[0], decode.size(), &x, &y, &channelsInFile, STBI_rgb_alpha);
					loadImageData(data, width, height);
				}

				return (GLuint)m_textureIDs.size() - 1;
			}

			//Load PNG file.
			int width, height, channels;
			unsigned char* data = stbi_load(file, &width, &height, &channels, 4);

			GLuint val = loadImageData(data, width, height);
			return val;
		}

		GLuint Renderer::loadImageData(unsigned char* data, const int& width, const int& height, bool freeData)
		{
			if (m_textureIDs.size() < 1)
			{
				m_textureIDs.reserve(120);
			}

			if (data == NULL)
			{
				std::cout << stbi_failure_reason() << std::endl;
			}

			//Add the texture to the list of textures.
			GLuint& textureBuffer = m_textureIDs.emplace_back(0);

			//Create the texture ID and assign the image to that ID.
			glGenTextures(1, &textureBuffer);
			glBindTexture(GL_TEXTURE_2D, textureBuffer);

			//Parameters for transparency, height, width, etc.
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			//Smoothing function.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST /*GL_LINEAR*/);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /*GL_LINEAR*/);

			//Wrapping.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			imageCount++;		//REMOVED FOR SOME REASON?
#if _DEBUG
			std::cout << "GLBUFFERLOAD: " << textureBuffer << std::endl;
			std::cout << "IMAGECOUNT: " << imageCount << std::endl;
#endif

			if (freeData)
			{
				free(data);
			}

			return (GLuint)m_textureIDs.size() - 1;
		}

		void Renderer::InitChunks(const std::array<int, DEFAULT_LEVEL_SIZE_X * DEFAULT_LEVEL_SIZE_Y>& chunkData)
		{
			for (int x = levelSizeX - 1; x >= 0; x--)
			{
				for (int y = 0; y < levelSizeY; y++)
				{
					int currChunk = y + static_cast<int>(x * levelSizeY);

					Renderer::m_chunks[currChunk] = Chunk(Vec2((float)x, (float)y), currChunk >= chunkData.size() ? 0 : (chunkData[currChunk] == 0 ? nullptr : getBackground(chunkData[currChunk])));
				}
			}
		}


		void Renderer::LoadObjects(GameObject* objects[], const unsigned int size, uint8_t layer, bool front)
		{
			for (unsigned int i = 0; i < size; i++)
			{
				LoadObject(objects[i], layer, front);
			}
		}
#define AsChunkPosition(x) (x >> ChunkShift)

		void Renderer::LoadObject(GameObject* object, uint8_t layer, bool front)
		{
			int chunkX = AsChunkPosition(static_cast<int>(object->GetPosition().X)) * levelSizeY;
			int chunkY = AsChunkPosition(static_cast<int>(object->GetPosition().Y));

#if _DEBUG
			std::cout << "Loaded Object in position: " + Vec2((float)chunkX, (float)chunkY).ToString() << std::endl;
			if (chunkY >= levelSizeY)
			{
				DebugLog::LogError("Object is outside of chunk height boundaries! WRAPPING TO NEXT CHUNK COLUMN WILL OCCUR.");
			}
#endif

			Renderer::m_chunks[chunkX + chunkY].Instantiate(object, layer, front);
			m_lastLoadedObject = object;
		}

		void Renderer::LoadActiveObjects(GameObject* objects[], const unsigned int size, int layer)
		{
			for (unsigned int i = 0; i < size; i++)
			{
				LoadActiveObject(objects[i], layer);
			}
		}

		void Renderer::LoadActiveObject(GameObject* object, int layer)
		{
			Renderer_activeGameObjectsLock.lock();
			m_activeGameObjects[layer].push_back(object);
			Renderer_activeGameObjectsLock.unlock();
			m_lastLoadedObject = object;
		}

		///Macro for converting to Chunk Coords.
		int renderCalls = 0;
		float tAlloc = 0;
		float m_curr = 0;

		void Renderer::Render(Camera::Camera* camera, Vec2* windowSize, RENDER_LAYER layer, GLFWwindow* window, float parallax)
		{
			//Update objects before rendering them.
			updateObjectQueues();

			CurrentlyRenderingLayer = layer;

			if (layer == RENDER_LAYER_GUI)
			{
				for (uint8_t currLayer = 0; currLayer < MICRO_GUI_LAYER_COUNT; currLayer++)
				{
					//Store size before rendering to avoid running into issues with the length of m_guiGameObjects being modified during rendering.
					const size_t size = m_guiGameObjects[currLayer].size();
					for (size_t i = 0; i < size; i++)
					{
						//Weird issue??? concurrent modification?
						if (i >= m_guiGameObjects[currLayer].size()) { break; }

						if (m_guiGameObjects[currLayer][i] != nullptr && i < m_guiGameObjects[currLayer].size())
						{
							try {
								m_guiGameObjects[currLayer][i]->Render(camera->GetZoom());
							}
							catch (...)
							{

							}
						}
					}
				}

				return;
			}

			Vec2 cameraPosition = camera->GetPosition() / parallax;

			constexpr unsigned int cameraBoundsPadding = (int)(chunkSize * (1.33f));

			//Define const variables.

			iVec2 cameraBoundingBox = (camera->GetZoom() != 1 ? (*windowSize) * (1 / camera->GetZoom()) + (cameraBoundsPadding << 1) : (*windowSize) + cameraBoundsPadding) + Vec2(0, chunkSize * 2.f);

			if (layer == RENDER_LAYER_ACTIVE_OBJECTS)
			{
				std::vector<GameObject*> renderBuffer;
				//Reserve all the space needed for the buffer.
				renderBuffer.reserve(m_activeGameObjects[1].size() + m_activeGameObjects[2].size() + m_activeGameObjects[3].size());

				Vec2 cameraPositionPadding = cameraPosition - cameraBoundsPadding / 2.f;
				Vec2 cameraBounds = Vec2(static_cast<const float>(cameraBoundingBox.GetX()), static_cast<const float>(cameraBoundingBox.GetY()));
				//Render active objects if they are on screen. Renders in order of layers from least to greatest.
				for (int i = 1; i < MICRO_RENDER_LAYER_COUNT; i++)
				{
					for (int d = 0; d < m_activeGameObjects[i].size(); d++)
					{
						GameObject* obj = m_activeGameObjects[i][d];
						if (obj == nullptr)
						{
							m_activeGameObjects[i].erase(m_activeGameObjects[i].begin() + (d--));
							continue;
							
						}

						if (!obj->GetActive()) { continue; }

						if (UpdateObjects)
						{
							obj->Update(window);
						}

						if (Utils::CollisionDetection::BoxWithinBox(obj->GetPosition(), obj->GetScale(), cameraPositionPadding, cameraBounds))
						{
							renderBuffer.emplace_back(obj);
						}

					}
				}

				for (int i = 0; i < renderBuffer.size(); i++)
				{
					renderBuffer[i]->Render(cameraPosition);
				}

				return;
			}

			iVec2 cameraChunkPosition = AsChunkPosition(cameraPosition);
			const Vec2 cameraPositionS = cameraPosition - cameraBoundsPadding;

			int cameraTopEdge = cameraChunkPosition.GetY() + AsChunkPosition(cameraBoundingBox.GetY());
			//if (AsChunkPosition(cameraBoundingBox.GetY()) >= LEVEL_SIZE_Y * 2) { return; }


			//delete cameraPositionPadding;

			const int iterInit = cameraChunkPosition.GetX() * levelSizeY + cameraChunkPosition.GetY() - (levelSizeY * 2);
			const int iterEnd = (AsChunkPosition(((int)cameraPosition.X + cameraBoundingBox.GetX() + 2))) * levelSizeY;

			for (int rLayer = 0; rLayer < MICRO_RENDER_LAYER_COUNT; rLayer++)
			{
				//Loop over chunks that are before the right edge of the camera.
				for (int i = iterInit; i < iterEnd; i++)
				{
					//Check if chunk is out of level bounds.
					if (i < 0 || i >= levelSizeX * levelSizeY) { continue; }

					//Check if a chunk is within the bounding box of the camera. (MAY NEED BUGFIXED FOR WHEN CAMERA CHANGES ZOOM!!!!!)
					//if (m_chunks[i].GetPosition().Y < cameraTopEdge)
					//{
						//Render a chunk if it is in the cameras bounding box.
					m_chunks[i].Render(cameraPosition, chunkSize, layer, window, rLayer);

					//Check if the chunk we just rendered is at the top of the screen.
					if ((i % levelSizeY) == (levelSizeY - 1))
					{
						//If the chunk just rendered is at the top of the screen, skip all chunks in the next column below the camera's position. 

						i += cameraChunkPosition.GetY();
					}

					//}
					//else
					//{
						//If we tried to render a chunk and failed, skip this entire column.
					//	i += cameraChunkPosition.GetY() + (levelSizeY - 1) - i % levelSizeY;
					//}
				}
			}

			if (layer == RENDER_LAYER_BG)
			{
				std::vector<GameObject*> renderBuffer;
				renderBuffer.reserve(m_activeGameObjects->size());

				//Two loops are used to ensure that all objects render, and then all objects update.
				for (GameObject* bg2Object : m_activeGameObjects[0])
				{
					if (!bg2Object->GetActive()) { continue; }

					if (UpdateObjects)
					{
						bg2Object->Update(window);
					}

					renderBuffer.push_back(bg2Object);
				}

				for (int i = 0; i < renderBuffer.size(); i++)
				{
					renderBuffer[i]->Render(cameraPosition);
				}
			}

		}

		std::vector<GameObject*> Renderer::GetAllObjectsInArea(Vec2 bottomLeft, Vec2 scale, bool boxOverlap, int8_t layer)
		{
			std::cout << "Searchcing from: " << bottomLeft.ToString() << std::endl;

			std::vector<GameObject*> ret;
			
			std::thread activeObjectCheck([layer, bottomLeft, scale, boxOverlap](std::vector<GameObject*>& ret) {
				if (layer != -1)
				{
					for (GameObject* obj : m_activeGameObjects[layer])
					{
						if (boxOverlap ? Utils::CollisionDetection::BoxWithinBox(bottomLeft, scale, obj->GetPosition(), obj->GetScale()) : Utils::CollisionDetection::PointWithinBoxBL(obj->GetPosition(), bottomLeft, scale))
						{
							ret.push_back(obj);
						}
					}
				}
				else {
					for (int i = MICRO_RENDER_LAYER_COUNT - 1; i >= 0; i--)
					{
						for (GameObject* obj : m_activeGameObjects[i])
						{
							if (boxOverlap ? Utils::CollisionDetection::BoxWithinBox(bottomLeft, scale, obj->GetPosition(), obj->GetScale()) : Utils::CollisionDetection::PointWithinBoxBL(obj->GetPosition(), bottomLeft, scale))
							{
								ret.push_back(obj);
							}
						}
					}
				}
			}, std::ref(ret));

			iVec2 chunkPos = AsChunkPosition(bottomLeft);
			iVec2 chunkScale = AsChunkPosition(scale);

			int8_t start = chunkPos.GetX() * levelSizeY + chunkPos.GetY();

			if (boxOverlap) { start -= levelSizeY; if (start < 0) { start = 0; } }

			int8_t endX = chunkScale.GetX(); 
			int8_t endY = chunkScale.GetY();

			if (boxOverlap && endX < levelSizeX) { endX += 1; }
			if (boxOverlap && endY < levelSizeY) { endY += 1; }

			for (int8_t x = 0; x <= endX; x++)
			{
				for (int8_t y = 0; y <= endY; y++)
				{
					for (int i = 0; i < CHUNK_OBJECT_RENDER_LAYER_COUNT; i++)
					{
						for (GameObject* add : m_chunks[start + (x * levelSizeY) + y].GetObjects()[i])
						{
							if (boxOverlap ? Utils::CollisionDetection::BoxWithinBox(bottomLeft, scale, add->GetPosition(), add->GetScale()) : Utils::CollisionDetection::PointWithinBoxBL(add->GetPosition(), bottomLeft, scale))
							{
								ret.push_back(add);
							}
						}

						for (GameObject* add : m_chunks[start + (x * levelSizeY) + y].GetFrontObjects())
						{
							if (boxOverlap ? Utils::CollisionDetection::BoxWithinBox(bottomLeft, scale, add->GetPosition(), add->GetScale()) : Utils::CollisionDetection::PointWithinBoxBL(add->GetPosition(), bottomLeft, scale))
							{
								ret.push_back(add);
							}
						}
					}
				}
			}

			if (activeObjectCheck.joinable()) { activeObjectCheck.join(); }

			return ret;
		}

		std::vector<GameObject*> Renderer::GetAllChunkObjectsInArea(Vec2 bottomLeft, Vec2 scale)
		{

			std::vector<GameObject*> ret;
			iVec2 chunkPos = AsChunkPosition(bottomLeft);
			iVec2 chunkScale = AsChunkPosition(scale) + 1;

			if (chunkPos.GetX() < 0 || chunkPos.GetY() < 0) { return ret; }

			const int16_t start = chunkPos.GetX() * levelSizeY + chunkPos.GetY();
			const int16_t endX = chunkScale.GetX();
			const int16_t endY = chunkScale.GetY();

			for (int16_t x = 0; x < endX; x++)
			{
				for (int16_t y = 0; y < endY; y++)
				{
					for (int i = 0; i < CHUNK_OBJECT_RENDER_LAYER_COUNT; i++)
					{
						for (GameObject* add : m_chunks[start + (x * levelSizeY) + y].GetObjects()[i])
						{
							if (Utils::CollisionDetection::BoxWithinBox(add->GetPosition(), add->GetScale(), bottomLeft, scale))
							{
								ret.push_back(add);
							}
						}

						for (GameObject* add : m_chunks[start + (x * levelSizeY) + y].GetFrontObjects())
						{
							if (Utils::CollisionDetection::BoxWithinBox(add->GetPosition(), add->GetScale(), bottomLeft, scale))
							{
								ret.push_back(add);
							}
						}
					}
				}
			}

			return ret;
		}

		std::vector<GameObject*> Renderer::GetAllActiveObjectsInArea(Vec2 bottomLeft, Vec2 scale, int8_t layer)
		{
			std::vector<GameObject*> ret;
			if (layer != -1)
			{
				for (GameObject* obj : m_activeGameObjects[layer])
				{
					if (Utils::CollisionDetection::BoxWithinBox(obj->GetPosition(), obj->GetScale(), bottomLeft, scale))
					{
						
						ret.push_back(obj);
						ret.push_back(obj);
					}
				}

				return ret;
			}
			else {
				for (int i = 0; i < MICRO_RENDER_LAYER_COUNT; i++)
				{
					for (GameObject* obj : m_activeGameObjects[i])
					{
						if (Utils::CollisionDetection::BoxWithinBox(obj->GetPosition(), obj->GetScale(), bottomLeft, scale))
						{
							ret.push_back(obj);
						}
					}
				}

				return ret;
			}
		}


	}
}