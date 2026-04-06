#include "CharacterNodeManager.h"

namespace GAME_NAME::Cutscenes
{
	std::unique_ptr<ICharacterNode>* CharacterNodeManager::GetNodeAtDistance(Vec2 position, unsigned int nthClosest)
	{
		//Store a list of the top n closest nodes.
		std::vector<std::tuple<int, float>> nodeDistances;
		nodeDistances.resize(nthClosest, { -1, 0.f });

		//Loop over all nodes. (kinda the brute force solution, TODO: maybe make faster, but also there are not really that many nodes...)
		for (int i = 0; i < m_nodes.size(); i++)
		{
			//Get the distance to the node.
			float dist = Vec2::Distance(position, m_nodes[i]->GetPosition());

			int insert = nthClosest - 1;
			std::tuple<int, float> swap = { -1, 0.f };
			while (insert >= 0)
			{
				if (std::get<int>(swap) >= 0)
				{
					auto temp = nodeDistances[insert];
					nodeDistances[insert] = swap;
					swap = temp;
				}else if (std::get<float>(nodeDistances[insert]) < dist)
				{
					swap = nodeDistances[insert];
					nodeDistances[insert] = { i, dist };
				}

				insert--;
			}
		}

		const int& index = std::get<int>(nodeDistances[nthClosest]);

		if (index == -1) { return nullptr; }

		return &(m_nodes[index]);
	}

	template<int size>
	std::array<ICharacterNode*, size> CharacterNodeManager::GetNodesAtDistance(Vec2 position)
	{
		std::array<std::unique_ptr<ICharacterNode>*, size> nodes;
		std::array<float, size> nodeDistances;

		for (int i = 0; i < m_nodes.size(); i++)
		{
			float dist = Vec2::Distance(position, m_nodes[i]->GetPosition());
			
			int insert = size - 1;
			std::tuple<std::unique_ptr<ICharacterNode>*, float> swap = { nullptr };
			while (insert >= 0)
			{

			}
		}
	}
}
