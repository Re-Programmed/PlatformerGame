#include "NodeEnjoyerAbility.h"

#include "CharacterNodeManager.h"
#include "../../../Utils/Time/GameTime.h"

#define NODE_ENJOYER_MAX_SECONDS_BEFORE_INTERATION 100.0

#define NODE_ENJOYER_INTERACTION_COOLDOWN 10.0

#define NODE_ENJOYER_NUM_NODES_CHECKED 5

namespace GAME_NAME::Cutscenes
{
	void NodeEnjoyerAbility::Trigger(AnimatingCharacter* character)
	{
		//pass
	}

	void NodeEnjoyerAbility::Update(AnimatingCharacter* character)
	{
		if (m_targetNode)
		{
			m_travelTimeTimeout += Utils::Time::GameTime::DeltaTime::GetDeltaTime();

			//Destination reached!
			if (character->AtTarget() || m_travelTimeTimeout > 12.0)
			{
				m_targetNode->Use(character);

				m_usingNode = m_targetNode;
				m_targetNode = nullptr;
			}
		}
		else {	//!m_targetNode

			//If the character is frozen, they are using a node and must wait to use it again and get up before moving somewhere else.
			if (character->IsFrozen())
			{
				if (m_usingNode != nullptr)
				{
					const double randMax = (double)RAND_MAX;
					if (((double)std::rand() / randMax) < (m_timeSinceLastInteraction / NODE_ENJOYER_MAX_SECONDS_BEFORE_INTERATION))
					{
						//Getting up from the node that was previously used.
						m_usingNode->Use(character);
						m_usingNode = nullptr;

						m_timeSinceLastInteraction = 0.0;
						m_travelTimeTimeout = 0.0;
					}
				}
				else {
					//The charater is still frozen but not using a node now. This shouldn't really ever happen...
					character->SetFrozen(false);
				}
			}
			else {	//!character->IsFrozen()

				//Is it time to interact?
				const double randMax = (double)RAND_MAX;
				if (((double)std::rand() / randMax) < (m_timeSinceLastInteraction / NODE_ENJOYER_MAX_SECONDS_BEFORE_INTERATION))
				{
					size_t found = 0;
					//Get the 4 closest nodes.
					std::array<ICharacterNode*, NODE_ENJOYER_NUM_NODES_CHECKED> nodes = CharacterNodeManager::GetNodesAtDistance<NODE_ENJOYER_NUM_NODES_CHECKED>(character->GetPosition(), found);

					int index = NODE_ENJOYER_NUM_NODES_CHECKED * std::rand() / RAND_MAX;

					//We did not find all four nodes and are trying to randomly choose one of the ones that was not found.
					if (index < NODE_ENJOYER_NUM_NODES_CHECKED - found) { return; }

					m_travelTimeTimeout = 0.0;
					m_targetNode = nodes[index];

					//Approach the target.
					character->SetTarget(m_targetNode->GetPosition());
					m_timeSinceLastInteraction = -NODE_ENJOYER_INTERACTION_COOLDOWN;
				}
			}


		}

		m_timeSinceLastInteraction += Utils::Time::GameTime::DeltaTime::GetDeltaTime();
	}

}