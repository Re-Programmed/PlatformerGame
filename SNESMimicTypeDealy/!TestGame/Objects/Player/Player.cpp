#include "Player.h"
#include <memory>
#if _DEBUG
#include "../../../Debug/DebugLog.h"

#define PlayerLogger(x) (DEBUG::DebugLog::Log(x, true, ";34;44;1"))

#include "../../../Debug/LevelBuilder/LevelBuilder.h"
#endif

#include "../../../Utils/Time/GameTime.h"
#include "../../../Input/InputManager.h"
#include "../../../Rendering/DynamicSprite.h"
#include "../../../Objects/GUI/Menus/GUIMenu.h"

#include "../../../Objects/Instantiate/LevelObjectHandler.h"

#include "../../TestGame.h"

#include "../../Items/FloorItem.h"

#include "../../../Utils/Math/VMath.h"

#include "../../Items/Types/Weapon.h"
#include "../../Items/Types/Food.h"

#include "../Environment/Effects/BloodMark.h"

#include "../Enemies/Enemy.h"

#include <thread>
#include "../Environment/Effects/Explosion.h"

#include "../../DeathEventManager.h"

#include "../../../Utils/CollisionDetection.h"
#include "../../Items/Types/Placeable.h"
#include "../../Items/Types/Blueprint.h"

#include "../../Objects/Furniture.h"
#include "../../Level/Hub/HouseManager.h"

#include "../Projectile.h"

#define PLAYER_ROOM_SPEED 40	//How fast the player moves vertically in room control mode.

#define PLAYER_ANIMATION_RUN_WALK_SWITCH 142.f //When the player should switch from the walking to running animation.

#define PLAYER_DAMAGE_ANIMATION_LENGTH 0.75f //How long the damage timer ticks for after the player is attacked.

#define PLAYER_ARMOR_REPLENISH_RATE 1.5f

namespace  GAME_NAME
{
	namespace  Objects
	{
		namespace Player
		{


///Used to define a texture variable in whatever the 
#define TextureDataBase(base)\
				m_textureData.base


#define PLAYER_NO_HEAD_SPRITE TextureDataBase(BagTurnaround) - 2
#define PLAYER_LOOK_BAG TextureDataBase(BagTurnaround) - 1
#define PLAYER_LOOK_BEHIND_SPRITE TextureDataBase(BagTurnaround)
#define PLAYER_FALLEN_SPRITE TextureDataBase(Fall)

#define PLAYER_HOLD_BALLOON_SPRITE TextureDataBase(VictoryBalloon)
#define PLAYER_HOLD_CONFETTI_GUN_SPRITE TextureDataBase(VictoryBalloon) + 1

#define PLAYER_DEAD_SPRITE TextureDataBase(Dead)

#define PLAYER_UNINTENTIONAL_DIVE_SPRITE TextureDataBase(UnintentionalFlying)

#define DefaultPlayerSprite TextureDataBase(DefaultSprites)		//The default sprite to use for the player.

#define PlayerWalkAnim {																																													\
	DefaultPlayerSprite + 1, DefaultPlayerSprite + 2, DefaultPlayerSprite + 3, DefaultPlayerSprite + 2, DefaultPlayerSprite + 1, DefaultPlayerSprite + 4, DefaultPlayerSprite + 5, DefaultPlayerSprite + 4	\
}

#define PlayerRunAnim {																																														\
	DefaultPlayerSprite + 6,DefaultPlayerSprite + 7,DefaultPlayerSprite + 8,DefaultPlayerSprite + 9,DefaultPlayerSprite + 10,DefaultPlayerSprite + 11,DefaultPlayerSprite + 12,DefaultPlayerSprite + 13		\
}

#define PlayerJumpAnim {																																			\
	DefaultPlayerSprite + 14,DefaultPlayerSprite + 15,DefaultPlayerSprite + 16,DefaultPlayerSprite + 17																\
}

#define PlayerFallAnim {																																			\
	DefaultPlayerSprite + 18,DefaultPlayerSprite + 19,DefaultPlayerSprite + 20,DefaultPlayerSprite + 21																\
}

#define PlayerSkidAnim {																																			\
	DefaultPlayerSprite + 22,DefaultPlayerSprite + 23,DefaultPlayerSprite + 24,DefaultPlayerSprite + 25																\
}

#define PlayerFallOverAnim {																															\
	PLAYER_FALLEN_SPRITE + 1, PLAYER_FALLEN_SPRITE + 2, PLAYER_FALLEN_SPRITE + 3, PLAYER_FALLEN_SPRITE													\
}

#define PlayerGetUpAnim {																																\
	PLAYER_FALLEN_SPRITE + 4, PLAYER_FALLEN_SPRITE + 5, PLAYER_FALLEN_SPRITE + 6, PLAYER_FALLEN_SPRITE + 7												\
}

#define PlayerBasicAttackAnim {																																																																						\
	TextureDataBase(BasicAttack), TextureDataBase(BasicAttack) + 1, TextureDataBase(BasicAttack) + 2, TextureDataBase(BasicAttack) + 3, TextureDataBase(BasicAttack) + 4, TextureDataBase(BasicAttack) + 5, TextureDataBase(BasicAttack) + 6, TextureDataBase(BasicAttack) + 7, TextureDataBase(BasicAttack) + 8	\
}

#define PlayerClimbingBehindAnim {																																																										\
	TextureDataBase(Climbing), TextureDataBase(Climbing) + 1, TextureDataBase(Climbing) + 2, TextureDataBase(Climbing) + 3, TextureDataBase(Climbing) + 4, TextureDataBase(Climbing) + 5, TextureDataBase(Climbing) + 6, TextureDataBase(Climbing) + 7					\
}

#define PLAYER_SIT_TEXTURE TextureDataBase(Climbing) + 8

#define PlayerSittingPuffAnim {																																						\
	TextureDataBase(Climbing) + 9, TextureDataBase(Climbing) + 10, TextureDataBase(Climbing) + 11, TextureDataBase(Climbing) + 12, TextureDataBase(Climbing) + 13					\
}

#define PlayerIdleTapToe {																																		\
	TextureDataBase(IdleAnimations), TextureDataBase(IdleAnimations) + 1, TextureDataBase(IdleAnimations) + 2, TextureDataBase(IdleAnimations) + 1				\
}

#define PlayerIdleStomp {																																		\
	TextureDataBase(IdleAnimations) + 3, TextureDataBase(IdleAnimations) + 4, TextureDataBase(IdleAnimations) + 5, TextureDataBase(IdleAnimations) + 6, TextureDataBase(IdleAnimations) + 7, TextureDataBase(IdleAnimations) + 8, TextureDataBase(IdleAnimations) + 8				\
}

#define PlayerBikeAnim {											\
	TextureDataBase(RidingBike), TextureDataBase(RidingBike + 1)	\
}

#pragma region LowqAnimations

