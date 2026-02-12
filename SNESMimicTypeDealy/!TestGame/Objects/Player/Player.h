#pragma once
#include "../../../Components/Physics/Collision/Helpers/ActiveBoxCollisionGravityObject.h"
#include "../../../Utils/Math/Vec2.h"
#include "../../../Rendering/Renderers/Renderer.h"
#include "../../../Components/Animation/AnimatorComponent.h"
#include "../../../Audio/SoundManager.h"
#include "../../../Components/ChildGameObject.h"
#include "../../../Objects/Particles/ParticleEmitter.h"
#include "../../../Objects/GUI/Progress/ProgressBar.h"

#include "ScreenInventory.h"
#include "Backpack/Backpack.h"
#include "Skills/SkillHolder.h"

#include "../../../Rendering/Lighting/LightingSource.h"

#include "../Environment/Effects/GlitchableObject.h"


#ifndef _PLAYERDEF
#define _PLAYERDEF


constexpr float DefaultPlayerScaleX = 16.f;	//The default size for the player.
constexpr float DefaultPlayerScaleY = 26.f;	//The default size for the player.


constexpr float PlayerSpeed = .04f;			//The amount of velocity to add for each frame the player is moving.
constexpr float PlayerSpeedCap = 160.f;		//The maximum amount of velocity the player can accelerate at (ignoring gravity).

constexpr float PlayerJumpHeight = 60.f;	//The vertical velocity applied on the inital press of jump.
constexpr float PlayerJumpBonus = 13.f;		//How vertical velocity to add for every frame the jump button is held after the inital jump.
constexpr int8_t PlayerJumpHoldLength = 10;	//The maximum amount of frames the player can hold jump for.
constexpr float PlayerXSpeedJumpMultiplier = 0.08f; //The multiplier for how much of the X speed is added to the players jump.

constexpr float DefaultPlayerGravity = 5.5f;	//The default gravity.
constexpr float SwimmingPlayerGravity = 0.4f;	//The default gravity while underwater.

constexpr float FlyingGravityReduction = 4.f;   //How much gravity will be reduced by when gliding.

constexpr float Drag = 4.f;					//How much the player should slow down each frame when they stop moving.

namespace  GAME_NAME
{
	namespace  Objects
	{
		namespace Player
		{
			using namespace Components;
			using namespace Physics;
			using namespace Collision;
			using namespace Animation;
			using namespace GUI;
			using namespace Environment::Effects;

			/// <summary>
			/// Possible effects that can be added to the player from eating food or other events.
			/// </summary>
			enum ATTRIBUTE_MODIFIER
			{
				SPEED_UP,
				JUMP_BOOST
			};

