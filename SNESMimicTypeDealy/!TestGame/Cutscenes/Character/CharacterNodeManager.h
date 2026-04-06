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
		std::unique_ptr<ICharacterNode>* GetNodeAtDistance(Vec2 position, unsigned int nthClosest = 1);

		template<int size>
		std::array<ICharacterNode*, size> GetNodesAtDistance(Vec2 position);

	private:
		static std::vector<std::unique_ptr<ICharacterNode>> m_nodes;
	};

}