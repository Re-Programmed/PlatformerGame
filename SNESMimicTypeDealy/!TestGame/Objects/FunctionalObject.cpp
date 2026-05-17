#include "FunctionalObject.h"
#include "../InputDisplay/DisplayIconManager.h"

namespace GAME_NAME::Objects
{
	std::unordered_map<std::string, std::function<void(GameObject* self)>> FunctionalObject::m_functionalCalls;

	FunctionalObject::FunctionalObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, std::string functionKey, bool triggeredOnTouch)
		: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, scale.X, position, scale, sprite), m_functionKey(functionKey), m_triggeredOnTouch(triggeredOnTouch)
	{

	}

	void Objects::FunctionalObject::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{
		if (m_triggeredOnTouch || state & InputManager::KEY_STATE_PRESSED)
		{
			trigger();
			return;
		}

		GAME_NAME::Input::DisplayIconManager::ShowKeyInputDisplay(keyRef::PLAYER_INTERACT, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), state & InputManager::KEY_STATE_HELD ? 8 : 0);
	}

	void FunctionalObject::trigger()
	{
		(getFunction(m_functionKey))(this);

		Renderer::DestroyObject(this);
	}

	void FunctionalObject::ClearRegisteredFunctions()
	{
		m_functionalCalls.clear();
	}

	void FunctionalObject::RegisterFunction(std::string key, std::function<void(GameObject* self)> function)
	{
		if (m_functionalCalls.contains(key)) { return; }
		
		m_functionalCalls.emplace(key, function);
	}

	const std::function<void(GameObject* self)>& Objects::FunctionalObject::getFunction(std::string key)
	{
		if (m_functionalCalls.contains(key))
		{
			return m_functionalCalls[key];
		}
		else {
			return [](GameObject* self) {};
		}
	}

}