#pragma endregion

			constexpr double PLAYER_CLIMBING_SPEED = 40;

			double Player_AttackingTracker = 0.0;

			typedef int8_t PlayerEmotion;
			
			enum PLAYER_EMOTIONS : PlayerEmotion
			{
				ANGRY = SpriteBase(26)
			};

			using namespace Utils;

			//TODO: Pixel Birb sitting animation is glitched because of weird offset issues from climbing sprites :(
			const Player::PlayerTextureData Player::TextureData[3] = {
				Player::PlayerTextureData(), //Default Sprites (0)
				Player::PlayerTextureData(SpriteBase(191), SpriteBase(217), SpriteBase(204), SpriteBase(212), SpriteBase(219), SpriteBase(225), 0 /*Missing Dead*/, 0 /*Missing Victory Balloon*/, 0 /*Missing Riding Bike*/, 0 /*Missing unintentional flying*/, new Player::PlayerAnimationData(	//Pixel Birb (1)
					Player::AnimationOverride(new int[4] { 1, 2, 1, 3 }, 4, ANIM_6_SPF * 1.33f),		//Walking 
					Player::AnimationOverride(new int[4] { 4, 5, 6, 7 }, 4, ANIM_6_SPF),				//Running
					Player::AnimationOverride(new int[2] { 8, 9 }, 2, ANIM_6_SPF),						//Jumping
					Player::AnimationOverride(new int[2] { 10, 11 }, 2, ANIM_6_SPF),					//Falling
					Player::AnimationOverride(new int[1] { 12 }, 1, ANIM_12_SPF),						//Skidding (changing direction)
					Player::AnimationOverride(new int[4] { 1, 2, 3, 0 }, 4, ANIM_6_SPF),				//Fall over
					Player::AnimationOverride(new int[4] { 0, 4, 5, 6 }, 4, ANIM_6_SPF),				//Get up
					Player::AnimationOverride(new int[4] { 0, 1, 2, 3 }, 4, ANIM_12_SPF * 1.5f),		//Basic attack
					Player::AnimationOverride(new int[4] { 0, 1, 0, 2 }, 4, ANIM_6_SPF),				//Climbing back
					Player::AnimationOverride(new int[6] { 4, 5, 4, 6, 4, 7 }, 6, ANIM_6_SPF * 1.5f),	//Sitting idle
					Player::AnimationOverride(new int[2] { 0, 1 }, 2, ANIM_12_SPF * 1.5f),				//Idle 1
					Player::AnimationOverride(new int[2] { 2, 3 }, 2, ANIM_12_SPF * 1.5f),				//Idle 2
					Player::AnimationOverride(0, 0)														//No Biking Animation
				)),
				Player::PlayerTextureData(SpriteBase(299), SpriteBase(326), SpriteBase(327), SpriteBase(335), SpriteBase(344), SpriteBase(357), SpriteBase(369), SpriteBase(368), SpriteBase(370), SpriteBase(367))
			};

			Player::Player(Vec2 position, bool loadFromSavedPosition)
				: ActiveBoxCollisionGravityObject(position, Vec2(DefaultPlayerScaleX, DefaultPlayerScaleY), NULL), m_screenInventory(new ScreenInventory()),
				m_heldItemLastSprite(nullptr),
				m_playerLight(nullptr),
				m_healthProgressBar(new ProgressBar(
					Vec2(30, 18), Vec2(32, -3), Renderer::GetSprite(SpriteBase(72))->GetSpriteId()
				)),
				m_armorProgressBar(new StaticGUIElement(
					Vec2(30, 18), Vec2(32, -3), Renderer::GetSprite(SpriteBase(382))->GetSpriteId()
				)),
				m_abilityMeterProgressBar(new ProgressBar(
					Vec2(20, 12), Vec2(46, -3), Renderer::GetSprite(SpriteBase(71))->GetSpriteId()
				)),
				MiscStateGroup("pl"), m_saveState(new PlayerSaveState(this)),
				m_particleEmitter(new Particles::ParticleEmitter(position)),
				m_backpack(new Backpack(18)),		//Create default backpack (load save in constructor).
				m_skillHolder({ 62.f, 7.f }),		//Skill holder (manages update and display of current skill and equipment effects)
				m_textureData(TextureData[0]),
				IsAlive(true),
				m_heldItemDisplayOffset({ 0.f, 0.f }, { 0.f, 0.f })
			{

				//Change to load based on what sprite the player should be using from the selected kit.
				SetSprite(Rendering::Renderer::GetSprite(DefaultPlayerSprite));
#if _DEBUG 
				PlayerLogger("Initilized Player");
#endif
				m_particleEmitter->SetScale(m_scale);

				//INIT WATER MUSIC
				//Audio::SoundManager::WaterMusic = Audio::SoundManager::Play(Audio::UnderwaterMusicID, Audio::SoundManager::BGMusic, -1.0F, 0.0F, true);

				//Load the stat overlay menu that contains sprites for health and inventory.
				GUI::Menus::GUIMenu::LoadMenu("/stat_overlay", nullptr);
				Renderer::LoadGUIElement(m_healthProgressBar, 0);
				Renderer::LoadGUIElement(m_abilityMeterProgressBar, 0);
				Renderer::LoadGUIElement(m_armorProgressBar, 0);

				//Register animations
				registerAnimations();

				m_physics->SetGravityStrength(DefaultPlayerGravity);

				m_emotionsObject = nullptr;
				//Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_emotionsObject, true, 2, false));

				m_heldItemDisplay = nullptr;

				std::shared_ptr<std::vector<std::string>> states = this->getStates();

				if (loadFromSavedPosition)
				{
					if (states->size() > 0)
					{
						decodeSave(states->at(0));
					}
				}

				assignState(m_saveState);
				
				//TEMP TESTING TODO:REMOVE
				//m_skillHolder.UnlockSkill(FLIGHT);

				//Test item.
				//m_screenInventory->AddItem(new InventoryItem(WOODEN_SHOES));

				//Force held item display to show up.
				SetHeldItem(this->m_screenInventory->GetHeldItem());


				//TODO: TESTING
				//SetPlayerTextureData(DEFAULT_FOX);
				//m_scaleMultiplier = Vec2{ 1.625f, 1.f };
				//m_heldItemDisplayOffset = { Vec2{9.f, 0.f}, Vec2{ 0.f, 0.f } };
			}

			Player::~Player()
			{
				ActiveBoxCollisionGravityObject::~ActiveBoxCollisionGravityObject();
				delete m_animator;
				delete m_saveState;
				delete m_heldItemDisplay;
			}

			
			int renderCalls = 0;
			float tAlloc = 0;
			float m_curr = 0;
			void Player::Update(GLFWwindow* window)
			{
				//Death by falling out of level.
				if (m_position.Y < -80.f && IsAlive)
				{
					this->SetFrozen(true);
					this->m_physics->SetGravitationalVelocity(0.f);
					this->m_physics->SetVelocityY(310.f);
					//Death sound.

					DeathEventManager::ShowDeathScreen(DeathEventManager::FALLEN);
					SetAnimationState(DEAD);

					IsAlive = false;
				}

				std::thread playerInput([this] {  std::srand(time(0)) /*Must reseed because thread dosen't carry over.*/; readKeys(); });

				std::thread animationUpdate([this, window] { m_animator->Update(window, this); });

				float totalArmorEffect = m_skillHolder.GetEquipmentBoostEffect(HEALTH_EQUIPMENT_BOOST_EFFECT, m_backpack);

				//If the armor stat is too high, replenish it until it reaches the desired value over time.
				if (m_stats.Armor < totalArmorEffect)
				{
					setArmor(m_stats.Armor + static_cast<float>(Utils::Time::GameTime::DeltaTime::GetDeltaTime()) * PLAYER_ARMOR_REPLENISH_RATE);
				}
				else if (m_stats.Armor > totalArmorEffect)
				{
					setArmor(totalArmorEffect);
				}

				float yOffset = 0;
				//Increment timers for all attributes.
				for (auto playerAttrib : m_currentAttribModifiers)
				{
					playerAttrib.second -= Time::GameTime::GetScaledDeltaTime();
					if (playerAttrib.second < 0)
					{
						m_currentAttribModifiers.erase(playerAttrib.first);
						break;
					}

					/*
						FIX: DO THESE SHOW UP?
						WHAT GIVES YOU ATTRIBUTES. POISON, FOOD, ETC
					*/

					auto attribModSprite = Renderer::GetSprite(4/*Change to show attrib modifier.*/);
					StaticGUIElement* attribModElement = new StaticGUIElement(Vec2{ 112, 12 + yOffset }, Vec2{ 16, 16 }, attribModSprite->GetSpriteId());

					attribModElement->Render(TestGame::INSTANCE->GetCamera()->GetZoom());

					delete attribModElement;
					yOffset += 20;
				}

				//Check if the player is trying to use an item and use it if so.
				if (m_screenInventory->GetHeldItem() != nullptr && InputManager::GetKey(keyRef::PLAYER_USE_ITEM) && !(m_frozen > 0) && !this->GetBackpack()->GetIsOpen())
				{
					//??? What is this doing? Attacking is handled on right click... 
					if (ITEM_DATA[m_screenInventory->GetHeldItem()->GetType()].Actions & WEAPON)
					{
						if (Weapon* w = dynamic_cast<Weapon*>(m_screenInventory->GetHeldItem()))
						{
							w->Use();
							Attack(w->GetDamage(), /*TODO: Give weapons a range attribute*/12.f, 0);
						}
					}
					else if (ITEM_DATA[m_screenInventory->GetHeldItem()->GetType()].Actions & FOOD)
					{
						if (Food* f = dynamic_cast<Food*>(m_screenInventory->GetHeldItem()))
						{
							if (f->Use())
							{
								m_screenInventory->SetItem(m_screenInventory->GetSelectedSlot() - 1, nullptr);
							}
						}
					}
					else if (Blueprint* bp = dynamic_cast<Blueprint*>(m_screenInventory->GetHeldItem()))
					{
						bp->Use();
						m_screenInventory->SetItem(m_screenInventory->GetSelectedSlot() - 1, nullptr);
					}

				}

				//Handle if the player is trying to attack an enemy.
				handleAttack();
				m_skillHolder.Update();

				if (m_frozenTimer > 0.f)
				{
					m_frozenTimer -= Utils::Time::GameTime::GetScaledDeltaTime();

					if (m_frozenTimer < 0.f)
					{
						SetFrozen(false);
						//delete m_sprite; NO CAUSES ERROR (DELETING ANIMATION SPRITES)
						m_sprite.reset(Renderer::GetSprite(SpriteBase(0)));

						m_rotation = 0.f;
					}
				}

				//Update the current backpack if it is open.
				if (m_backpack->GetIsOpen()) { m_backpack->Render(); }
				else { m_backpack->UpdateCursorItemDisplay(); }

				//This makes the current held item move up and down while running to look like it is moving around in the player's hand.
				if (m_heldItemDisplay != nullptr && m_heldItemDisplay->GetScale().X != 0)
				{
					if (m_heldItemDisplay->GetScale().X < 8)
					{
						float add = std::sin(m_position.X / 8.f) * 1.44f;
						m_heldItemDisplay->SetPosition(m_position + Vec2(2, 3 + (add)));
					}

				}

				//Calculate time spent in air.
				if (!m_onGround)
				{
					m_airTime += Time::GameTime::GetScaledDeltaTime();
				}

				//Update the health bar display.
				m_healthProgressBar->SetPercentage(static_cast<char>(std::clamp(m_stats.Health, 0.f, 100.f)));

				//Update the position of the armor bar.
				Vec2 armorProgressPosition = m_healthProgressBar->GetPosition();
				armorProgressPosition.X += m_healthProgressBar->GetScale().X * (m_healthProgressBar->GetDisplayPercentage() / 100.f);
				m_armorProgressBar->SetPosition(Vec2{ armorProgressPosition.X - m_armorProgressBar->GetScale().X, armorProgressPosition.Y });

				if (m_playerLight != nullptr)
				{
					m_playerLight->SetPosition(m_position + ((m_scale * m_scaleMultiplier) / 2.f));
				}

#if _DEBUG
				if (!m_flight)
				{
					CollisionManager::RegisterActiveColliderToBuffer(m_boxCollider);
				}
#else
				CollisionManager::RegisterActiveColliderToBuffer(m_boxCollider);
#endif


				if (playerInput.joinable())
				{
					playerInput.join();
				}

				if (animationUpdate.joinable())
				{
					animationUpdate.join();
				}


				/*
				Testing, implement something kinda like this but for looking at interesting objects.
				*/
				/*
				if (m_currentPlayerLookDirection == NO_LOOK_DIRECTION || m_currentPlayerLookDirection == FOLLOW_MOUSE)
				{
					if (InputManager::GetMouseButton(1))
					{
						m_currentPlayerLookDirection = FOLLOW_MOUSE;
					}
					else {
						m_currentPlayerLookDirection = NO_LOOK_DIRECTION;
					}
				}
				*/

				if (m_screenInventory->GetHeldItem() != nullptr)
				{
					Placeable* p = dynamic_cast<Placeable*>(m_screenInventory->GetHeldItem());
					if (p != nullptr)
					{
						if (InputManager::GetKeyUpDown(PLAYER_INTERACT) & InputManager::KEY_STATE_HELD && !m_feetOnlyCollision)
						{
							Placeable::InteractionTimer += Time::GameTime::DeltaTime::GetDeltaTime();

							if (Placeable::InteractionTimer > 1.0)
							{
								//Remove held item and place the placeable.
								p->Place(m_position + m_scale/2.f);
								Placeable::InteractionTimer = 0.0;
								
								this->m_screenInventory->SetItem(this->m_screenInventory->GetSelectedSlot() - 1, nullptr);
								return;
							}
						}
						else {
							Placeable::InteractionTimer = 0.0;
						}
					}

					ITEM_TYPE heldType = m_screenInventory->GetHeldItem()->GetType();
					if (ITEMTYPE_GetItemData(heldType).Actions & FURNITURE)
					{
						if (InputManager::GetKeyUpDown(PLAYER_INTERACT) & InputManager::KEY_STATE_HELD)
						{
							Furniture::InteractionTimer += Time::GameTime::DeltaTime::GetDeltaTime();

							if (Furniture::InteractionTimer > 1.0)
							{
								Furniture::InteractionTimer = 0.0;

								Furniture* f = new Furniture(m_textureFlipped ? (m_position + Vec2{ FURNITURE_PLACE_OFFSET, m_scale.Y / 2.f }) : (m_position + Vec2{ -FURNITURE_PLACE_OFFSET, m_scale.Y / 2.f }), m_textureFlipped, new InventoryItem(m_screenInventory->GetHeldItem()->GetType()));
								f->Translate(Vec2{ m_textureFlipped ? (1.f - f->GetScale().X * 2.f) : -f->GetScale().X, -2.5f});

								if (!HouseManager::CheckValidPlaceLocation(f->GetPosition(), f->GetScale()))
								{
									delete f;
								}
								else {
									Renderer::InstantiateObject(Renderer::InstantiateGameObject(f, true, 1, false));
									this->m_screenInventory->SetItem(this->m_screenInventory->GetSelectedSlot() - 1, nullptr);
									delete m_screenInventory->GetHeldItem();

									HouseManager::PlaceFurniture(f);
								}
							}
						}
						else {
							Furniture::InteractionTimer = 0.0;
						}
					}
				}

				updateAnimationState();

				m_screenInventory->Update();

			}

			void Player::SetSwimming(bool swimming)
			{
				m_swimming = swimming;
				if (m_swimming)
				{
					m_physics->SetVelocity(m_physics->GetVelocity() / 1.5f);
					m_physics->SetGravitationalVelocity(m_physics->GetGravitationalVelocity() / 1.3f);
					m_physics->SetGravityStrength(SwimmingPlayerGravity);
					m_physics->SetTerminalVelocity(0.04f);
					enableUnderwaterMusic();
				}
				else {
					m_physics->SetGravityStrength(DefaultPlayerGravity);
					m_physics->SetTerminalVelocity(240.f);
					m_physics->SetVelocity(m_physics->GetVelocity() / 1.5f);
					disableUnderwaterMusic();
				}
			}


			void Player::Render(const Vec2& cameraPosition)
			{
				m_didRender = true;
#if _DEBUG
				if (m_debug)
				{
					Debug::LevelBuilder::LevelBuilder::Render();
				}
#endif

				//Rendering disabled, just ignore everything.
				if (!m_enableRendering)
				{
					return;
				}

				Vec4 playerColorVerts[4] = {
					{1, 1, 1, 1},
					{1, 1, 1, 1},
					{1, 1, 1, 1},
					{1, 1, 1, 1}
				};
				bool playerColorVertsModified = false;

				if (m_damageAnimationTimer > 0.f)
				{
					m_damageAnimationTimer -= Utils::Time::GameTime::GetScaledDeltaTime();
					if (m_damageAnimationTimer < (PLAYER_DAMAGE_ANIMATION_LENGTH / 3.f) || m_damageAnimationTimer > PLAYER_DAMAGE_ANIMATION_LENGTH * (2.f / 3.f))
					{
						for (uint8_t i = 0; i < 4; i++)
						{
							playerColorVerts[i].Y = 0.25f;
							playerColorVerts[i].Z = 0.25f;
						}
						playerColorVertsModified = true;

					}
				}

				

				if (m_swimming)
				{
					const float sin = (std::sinf((float)glfwGetTime()) / 20.f);
					const float sin1 = (std::sinf((float)glfwGetTime() * 1.1f) / 20.f);
					const float sin2 = (std::sinf((float)glfwGetTime() * 1.2f) / 20.f);
					const float sin3 = (std::sinf((float)glfwGetTime() * 1.3f) / 20.f);

					Vec2 vertices[4]{
						Vec2(0.f + sin, 0.f),
						Vec2(0.f + sin1, 1.f),
						Vec2(1.f + sin2, 1.f),
						Vec2(1.f + sin3, 0.f)
					};
					
					DynamicSprite d(m_sprite->GetSpriteId(), vertices);
					d.UpdateTextureColor(playerColorVerts);
					d.Render(cameraPosition, m_position + (m_scale * m_scaleMultiplier), (m_scale * m_scaleMultiplier), m_rotation + 180.f);
					return;
				}
				else {
					if (m_sprite != nullptr)
					{
						//The player sprite must be shifted down if the dead sprite is showing.
						if (m_currentPlayerLookDirection == DEAD)
						{
							m_position -= Vec2{ 0.f, 4.f };
						}

						//Check if the player's color should update.
						if (playerColorVertsModified)
						{
							DynamicSprite* coloredSprite = new DynamicSprite(m_sprite->GetSpriteId());
							coloredSprite->UpdateTextureColor(playerColorVerts);
							coloredSprite->Render(cameraPosition, m_position + (m_scale * m_scaleMultiplier) + (m_textureFlipped ? ((m_scale * m_scaleMultiplier) * Vec2::MinusOneX) : 0), (m_scale * m_scaleMultiplier) * (m_textureFlipped ? Vec2::MinusOneXOneY : 1), m_rotation + 180.f);
							delete coloredSprite;
						}
						else {
							//DEFAULT RENDER MODE.
							
							m_sprite->Render(cameraPosition, m_position + (m_textureFlipped ? ((m_scale * m_scaleMultiplier) * Vec2::OneX) : 0), (m_scale * m_scaleMultiplier) * (m_textureFlipped ? Vec2::MinusOneXOneY : 1), m_rotation);
						}

						//The player sprite must be shifted down if the dead sprite is showing.
						if (m_currentPlayerLookDirection == DEAD)
						{
							m_position += Vec2{ 0.f, 4.f };
						}
					}
					//Render emotions object if it should be rendered.
					if (m_emotionsObject != nullptr)
					{
						m_emotionsObject->Render(cameraPosition);
					}
				}

				/*
					Held item rendering.
				*/

				if (m_screenInventory->GetHeldItem() != nullptr)
				{
					ITEM_TYPE heldItemType = m_screenInventory->GetHeldItem()->GetType();

					Placeable* p = dynamic_cast<Placeable*>(m_screenInventory->GetHeldItem());
					if (p != nullptr)
					{
						Vec2 placePos = m_position + Vec2(0.f, m_scale.Y / 2.f);
						p->RenderPreview(cameraPosition, p->GetPotentialPlaceLocation(placePos));
						return;
					}

					if (ITEMTYPE_GetItemData(heldItemType).Actions & FURNITURE)
					{
						Furniture::RenderPreview(cameraPosition, heldItemType, !m_textureFlipped ? (m_position - Vec2{ FURNITURE_PLACE_OFFSET, -m_scale.Y / 2.f }) : (m_position + Vec2{ FURNITURE_PLACE_OFFSET + m_scale.X, m_scale.Y / 2.f }), m_textureFlipped);
					}
					else if (ITEMTYPE_GetItemData(heldItemType).Actions & RANGED_WEAPON)
					{
						if (m_projectileCharge > 0.0)
						{
							if (m_projectileLaunchProgress == nullptr)
							{
								Sprite* sprite = Renderer::GetSprite(SpriteBase(42) /*Red texture.*/);
								m_projectileLaunchProgress = new ProgressBar(Vec2{0.f}, Vec2{ m_scale.X, 5.f }, sprite->GetSpriteId());
								delete sprite;
							}

							m_projectileLaunchProgress->SetPosition(TestGame::INSTANCE->GetCamera()->GlobalToUI(m_position + Vec2{ 0.f, m_scale.Y + 5.f }));
							m_projectileLaunchProgress->SetPercentage(m_projectileCharge * 100, true);
							m_projectileLaunchProgress->Render(TestGame::INSTANCE->GetCamera()->GetZoom());
						}
					}


					if (m_heldItemDisplay != nullptr && m_heldItemDisplay->GetScale().X > 8)
					{
						int&& baseSpriteId = Items::ITEM_DATA[heldItemType].HeldTexture;

						//If true, don't update the texture of the held item.
						const bool noAnimatingTexture = baseSpriteId == GLOBAL_SPRITE_BASE - 1;
						if (noAnimatingTexture) { baseSpriteId = Items::ITEM_DATA[heldItemType].Texture; }

						//If the player is currently attacking, the item will be rendered outward from their body.
						if (m_animator->GetCurrentAnimationIndex() == 7 /*Basic Attack Anim*/)
						{
							const int frame = 0;
							m_heldItemLastSprite = std::shared_ptr<Sprite>(Renderer::GetSprite(baseSpriteId + frame));
							m_heldItemDisplayFrameOffset = frame;
							m_heldItemDisplay->SetSprite(m_heldItemLastSprite);

							//Offset position is determined by what frame of attack the player is on.
							Vec2 offsetPosition = Vec2::Zero;
							switch (m_animator->GetCurrentAnimation()->GetFrame())
							{
							case 0:
								offsetPosition = { -6.25f, 3.f };
								break;
							case 1:
								offsetPosition = { -5.33f, 2.f };
								break;
							case 2:
								offsetPosition = { -5.f, 1.f };
								break;
							case 3:
								offsetPosition = { -4.75f, 0.f };
								break;
							case 4:
								offsetPosition = { -5.f, -1.f };
								break;
							case 5:
								offsetPosition = { -5.33f, -2.f };
								break;
							case 6:
								offsetPosition = { -6.25f, -3.f };
								break;
							default:
								offsetPosition = { -6.25f, -4.f };
								break;
							}

							if (m_textureFlipped)
							{
							
								m_heldItemDisplay->GetSprite()->Render(cameraPosition, offsetPosition + (m_textureFlipped ? std::get<0>(m_heldItemDisplayOffset) : std::get<1>(m_heldItemDisplayOffset)) + m_position + Vec2{16.f + m_heldItemDisplay->GetScale().X - 7, 0}, m_heldItemDisplay->GetScale()* Vec2 { -1, 1 }, 0.0F);
							}
							else {
								m_heldItemDisplay->GetSprite()->Render(cameraPosition, m_position + (m_textureFlipped ? std::get<0>(m_heldItemDisplayOffset) : std::get<1>(m_heldItemDisplayOffset)) + Vec2{ 0.f, 0.f }, m_heldItemDisplay->GetScale(), 0.0F);
							}
							return;
						}

						const float playerYVel = m_physics->GetVelocity().Y + m_physics->GetGravitationalVelocity();
						if (playerYVel > 1.5f && !m_onGround)
						{
							const int frame = 0;
							m_heldItemLastSprite.reset(Renderer::GetSprite(baseSpriteId + frame));
							m_heldItemDisplayFrameOffset = frame;
							m_heldItemDisplay->SetSprite(m_heldItemLastSprite);

							m_heldItemDisplay->SetPosition(m_position + (m_textureFlipped ? Vec2(6.75f, -0.5f) : Vec2(0, -0.5f)));
						}else if (playerYVel < -1.5f && !m_onGround)
						{
							const int frame = noAnimatingTexture ? 0 : 2;
							m_heldItemLastSprite.reset(Renderer::GetSprite(baseSpriteId + frame));
							m_heldItemDisplayFrameOffset = frame;
							m_heldItemDisplay->SetSprite(m_heldItemLastSprite);

							m_heldItemDisplay->SetPosition(m_position + (m_textureFlipped ? Vec2(2.6f, 7.f) : Vec2(4.5f, 7.f)));
						}
						else if (m_physics->GetVelocity().X > 0.3 || m_physics->GetVelocity().X < -0.3)
						{
							if (m_animator->GetCurrentAnimation() != nullptr)
							{
								int&& frame = noAnimatingTexture ? 0 : m_animator->GetCurrentAnimation()->GetFrame();

								if (!noAnimatingTexture)
								{
									//Determine what frame of item animation to show.
									switch (frame)
									{
									case 0:
										frame = 0;
										break;
									case 1:
										frame = 1;
										break;
									case 2:
										frame = 2;
										break;
									case 3:
										frame = 1;
										break;
									case 4:
										frame = 0;
										break;
									case 5:
										frame = 3;
										break;
									case 6:
										frame = 4;
										break;
									case 7:
										frame = 3;
										break;
									}
								}

								m_heldItemLastSprite = std::shared_ptr<Sprite>(Renderer::GetSprite(baseSpriteId + frame));
								m_heldItemDisplayFrameOffset = frame;
								m_heldItemDisplay->SetSprite(m_heldItemLastSprite);

								m_heldItemDisplay->SetPosition(m_position + (m_textureFlipped ? Vec2(6.75f, -0.5f) : Vec2(0, -0.5f)));

								//Apply a motion effect if no animation is used.
								if (noAnimatingTexture)
								{
									switch (m_animator->GetCurrentAnimation()->GetFrame())
									{
									case 0:
										break;
									case 1:
										m_heldItemDisplay->Translate(Vec2{ -0.5f, 0.5f });
										break;
									case 2:
										m_heldItemDisplay->Translate(Vec2{ -1.f, 1.f });
										break;
									case 3:
										m_heldItemDisplay->Translate(Vec2{ -0.5f, 0.5f });
										break;
									case 4:
										break;
									case 5:
										m_heldItemDisplay->Translate(Vec2{ 0.5f, -0.5f });
										break;
									case 6:
										m_heldItemDisplay->Translate(Vec2{ 1.f, -1.f });
										break;
									case 7:
										m_heldItemDisplay->Translate(Vec2{ 0.5f, -0.5f });
										break;
									}
								}
							}
						}
						else {
							if (m_heldItemDisplayFrameOffset != 0)
							{
								m_heldItemDisplay->SetSprite(new Sprite(m_heldItemDisplay->GetSprite()->GetSpriteId() - m_heldItemDisplayFrameOffset));
								m_heldItemDisplayFrameOffset = 0;
							}
							m_heldItemDisplay->SetPosition(m_position + Vec2(3.5, 0.95f));
						}

						//Account for scale changes.
						m_heldItemDisplay->Translate(Vec2{ 0.f, 16.f - m_heldItemDisplay->GetScale().Y });

						if (m_textureFlipped)
						{
							m_heldItemDisplay->GetSprite()->Render(cameraPosition, m_heldItemDisplay->GetPosition() + (m_textureFlipped ? std::get<0>(m_heldItemDisplayOffset) : std::get<1>(m_heldItemDisplayOffset)) + Vec2{ m_heldItemDisplay->GetScale().X - 7, 0 }, m_heldItemDisplay->GetScale() * Vec2{ -1, 1 }, 0.0F);
						}
						else {
							m_heldItemDisplay->GetSprite()->Render(cameraPosition, m_heldItemDisplay->GetPosition() + (m_textureFlipped ? std::get<0>(m_heldItemDisplayOffset) : std::get<1>(m_heldItemDisplayOffset)), m_heldItemDisplay->GetScale(), 0.0F);
						}
					}

				}

			}

			void Player::SetGlitched(bool glitched)
			{
				//Update to use new sprites.

				if (glitched)
				{
					SetPlayerTextureData(PIXEL_BIRB);
					m_scaleMultiplier = 0.66f;
				}
				else {
					SetPlayerTextureData(DEFAULT_BIRB);
					m_scaleMultiplier = 1.f;
				}

				GlitchableObject::SetGlitched(glitched);
			}