			class Player	//The class used for the player GameObject.
				: public ActiveBoxCollisionGravityObject, public MiscStateGroup, public GlitchableObject
			{
			public:
				Player(Vec2 position, bool loadFromSavedPosition = true);
				~Player();

				void Update(GLFWwindow* window);

				void SetSwimming(bool swimming);		//Enables low gravity and swimming mode. (Sets m_swimming to true)

				void Render(const Vec2& cameraPosition) override;	

				void SetGlitched(bool glitched) override;

				struct TargetEvent
				{
					Vec2 WalkTo;
					bool IgnoreGravity = false;
					bool Sprinting = true;
					double Delay = 0.0;
					double StartTime;

					TargetEvent(Vec2 walkTo, bool ignoreGravity = false, bool sprinting = true, double delay = 0.0)
						: WalkTo(walkTo), IgnoreGravity(ignoreGravity), Sprinting(sprinting), Delay(delay), StartTime(glfwGetTime())
					{

					}
				};

				/// <summary>
				/// Queues a new target for the player to reach.
				/// </summary>
				/// <param name="te"></param>
				inline void QueueTargetEvent(TargetEvent te)
				{
					m_targetSequence.push_back(te);
				}

				inline void ClearTargetEvents()
				{
					m_targetSequence.clear();
				}

#if _DEBUG
				inline bool GetDebug() { return m_debug; }

				void EnterDebug();
				void ToggleFlight();
#endif

				/// <summary>
				/// Damage the player the specified amount with the given object as the cause. If this was fall damage, the ground will recieve a splatter (Otherwise cause should be nullptr).
				/// </summary>
				/// <param name="damage"></param>
				/// <param name="cause"></param>
				void Damage(float damage, GameObject* cause, bool causeFainting = true);

				/// <summary>
				/// True if the player is alive.
				/// </summary>
				bool IsAlive = true;

				/// <summary>
				/// Heal the player and ensure that the players health doesn't exceed the maximum with armour effects applied.
				/// </summary>
				/// <param name="health"></param>
				void Heal(float health);
				void Kill();


				/// <summary>
				/// Uses the specified damage and range to play an attacking animation and create a damage hitbox.
				/// </summary>
				void Attack(float damage, float range, int atkAnimation = 0);

				/// <summary>
				/// Spawns random blood particles around the player. (can be used to imitate damage, is also called when Damage() is called)
				/// </summary>
				void CreateBloodParticle(GameObject* cause, bool causeFainting = true);

				enum TEXTURE_OFFSETS
				{
					DEFAULT_BIRB = 0,
					PIXEL_BIRB = 1,
					DEFAULT_FOX = 2
				};

				struct AnimationOverride
				{
					int* anim;
					int size;
					float speed = 0.f;

					~AnimationOverride()
					{
						//delete[size] anim;
					}
				};

				struct PlayerAnimationData
				{
					AnimationOverride walk_anim, run_anim, jump_anim, fall_anim, skid_anim, fall_over_anim, get_up_anim, basic_attack_anim, climbing_behind_anim, player_sitting_puff_anim, player_idle_tap_toe_anim, player_idle_stomp_anim, player_biking_anim;

					/// <summary>
					/// Creates a player animation system that can be used by registerAnimations when changing sprite "pallettes".
					/// </summary>
					/// <param name="walk_anim_data"></param>
					/// <param name="run_anim"></param>
					/// <param name="jump_anim"></param>
					/// <param name="fall_anim"></param>
					/// <param name="skid_anim"></param>
					/// <param name="fall_over_anim"></param>
					/// <param name="get_up_anim"></param>
					/// <param name="basic_attack_anim"></param>
					/// <param name="climbing_behind_anim"></param>
					/// <param name="player_sitting_puff_anim"></param>
					/// <param name="player_idle_tap_toe_anim"></param>
					/// <param name="player_idle_stomp_anim"></param>
					PlayerAnimationData(AnimationOverride walk_anim_data, AnimationOverride run_anim = AnimationOverride(0, 0), AnimationOverride jump_anim = AnimationOverride(0, 0), AnimationOverride fall_anim = AnimationOverride(0, 0),
						AnimationOverride skid_anim = AnimationOverride(0, 0), AnimationOverride fall_over_anim = AnimationOverride(0, 0), AnimationOverride get_up_anim = AnimationOverride(0, 0), AnimationOverride basic_attack_anim = AnimationOverride(0, 0),
						AnimationOverride climbing_behind_anim = AnimationOverride(0, 0), AnimationOverride player_sitting_puff_anim = AnimationOverride(0, 0), AnimationOverride player_idle_tap_toe_anim = AnimationOverride(0, 0), AnimationOverride player_idle_stomp_anim = AnimationOverride(0, 0),
						AnimationOverride biking_anim = AnimationOverride(0, 0))
						: walk_anim(walk_anim_data), run_anim(run_anim), jump_anim(jump_anim), fall_anim(fall_anim), skid_anim(skid_anim), fall_over_anim(fall_over_anim), get_up_anim(get_up_anim), basic_attack_anim(basic_attack_anim),
						climbing_behind_anim(climbing_behind_anim), player_sitting_puff_anim(player_sitting_puff_anim), player_idle_tap_toe_anim(player_idle_tap_toe_anim), player_idle_stomp_anim(player_idle_stomp_anim), player_biking_anim(biking_anim)
						//		:(
					{

					}
				};

				struct PlayerTextureData
				{
					int DefaultSprites = GLOBAL_SPRITE_BASE;	//First sprite for standing still, followed by sprites for walking, running, jumping, falling in air, skidding, and emotions.
					int BagTurnaround = SpriteBase(107);		//The sprite for the player opening their bag, followed by the player facing backwards, and preceeded by the player with no head.
					int Fall = SpriteBase(109);				//The player lying on the ground followed by the player falling over and the player getting up.
					int BasicAttack = SpriteBase(122);		//A basic punching animation.
					int Climbing = SpriteBase(134);			//Climbing backwards animation.
					int IdleAnimations = SpriteBase(148);	//The beginning of all idle animations.
					int Dead = SpriteBase(250);
					int VictoryBalloon = SpriteBase(259);	//The player holding a balloon for the level exit.
					int RidingBike = SpriteBase(265);		//The first sprite of the bike riding animation (2 sprites).

					PlayerAnimationData* AnimationOverride = nullptr;

					PlayerTextureData() 
					{
						//Load default animation data.
						if (AnimationOverride == nullptr)
						{
							AnimationOverride = new PlayerAnimationData(
								Player::AnimationOverride(new int[8] { 1, 2, 3, 2, 1, 4, 5, 4 }, 8, ANIM_12_SPF),		//Default walking.
								Player::AnimationOverride(new int[8] { 6, 7, 8, 9, 10, 11, 12, 13 }, 8, ANIM_16_SPF),	//Default running.
								Player::AnimationOverride(new int[4] { 18, 19, 20, 21 }, 4, ANIM_12_SPF),				//Default falling.
								Player::AnimationOverride(new int[4] { 14, 15, 16, 17 }, 4, ANIM_12_SPF),				//Default jumping.
								Player::AnimationOverride(new int[4] { 22, 23, 24, 25 }, 4, ANIM_12_SPF),				//Default skdding.
								Player::AnimationOverride(new int[4] { 1, 2, 3, 0 }, 4, ANIM_6_SPF),					//Default fall over.
								Player::AnimationOverride(new int[4] { 4, 5, 6, 7 }, 4, ANIM_6_SPF),					//Default get up.
								Player::AnimationOverride(new int[9] { 0, 1, 2, 3, 4, 5, 6, 7, 8 }, 9, ANIM_16_SPF),	//Default basic attack.
								Player::AnimationOverride(new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 }, 8, ANIM_12_SPF),		//Default climbing behind.
								Player::AnimationOverride(new int[5] { 9, 10, 11, 12, 13 }, 5, ANIM_6_SPF),				//Default sitting puff.
								Player::AnimationOverride(new int[4] { 0, 1, 2, 1 }, 4, ANIM_6_SPF),					//Default idle toe tap.
								Player::AnimationOverride(new int[8] { 3, 4, 5, 6, 7, 8, 8 }, 8, ANIM_6_SPF),			//Default idle stomp.
								Player::AnimationOverride(new int[2] { 0, 1 }, 2, ANIM_6_SPF)							//Default biking.
							);
						}
					};

					~PlayerTextureData()
					{
						if (AnimationOverride != nullptr)
						{
							//delete AnimationOverride;
						}
					}

					PlayerTextureData(int defaultSprite, int bagTurnaround, int fall, int basicAttack, int climbing, int idleAnimation, int dead, int victoryBalloon, int ridingBike, PlayerAnimationData* animationOverride = nullptr)
						: DefaultSprites(defaultSprite), BagTurnaround(bagTurnaround), Fall(fall), BasicAttack(basicAttack), Climbing(climbing), AnimationOverride(animationOverride),
						IdleAnimations(idleAnimation), Dead(dead), VictoryBalloon(victoryBalloon), RidingBike(ridingBike)
					{

					}

					PlayerTextureData& operator= (const PlayerTextureData& other)
					{
						DefaultSprites = other.DefaultSprites;
						BagTurnaround = other.BagTurnaround;
						Fall = other.Fall;
						BasicAttack = other.BasicAttack;
						Climbing = other.Climbing;
						IdleAnimations = other.IdleAnimations;
						Dead = other.Dead;
						VictoryBalloon = other.VictoryBalloon;
						RidingBike = other.RidingBike;
						AnimationOverride = other.AnimationOverride;

						return *this;
					}
				};

