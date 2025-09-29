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
		if (miscState == nullptr) { continue; }
		if (miscState->HasStates())
		{
			Resources::SaveManager::CreateSaveFile(miscState->GetSaveString(), miscState->GetName());
		}
	}
}