#if _DEBUG
			void Player::EnterDebug()
			{
				m_debug = !m_debug;

				if (m_debug)
				{
					Debug::LevelBuilder::LevelBuilder::InitLevelBuilder(TestGame::INSTANCE);			
				}
				else
				{
					Debug::LevelBuilder::LevelBuilder::DestroyLevelBuilder();
				}
			}

			void Player::ToggleFlight()
			{
				{
					m_flight = !m_flight;

					if (m_flight)
					{
						m_boxCollider->SetBeforeUpdate(nullptr);
						((GAME_NAME::Camera::GameCamera*)TestGame::INSTANCE->GetCamera())->SetFollowPlayerExact(true);
					}
					else
					{
						m_boxCollider->SetBeforeUpdate(beforeUpdate);
						((GAME_NAME::Camera::GameCamera*)TestGame::INSTANCE->GetCamera())->SetFollowPlayerExact(false);
					}
				}
			}

#endif

#define KNOCKBACK_DAMAGE_MULTIPLIER 100.f

			void Player::Damage(float damage, GameObject* cause, bool causeFainting)
			{
				constexpr float maxHealth = 100.f;

				//Damage knockback.
				if (cause != nullptr)
				{
					m_physics->AddVelocity(Vec2{ (cause->GetPosition().X > m_position.X + m_scale.X / 2.f ? -1.f : 1.f) * KNOCKBACK_DAMAGE_MULTIPLIER, 0.f });
				}

				if(damage > 0.f){ CreateBloodParticle(cause, causeFainting); }

				//Damage with the applied damage minus the armor effect.
				float finalDamage = damage - m_stats.Armor;
				if (finalDamage < 0.f) { finalDamage = 0.f; }
				m_stats.Health -= (finalDamage);

				{
					//Remove from the armor applied based on how much damage was taken.
					float newArmor = m_stats.Armor - damage;
					if (newArmor < 0.f) { newArmor = 0.f; }
					setArmor(newArmor);
				}

				//m_healthProgressBar->SetPercentage(static_cast<char>(std::clamp(m_stats.Health, 0.f, 100.f))); This will happen in update.

				m_damageAnimationTimer = PLAYER_DAMAGE_ANIMATION_LENGTH;

				if (m_stats.Health <= 0)
				{
					Kill();
				}
			}

			void Player::Heal(float amount)
			{
				constexpr float maxHealth = 100.f;

				m_stats.Health += maxHealth;

				if (m_stats.Health > maxHealth)
				{
					m_stats.Health = maxHealth;
				}

				//m_healthProgressBar->SetPercentage(static_cast<char>(std::clamp(m_stats.Health, 0.f, 100.f))); This will happen in update.

			}

			void Player::Kill()
			{
				if (!IsAlive) { return; }

				SetFrozen(true);
				
				DeathEventManager::ShowDeathScreen(DeathEventManager::KILLED_BY_ENEMY);
				SetAnimationState(DEAD);

				IsAlive = false;
			}