				/// <summary>
				/// A list of all possible texture palettes the player could have, indexed based on TEXTURE_OFFSETS.
				/// </summary>
				static const PlayerTextureData TextureData[3];

				/// <summary>
				/// Updates the player's texture to use these offsets.
				/// </summary>
				void SetPlayerTextureData(TEXTURE_OFFSETS offsets);

				/// <summary>
				/// Apply some modifier to the player for some amount of time.
				/// </summary>
				/// <param name="attrib"></param>
				void ApplyAttributeModifier(ATTRIBUTE_MODIFIER attrib, float seconds);

				inline void SetAirTime(float airTime)
				{
					m_airTime = airTime;
				}

				void SetHeldItem(Items::InventoryItem* item);
				inline void RemoveHeldItem()
				{
					if (m_heldItemDisplay != nullptr)
					{
						m_heldItemDisplay->SetScale(Vec2(0));
					}
				}

				inline ScreenInventory* GetInventory()
				{
					return m_screenInventory;
				}

				enum PLAYER_ANIMATION_STATE
				{
					NO_LOOK_DIRECTION,
					BEHIND,
					AT_POINT,
					FOLLOW_MOUSE,
					BAG,
					FALLEN,
					SITTING_FORWARD,
					CLIMBING_BACK,	//Climbing with back turned to the camera.
					DEAD,
					VICTORY_BALLOON,
					VICTORY_CONFETTI,
					BIKING
				};

