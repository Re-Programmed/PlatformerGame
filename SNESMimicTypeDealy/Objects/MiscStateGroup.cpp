#include "MiscStateGroup.h"
#include "StateSaver.h"
#include "../Resources/Save/SaveManager.h"

namespace GAME_NAME
{
	MiscStateGroup::MiscStateGroup(std::string destination)
		: m_destination(destination)
	{
		Objects::StateSaver::RegisterToBeSaved(this);
	}

	MiscStateGroup::~MiscStateGroup()
	{
		Objects::StateSaver::UnregisterToBeSaved(this);
	}

	void MiscStateGroup::assignState(MiscState* state)
	{
		m_states.push_back(state);
	}

	std::shared_ptr<std::vector<std::string>> MiscStateGroup::getStates() const
	{
		std::shared_ptr<std::vector<std::string>> data = Resources::SaveManager::ReadSaveFile(m_destination);

		if (data->size() == 0) { return std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>); }

		std::vector<std::string>* vals = new std::vector<std::string>();

		std::stringstream ss(data->at(0));
		std::string line;

		while(std::getline(ss, line, MISC_GROUP_DELIM))
		{
			if(!line.empty())
			{
				vals->push_back(line);
			}
		}

		vals->erase(vals->begin() + vals->size() - 1);

		return std::shared_ptr<std::vector<std::string>>(vals);
	}



	std::string MiscStateGroup::GetSaveString() const
	{
		int i = 0;

		std::string returnVal = "";

		while (i < m_states.size())
		{
			MiscState* state = this->m_states[i++];

			returnVal += state->Encode() + MISC_GROUP_DELIM;
		}

		return returnVal;
	}

}   