#define GetObjectW GetObject //Stupid windows define thing that needs fixed.

			void Player::Attack(float damage, float range, int atkAnimation)
			{
				for (ActiveCollider* ac : CollisionManager::GetActiveColliders())
				{
					if (Enemies::Enemy* enemy = dynamic_cast<Enemies::Enemy*>(ac->GetObject()))
					{
						if (Vec2::Distance(enemy->GetPosition() + (enemy->GetScale() / 2.f), this->m_position + (range / 2.f)) <= range)
						{
							
						}
					}
				}
			}

			void Player::SetPlayerTextureData(TEXTURE_OFFSETS offsets)
			{
				m_textureData = TextureData[offsets];
				registerAnimations();
			}

			void Player::ApplyAttributeModifier(ATTRIBUTE_MODIFIER attrib, float seconds)
			{
				m_currentAttribModifiers.emplace(std::make_pair(attrib, seconds));
			}

			//Sets the display for the held item/tool.
			void Player::SetHeldItem(Items::InventoryItem* item)
			{
				if (item == nullptr)
				{
					RemoveHeldItem();
					return;
				}

				//If no display exists yet, create one.
				if (m_heldItemDisplay == nullptr)
				{
					m_heldItemDisplay = new GameObject(m_position, Vec2(0), Renderer::GetSprite(0));
					//Renderer::LoadActiveObject(m_heldItemDisplay, 2); NOT NEEDED SINCE THE HELD ITEM IS RENDERED THROUGH THE PLAYER RENDERER.
				}

				//Update item position, scale, and sprite.
				m_heldItemDisplay->SetPosition(m_position + Vec2(2, 3));
				m_heldItemDisplay->SetScale(Vec2(8));


				//If the item has a custom display.
				const int& customHeldTexture = Items::ITEM_DATA[item->GetType()].HeldTexture;

				if (customHeldTexture != GLOBAL_SPRITE_BASE)
				{
					//delete m_heldItemDisplay->GetSprite();

					//Use one texture for all display.
					if (customHeldTexture == GLOBAL_SPRITE_BASE - 1)
					{
						m_heldItemDisplay->SetSprite(Renderer::GetSprite(Items::ITEM_DATA[item->GetType()].Texture));
						m_heldItemDisplay->SetScale({ 10.f, 10.f });
					}
					else {
						m_heldItemDisplay->SetScale({ 16, 16 });
						m_heldItemDisplay->SetSprite(Renderer::GetSprite(customHeldTexture));
					}
					return;
				}

				m_heldItemDisplay->SetSprite(Items::ITEMTYPE_GetItemTypeTexture(item->GetType()));

				//TODO: ITEM RANDOMLY CHANGES TO OTHER SPRITES WHEN THE PLAYER SWITCHES ITEMS WHILE RUNNING.
				m_heldItemDisplayFrameOffset = 0;
			}