				/// <summary>
				/// Sets an object to freeze the player.
				/// </summary>
				/// <param name="frozen">Freeze the player?</param>
				inline void SetFrozen(bool frozen, PLAYER_ANIMATION_STATE direction = NO_LOOK_DIRECTION, Vec2 point = Vec2::Zero)
				{
					m_frozen += frozen ? 1 : -1;
					if (m_frozen < 0) { m_frozen = 0; }
					SetAnimationState(direction, point);
				}
				
				inline PLAYER_ANIMATION_STATE GetLookDirection()
				{
					return m_currentPlayerLookDirection;
				}

				inline bool GetFrozen()
				{
					return m_frozen > 0;
				}

				/// <summary>
				/// Sets if the player should be rendered to the screen. Useful during cutscenes or events where the player is frozen and shouldn't be seen.
				/// </summary>
				/// <param name="render"></param>
				inline void SetEnableRendering(bool render)
				{
					m_enableRendering = render;
				}

				inline Backpack* const GetBackpack()
				{
					return m_backpack;
				}
				
				inline void SetAnimationState(PLAYER_ANIMATION_STATE direction, Vec2 point = Vec2::Zero)
				{
					m_currentPlayerLookDirection = direction;
					m_playerLookPoint = point;
				}

				inline PLAYER_ANIMATION_STATE GetAnimationState()
				{
					return m_currentPlayerLookDirection;
				}

				void AddVelocity(Vec2 velocity);

				/// <summary>
				/// Set what object the player is currently climbing.
				/// </summary>
				/// <param name="climbing"></param>
				void SetClimbing(GameObject* climbing);

				inline GameObject* GetClimbing()
				{
					return m_climbing;
				}

				void Dive(Vec2 direction, float damage);

				/// <summary>
				/// Checks if the player's backpack or inventory has the item type and returns that item.
				/// </summary>
				/// <returns></returns>
				Inventory::ReturnItem GetItemByType(const ITEM_TYPE& type, int& foundSlot, bool& wasBackpack);

				/// <summary>
				/// Checks if the player's backpack or inventory has the item type and removes it.
				/// </summary>
				/// <returns>Returns the amount of items that could not be removed.</returns>
				int RemoveItemByType(const ITEM_TYPE& type, unsigned int count = 1);

