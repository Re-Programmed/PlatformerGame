#include "CharacterNodeManager.h"

namespace GAME_NAME::Cutscenes
{
	std::vector<ICharacterNode*> CharacterNodeManager::m_nodes;

#if _DEBUG
	bool CharacterNodeManager::m_showDebugGraph = false;
#endif

	ICharacterNode* CharacterNodeManager::GetNodeAtDistance(Vec2 position, unsigned int nthClosest)
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

		return m_nodes[index];
	}

#if _DEBUG
	void CharacterNodeManager::DebugRender()
	{
		if (!m_showDebugGraph) { return; }

		const Vec2 cameraPos = TestGame::INSTANCE->GetCamera()->GetPosition();

		Sprite* s = Renderer::GetSprite(SpriteBase(91));

		for (ICharacterNode* node : m_nodes)
		{
			s->Render(cameraPos, node->GetPosition(), Vec2{ 16.f }, 0.f);
		}

		delete s;
	}
#endif
}