#define COLLISION_VEL_STOP_DAMPING 0.1f

			void Player::AddVelocity(Vec2 velocity)
			{
				m_physics->AddVelocity(velocity);
			}

			void Player::SetClimbing(GameObject* climbing)
			{
				//Update the player to be climbing a given object, frozen so they can't move using the normal movement info, and gravity to 0.
				m_climbing = climbing;
				SetFrozen(true, CLIMBING_BACK);
				m_physics->SetGravityStrength(0.f);
				m_physics->SetGravitationalVelocity(0.f);
				m_physics->SetVelocity(Vec2::Zero);
			}

			void Player::Dive(Vec2 direction, float damage)
			{
				m_position += Vec2(0, 2.f);
				AddVelocity(direction);
				m_diving = damage;

				SetFrozen(true, DIVING);
			}

			Inventory::ReturnItem Player::GetItemByType(const ITEM_TYPE& type, int& foundSlot, bool& wasBackpack)
			{
				Inventory::ReturnItem foundItem = { nullptr, true };

				foundItem = TestGame::ThePlayer->GetInventory()->GetItemByType(type, foundSlot);
				if (foundItem.ri_IsNull)
				{
					foundItem = TestGame::ThePlayer->GetBackpack()->GetItemByType(type, foundSlot);
					wasBackpack = true;
				}

				return foundItem;
			}

			int Player::RemoveItemByType(const ITEM_TYPE& type, unsigned int count)
			{
				count -= m_screenInventory->RemoveItemType(type, count);

				if (count > 0)
				{
					count -= m_backpack->Remove(type, count);
				}

				return count;
			}

			void Player::HideAllUI()
			{
				m_screenInventory->HidePlayerSlots();
				Renderer::UnloadGUIElement(m_abilityMeterProgressBar, 0);
				Renderer::UnloadGUIElement(m_healthProgressBar, 0);
				Renderer::UnloadGUIElement(m_armorProgressBar, 0);

				GUI::Menus::GUIMenu::RemoveLastMenu();
			}

			void Player::SetControlType(ControlType type)
			{
				m_controlType = type;
				if (type == ROOM) {
					m_storedRoomHeight = this->m_position.Y; 
					m_onGround = true; 
					dynamic_cast<GAME_NAME::Camera::GameCamera*>(TestGame::INSTANCE->GetCamera())->SetStrictFollowing(true);

					//Breaks stuff in room mode if not disabled.
					this->m_boxCollider->SetAllowStep(false);
				}
				else {
					dynamic_cast<GAME_NAME::Camera::GameCamera*>(TestGame::INSTANCE->GetCamera())->SetStrictFollowing(false);
					this->m_boxCollider->SetAllowStep(true);
				}
			}


			void Player::onCollision(Vec2 push, GameObject* collided)
			{
				if (m_swimming) { return; }
				//if (m_foundCollisionInTick) { return; }

				if (!IsAlive)
				{
					m_deadBloodTimer += Time::GameTime::GetScaledDeltaTime();
					if (m_deadBloodTimer > 0.14 && m_deadBloodTimer < 999.0)
					{
						spawnBloodOnFloor(collided);
						m_deadBloodTimer = 1000.0;
					}
				}

				if (push.Y > 0)
				{
					if (m_diving > 0.f)
					{
						if (m_currentPlayerLookDirection == DIVING)
						{
							SetFrozen(false); SetFrozen(false); SetFrozen(false);
						}

						Damage(m_diving, collided);
						m_airTime = 0.f;
						m_diving = 0.f;
					}

					if (m_isFlying)
					{
						//Reset flying graivty.
						m_isFlying = false;
						m_physics->SetGravityStrength(DefaultPlayerGravity);
					}

					m_onGround = true;
					m_foundCollisionInTick = true;
					m_physics->SetVelocityY(0.f);

					//Correct scale back to normal for rendering etc.
					if (m_feetOnlyCollision && !m_frozen && m_currentPlayerLookDirection == NO_LOOK_DIRECTION)
					{
						m_scale = m_scale * Vec2{ 1.f, 3.f };
					}

					//Calculate fall damage.
					if (m_airTime > 1.35f)
					{
						std::cout << m_airTime << std::endl;

						Damage(((float)m_airTime - 1.35f) * 10.f, collided);
					}

					m_airTime = 0;
				}

				

				if (push.X > COLLISION_VEL_STOP_DAMPING || push.X < -COLLISION_VEL_STOP_DAMPING)
				{
					m_physics->SetVelocityX(0);
				}

				if (push.Y > COLLISION_VEL_STOP_DAMPING || push.Y < -COLLISION_VEL_STOP_DAMPING)
				{
					m_physics->SetVelocityY(0);
				}
			}

			void Player::beforeCollision()
			{
				if (m_swimming) { return; }
				if (m_physics->GetGravitationalVelocity() < -m_physics->GetGravityStrength() - 1)
				{
					m_onGround = false;
				}
				m_foundCollisionInTick = false;

				//Shrink scale before calculating collision.
				if (m_feetOnlyCollision && !m_frozen && m_currentPlayerLookDirection == NO_LOOK_DIRECTION && !TestGame::GetGamePaused())
				{
					m_scale = m_scale / Vec2{ 1.f, 3.f };
				}
			}

			void Player::spawnBloodOnFloor(GameObject* floor)
			{
				CreateBloodParticle(nullptr, false);

				if (!m_onGround) { return; }
				Environment::BloodMark* floorMark = new Environment::BloodMark(floor, m_position + Vec2((m_scale * m_scaleMultiplier).X / 2.f, 0.f));
				Renderer::LoadObject(floorMark, 1);
			}

			void Player::decodeSave(const std::string params)
			{
				std::stringstream paramDecode(params);
				std::string param;

				int i = 0;
				while (std::getline(paramDecode, param, '+'))
				{
					switch (i)
					{
					case 0:
						m_position.X = std::stof(param);
						break;
					case 1:
						m_position.Y = std::stof(param);
						break;
					}

					i++;
				}
			}

			std::string Player::encodeSave()
			{
				//		xPos+yPos
				return std::to_string(std::round(m_position.X)).append("+").append(std::to_string(std::round(m_position.Y)));
			}

			void Player::setArmor(float armor)
			{
				m_stats.Armor = armor;

				float xScale = (armor / 100.f) * m_healthProgressBar->GetScale().X;

				m_armorProgressBar->SetScale(Vec2{ xScale, m_armorProgressBar->GetScale().Y });
			}

			bool Player::dropHeldItem()
			{
				//Don't allow dropping in top-down rooms.
				if (m_feetOnlyCollision) { return false; }

				Items::InventoryItem* item = m_screenInventory->GetHeldItem();
				
				if (item == nullptr) { return false; }

				//FIX, DROPPING WRONG ITEM FOR SOME REASON.
				//Create the object for the item.
				Items::FloorItem* createdItem = new Items::FloorItem(m_position + ((m_scale * m_scaleMultiplier) / 2), item, 3.5F);
				createdItem->GetPhysics()->AddVelocity(m_textureFlipped ? Vec2{ 1.f, 0.3f } : Vec2{ -1.f, 0.3f });
				createdItem->GetPhysics()->AddRotationalVelocity(std::rand() * 30.f / RAND_MAX);
				Renderer::InstantiateObject(Renderer::InstantiateGameObject(createdItem, true, 1, false));

				m_screenInventory->SetItem(m_screenInventory->GetSelectedSlot() - 1, nullptr);

				return true;
			}

			void Player::registerAnimations()
			{
#define RegisterAnimation(data_name, data_source, data_out, spf, baseRef) AnimData data_name;									\
				speed = spf;																								\
				if(m_textureData.AnimationOverride != nullptr && m_textureData.AnimationOverride->data_out.size > 0)			\
				{																												\
					for(int i = 0; i < m_textureData.AnimationOverride->data_out.size; i++)																\
					{																																	\
						data_name.Sprites.emplace_back(std::shared_ptr<Sprite>(Renderer::GetSprite(m_textureData.AnimationOverride->data_out.anim[i] + baseRef)));\
					}																											\
				if (m_textureData.AnimationOverride->data_out.speed > 0) { speed = m_textureData.AnimationOverride->data_out.speed; } \
				}else{																											\
					for (int i : data_source) { data_name.Sprites.emplace_back(std::shared_ptr<Sprite>(Renderer::GetSprite(i))); }; \
				}																												\
																																\
				std::shared_ptr<GAME_NAME::Components::Animation::Animation> data_out(new GAME_NAME::Components::Animation::Animation(data_name, speed)); 

#pragma region Init Animation

				float speed;
				//Not all should be DefaultPlayerSprite...

				RegisterAnimation(walk_data, PlayerWalkAnim, walk_anim, ANIM_12_SPF, DefaultPlayerSprite);						  //0
				RegisterAnimation(run_data, PlayerRunAnim, run_anim, ANIM_16_SPF, DefaultPlayerSprite);							  //1
				RegisterAnimation(fall_data, PlayerFallAnim, fall_anim, ANIM_12_SPF, DefaultPlayerSprite);						  //2
				RegisterAnimation(jump_data, PlayerJumpAnim, jump_anim, ANIM_12_SPF, DefaultPlayerSprite);						  //3
				RegisterAnimation(skid_data, PlayerSkidAnim, skid_anim, ANIM_12_SPF, DefaultPlayerSprite);						  //4 
				RegisterAnimation(fall_over_data, PlayerFallOverAnim, fall_over_anim, ANIM_6_SPF, TextureDataBase(Fall));			  //5
				RegisterAnimation(get_up_data, PlayerGetUpAnim, get_up_anim, ANIM_6_SPF, TextureDataBase(Fall));					  //6
				RegisterAnimation(basic_attack_data, PlayerBasicAttackAnim, basic_attack_anim, ANIM_16_SPF, TextureDataBase(BasicAttack)); //7
				RegisterAnimation(climbing_behind_data, PlayerClimbingBehindAnim, climbing_behind_anim, ANIM_12_SPF, TextureDataBase(Climbing));	    //8
				RegisterAnimation(player_sitting_puff_data, PlayerSittingPuffAnim, player_sitting_puff_anim, ANIM_6_SPF, TextureDataBase(Climbing));  //9
				RegisterAnimation(player_idle_tap_toe_data, PlayerIdleTapToe, player_idle_tap_toe_anim, ANIM_6_SPF, TextureDataBase(IdleAnimations));		//10
				RegisterAnimation(player_idle_stomp_data, PlayerIdleStomp, player_idle_stomp_anim, ANIM_6_SPF, TextureDataBase(IdleAnimations));		    //11
				RegisterAnimation(player_biking_data, PlayerBikeAnim, player_biking_anim, ANIM_6_SPF, TextureDataBase(RidingBike));		    //12


#pragma endregion

				std::vector<std::shared_ptr<GAME_NAME::Components::Animation::Animation>> anims{ walk_anim, run_anim, fall_anim, jump_anim, skid_anim, fall_over_anim, get_up_anim, basic_attack_anim, climbing_behind_anim, player_sitting_puff_anim, player_idle_tap_toe_anim, player_idle_stomp_anim, player_biking_anim };

				m_animator = new AnimatorComponent(anims);
			}

			void Player::CreateBloodParticle(GameObject* cause, bool allowFainting)
			{
				if (m_particleEmitter->GetParticleCount() == 0)
				{
					//Register blood particles

					auto bParticles = Instantiate::LevelObjectHandler::GetLevelObjects("blood");

					for (auto particleObj : bParticles)
					{
						Particles::Particle bloodParticle = Particles::Particle(particleObj);

						bloodParticle.TargetOpacity = 0;
						
						m_particleEmitter->RegisterParticle(bloodParticle);
					}


					Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_particleEmitter, true, 2, true));
				}

				m_particleEmitter->SetPosition(m_position);
				m_particleEmitter->SetAllowCollisions(true);
				

				//Create blood on floor if the player fell.
				if (cause == nullptr)
				{
					m_particleEmitter->SetForcedSpawnVelocity(0);
					m_particleEmitter->SpawnParticles(75, { 0.5f, 0.5f }, 0.04f);
					return;
				}

				if (instanceof<Environment::Effects::Explosion>(cause))
				{
					Environment::BloodMark* floorMark = new Environment::BloodMark(this, m_position + Vec2((m_scale * m_scaleMultiplier).X / 2.f, 0.f));
					Renderer::LoadObject(floorMark, 1);
					m_particleEmitter->SetForcedSpawnVelocity(Vec2{ ((m_position.X + (m_scale * m_scaleMultiplier).X/2) - cause->GetPosition().X) / 5.f , ((m_position.Y + (m_scale * m_scaleMultiplier).Y/2) - cause->GetPosition().Y) / 5.f});
					m_particleEmitter->SpawnParticles(75, { 0.5f, 0.5f }, 0.04f);
					return;
				}

				if (!allowFainting || m_frozenTimer > 0.0f) { return; }

				//ONLY CONTINUE TO HERE IF THE CAUSE OF DAMAGE WAS FALLING.

				//Freeze player after falling from a great height.
				SetFrozen(true, FALLEN);
				m_frozenTimer = 5.f;

				if (!m_onGround) { return; }
				Environment::BloodMark* floorMark = new Environment::BloodMark(cause, m_position + Vec2((m_scale * m_scaleMultiplier).X / 2.f, 0.f));
				Renderer::LoadObject(floorMark, 1);
				//Renderer::InstantiateObject(Renderer::InstantiateGameObject(floorMark, false, 2, false));
			}


			void Player::readKeys()
			{
				if (m_controlType == ControlType::ROOM)
				{
					m_airTime = 0;

					if (m_frozen <= 0)
					{
						if (InputManager::GetKey(PLAYER_MOVE_UP))
						{
							Translate(Vec2{ 0, static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * PLAYER_ROOM_SPEED });
						}

						if (InputManager::GetKey(PLAYER_MOVE_DOWN))
						{
							Translate(Vec2{ 0, static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * -PLAYER_ROOM_SPEED });
						}

						if (m_onGround)
						{
							m_storedRoomHeight = m_position.Y;
						}
					}
				}

				//Setting these values to true causes the player to walk in that direction this frame.
				bool animateMoveRight = false, animateMoveLeft = false;

				//The player is currently locked in an animation sequence.
				if (m_targetSequence.size() > 0)
				{
					if (m_targetSequence[0].IgnoreGravity)
					{
						m_physics->SetGravityStrength(0);
						m_airTime = 0.f;
					}

					if (glfwGetTime() - m_targetSequence[0].StartTime > m_targetSequence[0].Delay)
					{
						Vec2& walkTo = m_targetSequence[0].WalkTo;

						//Player has not reached the target location (currently excludes the Y position [CHANGE LATER]).
						if (std::abs(m_position.X - walkTo.X) > 1.f)
						{
							animateMoveRight = m_position.X - walkTo.X < 0.f;
							animateMoveLeft = !animateMoveRight;

						}
						else {
							//This step is complete.

							if (m_targetSequence[0].IgnoreGravity)
							{
								m_physics->SetGravityStrength(DefaultPlayerGravity);
							}

							m_targetSequence.erase(m_targetSequence.begin());

							if (m_targetSequence.size() > 0)
							{
								m_targetSequence[0].StartTime = glfwGetTime();
							}
						}
					}	
				}

				//Jump to stop sitting on a bench.
				if (m_currentPlayerLookDirection == SITTING_FORWARD)
				{
					if (InputManager::GetKey(PLAYER_JUMP))
					{
						SetFrozen(false);
					}
				}

				if (m_climbing)
				{
					m_airTime = 0;
					m_animator->SetSpeedMult(0);

					if (InputManager::GetKey(PLAYER_MOVE_UP))
					{
						if (m_climbing->GetPosition().Y + m_climbing->GetScale().Y > m_position.Y + 6.5f)
						{
							Translate({ 0.f, (float)(Utils::Time::GameTime::GetScaledDeltaTime() * PLAYER_CLIMBING_SPEED) });
							m_animator->SetSpeedMult(1);
						}
					}
					else if (InputManager::GetKey(PLAYER_MOVE_DOWN))
					{
						if (m_climbing->GetPosition().Y < m_position.Y + (m_scale * m_scaleMultiplier).Y - 6.5f)
						{
							Translate({ 0.f, (float)(-Utils::Time::GameTime::GetScaledDeltaTime() * PLAYER_CLIMBING_SPEED) });
							m_animator->SetSpeedMult(1);
						}
					}
					else if(InputManager::GetKey(PLAYER_MOVE_LEFT))
					{
						if (m_climbing->GetPosition().X < m_position.X + 6.5f)
						{
							Translate({ (float)(-Utils::Time::GameTime::GetScaledDeltaTime() * PLAYER_CLIMBING_SPEED), 0.f });
							m_animator->SetSpeedMult(1);
						}
					}
					else if(InputManager::GetKey(PLAYER_MOVE_RIGHT))
					{
						if (m_climbing->GetPosition().X + m_climbing->GetScale().X > m_position.X + (m_scale * m_scaleMultiplier).X - 6.5f)
						{
							Translate({ (float)(Utils::Time::GameTime::GetScaledDeltaTime() * PLAYER_CLIMBING_SPEED), 0.f });
							m_animator->SetSpeedMult(1);
						}
					}

					
					if (InputManager::GetKey(PLAYER_JUMP) || InputManager::GetKey(PLAYER_INTERACT))
					{
						m_climbing = nullptr;
						SetFrozen(false);
						m_physics->SetGravityStrength(DefaultPlayerGravity);
					}
					else {
						return;
					}
				}

				if (m_diving)
				{
					m_physics->SetFrictionDrag(0);
				}
				else {
					m_physics->SetFrictionDrag(Drag);
				}

				//Pedal animation! TODO: Add for other vehicles speed control here.
				if (m_currentPlayerLookDirection == BIKING)
				{
					m_animator->SetSpeedMult(std::abs(m_physics->GetVelocity().X / 300.f));

					//Change facing direction based on bike vehicle motion.
					if (std::abs(m_physics->GetVelocity().X) > 0.05f)
					{
						m_textureFlipped = m_physics->GetVelocity().X > 0;
					}
				}

				//TESTING: REMOVE
				//Fake damage by pressing TAB
				/*if ((InputManager::GetKeyUpDown(DEBUG_OBJECT_MENU) & InputManager::KEY_STATE::KEY_STATE_PRESSED) && !m_debug)
				{
					Damage(10, nullptr);
				}*/

#if _DEBUG
				if (InputManager::GetKey(PLAYER_DEBUG))
				{
					if (!m_debugKey)
					{
						m_debugKey = true;

						EnterDebug();
					}
				}
				else if (m_debugKey)
				{
					m_debugKey = false;
				}
#endif

				//Check if the player is pressing the key to toggle their flashlight.
				if (InputManager::GetKeyUpDown(PLAYER_TOGGLE_FLASHLIGHT) & InputManager::KEY_STATE::KEY_STATE_PRESSED)
				{
					togglePlayerLight();
				}

				//If the player is pressing the drop item key, drop the item they are holding.
				if (InputManager::GetKeyUpDown(PLAYER_DROP_HELD_ITEM) & InputManager::KEY_STATE::KEY_STATE_PRESSED)
				{
					dropHeldItem();
				}

				/*
					Player backpack opening.
				*/
				if (InputManager::GetKeyUpDown(PLAYER_OPEN_BACKPACK) & InputManager::KEY_STATE_PRESSED)
				{

					if (m_backpack->GetIsOpen())
					{
						//Close backpack.
						if (m_backpack->Close())
						{
							//Update held item in case the player changed their current selected item.
							m_screenInventory->SelectSlot(m_screenInventory->GetSelectedSlot());

							SetFrozen(false);
							return;
						}				
					}

					//Is the player able to open their bag?
					if (!m_onGround || m_frozen) { return; }

					//Open backpack GUI.
					if (m_backpack->Open())
					{
						//Freeze player inputs and use bag sprite.
						SetFrozen(true, BAG);

						//Correct scale back to normal for rendering etc.
						if (m_feetOnlyCollision)
						{
							m_scale = m_scale * Vec2{ 1.f, 3.f };
						}
						return;
					}
				
				//Allow the backpack to close using the ESCAPE key -- or pause key.
				}else if (InputManager::GetKeyUpDown(DEFAULT_PAUSE_GAME) & InputManager::KEY_STATE_PRESSED)
				{
					if (m_backpack->GetIsOpen())
					{
						if (m_backpack->Close())
						{
							//Update held item in case the player changed their current selected item.
							m_screenInventory->SelectSlot(m_screenInventory->GetSelectedSlot());

							SetFrozen(false);
							return;
						}
					}
				}

				bool playerIsSkidding = false;


				//int joyAxesCount;		JOYSTICK INPUT
				//const float* joyAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &joyAxesCount);


				//If the player is frozen, do not check any key presses.
				if ((m_diving <= 0.f && m_frozen <= 0.f) || animateMoveLeft || animateMoveRight)
				{
					float addSpeedCap = 0.f;

					//Check for speed modifier.
					if (m_currentAttribModifiers.contains(ATTRIBUTE_MODIFIER::SPEED_UP))
					{
						addSpeedCap += PlayerSpeedCap / 2.f;
					}
	
					if (!animateMoveLeft && (animateMoveRight || InputManager::GetKey(PLAYER_MOVE_RIGHT)))
					{

#if _DEBUG
						if (m_flight)
						{
							m_position += Vec2(InputManager::GetKey(PLAYER_DEBUG_ADD_SPEED) ? 3.0f : 1.4f, 0.f);
							return;
						}
#endif
						if (m_physics->GetVelocity().X < PlayerSpeedCap + addSpeedCap)
						{
							m_physics->SetFrictionDrag(0.f);
							m_physics->AddVelocity(Vec2(((float)Time::GameTime::GetScaledDeltaTime() / 0.017f) * (PlayerSpeedCap - m_physics->GetVelocity().X) * (PlayerSpeed), 0.f));

							//Check if the player has begun to move to the right, play a sliding animation if slowing down, flip the sprite if moving right.
							if (m_physics->GetVelocity().X > 0)
							{
								m_textureFlipped = true;
							}
							else
							{
								m_physics->SetFrictionDrag(3.f);
								playerIsSkidding = true;
							}
						}
					}

					if (!animateMoveRight && (animateMoveLeft || InputManager::GetKey(PLAYER_MOVE_LEFT)))
					{
#if _DEBUG
						if (m_flight)
						{
							m_position += Vec2(InputManager::GetKey(PLAYER_DEBUG_ADD_SPEED) ? -3.0f : -1.4f, 0.f);
							return;
						}
#endif
						if (m_physics->GetVelocity().X > -PlayerSpeedCap - addSpeedCap)
						{
							m_physics->SetFrictionDrag(0);
							m_physics->AddVelocity(Vec2(((float)Time::GameTime::GetScaledDeltaTime() / 0.017f) * (PlayerSpeedCap - m_physics->GetVelocity().X) * (-(PlayerSpeed - 0.0165F)), 0.f));
						}

						//Check if the player has begun to move to the left, play a sliding animation if slowing down, flip the sprite if moving left.
						if (m_physics->GetVelocity().X < 0)
						{
							m_textureFlipped = false;
						}
						else
						{
							m_physics->SetFrictionDrag(3.f);
							playerIsSkidding = true;
						}
					}
				}
				
				//If the player is not attacking, calculate animations.
				if(m_attackCooldown <= 0 && m_frozen <= 0)
				{
					
					//Determines what animation to play currently.
					float anim_momentum;
					setAnimations(playerIsSkidding, anim_momentum);

					//Check if the user is holding the FORCE_WALK [Left Shift] key and damps the player's speed to 1/2 walking speed.
					if (InputManager::GetKey(PLAYER_FORCE_WALK) || ((animateMoveLeft || animateMoveRight) && !m_targetSequence[0].Sprinting))
					{
						if (anim_momentum > (PLAYER_ANIMATION_RUN_WALK_SWITCH/2))
						{
							m_physics->AddVelocity(Vec2(-m_physics->GetVelocity().NormalizeX() * (anim_momentum - (PLAYER_ANIMATION_RUN_WALK_SWITCH/2.f)) / 4.f, 0));
						}
					}
				}

#if _DEBUG
				if (m_flight)
				{
					if (InputManager::GetKey(PLAYER_MOVE_UP))
					{
						m_position += Vec2(0.f, InputManager::GetKey(PLAYER_DEBUG_ADD_SPEED) ? 3.0f : 1.4f);
					}

					if (InputManager::GetKey(PLAYER_MOVE_DOWN))
					{
						m_position += Vec2(0.f, InputManager::GetKey(PLAYER_DEBUG_ADD_SPEED) ? -3.0f : -1.4f);
					}
				}
#endif


				//Check if player is jumping and not frozen.
				if (m_diving <= 0 && m_frozen <= 0 && InputManager::GetKey(PLAYER_JUMP) && !animateMoveLeft && !animateMoveRight && m_controlType != ControlType::ROOM)
				{
					unsigned char checks = m_physics->GetUpdatesThisFrame();
					while (checks > 0)
					{
						if (m_jumpHeld != 0 && m_jumpHeld < PlayerJumpHoldLength || m_swimming)
						{
							m_jumpHeld++;
							m_physics->AddVelocity(Vec2(0.f, PlayerJumpBonus));
						}
						checks--;
					}

					if (!m_swimming)
					{
						if (m_onGround)
						{
							//Jump Sound
							//Audio::SoundManager::Play("boing", Audio::SoundManager::SFXGroup, 1.f, 0.f, false, Audio::SoundManager::SFXBus.mChannelHandle);

							m_jumpHeld = 1;
							m_onGround = false;
							m_physics->AddVelocity(Vec2(0.f, PlayerJumpHeight + (std::abs(m_physics->GetVelocity().X) * PlayerXSpeedJumpMultiplier)));
						}
					}
					else if (m_jumpHeld != 0)
					{
						m_jumpHeld = 0;
					}
				}
				
				/*
					-----
					CHECK PLAYER ABILITIES AND ACTIONS.
					-----
				*/
				if (m_skillHolder.CanUseAbility(FLIGHT))
				{
					if (m_stats.AbilityMeter >= 10.f)
					{
						//FLIGHT (GLIDING) ABILITY, DISABLE IF PLAYER IS MISSING CERTAIN ITEMS.
						if (InputManager::GetKeyUpDown(PLAYER_JUMP) & InputManager::KEY_STATE_PRESSED)
						{
							if (!m_swimming && !m_isFlying && m_frozen <= 0 && m_jumpHeld != 1 && m_airTime > 0.4f /*Make sure player has to start gliding at the peak of their jump.*/)
							{
								//DEBUGGING
								std::cout << "FLIGHT ENABLED" << std::endl;

								//Subtract from ability meter based on cost of ability.
								updateAbilityMeter(-10.f);

								//Set flying, "double jump" occured.
								m_isFlying = true;
								m_physics->SetVelocityY(200.f);
								m_physics->SetGravityStrength(DefaultPlayerGravity - FlyingGravityReduction - 0.5f);
							}
						}
					}

				}

				if (m_controlType == ROOM)
				{

					if (m_position.Y <= m_storedRoomHeight)
					{
						m_onGround = true;
						m_position.Y = m_storedRoomHeight;
						m_physics->SetGravitationalVelocity(0.f);
						m_physics->SetVelocityY(0.f);
					}
					
					if (m_onGround)
					{
						m_physics->SetGravityStrength(0.f);
					}
					else {
						m_physics->SetGravityStrength(DefaultPlayerGravity);
					}

				}


			}


			void Player::setAnimations(bool playerIsSkidding, float& anim_momentum)
			{
				//Rand to fix issue with seed getting set every single time.
				std::rand();

				anim_momentum = std::abs(m_physics->GetVelocity().X);	//X Speed of player.

				if (m_onGround || (m_targetSequence.size() > 0 && m_targetSequence[0].IgnoreGravity))
				{
					//Don't allow the player to try to play idle animations while in a sequence.
					if (m_targetSequence.size() > 0) { m_timeSpentNotMoving = 0; }

					if (playerIsSkidding && anim_momentum > 60.f)
					{
						m_timeSpentNotMoving = 0.0;
						m_animator->SetCurrentAnimation(4); //Skidding Animation
						return;
					}

					if (anim_momentum > 0.1f && (m_onGround || (m_targetSequence.size() > 0 && m_targetSequence[0].IgnoreGravity)) && !m_swimming)	//Check if player is moving a certain speed, is on the ground, and is not swimming.
					{
						m_timeSpentNotMoving = 0.0;
						m_animator->SetSpeedMult((double)anim_momentum / 100.0);
						if (anim_momentum > PLAYER_ANIMATION_RUN_WALK_SWITCH)
						{
							m_animator->SetCurrentAnimation(1);  //Running Animation
						}
						else {
							m_animator->SetCurrentAnimation(0);  //Walking Animation
						}
						m_begunMotion = true;
						return;
					}
					

					//Is currently in an idle animation.
					if (m_playingIdleAnimation > 0)
					{
						//If this is the stomping animation, freeze the animation on the last frame.
						if (m_animator->GetCurrentAnimationIndex() == 11 && m_animator->GetCurrentAnimation()->GetFrame() > 5)
						{
							m_animator->SetSpeedMult(0);
						}

						m_playingIdleAnimation -= Utils::Time::GameTime::GetScaledDeltaTime();
						return;
					}

					m_timeSpentNotMoving += Time::GameTime::DeltaTime::GetDeltaTime();

					//Reset sprite to default sprite if it needs to be changed. (After idle animations).
					if (m_begunMotion || (m_playingIdleAnimation < 0 && m_animator->GetCurrentAnimationIndex() > 9))
					{
						//if (m_sprite != nullptr) { delete m_sprite; } //TODO: Maybe.

						m_animator->SetCurrentAnimation(-1); //No Animation
						m_sprite.reset(Renderer::GetSprite(DefaultPlayerSprite));
						m_begunMotion = false;
					}

					if (m_timeSpentNotMoving < 3.1) { return; }

					/*
						-- Random checks to determine if the player should begin an idle animation.
						These become more likely as the player stands still.
					*/

					int myRand = std::rand();

					if (myRand < m_timeSpentNotMoving * (RAND_MAX / 20 /*How long it takes to become inevitable that the idle animation will play.*/))
					{
						int ran = std::rand();
						int chosen = (int)(std::floor((float)(ran) / (float)(RAND_MAX) * 4));

						m_timeSpentNotMoving = 0.0;
						switch (chosen)
						{
						case 1:	//TAP TOE
							m_animator->SetCurrentAnimation(10);
							m_animator->SetSpeedMult(1.2f);
							m_playingIdleAnimation = 4.0;
							break;
						case 2:	//STOMP
							m_animator->SetCurrentAnimation(11);
							m_animator->SetSpeedMult(1.f);
							m_playingIdleAnimation = 4.25;
							break;
							
						default:
							break;
						}
					}


					return;
				}

				m_timeSpentNotMoving = 0.0;


				m_animator->SetSpeedMult(1.0);
				float player_y_vel = m_physics->GetVelocity().Y + m_physics->GetGravitationalVelocity();

				if (player_y_vel > 1.5f)
				{
					m_animator->SetCurrentAnimation(3); //Jumping Animation
					m_begunMotion = true;
					return;
				}
				
				if (player_y_vel < -1.5f)
				{
					m_animator->SetCurrentAnimation(2); //Falling Animation
					m_begunMotion = true;
					return;
				}
			}

			void Player::updateAnimationState()
			{
				//Player is currently in an attacking animation, do not interrupt.
				if (m_animator->GetCurrentAnimationIndex() == 7) { return; }

				if (m_currentPlayerLookDirection == FOLLOW_MOUSE)
				{
					m_sprite = std::shared_ptr<Sprite>(Renderer::GetSprite(PLAYER_NO_HEAD_SPRITE));

					if (m_emotionsObject == nullptr)
					{
						m_emotionsObject = new ChildGameObject(Vec2(DefaultPlayerScaleX, 12.25), Vec2(-DefaultPlayerScaleX, DefaultPlayerScaleY * 0.519230769f), Renderer::GetSprite(PLAYER_EMOTIONS::ANGRY), this);
					}
					else {
						m_emotionsObject->Update(TestGame::FirstWindow);
					}
				}
				else {
					if (m_emotionsObject != nullptr)
					{
						delete m_emotionsObject;
						m_emotionsObject = nullptr;
					}
				}

				switch (m_currentPlayerLookDirection)
				{
					case FOLLOW_MOUSE:
					{
						/*
							TODO: FIX THIS SO THE HEAD LOOKS BETTER.
						*/


						//Get what angle the head must rotate to point at the mouse.
						Vec2 mouseDistance = InputManager::GetMouseWorldPosition(TestGame::INSTANCE->GetCamera()) - m_emotionsObject->GetPosition();
						float rotation = -MathUtils::to_degf(std::atanf(mouseDistance.Y / mouseDistance.X));
						m_emotionsObject->SetRotationAboutCenter(rotation);
						break;
					}

					case VICTORY_CONFETTI:
					{
						m_sprite.reset(Renderer::GetSprite(PLAYER_HOLD_CONFETTI_GUN_SPRITE));
						break;
					}

					case VICTORY_BALLOON:
					{
						m_sprite.reset(Renderer::GetSprite(PLAYER_HOLD_BALLOON_SPRITE));
						break;
					}

					case BEHIND:
					{
						//delete m_sprite;
						m_sprite.reset(Renderer::GetSprite(PLAYER_LOOK_BEHIND_SPRITE));
						break;
					}

					case FALLEN:
					{
						if (m_frozenTimer < 0.f) { break; }

						if (m_frozenTimer < ANIM_6_SPF * 4.f)
						{
							if (m_animator->GetCurrentAnimation() == nullptr)
							{
								break;
							}

							//Reset falling animation via incrementing frame.
							m_animator->GetCurrentAnimation()->IncrementFrame(nullptr);
							m_animator->SetCurrentAnimation(6); //Get Up Anim
							m_animator->SetSpeedMult(1.f);

							m_rotation = 0.f;

							if (m_animator->GetCurrentAnimation()->GetFrame() == 3)
							{
								//Reset animation via incrementing frame.
								m_animator->GetCurrentAnimation()->IncrementFrame(nullptr);
								m_animator->SetCurrentAnimation(-1); //No Animation
								m_animator->SetSpeedMult(0.f);
							}

							break;
						}

						m_animator->SetCurrentAnimation(5); //Fall Over Anim
						

						//Update scale to keep sprites relative size the same.
						if (m_animator->GetCurrentAnimation()->GetFrame() > 0)
						{
							m_scale = Vec2(26, 26);
						}

						//Animation done.
						if (m_animator->GetCurrentAnimation()->GetFrame() == 3)
						{
							m_animator->SetSpeedMult(0.f);

							//delete m_sprite;
							m_sprite.reset(Renderer::GetSprite(PLAYER_FALLEN_SPRITE));
							m_rotation = -11.f;

						}
						else {
							m_animator->SetSpeedMult(1.f);
						}

						break;
					}

					case BAG:
					{
						m_sprite.reset(Renderer::GetSprite(PLAYER_LOOK_BAG));
						break;
					}

					case SITTING_FORWARD:
					{


						m_scale = Vec2(17, 24.5f);

						if (m_animator->GetCurrentAnimationIndex() != 9)
						{
							//Do a puff animation.
							if (std::rand() < RAND_MAX / 525)
							{
								m_animator->SetCurrentAnimation(9);
								m_animator->SetSpeedMult(1);
							}
							else {
								auto nSprite = Renderer::GetSprite(PLAYER_SIT_TEXTURE);
								if (m_sprite->GetSpriteId() != nSprite->GetSpriteId())
								{
									m_sprite.reset(nSprite);
								}
							}
						}
						else if(m_animator->GetCurrentAnimation()->GetFrame() == 4) {
							m_animator->GetCurrentAnimation()->IncrementFrame(this);
							m_animator->SetCurrentAnimation(0);
							m_animator->SetSpeedMult(0);
						}
						

						//Just started sitting, force puff animation.
						if (m_scale.X != 17)
						{
							m_animator->SetCurrentAnimation(9);
							m_animator->SetSpeedMult(1);
						}

						m_airTime = 0;
						m_physics->SetVelocity(0);

						break;
					}

					case CLIMBING_BACK:
					{
						//TODO: Animate climbing.

						m_animator->SetCurrentAnimation(8);

						break;
					}

					case DEAD:
					{
						m_scale = Vec2(26, 16);
						m_sprite.reset(Renderer::GetSprite(PLAYER_DEAD_SPRITE));
						break;
					}

					case BIKING:
					{
						m_scale = Vec2(26.f, 28.5f);
						//Biking animation.
						if (m_animator->GetCurrentAnimationIndex() != 12)
						{
							m_animator->SetCurrentAnimation(12, this);
							m_animator->SetSpeedMult(0);
						}

						break;
					}

					case DIVING:
					{
						m_scale = Vec2(45.f / 2.f, 41.f / 2.f);
						m_sprite.reset(Renderer::GetSprite(PLAYER_UNINTENTIONAL_DIVE_SPRITE));
						break;
					}

					default:
					{
						//Reset animation if currently playing.

						m_scale = Vec2(16, 26);
						return;
					}
				}
			}

			void Player::updateAbilityMeter(float amount)
			{
				m_stats.AbilityMeter += amount;
				m_abilityMeterProgressBar->SetPercentage(static_cast<char>(std::clamp(m_stats.AbilityMeter, 0.f, 100.f)));
			}

			void Player::togglePlayerLight()
			{
using namespace Lighting;

				if (m_playerLight == nullptr)
				{
					m_playerLight = new LightingSource(m_position, 45.f, 5.f, POINT_LIGHT, true, true);
				}
				else
				{
					m_playerLight->Off(true);
					delete m_playerLight;
					m_playerLight = nullptr;
				}
			}


			void Player::handleAttack()
			{

				if (m_attackCooldown > 0)
				{
					m_attackCooldown -= Utils::Time::GameTime::GetScaledDeltaTime();
					Player_AttackingTracker -= Utils::Time::GameTime::GetScaledDeltaTime();

					if (Player_AttackingTracker < 0 && Player_AttackingTracker > -100)
					{
						Player_AttackingTracker = -100;

						//Completed attack animation.

						if (!m_textureFlipped)
						{
							//Offset position to adjust for offset visual position in attack animation.
							m_position += Vec2(13.f, 0) * m_scaleMultiplier;
						}

						m_frozen = false;
						m_animator->SetCurrentAnimation(-1);
						m_sprite.reset(Renderer::GetSprite(DefaultPlayerSprite));
					}

					return;
				}

				//Player is trying to attack.
				if (InputManager::GetKey(keyRef::PLAYER_ATTACK))
				{
					if (m_frozen) { return; }
					
					//If the player is using the mouse to attack and not the controller.
					if (InputManager::GetUsingMouse())
					{
						Vec2 mousePos = InputManager::GetMouseWorldPosition(TestGame::INSTANCE->GetCamera());

						if (mousePos.X < m_position.X + m_scale.X / 2.f)
						{
							m_textureFlipped = false;
						}
						else
						{
							m_textureFlipped = true;
						}
					}

					if (!m_textureFlipped)
					{
						//Offset position to adjust for offset visual position in attack animation.
						m_position -= Vec2(13.f, 0) * m_scaleMultiplier;

						m_physics->AddVelocity({ 6.5f, 0.f });
					}

					//Play attacking animation.
					//m_physics->SetVelocityX(0.f);
					m_animator->SetCurrentAnimation(7, this);
					m_animator->SetSpeedMult(2.f);
					m_scale = { 26.f, 26.f }; //Adjust scale to that of the animation sprites.
					m_frozen = true;
					m_attackCooldown = (8.0 * 0.5) * (double)ANIM_16_SPF; //(frames * 1/speed * seconds_per_frame)
					Player_AttackingTracker = m_attackCooldown;	//Ignores the weapon speed.

					const Vec2 damageOrigin = m_position + (Vec2{ m_scale.X / 2.f, 0.f }) + Vec2{ (m_textureFlipped ? 12.f : -12.f), 0.f };

					int AOE = 22, damage = 2;

					//Retrive Area of Effect and Damage stats of current weapon.
					if (GetInventory()->GetHeldItem())
					{
						const ItemData& heldItemData = ITEMTYPE_GetItemData(this->GetInventory()->GetHeldItem()->GetType());

						if (heldItemData.Actions & RANGED_WEAPON)
						{
							m_projectileCharge += 0.20;
							return;
						}

						if (heldItemData.Actions & WEAPON)
						{
							const std::string& attribute = heldItemData.Attributes.at(TOOL_ACTION::WEAPON);
							damage = std::stoi(attribute.substr(0, attribute.find_first_of(',')));
							AOE = std::stoi(attribute.substr(attribute.find_last_of(',') + 1));

							std::string inner = attribute.substr(attribute.find_first_of(',') + 1, attribute.find_last_of(','));
							m_attackCooldown = std::stod(inner.substr(inner.find_first_of(',') + 1));

							//m_animator->SetSpeedMult(((8.0 * 0.5) * (double)ANIM_16_SPF / m_attackCooldown) * 2.0);
						}
					}

					//Handle damage.
					for (Enemies::Enemy* enemy : Enemies::Enemy::EnemyRegistry)
					{
						if (Vec2::Distance(enemy->GetPosition() + (enemy->GetScale()/2.f), damageOrigin) < AOE)
						{
							enemy->Damage(damage, m_position + (m_scale/2.f));
						}
					}
				}
				//Player has been holding projectile button, now it is released, throw.
				else if (m_projectileCharge > 0.0)
				{
					if (m_screenInventory->GetHeldItem())
					{
						const ItemData& heldItemData = ITEMTYPE_GetItemData(this->GetInventory()->GetHeldItem()->GetType());
						const std::string& attribute = heldItemData.Attributes.at(TOOL_ACTION::RANGED_WEAPON);
						std::stringstream attributeStream(attribute);
						std::string value; uint8_t i = 0;
						//damage,powerconsume,reloadseconds,range,projectile_type

						int damage = 0; float range = 0; uint8_t projectileType = 0;

						while (std::getline(attributeStream, value, ','))
						{
							switch (i++)
							{
							case 0:
								damage = std::stoi(value);
								break;
							case 1:
								if (value == "0") { break; }
								this->updateAbilityMeter(-std::stof(value));
								break;
							case 2:
								m_attackCooldown = std::stod(value);
								break;
							case 3:
								range = std::stof(value);
								break;
							case 4:
								projectileType = std::stoi(value);
								break;
							}
						}

						Vec2 mousePos = InputManager::GetMouseWorldPosition(TestGame::INSTANCE->GetCamera());
						Vec2 direction = mousePos - (m_position + m_scale / 2.f);

						throwProjectile(damage, range * std::clamp(m_projectileCharge, 0.0, 1.0)/3.0, projectileType, direction.X < 0);
						m_screenInventory->SetItem(m_screenInventory->GetSelectedSlot() - 1, nullptr);

						m_projectileCharge = 0.0;
					}

					
				}
			}

			void Player::throwProjectile(int damage, float range, uint8_t projectileType, bool throwLeft)
			{	
				GAME_NAME::Objects::Projectile* projectile = new GAME_NAME::Objects::Projectile(throwLeft ? (m_position + Vec2{ -10.f, m_scale.Y / 2.f }) : (m_position + Vec2{ m_scale.X, m_scale.Y / 2.f }), damage, range, projectileType, throwLeft);
				Renderer::InstantiateObject(Renderer::InstantiateGameObject(projectile, true, 1, false));
			}


		}
	}
}