				/// <summary>
				/// Call this after loading the player to remove all HUD overlays.
				/// </summary>
				void HideAllUI();

				enum ControlType
				{
					PLATFORMER, //Default
					ROOM		//Move up and down as well.
				};

				void SetControlType(ControlType type);

				/// <summary>
				/// If true, the player's collider will be shrunk vertically. Useful for ROOM control type.
				/// </summary>
				/// <param name="value"></param>
				inline void SetFootOnlyCollision(bool value)
				{
					m_feetOnlyCollision = value;
				}

			protected:
				void onCollision (Vec2 push, GameObject* gameObject) override;	//Called when a collision occurs.
				void beforeCollision() override;		//Called before any collisions are calculated to allow for resetting the jump conditions.

			private:
				bool m_feetOnlyCollision = false;

				ControlType m_controlType = ControlType::PLATFORMER;

				/// <summary>
				/// Used for room ControlType to store where the player is vertically standing in the room.
				/// </summary>
				int m_storedRoomHeight = 0;

				/// <summary>
				/// Used to hide the player during certain events...
				/// </summary>
				bool m_enableRendering = true;

				/// <summary>
				/// Multiplied by all scale variables to allow for relative scaling. m_scale is utilized to scale the player based on the current sprite, maintaining a similar size.
				/// </summary>
				Vec2 m_scaleMultiplier = 1.f;

				/// <summary>
				/// If the player is currently taking a dive.
				/// </summary>
				float m_diving = 0.f;

				/// <summary>
				/// A list of the positions the player is currently animating to. (Used to program in sequenced events).
				/// </summary>
				std::vector<TargetEvent> m_targetSequence;

				GameObject* m_climbing = nullptr;

				/// <summary>
				/// The player's current texture reference. Can be changed to use different textures.
				/// </summary>
				Player::PlayerTextureData m_textureData;

				std::unordered_map<ATTRIBUTE_MODIFIER, float> m_currentAttribModifiers;

				/// <summary>
				/// Used to turn the player red for some period of time when they are damaged.
				/// </summary>
				float m_damageAnimationTimer = 0.f;

				Lighting::LightingSource* m_playerLight;

				/// <summary>
				/// If using AT_POINT for player look direction, where to look.
				/// </summary>
				Vec2 m_playerLookPoint = Vec2::Zero;
				/// <summary>
				/// The current player look state.
				/// </summary>
				PLAYER_ANIMATION_STATE m_currentPlayerLookDirection = NO_LOOK_DIRECTION;

				/// <summary>
				/// The players current skill data.
				/// </summary>
				SkillHolder m_skillHolder;

				void spawnBloodOnFloor(GameObject* floor);

				/// <summary>
				/// Decodes the given string as player save data.
				/// </summary>
				/// <param name="params"></param>
				void decodeSave(const std::string params);
				/// <summary>
				/// Returns the encoded version of the player save data.
				/// </summary>
				/// <returns></returns>
				std::string encodeSave();


				/// <summary>
				/// Misc state for player save data.
				/// </summary>
				class PlayerSaveState
					: public MiscState
				{
				public:
					PlayerSaveState(Player* player)
						: m_player(player)
					{
						
					}

					MiscState::SaveParam Encode() override final
					{
						return m_player->encodeSave();
					}

					void Decode(const MiscState::SaveParam param) override final
					{
						m_player->decodeSave(param);
					}

				private:
					Player* const m_player;
				};

				PlayerSaveState* const m_saveState;			//Stores all data for the players current save state.

				/// <summary>
				/// The players current backpack that can be used to store items and equip gear.
				/// </summary>
				Backpack* m_backpack;

				/// <summary>
				/// Used to track when blood should spawn if the player is dead.
				/// </summary>
				double m_deadBloodTimer = 0.0;

				/// <summary>
				/// Drops the currently held item and returns true if the item was sucussfully dropped.
				/// </summary>
				/// <returns></returns>
				bool dropHeldItem();

				/// <summary>
				/// Updates all the player's animations to use the current m_textureData.
				/// </summary>
				void registerAnimations();

