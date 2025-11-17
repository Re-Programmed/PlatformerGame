#include "StateSaver.h"
#include "../Resources/Save/SaveManager.h"

std::vector<GAME_NAME::Objects::GameObjectState*> GAME_NAME::Objects::StateSaver::m_toBeSaved;
std::vector<GAME_NAME::MiscStateGroup*> GAME_NAME::Objects::StateSaver::m_miscStates;

void GAME_NAME::Objects::StateSaver::RegisterToBeSaved(GameObjectState* state)
{
	m_toBeSaved.push_back(state);
	state->SetToBeSaved(true);
}

void GAME_NAME::Objects::StateSaver::RegisterToBeSaved(MiscStateGroup* state)
{
	//Remove duplicate states and use newest version.
	for (int i = 0; i < m_miscStates.size(); i++)
	{
		MiscStateGroup*& miscState = m_miscStates[i];
		if (miscState == nullptr || miscState->StateCount() > 1000) 
		{ 
			m_miscStates.erase(m_miscStates.begin() + i);
			i--;
			continue;
		}

		for (int z = i + 1; z < m_miscStates.size(); z++)
		{
			if (miscState->GetName() == m_miscStates[z]->GetName())
			{
				m_miscStates.erase(m_miscStates.begin() + i);
				i--;
				continue;
			}
		}

	}

	m_miscStates.push_back(state);
}

void GAME_NAME::Objects::StateSaver::SaveStates()
{
	for (GameObjectState* gos : m_toBeSaved)
	{
		gos->SaveState();
		gos->SetToBeSaved(false);

		//delete gos;
	}

	m_toBeSaved.clear();
}

void GAME_NAME::Objects::StateSaver::SaveMisc()
{
	for (MiscStateGroup* miscState : m_miscStates)
	{
		if (miscState == nullptr || miscState->StateCount() > 1000) { continue; }
		if (miscState->HasStates())
		{
			Resources::SaveManager::CreateSaveFile(miscState->GetSaveString(), miscState->GetName());
		}
	}
}
