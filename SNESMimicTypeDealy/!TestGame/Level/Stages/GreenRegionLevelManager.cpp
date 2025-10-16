#include "GreenRegionLevelManager.h"

#include "../../../Objects/Helpers/Interactable.h"
#include "../../../Objects/Tags/ObjectTagManager.h"
#include "../../Cutscenes/DialogueManager.h"

#include "../../../Utils/Time/GameTime.h"

#include "../../Objects/Enemies/Types/FeralRabbit.h"

#include "../../Objects/Environment/Effects/Explosion.h"

#include "../../../MusicSync/MusicSync.h"

#define HOUSE_EXPLOSION_TRIGGER_TAG "HouseExplosionTrigger"
#define HOUSE_EXPLOSION_DIALOGUE "HouseExplode"
#define RABBIT_TRIGGER_TAG "SleepingRabbitTrigger"
#define RABBIT_DIALOGUE "SeeRabbit"
#define RABBIT_IDENTIFIED_DIALOGUE "IdentifyRabbit"
#define EVIL_RABBIT_TAG "SleepingRabbit"

#define ZAP_EFFECT_TAG "ElectricalBoxZap"
#define ZAP_BOX_TAG "ElectricalBox"

#define SPRITE_RABBIT_AWAKE 89

#define HOUSE_EXPLOSION_POSITION Vec2{82.f, 41.f}

namespace GAME_NAME::Level
{
	//Used when tracking what stage of the first encounter with the rabbit the player is at.
	uint8_t GreenRegionLevelManager_rabbitSeen = 0;
	double GreenRegionLevelManager_rabbitTimer = 0;
	bool GreenRegionnLevelManager_effectFlag = false;

	class RabbitTransformTrigger
		: public Interactable
	{
	public:
		RabbitTransformTrigger(GameObject* objRef)
			: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, objRef->GetScale().X, objRef->GetPosition(), objRef->GetScale(), objRef->GetSprite())
		{

		}

