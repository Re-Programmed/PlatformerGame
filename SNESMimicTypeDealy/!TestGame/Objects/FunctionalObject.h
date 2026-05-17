#pragma once

#include "../../Objects/GameObject.h"
#include "../../Objects/Helpers/Interactable.h"

namespace GAME_NAME::Objects
{
	/// <summary>
	/// Objects that can call specific stored functions based on a string key.
	/// </summary>
	class FunctionalObject
		: public Interactable
	{
	public:
		static void ClearRegisteredFunctions();
		static void RegisterFunction(std::string key, std::function<void(GameObject* self)> function);

		FunctionalObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, std::string functionKey, bool triggeredOnTouch = false);

	protected:
		void trigger();
		void onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state) override;

	private:
		static std::unordered_map<std::string, std::function<void(GameObject* self)>> m_functionalCalls;
		static const std::function<void(GameObject* self)>& getFunction(std::string key);

		const bool m_triggeredOnTouch;
		const std::string m_functionKey;
	};
}