				/// <summary>
				/// If the player is frozen they cannot move but can still be affected by gravity or other objects.
				/// The value of frozen can go above 1, that way if multiple objects are currently freezing the player, the player will remain frozen until all objects have stopped freezing the player.
				/// </summary>
				int8_t m_frozen = 0;


				/// <summary>
				/// If set to some value, will tick down each frame until 0 and then will unfreeze player.
				/// </summary>
				float m_frozenTimer = -1.f;

				GameObject* m_heldItemDisplay;				//Follows the player to display the current held item.
				int m_heldItemDisplayFrameOffset = 0;		//How many sprite difference from the default held item sprite to its current animation frame.
				std::shared_ptr<Sprite> m_heldItemLastSprite;

				ScreenInventory* const m_screenInventory;	//Player's three main inventory slots.

				/// <summary>
				/// Represents data about the players stamina, health, etc.
				/// </summary>
				struct {
					float Health = 100.f;
					float AbilityMeter = 100.f;
				} m_stats;

				ProgressBar* m_healthProgressBar;		//Progress bar for displaying health.
				ProgressBar* m_abilityMeterProgressBar;	//Progress bar for ability meter.

				Particles::ParticleEmitter* const m_particleEmitter;

				bool m_onGround = false;				//If the player is on the ground.
				bool m_foundCollisionInTick = false;	//Used for checking when the player leaves the ground.
				bool m_swimming = false;				//If the player is swimming.
				int m_jumpHeld = 0;						//Used for determining how many frames the jump button has been held after the player jumps.
				bool m_begunMotion = false;				//Used for animation checks.
				double m_playingIdleAnimation = 0;		//Set to a timer if the player is engaged in an idle activity (e.g. tapping toe).
				double m_timeSpentNotMoving = 0;		//The amount of time the player has spent not moving.

				double m_airTime = 0;					//Time the player has spent in the air. (Used to calculate fall damage)

				bool m_isFlying = false;				//True if the player is currently flying.

				AnimatorComponent* m_animator;			//Animator for walking, jumping, etc.
				Components::ChildGameObject* m_emotionsObject;	//Object overlayed on the player for expressions, etc.

				void readKeys();						//Called to determine what buttons are pressed and apply velocity based on those buttons.
				void setAnimations(bool playerIsSkidding, float& anim_momentum);	//Called to determine what animation the player should be playing.
				void updateAnimationState();				//Used to update player look state.


#if _DEBUG
				bool m_debug = false, m_debugKey = false, m_flight = false;
#endif
				
				inline void enableUnderwaterMusic() const	//Pauses the current level music and starts the underwater music in the same position.
				{
					Audio::AudioPlayer* player = Audio::SoundManager::GetAudioPlayer();

					Audio::TimeSeconds ts = player->getStreamTime(Audio::SoundManager::BGMusic);
					player->setPause(Audio::SoundManager::BGMusic, true);

					player->seek(Audio::SoundManager::WaterMusic, ts);
					player->setPause(Audio::SoundManager::WaterMusic, false);
				}

				inline void disableUnderwaterMusic() const	//Pauses the underwater music and starts the current level music in the same position.
				{
					Audio::AudioPlayer* player = Audio::SoundManager::GetAudioPlayer();

					Audio::TimeSeconds ts = player->getStreamTime(Audio::SoundManager::WaterMusic);
					player->setPause(Audio::SoundManager::WaterMusic, true);

					player->seek(Audio::SoundManager::BGMusic, ts);
					player->setPause(Audio::SoundManager::BGMusic, false);
				}

				/// <summary>
				/// Updates the ability meter stat and corrseponding graphics.
				/// </summary>
				/// <param name="amount">[float] - How much to add to the ability meter.</param>
				void updateAbilityMeter(float amount);

				void togglePlayerLight();

				/// <summary>
				/// Counts down between attacks to prevent spamming one attack.
				/// </summary>
				double m_attackCooldown;
				void handleAttack();
			};

		};
	};
};

#endif