	protected:
		void onInteract(std::shared_ptr<Objects::Player::Player> player, InputManager::KEY_STATE state) override
		{
			Cutscenes::DialogueManager::INSTANCE->PlayDialogueSequence(Cutscenes::DialogueManager::INSTANCE->GetDialogueSequence(RABBIT_DIALOGUE));
			GreenRegionLevelManager_rabbitSeen = 1;

			Renderer::DestroyObject(this);
		}
	};

	class DialougeEnterTrigger
		: public Interactable
	{
	public:
		DialougeEnterTrigger(GameObject* objRef, std::string dialogueSource)
			: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, objRef->GetScale().X, objRef->GetPosition(), objRef->GetScale(), objRef->GetSprite()), m_dialogueSource(dialogueSource)
		{

		}

	protected:
		void onInteract(std::shared_ptr<Objects::Player::Player> player, InputManager::KEY_STATE state) override
		{
			Cutscenes::DialogueManager::INSTANCE->PlayDialogueSequence(Cutscenes::DialogueManager::INSTANCE->GetDialogueSequence(m_dialogueSource));

			Renderer::DestroyObject(this);
		}

	private:
		const std::string m_dialogueSource;
	};

	class HouseExplosionTrigger
		: public Interactable
	{
	public:
		HouseExplosionTrigger(GameObject* objRef)
			: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, objRef->GetScale().X, objRef->GetPosition(), objRef->GetScale(), objRef->GetSprite())
		{

		}

	protected:
		void onInteract(std::shared_ptr<Objects::Player::Player> player, InputManager::KEY_STATE state) override
		{
			//Explode house.
			Explosion* explosion = new Explosion(HOUSE_EXPLOSION_POSITION, 50.f, 35.f);
			Renderer::LoadActiveObject(explosion);

			GAME_NAME::Camera::GameCamera* gc = dynamic_cast<GAME_NAME::Camera::GameCamera*>(TestGame::INSTANCE->GetCamera());
			gc->ScreenShake(2.6f, 1.5);

			Cutscenes::DialogueManager::INSTANCE->PlayDialogueSequence(Cutscenes::DialogueManager::INSTANCE->GetDialogueSequence(HOUSE_EXPLOSION_DIALOGUE));

			Renderer::DestroyObject(this);
		}

	};

	HouseExplosionTrigger* GreenRegionLevelManager_houseExplosionTrigger = nullptr;
	bool GreenRegionLevelManager_createdExplosionTrigger = false;


	GreenRegionLevelManager::GreenRegionLevelManager()
	{
		GreenRegionLevelManager_houseExplosionTrigger = nullptr;
		GreenRegionLevelManager_createdExplosionTrigger = false;

		MusicSync::MusicSync::SetBPM(120);
		MusicSync::MusicSync::SetBPMeasure(4);
	}

	constexpr const char* dialogueTriggers[3] = {
		"NeighborHouse",
		"TreeShakeTrigger",
		"ImpossibleJump"
	};

	void GreenRegionLevelManager::Update(GLFWwindow* window)
	{
		if (!Cutscenes::DialogueManager::INSTANCE->IsSequencePlaying())
		{
			if (GreenRegionLevelManager_rabbitSeen == 1)
			{
				GameObject* rabbit = Objects::Tags::ObjectTagManager::GetObjectWithTag(EVIL_RABBIT_TAG);

				if (rabbit != nullptr)
				{
					rabbit->SetSprite(Renderer::GetSprite(SPRITE_RABBIT_AWAKE));
					GreenRegionLevelManager_rabbitSeen++;
					Cutscenes::DialogueManager::INSTANCE->PlayDialogueSequence(Cutscenes::DialogueManager::INSTANCE->GetDialogueSequence(RABBIT_IDENTIFIED_DIALOGUE));
					TestGame::ThePlayer->SetFrozen(true);
				}
			}
			else if (GreenRegionLevelManager_rabbitSeen == 2)
			{
				GreenRegionLevelManager_rabbitTimer += Utils::Time::GameTime::GetScaledDeltaTime();

				if (GreenRegionLevelManager_rabbitTimer < 3.0)
				{
					GameObject* zapBox = Objects::Tags::ObjectTagManager::GetObjectWithTag(ZAP_BOX_TAG);
					if (zapBox != nullptr)
					{
						//Jiggle box up and down.
						if (GreenRegionnLevelManager_effectFlag)
						{
							zapBox->Translate(Vec2{ 0, -1 });
						}
						else {
							zapBox->Translate(Vec2{ 0, 1 });
						}

						GreenRegionnLevelManager_effectFlag = !GreenRegionnLevelManager_effectFlag;

						//TODO: PLAY ZAP SOUND
					}
				}
				else if (GreenRegionLevelManager_rabbitTimer > 3.0 && GreenRegionLevelManager_rabbitTimer < 7.0)
				{
					//Display zap effect.
					GameObject* zapEffectGraphic = Objects::Tags::ObjectTagManager::GetObjectWithTag(ZAP_EFFECT_TAG);
					if (zapEffectGraphic != nullptr)
					{
						zapEffectGraphic->SetScale(Vec2{ 40, -3 });

						//Jiggle zap effect up and down.
						if (GreenRegionnLevelManager_effectFlag)
						{
							zapEffectGraphic->Translate(Vec2{ 0, -2 });
						}
						else {
							zapEffectGraphic->Translate(Vec2{ 0, 2 });
						}

						GreenRegionnLevelManager_effectFlag = !GreenRegionnLevelManager_effectFlag;

						if (GreenRegionLevelManager_rabbitTimer > 6.0)
						{
							GreenRegionnLevelManager_effectFlag = false;
							GreenRegionLevelManager_rabbitTimer = 7.0;
						}
					}

				}
				else if (GreenRegionLevelManager_rabbitTimer > 7.0 && GreenRegionLevelManager_rabbitTimer < 8.0)
				{
					GameObject* zapEffectGraphic = Objects::Tags::ObjectTagManager::GetObjectWithTag(ZAP_EFFECT_TAG);
					if (zapEffectGraphic != nullptr)
					{
						zapEffectGraphic->SetScale(0);
						Renderer::DestroyObject(zapEffectGraphic);
					}

					GameObject* rabbit = Objects::Tags::ObjectTagManager::GetObjectWithTag(EVIL_RABBIT_TAG);
					if (rabbit)
					{
						TestGame::ThePlayer->SetFrozen(false);

						Objects::Enemies::FeralRabbit* enemy = new Objects::Enemies::FeralRabbit(rabbit->GetPosition(), rabbit->GetScale(), Renderer::GetSprite(92), 93);
						Renderer::LoadActiveObject(enemy, 2);
						Renderer::DestroyObject(rabbit);

						GreenRegionLevelManager_rabbitTimer = 9.0;
					}
				}

			}
		}

		if (!GreenRegionLevelManager_createdExplosionTrigger)
		{
			GameObject* trigger = Objects::Tags::ObjectTagManager::GetObjectWithTag(HOUSE_EXPLOSION_TRIGGER_TAG);
			if (trigger != nullptr)
			{
				GreenRegionLevelManager_houseExplosionTrigger = new HouseExplosionTrigger(trigger);
				Renderer::LoadObject(GreenRegionLevelManager_houseExplosionTrigger);
				Renderer::DestroyObject(trigger);
			}

			trigger = Objects::Tags::ObjectTagManager::GetObjectWithTag(RABBIT_TRIGGER_TAG);
			if (trigger != nullptr)
			{
				RabbitTransformTrigger* rtt = new RabbitTransformTrigger(trigger);
				Renderer::LoadObject(rtt);
				Renderer::DestroyObject(trigger);
			}

			for (std::string triggerTag : dialogueTriggers)
			{
				GameObject* triggerObject = Objects::Tags::ObjectTagManager::GetObjectWithTag(triggerTag);
				if (triggerObject != nullptr)
				{
					Renderer::LoadObject(new DialougeEnterTrigger(triggerObject, triggerTag));
					Renderer::DestroyObject(triggerObject);
				}
			}

			GreenRegionLevelManager_createdExplosionTrigger = true;
		}
	}

}