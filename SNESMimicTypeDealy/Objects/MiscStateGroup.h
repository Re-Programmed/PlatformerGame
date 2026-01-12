#pragma once
#include <vector>
#include "MiscState.h"
#include <memory>

/*
	A MiscStateGroup contains a list of MiscState objects that are serialized and saved to a file when the game is saved.
	A MiscStateGroup requres one string that represents the file name of the location where the MiscStates will be saved under it should go.
	(MiscStateGroup objects have no control over how the individual MiscState strings are formatted other than the '/' delim between each MiscState.)

	Ex.
		inventory (MiscStateGroup './inventory_data')
		{
			item (MiscState {serialized="1+15+wood"})
			item (MiscState {serialized="1+12+iron"})
			item (MiscState {serialized="6+21+slab"})
		}

	becomes
		
		'%appdata%/inventory_data' -> "1+15+wood/1+12+iron/6+21+slab"
*/

constexpr char MISC_GROUP_DELIM = '/';

namespace GAME_NAME
{
	class MiscStateGroup
	{
	public:
		MiscStateGroup(std::string destination);

		~MiscStateGroup();

		inline std::string GetName() const
		{
			return m_destination;
		}

		inline const size_t StateCount()
		{
			return m_states.size();
		}

		inline bool HasStates()
		{
			return m_states.size() > 0;
		}

		std::string GetSaveString() const;
	protected:
		void assignState(MiscState* state);

		/// <summary>
		/// Returns a list of all the states saved. May be used to load different classes for each state (ex. Tool if prefix 't' but InventoryItem if no prefix)
		/// </summary>
		/// <returns></returns>
		std::shared_ptr<std::vector<std::string>> getStates() const;

		inline void clearStates(bool deleteS = false)
		{
			if (deleteS)
			{
				for (MiscState* state : m_states)
				{
					delete state;
				}
			}

			m_states.clear();
		}
	private:
		std::vector<MiscState*> m_states;
		std::string m_destination;
	};

}