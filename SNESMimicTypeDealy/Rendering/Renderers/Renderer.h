#pragma once
#include "../../glad/include/glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "../../stbi_image/stbi_image.h"
#include "../../Objects/Chunk.h"
#include "../Camera/Camera.h"
#include "../../Utils/Math/iVec2.h"
#include "../../Objects/GUI/IGUIElement.h"
#include "../DynamicSprite.h"

#include <array>

#include "../../EngineCompileOptions.h"

#define SpriteBase(x) (GLOBAL_SPRITE_BASE+x)  //Returns the offset of a sprite from the sprite base.

#if _DEBUG
#include "../../Debug/DebugLog.h"
#endif

namespace GAME_NAME
{
	namespace Rendering
	{
#define MICRO_RENDER_LAYER_COUNT 4
#define MICRO_GUI_LAYER_COUNT 3
		constexpr int ChunkShift = 6;

		using namespace Objects;
		class ActiveObject {
		public:
			ActiveObject(GameObject* object, bool loaded)
				: Object(object), Loaded(loaded)
			{

			}

			GameObject* Object;
			bool Loaded;
		};

		//The main class to draw things to the screen.
		class Renderer
		{
		public:
			static bool UpdateObjects;

			//Load a file to the sprite buffer.
			static GLuint LoadSprite(const char* file);

			struct TextureData { unsigned char* Data; int Width, Height; };
			static inline GLuint LoadSpriteFromData(const TextureData& data) 
			{
				return LoadSpriteFromData(data.Data, data.Width, data.Height);
			}

			//Load a sprite into the buffer from given pixel data.
			static GLuint LoadSpriteFromData(unsigned char* data, const int& width, const int& height);

			//Load a file to the BG buffer.
			static GLuint LoadBG(const char* file);

			static const TextureData GetTextureData(GLuint textureID, GLuint mipmapLevel = 0);

			/// <summary>
			/// Removes all loaded buffers after the given index.
			/// </summary>
			static void ClearTextures(const unsigned int startIndex = 0);

			/// <summary>
			/// Removes all loaded objects.
			/// </summary>
			static void ClearObjects();

			/// <summary>
			/// Clears all GUI objects from after a given index.
			/// </summary>
			static void ClearGUIObjects(uint16_t startIndex, uint8_t layer);

			//Create a sprite from its texture.
			static Sprite* const GetSprite(const int spriteTexture);
			static DynamicSprite* const GetDynamicSprite(const int spriteTexture);

			static inline unsigned int GetSpriteIdFromTextureId(const int spriteTexture)
			{
				return spriteTexture + bgCount + lastFileOff;
			}

			static inline unsigned int GetSpriteCount()
			{
				return spriteCount;
			}

			static inline unsigned int GetBackgroundSpriteCount()
			{
				return bgCount;
			}

			/// <summary>
			/// Render is called every frame to draw everything to the screen.
			/// </summary>
			/// <param name="camera">The camera to use as a projection matrix.</param>
			/// <param name="windowSize">The size of the window. (Pointer for no reason, should probably be changed to reference)</param>
			/// <param name="layer">The current layer to render.</param>
			/// <param name="window">The window to render to.</param>
			/// <param name="parallax">The amount to parallax off of the camera.</param>
			static void Render(Camera::Camera* camera, Vec2* windowSize, RENDER_LAYER layer, GLFWwindow* window, float parallax = 1.f);

			/// <summary>
			/// The current layer that is getting rendered. Useful for applying vertex effects to only a specific layer.
			/// </summary>
			static RENDER_LAYER CurrentlyRenderingLayer;

			/// <summary>
			/// Returns every GameObject in an area.
			/// Use boxOverlap if you want all the objects overlapping the given area, otherwise this function will only return the objects with a position in the given area.
			/// </summary>
			static std::vector<GameObject*> GetAllObjectsInArea(Vec2 bottomLeft, Vec2 scale, bool boxOverlap = false, int8_t layer = -1);
			/// <summary>
			/// Returns every chunk GameObject in an area.
			/// </summary>
			static std::vector<GameObject*> GetAllChunkObjectsInArea(Vec2 bottomLeft, Vec2 scale);
			/// <summary>
			/// Returns every active GameObject in an area.
			/// </summary>
			static std::vector<GameObject*> GetAllActiveObjectsInArea(Vec2 bottomLeft, Vec2 scale, int8_t layer = -1);

			/// <summary>
			/// Begins drawing the objects in objects. CANNOT BE CALLED DURING RENDERING
			/// </summary>
			/// <param name="objects"></param>
			/// <param name="size"></param>
			static void LoadObjects(GameObject* objects[], const unsigned int size, uint8_t layer = 1, bool front = false);
			/// <summary>
			/// Begins drawing the object. CANNOT BE CALLED DURING RENDERING
			/// </summary>
			/// <param name="objects"></param>
			/// <param name="size"></param>
			static void LoadObject(GameObject* object, uint8_t layer = 1, bool front = false);
			
			static inline GameObject* GetLastLoadedObject()
			{
				return m_lastLoadedObject;
			}

			/// <summary>
			/// Used for specifying params when instantiating an object. (GameObject*, bool Active, int Layer, bool Front)
			/// </summary>
			struct InstantiateGameObject {
				GameObject* MyObject; //The object to spawn.
				const bool Active; //If it's an active object.
				const int Layer; //The layer to load it on.
				const bool Front; //If it's a front chunk object.

