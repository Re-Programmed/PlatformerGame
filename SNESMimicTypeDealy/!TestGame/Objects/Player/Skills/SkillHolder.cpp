#include "SkillHolder.h"

#include "../../../../Input/InputManager.h"
#include "../../../../Utils/CollisionDetection.h"
#include "../../../../Objects/GUI/GUIManager.h"

#define SKILL_HOLDER_SKILL_DISPLAY_LAYER 1 //The layer that the skill display is rendered on.


namespace GAME_NAME::Objects::Player
{
	SkillHolder* SkillHolder::m_currentOpenSkillHolder = nullptr;

	GLuint SkillHolder_GetSkillSpriteId(SKILL_TYPE skill)
	{
		switch (skill)
		{
		case NO_SKILL:
			return Renderer::GetSprite(SpriteBase(103))->GetSpriteId();
		case FLIGHT:
			return Renderer::GetSprite(SpriteBase(104))->GetSpriteId();
		}
	}

	SkillHolder::SkillHolder(const Vec2& skillDisplayPosition)
		: m_skillDisplay(new StaticGUIElement(skillDisplayPosition, Vec2(16), Renderer::GetSprite(SpriteBase(103))->GetSpriteId())), m_currentSkill(SKILL_TYPE::NO_SKILL),
		MiscStateGroup("sh")
	{
		//Always allow selection of NO_SKILL.
		m_unlockedSkills.push_back(NO_SKILL);

		Renderer::LoadGUIElement(m_skillDisplay, SKILL_HOLDER_SKILL_DISPLAY_LAYER);

		auto states = getStates();

		if (states->size() >= 1)
		{
			//The first state listed is the current skill.
			m_currentSkill.Decode(states->at(0));
		}

		m_skillDisplay->SetSprite(new Sprite(SkillHolder_GetSkillSpriteId(m_currentSkill.Skill)));

		//The current skill will always be the first state saved.
		assignState(&m_currentSkill);

		for (int i = 1; i < states->size(); i++)
		{
			SkillState skill(NO_SKILL);
			skill.Decode(states->at(i));

			//Unlock all saved skills.
			UnlockSkill(skill.Skill);
		}
	}

	SkillHolder::~SkillHolder()
	{
		if (m_skillDisplay != nullptr)
		{
			Renderer::UnloadGUIElement(m_skillDisplay, SKILL_HOLDER_SKILL_DISPLAY_LAYER);
			delete m_skillDisplay;
		}
	}

	void SkillHolder::SetCurrentSkill(SKILL_TYPE type)
	{
		m_currentSkill.Skill = type;

		m_skillDisplay->SetSprite(new Sprite(SkillHolder_GetSkillSpriteId(type)));
	}

	void SkillHolder::Update()
	{
		if (InputManager::GetMouseButton(0))
		{
			if (Utils::CollisionDetection::PointWithinBoxBL(InputManager::GetMouseScreenPosition(), m_skillDisplay->GetPosition(), m_skillDisplay->GetScale()))
			{
				//Skill display clicked.
				OpenSkillMenu();
			}
		}
	}

	void SkillHolder::OpenSkillMenu()
	{
		if (GUI::GUIManager::PreventMenus) { return; }
		if (m_skillMenuIsOpen) { return; }
		if (m_currentOpenSkillHolder != nullptr) { return; }

		GUI::GUIManager::PreventMenus = true;

		m_currentOpenSkillHolder = this;

		int y = 0;
		for (int i = 0; i < m_unlockedSkills.size(); i++)
		{
			//Set the y position of the buttons to each spot 20 above the display, unless this is the current skill, in which case it is placed in the same position as the display.
			float yPosition = m_skillDisplay->GetPosition().Y;
			if (m_unlockedSkills[i] != m_currentSkill.Skill)
			{
				yPosition += (20.f + y * 20.f);
			} else {
				y--;
			}

			GUIButton* skillOption = new GUIButton(Vec2{ m_skillDisplay->GetPosition().X, yPosition }, Vec2{ 16.f, 16.f }, SkillHolder_GetSkillSpriteId(m_unlockedSkills[i]), new std::function(skillMenuButtonClicked), { 1.f, 1.f, 1.f, 0.f });
			m_skillMenuButtons.push_back(skillOption);

			GUIManager::RegisterButton(skillOption);
			Renderer::LoadGUIElement(skillOption);

			y++;
		}

		std::thread fadeUpButtons([this]() {
			for (int i = 0; i < 500; i++)
			{
				for (GUIButton*& button : m_skillMenuButtons)
				{
					button->SetBaseColor({ 1.f, 1.f, 1.f, std::lerp(button->GetBaseColor().W, 1.f, 0.01f) });
					std::this_thread::sleep_for(std::chrono::microseconds(1));
				}

				if(i == 499){ m_skillMenuIsOpen = true; } //Prevent button click until fade is complete.
			}
		});
		fadeUpButtons.detach();
	}

	void SkillHolder::CloseSkillMenu()
	{
		if (!m_skillMenuIsOpen) { return; }

		m_skillMenuIsOpen = false;
		GUI::GUIManager::PreventMenus = false;

		std::thread fadeAwayButtons([this]() {
		for (int i = 0; i < 500; i++)
		{
			for (GUIButton*& button : m_skillMenuButtons)
			{
				if (i == 499) { Renderer::UnloadGUIElement(button); GUIManager::UnregisterButton(button); delete button; continue; }

				button->SetBaseColor({ 1.f, 1.f, 1.f, std::lerp(button->GetBaseColor().W, 0.f, 0.01f) });
				button->SetHoverColor({ 1.f, 1.f, 1.f, std::lerp(button->GetHoverColor().W, 0.f, 0.01f) });

				std::this_thread::sleep_for(std::chrono::microseconds(1));
			}

			if (i == 499) { m_skillMenuButtons.clear(); m_currentOpenSkillHolder = nullptr; }
		}
		});
		fadeAwayButtons.detach();

	}

	void SkillHolder::skillMenuButtonClicked(int id)
	{
		if (m_currentOpenSkillHolder == nullptr) { return; }
		if (!m_currentOpenSkillHolder->m_skillMenuIsOpen) { return; }

		m_currentOpenSkillHolder->SetCurrentSkill(m_currentOpenSkillHolder->m_unlockedSkills[id]);
		m_currentOpenSkillHolder->CloseSkillMenu();
	}

	float SkillHolder::GetEquipmentBoostEffect(std::string effect, Backpack* backpack)
	{
		float total = 0.f;

		for (uint8_t i = 0; i < BACKPACK_NUM_EQUIPMENT_SLOTS; i++)
		{
			Inventory::ReturnItem equipment = backpack->GetItem(i);

			if (!equipment.ri_IsNull)
			{
				const ItemData& data = ITEMTYPE_GetItemData(equipment.ri_Item->GetType());
				if (data.Actions & TOOL_ACTION::EQUIPMENT && data.Attributes.find(TOOL_ACTION::EQUIPMENT) != data.Attributes.end())
				{
					const std::string& attribute = data.Attributes.at(TOOL_ACTION::EQUIPMENT);
					if (attribute.starts_with(effect))
					{
						std::string found = attribute.substr(effect.length());

						total += std::stof(found);
					}
				}
			}
		}

		return total;
	}
}