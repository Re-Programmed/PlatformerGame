#include "Chunk.h"
#include "../glad/include/glad/glad.h"
#include "../Rendering/Renderers/Renderer.h"
namespace GAME_NAME
{

	namespace Objects
	{
		Chunk::Chunk()
			: m_position(Vec2::Zero), m_bgSprite(nullptr)
		{

		}

		/// <summary>
		/// (Cannot be called during rendering) Adds object to chunk renderer.
		/// </summary>
		void Chunk::Instantiate(GameObject* object, uint8_t layer, bool front)
		{
			m_objectsLock.lock();
			try
			{
				if (front)
				{
					m_frontObjects.push_back(object);
				}
				else {
					if (layer >= CHUNK_OBJECT_RENDER_LAYER_COUNT) { m_objectsLock.unlock(); return; }
					
					//m_objects not mutex?
					m_objects[layer].push_back(object);
				}
			}
			catch (...)
			{

			}
			m_objectsLock.unlock();
		}

		Vec2 Chunk::GetPosition()
		{
			return m_position;
		}


		std::vector<GameObject*>* Chunk::GetObjects()
		{
			return m_objects;
		}

		std::vector<GameObject*>& Chunk::GetFrontObjects()
		{
			return m_frontObjects;
		}

		void Chunk::Render(const Vec2 cameraPosition, const int chunkSize, RENDER_LAYER layer, GLFWwindow* window, int microLayer)
		{
			switch (layer)
			{
			case RENDER_LAYER_BG:
				if (m_bgSprite != nullptr)
				{
					m_bgSprite->Render(cameraPosition, m_position << ChunkShift, Vec2(-(float)chunkSize, (float)chunkSize));
				}
				break;
			case RENDER_LAYER_OBJECTS:
				if (microLayer < 0)
				{
					for (int i = 0; i < CHUNK_OBJECT_RENDER_LAYER_COUNT; i++)
					{
						for (int d = 0; d < m_objects[i].size(); d++)
						{
							if (m_objects[i][d] == nullptr) { m_objects[i].erase(m_objects[i].begin() + d); d--; continue; }
							if (!m_objects[i][d]->GetActive()) { continue; }
							if (Renderer::UpdateObjects) { m_objects[i][d]->Update(window); }
							m_objects[i][d]->Render(cameraPosition);
						}
					}
				}
				else {
					for (int d = 0; d < m_objects[microLayer].size(); d++)
					{
						if (m_objects[microLayer][d] == nullptr) { m_objects[microLayer].erase(m_objects[microLayer].begin() + d); d--; continue; }
						if (!m_objects[microLayer][d]->GetActive()) { continue; }
						if (Renderer::UpdateObjects) { m_objects[microLayer][d]->Update(window); }
						m_objects[microLayer][d]->Render(cameraPosition);
					}
				}
				break;
			case RENDER_LAYER_OBJECTS_FRONT:
				for (int d = 0; d < m_frontObjects.size(); d++)
				{
					if (m_frontObjects[d] == nullptr) { m_frontObjects.erase(m_frontObjects.begin() + d); d--; continue; }
					if (!m_frontObjects[d]->GetActive()) { continue; }
					if (Renderer::UpdateObjects) { m_frontObjects[d]->Update(window); }
					m_frontObjects[d]->Render(cameraPosition);
				}
				break;
			default:
				break;
			}
		}

	}
}