				InstantiateGameObject(GameObject* myObject, bool active, int layer, bool front) : MyObject(myObject), Active(active), Layer(layer), Front(front) { }
			};

			static inline void InstantiateObject(InstantiateGameObject object)
			{
				m_instantiations.push_back(object);
			}

			/*
				Do not delete the reference to the object you are destroying, the destroy method does it for you and also takes 1 extra cycle to complete.
			*/

			static inline void DestroyObject(GameObject* object)
			{
				m_destroyQueue.push_back(object);
			}

			static inline void DestroyActiveObject(GameObject* object)
			{
				m_destroyActiveQueue.push_back(object);
			}

			static void DestroyActiveObjectImmediate(GameObject* object);

			static void LoadActiveObjects(GameObject* objects[], const unsigned int size, int layer = 1);
			static void LoadActiveObject(GameObject* object, int layer = 1);

			/// <summary>
			/// Instantiates a GUI element with given layer.
			/// </summary>
			/// <param name="element">- the element to create.</param>
			/// <param name="layer">- the layer to spawn the element on.</param>
			static inline void LoadGUIElement(GUI::IGUIElement* element, int layer = 1)
			{
				m_guiGameObjects[layer].push_back(element);
			}

			/// <summary>
			/// Destroys the given gui element on the given layer.
			/// </summary>
			/// <param name="element">[IGUIElement*] - The element to destroy.</param>
			/// <param name="layer">[int] - The layer the element is on.</param>
			static void UnloadGUIElement(GUI::IGUIElement* element, int layer = 1)
			{
				uint16_t i = 0;
				for (GUI::IGUIElement* g : m_guiGameObjects[layer])
				{
					if (g == element)
					{
						m_guiGameObjects[layer].erase(m_guiGameObjects[layer].begin() + i);
						return;
					}

					i++;
				}
			}

			/// <summary>
			/// Must be called before the creation of any objects.
			/// </summary>
			static void InitChunks(const std::array<int, DEFAULT_LEVEL_SIZE_X* DEFAULT_LEVEL_SIZE_Y>& chunkData);

			/// <summary>
			/// Sets a constant to add to all sprite texture indices. (For loading multiple levels at once. The last loaded level will always use positive indices with other levels having negative values.)
			/// </summary>
			/// <param name="fileOffset">[int] - the offset to add to all sprite texture indicies.</param>
			inline static void SetLastFileOffset(int fileOffset)
			{
				lastFileOff = fileOffset;
			}

			/// <summary>
			/// Returns the constant added to all sprite texture indices.
			/// </summary>
			/// <returns>lastFileOff [int] - The offset that is currently applied to all sprite texture indicies.</returns>
			inline static int GetLastFileOffest()
			{
				return lastFileOff;
			}

			static inline const int GetTextureIndexFromID(const GLuint& ID)
			{
				//(ID = index - bgCount + lastFileOff)
				return ID - bgCount - lastFileOff;
			}

			/// <summary>
			/// Returns the GL sprite buffer from a given sprite index. *****DOES NOT ADHERE TO SPRITE_BASE.*****
			/// </summary>
			static inline const GLuint GetTextureIDFromIndex(const unsigned int& index)
			{
#if _DEBUG
				if (m_textureIDs.size() <= index)
				{
					DEBUG::DebugLog::LogError("[Renderer.h:206] Attempted to get non existant texture ID. (" + std::to_string(index) + ")");
					return 0;
				}
#endif
				return m_textureIDs[index - 1];
			}

			static inline const unsigned int GetImageCount()
			{
				return imageCount;
			}

			static inline const unsigned int GetIDCount()
			{
				return m_textureIDs.size();
			}

		private:
			/// <summary>
			/// Used to relate a sprite index back to a GL sprite buffer.
			/// </summary>
			static std::vector<GLuint> m_textureIDs;

			//A constant added to all sprite indices.
			static int lastFileOff;

			//The number of sprites loaded.
			static unsigned int spriteCount, bgCount, imageCount;


			/// <summary>
			/// Loads a texture to a buffer.
			/// </summary>
			/// <returns>The buffer loaded to.</returns>
			static GLuint loadImage(const char* file);

			static GLuint loadImageData(unsigned char* data, const int& width, const int& height, bool freeData = true);

			/// <summary>
			/// Objects that need updated no matter where they are in game.
			/// </summary>
			static std::vector<GameObject*> m_activeGameObjects[MICRO_RENDER_LAYER_COUNT];

			static std::vector<GUI::IGUIElement*> m_guiGameObjects[MICRO_GUI_LAYER_COUNT];

			static Chunk m_chunks[];
			static Sprite* const getBackground(const unsigned int bgTexture);

			static std::vector<GameObject*> m_destroyQueue;
			static std::vector<GameObject*> m_destroyActiveQueue;

			static std::vector<InstantiateGameObject> m_instantiations;

			/// <summary>
			/// Destroys objects in the destroy queue and instantiates all the objects in m_instantiations.
			/// </summary>
			static void updateObjectQueues();

			static GameObject* m_lastLoadedObject;
		};


	}
}