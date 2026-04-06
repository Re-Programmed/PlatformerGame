#pragma once

#include "ICharacterNode.h"

namespace GAME_NAME::Cutscenes
{
	class CharacterNodeManager
	{
	public:
		/// <summary>
		/// Returns the nth closest node to the given position. Returns nullptr if no node exists.
		/// </summary>
		/// <param name="position">The position to search from.</param>
		/// <param name="nthClosest">How many positions down the node should be.</param>
		/// <returns></returns>
		static ICharacterNode* GetNodeAtDistance(Vec2 position, unsigned int nthClosest = 1);

		template<int size>
		static std::array<ICharacterNode*, size> GetNodesAtDistance(Vec2 position, size_t& numFound)
		{
			std::array<ICharacterNode*, size> nodes;
			std::array<float, size> nodeDistances;

			if (m_nodes.size() < size) { numFound = m_nodes.size(); }

			for (int i = 0; i < m_nodes.size(); i++)
			{
				float dist = Vec2::Distance(position, m_nodes[i]->GetPosition());

				int insert = size - 1;
				std::tuple<ICharacterNode*, float> swap{ nullptr, 0.f };
				while (insert >= 0)
				{
					if (std::get<ICharacterNode*>(swap) != nullptr)
					{
						auto temp = nodes[insert];
						auto tempDist = nodeDistances[insert];
						nodes[insert] = std::get<ICharacterNode*>(swap);
						nodeDistances[insert] = std::get<float>(swap);
						swap = { temp, tempDist };
					}
					else if (nodeDistances[insert] < dist)
					{
						swap = { nodes[insert], nodeDistances[insert] };
						nodeDistances[insert] = dist;
						nodes[insert] = m_nodes[i];
					}

					insert--;
				}
			}

			return nodes;
		}

		inline static void ClearNodes() { m_nodes.clear(); }
		inline static void RegisterNode(ICharacterNode* node)
		{
			m_nodes.push_back(node);
		}

		inline static void RemoveNode(ICharacterNode* node)
		{
			for (int i = 0; i < m_nodes.size(); i++)
			{
				if (m_nodes[i] == node) { m_nodes.erase(m_nodes.begin() + i); }
				return;
			}
		}

#if _DEBUG
		static void DebugRender();

		inline static void ToggleDebugGraph() { m_showDebugGraph = !m_showDebugGraph; }
#endif

	private:
#if _DEBUG
		static bool m_showDebugGraph;
#endif

		static std::vector<ICharacterNode*> m_nodes;
	};

}