#include "Cog.h"

#include "../../../Utils/Time/GameTime.h"
#include "../../../Rendering/Renderers/Renderer.h"

#include "../../../Input/InputManager.h"
#include "../../TestGame.h"

#include "../../../Utils/CollisionDetection.h"

#include "../../Items/FloorItem.h"


//Gets multiplied with all rotating gears' speeds.
#define GEAR_SPEED_MULTIPLIER 100.f

//How far inward of the gear bounding box is used to show the teeth connecting.
#define GEAR_TEETH_OFFSET_PERCENT 0.12f

//How much of the rotation 1 tooth represents.
#define GEAR_TEETH_ROTATION_OFFSET 16.f

namespace GAME_NAME::Objects::Mechanical
{
	Cog::DisplayCog Cog::m_displayCog;

	std::vector<Cog*> Cog::m_cogs;

	double Cog::m_cogPlaceTimer = 0.0;
	double Cog::m_cogRemoveTimer = 0.0;

	std::unordered_map<int, const int> Cog::CogSpriteReference = {
		{ 38, 39 },
		{ SpriteBase(441), SpriteBase(442) }
	};

	Cog::CogConnection Cog::OppositeConnection(const CogConnection& connection)
	{
		switch (connection)
		{
		case CogConnection::Up:
			return CogConnection::Down;
		case CogConnection::Down:
			return CogConnection::Up;
		case CogConnection::Left:
			return CogConnection::Right;
		case CogConnection::Right:
			return CogConnection::Left;
		case CogConnection::Up_Right:
			return CogConnection::Down_Left;
		case CogConnection::Up_Left:
			return CogConnection::Down_Right;
		case CogConnection::Down_Right:
			return CogConnection::Up_Left;
		case CogConnection::Down_Left:
			return CogConnection::Up_Right;
		}
	}

	Cog::Cog(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, bool counterClockwise, uint8_t gearRatio, float initalRotationOffset, bool isRotationSource)
		: GameObject(position, scale, sprite), m_gearRatio(gearRatio), m_counterClockwise(counterClockwise), m_isRotationSource(isRotationSource)
	{
		//Add cog to the cog vector.
		m_cogs.emplace_back(this);
		RotateAboutCenter(initalRotationOffset);
	}

	Cog::~Cog()
	{

		//Remove cog from the cog vector and remove any cogs connections to this cog.
		for (size_t i = 0; i < m_cogs.size(); i++)
		{
			if (m_cogs[i] == this)
			{
				m_cogs.erase(m_cogs.begin() + i);
				continue;
			}

			//Remove any connections to this cog if they exist.
			m_cogs[i]->removeConnection(this);

			i++;
		}
	}

	void Cog::DetectPlacingCog(uint8_t gearRatio)
	{
		Vec2 mouseWorldPos = InputManager::GetMouseWorldPosition(TestGame::INSTANCE->GetCamera());
		
		Cog* cog = nullptr;
		float cogDistance = 100.f;

		for (Cog* check : m_cogs)
		{
			float distance = Vec2::Distance(check->GetPosition() + check->GetScale() / 2.f, mouseWorldPos);
			if (cogDistance > distance)
			{
				cog = check;
				cogDistance = distance;
			}
		}

		if (cog == nullptr) { return; }
		
		//Different ratios? place diagonally.
		bool placingDiagonal = cog->m_gearRatio != gearRatio;

		//Find the closest connection point to the mouse.
		std::vector<std::tuple<Vec2, CogConnection>> cogConnectionPoints;

		if (placingDiagonal)
		{
			cogConnectionPoints = {
				{ cog->GetPosition() + Vec2{ cog->GetScale().X, cog->GetScale().Y }, CogConnection::Up_Right },
				{ cog->GetPosition() + Vec2{ -cog->GetScale().X, cog->GetScale().Y }, CogConnection::Up_Left },
				{ cog->GetPosition() + Vec2{ cog->GetScale().X, -cog->GetScale().Y }, CogConnection::Down_Right },
				{ cog->GetPosition() + Vec2{ -cog->GetScale().X, -cog->GetScale().Y }, CogConnection::Down_Left }
			};
		}
		else {
			cogConnectionPoints = {
			{ cog->GetPosition() + Vec2{ cog->GetScale().X, 0.f }, CogConnection::Right },
			{ cog->GetPosition() - Vec2{ cog->GetScale().X, 0.f }, CogConnection::Left },
			{ cog->GetPosition() + Vec2{ 0.f, cog->GetScale().Y }, CogConnection::Up },
			{ cog->GetPosition() - Vec2{ 0.f, cog->GetScale().Y }, CogConnection::Down }
			};
		}

		std::tuple<float, CogConnection> winningPoint = std::make_tuple(1000.f, placingDiagonal ? CogConnection::Up_Right : CogConnection::Right);

		for (std::tuple<Vec2, CogConnection>& point : cogConnectionPoints)
		{
			float distance = Vec2::Distance(std::get<Vec2>(point), mouseWorldPos);
			if (distance < std::get<float>(winningPoint))
			{
				winningPoint = std::make_tuple(distance, std::get<CogConnection>(point));
			}
		}

		Cog::displayCogPlaceGraphic(cog, std::get<CogConnection>(winningPoint));
		
	}

	void Cog::HidePlacingCog()
	{
		m_displayCog.ReferenceCog = nullptr;
		m_displayCog.Sprite = nullptr;
	}

	void Cog::Update(GLFWwindow* window)
	{
		//Detect if we are trying to remove the current cog.
		if (m_displayCog.ReferenceCog == this && m_displayCog.Sprite != nullptr)
		{
			if (InputManager::GetKeyUpDown(keyRef::PLAYER_ATTACK) & InputManager::KEY_STATE::KEY_STATE_HELD)
			{
				m_cogRemoveTimer += Utils::Time::GameTime::DeltaTime::GetDeltaTime();

				if (m_cogRemoveTimer > 1.0)
				{
					//Remove cog.
					DestroyAndUpdateAdjacent();
					m_cogRemoveTimer = 0.0;

					//Give player the corresponding item.
					if (TestGame::ThePlayer->GetInventory()->GetItem(TestGame::ThePlayer->GetInventory()->GetSelectedSlot() - 1).ri_IsNull)
					{
						TestGame::ThePlayer->GetInventory()->SetItem(TestGame::ThePlayer->GetInventory()->GetSelectedSlot() - 1, new InventoryItem(m_gearRatio == 2 ? ITEM_TYPE::LARGE_WOODEN_COG : ITEM_TYPE::SMALL_WOODEN_COG));
					}
					else {
						FloorItem* droppedCog = new FloorItem(m_position, new InventoryItem(m_gearRatio == 2 ? ITEM_TYPE::LARGE_WOODEN_COG : ITEM_TYPE::SMALL_WOODEN_COG), 0.5f);
						Renderer::InstantiateObject(Renderer::InstantiateGameObject(droppedCog, true, 2, false));
					}
					

					return;
				}


			}
			else {
				m_cogRemoveTimer = 0.0;
			}
		}


		if (m_cogPlaceTimer >= 0.0)
		{
			if (m_displayCog.ReferenceCog == this && m_displayCog.Sprite != nullptr && (InputManager::GetKeyUpDown(keyRef::PLAYER_USE_ITEM) & InputManager::KEY_STATE::KEY_STATE_HELD))
			{
				m_cogPlaceTimer += Utils::Time::GameTime::DeltaTime::GetDeltaTime() * 2.0;

				//Update alpha of display.
				float alpha = static_cast<float>(m_cogPlaceTimer == 0.0 ? 1.0 : m_cogPlaceTimer);
				const Vec4 Cog_PlaceGraphicColor[4] = {
					Vec4{ 0.75f, 0.75f, 1.f, alpha },
					Vec4{ 0.75f, 0.75f, 1.f, alpha },
					Vec4{ 0.75f, 0.75f, 1.f, alpha },
					Vec4{ 0.75f, 0.75f, 1.f, alpha }
				};

				m_displayCog.Sprite->UpdateTextureColor(Cog_PlaceGraphicColor);

				if (m_cogPlaceTimer > 1.0)
				{
					/*
					* ----HELD KEY ENOUGH, PLACE A GEAR----
					*/

				  //Create the new cog and reset the display data.
					m_displayCog.ReferenceCog->CreateAdjacentCog(m_displayCog.Direction);
					m_displayCog.Direction = CogConnection::Right;
					m_displayCog.DisplayOffset = Vec2{ 0.f, 0.f };
					m_displayCog.DisplayRotation = 0.f;
					m_displayCog.ReferenceCog = nullptr;
					m_displayCog.Sprite.release(); m_displayCog.Sprite = nullptr;

					//Remove held item from player (which would be the cog.)
					TestGame::ThePlayer->GetInventory()->SetItem(TestGame::ThePlayer->GetInventory()->GetSelectedSlot() - 1, nullptr);

					//Setting the timer negative will cause it to wait until the mouse button is released to allow more placing.
					m_cogPlaceTimer = -1.0;
				}
			} 
			else if(m_displayCog.ReferenceCog == this){
				m_cogPlaceTimer = 0.0;

				if (m_displayCog.Sprite != nullptr)
				{
					//Update alpha of display.
					float alpha = 1.f;
					const Vec4 Cog_PlaceGraphicColor[4] = {
						Vec4{ 0.75f, 0.75f, 1.f, alpha },
						Vec4{ 0.75f, 0.75f, 1.f, alpha },
						Vec4{ 0.75f, 0.75f, 1.f, alpha },
						Vec4{ 0.75f, 0.75f, 1.f, alpha }
					};

					m_displayCog.Sprite->UpdateTextureColor(Cog_PlaceGraphicColor);
				}
			}
		}
		//Ensure gears are not spammed if the key is held.
		else if (InputManager::GetKeyUpDown(keyRef::PLAYER_USE_ITEM) & InputManager::KEY_STATE::KEY_STATE_RELEASED)
		{
			m_cogPlaceTimer = 0.0;
		}

		if (m_isRotationSource)
		{
			//Rotation speed will be some constant based on gear ratio if this is a source.
			m_gearSpeed = 1.f / m_gearRatio;
			RotateAboutCenter((GEAR_SPEED_MULTIPLIER * m_gearSpeed) * static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * (m_counterClockwise ? -1.f : 1.f));
		}
		else if(m_connections.size() > 0 || m_attachedBelt){
			
			//Set to 0 so it can update based on the first non-zero connection.
			m_gearSpeed = 0.f;
			m_isRecievingRotationPower = m_isRotationSource;


			//If a belt is connected, inherit the belt speed.
			if (m_attachedBelt)
			{
				if (m_attachedBelt->GetBeltSpeed() > 0.f)
				{
					m_isRecievingRotationPower = true;
					m_gearSpeed = m_attachedBelt->GetBeltSpeed();
				}
			}

			//If other connections exist with this gear, they must have the same speed or the gear will break.
			for (int i = 0; i < m_connections.size(); i++)
			{
				//If a connected gear has 0 speed, it just needs to update itself to figure out its speed.
				if (std::get<Cog*>(m_connections[i])->GetGearSpeed() == 0.f) { continue; }

				//If connected to a power source or connected to a gear that is connected to a power source, this gear must be powered.
				if (!m_isRecievingRotationPower && (std::get<Cog*>(m_connections[i])->m_isRecievingRotationPower || std::get<Cog*>(m_connections[i])->m_isRotationSource))
				{
					m_isRecievingRotationPower = true;
				}

				float ratio = ((float)std::get<Cog*>(m_connections[i])->GetGearRatio() / (float)m_gearRatio);
				float otherSpeedNormalized = ratio * std::get<Cog*>(m_connections[i])->GetGearSpeed();

				//Rotation speed will be based on the ratio between the first non-zero connection that exists for this gear.
				if (m_gearSpeed == 0.f)
				{
					m_gearSpeed = otherSpeedNormalized;
					continue;
				}

				//This means a bad gear connection was made (two systems with different speeds!)
				if (otherSpeedNormalized != m_gearSpeed)
				{
					//TODO: Gear breaking removal animation.
					DestroyAndUpdateAdjacent();
					return;
				}
			}

			//If no gear in this chain was found to be a rotation source, this gear should not be recieving motion.
			if (!m_isRecievingRotationPower) { m_gearSpeed = 0.f; return; }

			//Upodate rotation.
			RotateAboutCenter((GEAR_SPEED_MULTIPLIER * m_gearSpeed) * static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * (m_counterClockwise ? -1.f : 1.f));
		}
		else {
			//If the gear is not connected to anything, it should get 0 speed.
			m_gearSpeed = 0.f;
		}
		
		//Rotate the display cog in the oposite direction if this is the currently hovered cog.
		if (m_displayCog.ReferenceCog == this)
		{
			float ratio = 1.f;
			if (m_displayCog.Direction == CogConnection::Up_Left || m_displayCog.Direction == CogConnection::Up_Right || m_displayCog.Direction == CogConnection::Down_Left || m_displayCog.Direction == CogConnection::Down_Right)
			{
				if (m_gearRatio == 2)
				{
					ratio = 2.f;
				}
				else {
					ratio = 0.5f;
				}
			}

			GameObject calculateObject(Vec2{ 0.f }, m_scale * (1.f / ratio), nullptr, 0.f);

			m_displayCog.DisplayRotation += (GEAR_SPEED_MULTIPLIER * m_gearSpeed * ratio) * static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * (!m_displayCog.ReferenceCog->IsCounterClockwise() ? -1.f : 1.f);
			calculateObject.RotateAboutCenter(m_displayCog.DisplayRotation);
			m_displayCog.DisplayOffset = calculateObject.GetRotationOffset();
		}
	}



	void Cog::Render(const Vec2& cameraPosition)
	{
		if (m_displayCog.ReferenceCog == this && m_displayCog.Sprite != nullptr)
		{
			const Vec2 ogScale = m_displayCog.ReferenceCog->GetScale();
			Vec2 scale = ogScale;

			float offsetMult = 1.f;

			if (m_displayCog.Direction == CogConnection::Up_Left || m_displayCog.Direction == CogConnection::Up_Right || m_displayCog.Direction == CogConnection::Down_Left || m_displayCog.Direction == CogConnection::Down_Right)
			{
				if (m_displayCog.ReferenceCog->GetGearRatio() == 1)
				{
					scale = scale * 2.f;
					offsetMult = 1.9f;
				}
				else {
					scale = scale * 0.5f;
				}
			}

			Vec2 position = m_position + (m_scale / 2.f) - (scale / 2.f) + (m_displayCog.ReferenceCog->GetAdjacentCogOffset(m_displayCog.Direction) * offsetMult);

			std::vector<std::tuple<CogInteraction, Cog*>> interactions = GetInteractingCogs(position, scale, offsetMult < 1.5f ? 1 : 2);
			bool failRender = false;
			for (auto&[interaction, cog] : interactions)
			{
				if (cog == m_displayCog.ReferenceCog) { continue; }
				
				if (interaction.IsOverlap)
				{
					failRender = true; break;
				}
			}

			if (!failRender)
			{
				m_displayCog.Sprite->Render(cameraPosition, position + m_displayCog.DisplayOffset, scale, m_displayCog.DisplayRotation);
			}
			else {
				m_displayCog.Sprite = nullptr;
				m_displayCog.ReferenceCog = nullptr;
			}
		}

		GameObject::Render(cameraPosition);

		if (m_displayCog.ReferenceCog == this)
		{
			DynamicSprite selectSprite(m_sprite->GetSpriteId());

			float alpha = m_cogRemoveTimer == 0.0 ? 0.8f : static_cast<float>(m_cogRemoveTimer);
			const Vec4 Cog_SelectGraphicColor[4] = {
			Vec4{ 1.f, 0.5f, 0.5f, alpha },
			Vec4{ 1.f, 0.5f, 0.5f, alpha },
			Vec4{ 1.f, 0.5f, 0.5f, alpha },
			Vec4{ 1.f, 0.5f, 0.5f, alpha }
			};

			selectSprite.UpdateTextureColor(Cog_SelectGraphicColor);

			selectSprite.Render(cameraPosition, m_position + GetRotationOffset(), m_scale, m_rotation);
		}
	}

	void Cog::DestroyAndUpdateAdjacent()
	{
		if (m_attachedBelt)
		{
			m_attachedBelt->RemoveConnection(this);
			m_attachedBelt = nullptr;
		}

		//Remove cog from the cog vector and remove any cogs connections to this cog.
		for (size_t i = 0; i < m_cogs.size(); i++)
		{
			if (m_cogs[i] == this)
			{
				m_cogs.erase(m_cogs.begin() + (i--));
				continue;
			}

			//Remove any connections to this cog if they exist.
			m_cogs[i]->removeConnection(this);
		}

		Renderer::DestroyObject(this);
	}


	Vec2 Cog::GetAdjacentCogOffset(CogConnection direction)
	{
		Vec2 offset{ 0.f, 0.f };

		switch (direction)
		{
		case CogConnection::Up:
			offset.Y = m_scale.Y;
			break;
		case CogConnection::Down:
			offset.Y = -m_scale.Y;
			break;
		case CogConnection::Left:
			offset.X = -m_scale.X;
			break;
		case CogConnection::Right:
			offset.X = m_scale.X;
			break;

		case CogConnection::Down_Right:
			offset = { m_scale.X * 0.6f, -m_scale.Y * 0.6f };
			break;
		case CogConnection::Down_Left:
			offset = { -m_scale.X * 0.6f, -m_scale.Y * 0.6f };
			break;
		case CogConnection::Up_Right:	
			offset = { m_scale.X * 0.6f, m_scale.Y * 0.6f };
			break;
		case CogConnection::Up_Left:
			offset = { -m_scale.X * 0.6f, m_scale.Y * 0.6f };
			break;
		}

		offset = offset * Vec2{ 1.f - GEAR_TEETH_OFFSET_PERCENT, 1.f - GEAR_TEETH_OFFSET_PERCENT };

		return offset;
	}

	Cog* Cog::CreateAdjacentCog(CogConnection direction)
	{
		Cog* newCog = nullptr;
		Vec2 offset = GetAdjacentCogOffset(direction);

		//Determine the newCog.
		if (direction == CogConnection::Up || direction == CogConnection::Down || direction == CogConnection::Left || direction == CogConnection::Right)
		{
			//Same ratio if placing to the side.
			const uint8_t& gearRatio = this->m_gearRatio;

			newCog = new Cog(m_position + offset, m_scale, new Rendering::Sprite(m_sprite->GetSpriteId()), !m_counterClockwise, gearRatio, m_rotation + GEAR_TEETH_ROTATION_OFFSET);
		}
		else {
			//Opposite ratio if placing diagonal.
			const uint8_t gearRatio = this->m_gearRatio == 2 ? 1 : 2;

			//Find the respective large cog sprite.
			Sprite* sprite = new Sprite(m_sprite->GetSpriteId());
			for (auto& [smallCog, largeCog] : Cog::CogSpriteReference)
			{
				if (Renderer::GetSpriteIdFromTextureId(smallCog) == m_sprite->GetSpriteId())
				{
					delete sprite;
					sprite = Renderer::GetSprite(largeCog);
					break;
				}

				if (Renderer::GetSpriteIdFromTextureId(largeCog) == m_sprite->GetSpriteId())
				{
					delete sprite;
					sprite = Renderer::GetSprite(smallCog);
					break;
				}
			}

			Vec2 scale = gearRatio == 2 ? m_scale * 2.f : m_scale * 0.5f;

			//New cog spins in opposite direction.
			newCog = new Cog(m_position + (m_scale / 2.f) - (scale / 2.f) + (m_displayCog.ReferenceCog->GetAdjacentCogOffset(m_displayCog.Direction) * (gearRatio == 2 ? 1.9f : 0.95f)), scale, sprite, !m_counterClockwise, gearRatio, m_rotation + GEAR_TEETH_ROTATION_OFFSET);
		}

		if (newCog == nullptr) { return nullptr; }

		//Find any other necessary connections.
		std::vector<std::tuple<CogInteraction, Cog*>> interactions = GetInteractingCogs(newCog);

		for (auto& [interaction, cog] : interactions)
		{
			//Already going to add this connection.
			if (cog == this) { continue; }
			//Unrelated cogs.
			if (interaction.Distant) { continue; }

			if (interaction.IsOverlap)
			{
				//Uh oh invalid placement.
				delete newCog;
				return nullptr;
			}

			//If we reach this point, the cogs are connected.
			newCog->addConnection(cog, Cog::OppositeConnection(interaction.Connected));
			cog->addConnection(newCog, interaction.Connected);
		}

		Rendering::Renderer::InstantiateObject(Renderer::InstantiateGameObject(newCog, false, 2, false));

		newCog->addConnection(this, Cog::OppositeConnection(direction));
		this->addConnection(newCog, direction);

		return newCog;

	}

	Cog::CogInteraction Cog::GetInteraction(Cog* otherCog)
	{
		return Cog::GetInteraction(otherCog->m_position, otherCog->m_scale, otherCog->m_gearRatio);
	}

	Cog::CogInteraction Cog::GetInteraction(Vec2 bottomLeft, Vec2 scale, uint8_t gearRatio)
	{

		float distance = Vec2::Distance(this->m_position + this->m_scale / 2.f, bottomLeft + scale / 2.f);
		//Cogs overlap past the teeth.
		if (distance < ((scale.X + m_scale.X) / 2.f ) * (1.f - GEAR_TEETH_OFFSET_PERCENT))
		{
			//Overlapping.
			return Cog::CogInteraction{ true, false, CogConnection::Right };
		}
		else if (distance < ((scale.X + m_scale.X) / 2.f))
		{
			//Teeth are interacting.

			if (gearRatio == m_gearRatio)
			{
				//Determine what side the gears are interacting on.
				if (bottomLeft.X + scale.X /2.f > this->m_position.X + this->m_scale.X)
				{
					return Cog::CogInteraction{ false, false, CogConnection::Right };
				}
				else if (bottomLeft.X + scale.X / 2.f < this->m_position.X)
				{
					return Cog::CogInteraction{ false, false, CogConnection::Left };
				}
				else if (bottomLeft.Y + scale.Y / 2.f > this->m_position.Y + this->m_scale.Y)
				{
					return Cog::CogInteraction{ false, false, CogConnection::Up };
				}
				else if (bottomLeft.Y + scale.Y / 2.f < this->m_position.Y)
				{
					return Cog::CogInteraction{ false, false, CogConnection::Down };
				}
			}
			else {
				if (bottomLeft.X + scale.X / 2.f > this->m_position.X + this->m_scale.X && bottomLeft.Y + scale.Y / 2.f > this->m_position.Y + this->m_scale.Y)
				{
					return Cog::CogInteraction{ false, false, CogConnection::Up_Right };
				}
				else if (bottomLeft.X + scale.X / 2.f > this->m_position.X + this->m_scale.X && bottomLeft.Y + scale.Y / 2.f < this->m_position.Y)
				{
					return Cog::CogInteraction{ false, false, CogConnection::Down_Right };
				}
				else if (bottomLeft.X + scale.X / 2.f < this->m_position.X && bottomLeft.Y + scale.Y / 2.f > this->m_position.Y + this->m_scale.Y)
				{
					return Cog::CogInteraction{ false, false, CogConnection::Up_Left };
				}
				else if (bottomLeft.X + scale.X / 2.f < this->m_position.X && bottomLeft.Y + scale.Y / 2.f < this->m_position.Y)
				{
					return Cog::CogInteraction{ false, false, CogConnection::Down_Left };
				}
			}

		}

		//No relation between cogs.
		return Cog::CogInteraction{ false, true, CogConnection::Right };
	}

	void Cog::SetBelted(Belt* belt)
	{
		m_attachedBelt = belt;
	}

	std::vector<std::tuple<Cog::CogInteraction, Cog*>> Cog::GetInteractingCogs(Vec2 bottomLeft, Vec2 scale, uint8_t gearRatio)
	{
		std::vector<std::tuple<CogInteraction, Cog*>> cogs;

		for (Cog* cog : m_cogs)
		{
			//This is the cog we are checking.
			if (cog->m_position.X == bottomLeft.X && cog->m_position.Y == bottomLeft.Y) { continue; }

			cogs.emplace_back(cog->GetInteraction(bottomLeft, scale, gearRatio), cog);
		}

		return cogs;
	}

	std::vector<std::tuple<Cog::CogInteraction, Cog*>> Cog::GetInteractingCogs(Cog* cog)
	{
		return GetInteractingCogs(cog->m_position, cog->m_scale, cog->m_gearRatio);
	}

	bool Cog::removeConnection(Cog* cog)
	{
		for (size_t c = 0; c < m_connections.size(); c++)
		{
			if (std::get<Cog*>(m_connections[c]) == cog)
			{
				m_connections.erase(m_connections.begin() + c);

				//Remove rotation power from all cogs since we now need to update again to determine if these cogs are actually supposed to be spinning.
				//i.e. if you removed a cog in the middle of a chain, one side of the chain should probably stop.
				for (Cog* cog : m_cogs)
				{
					cog->m_isRecievingRotationPower = false;
				}
				
				return true;
			}
		}

		return false;
	}

	bool Cog::removeConnection(CogConnection direction)
	{
		for (size_t c = 0; c < m_connections.size(); c++)
		{
			if (std::get<CogConnection>(m_connections[c]) == direction)
			{
				m_connections.erase(m_connections.begin() + c);

				//Remove rotation power from all cogs since we now need to update again to determine if these cogs are actually supposed to be spinning.
				//i.e. if you removed a cog in the middle of a chain, one side of the chain should probably stop.
				for (Cog* cog : m_cogs)
				{
					cog->m_isRecievingRotationPower = false;
				}

				return true;
			}
		}

		return false;
	}

	void Cog::displayCogPlaceGraphic(Cog* cog, CogConnection direction)
	{
		bool failedOverlapCheck = false;

		//Check if a cog already exists in the spot we are trying to display.
		for (std::tuple<Cog*, CogConnection> connection : cog->GetConnections())
		{
			//Cannot place cog overlaping another cog.
#pragma region No Overlaps
			if (direction == CogConnection::Up_Left || direction == CogConnection::Down_Left)
			{
				if (std::get<CogConnection>(connection) == CogConnection::Left)
				{
					failedOverlapCheck = true;
					break;
				}
			}
			if (direction == CogConnection::Up_Right || direction == CogConnection::Down_Right)
			{
				if (std::get<CogConnection>(connection) == CogConnection::Right)
				{
					failedOverlapCheck = true;
					break;
				}
			}
			if (direction == CogConnection::Up_Left || direction == CogConnection::Up_Right)
			{
				if (std::get<CogConnection>(connection) == CogConnection::Up)
				{
					failedOverlapCheck = true;
					break;
				}
			}
			if (direction == CogConnection::Down_Left || direction == CogConnection::Down_Right)
			{
				if (std::get<CogConnection>(connection) == CogConnection::Down)
				{
					failedOverlapCheck = true;
					break;
				}
			}
			if (direction == CogConnection::Down)
			{
				if (std::get<CogConnection>(connection) == CogConnection::Down_Left || std::get<CogConnection>(connection) == CogConnection::Down_Right)
				{
					failedOverlapCheck = true;
					break;
				}
			}
			if (direction == CogConnection::Up)
			{
				if (std::get<CogConnection>(connection) == CogConnection::Up_Left || std::get<CogConnection>(connection) == CogConnection::Up_Right)
				{
					failedOverlapCheck = true;
					break;
				}
			}
			if (direction == CogConnection::Left)
			{
				if (std::get<CogConnection>(connection) == CogConnection::Down_Left || std::get<CogConnection>(connection) == CogConnection::Up_Left)
				{
					failedOverlapCheck = true;
					break;
				}
			}
			if (direction == CogConnection::Right)
			{
				if (std::get<CogConnection>(connection) == CogConnection::Down_Right || std::get<CogConnection>(connection) == CogConnection::Up_Right)
				{
					failedOverlapCheck = true;
					break;
				}
			}
#pragma endregion

			if (std::get<CogConnection>(connection) == direction)
			{
				failedOverlapCheck = true;
				break;
			}
		}

		if (failedOverlapCheck)
		{
			m_displayCog.ReferenceCog = nullptr;
			m_displayCog.Sprite = nullptr;

			return;
		}

		unsigned int spriteId = cog->GetSprite()->GetSpriteId();

		//If placing diagonal, find the correct diagonal sprite.
		if (direction == CogConnection::Up_Left || direction == CogConnection::Up_Right || direction == CogConnection::Down_Left || direction == CogConnection::Down_Right)
		{
			//Get the texture for the other cog if we are trying to place diagonally.
			for (auto& [smallCog, bigCog] : Cog::CogSpriteReference)
			{
				if (Renderer::GetSpriteIdFromTextureId(smallCog) == spriteId)
				{
					spriteId = Renderer::GetSpriteIdFromTextureId(bigCog);
					break;
				}

				if (Renderer::GetSpriteIdFromTextureId(bigCog) == spriteId)
				{
					spriteId = Renderer::GetSpriteIdFromTextureId(smallCog);
					break;
				}
			}
		}


		/// <summary>
		/// Color for the cog preview displayed when placing.
		/// </summary>
		const Vec4 Cog_PlaceGraphicColor[4] = {
			Vec4{ 0.75f, 0.75f, 1.f, 1.f },
			Vec4{ 0.75f, 0.75f, 1.f, 1.f },
			Vec4{ 0.75f, 0.75f, 1.f, 1.f },
			Vec4{ 0.75f, 0.75f, 1.f, 1.f }
		};


		//If the sprite needs to be changed, change it.
		if (m_displayCog.Sprite == nullptr)
		{
			m_displayCog.Sprite = std::make_unique<DynamicSprite>(spriteId);
			m_displayCog.Sprite->UpdateTextureColor(Cog_PlaceGraphicColor);
			m_displayCog.DisplayRotation = cog->GetRotation() + GEAR_TEETH_ROTATION_OFFSET;
		}
		else if(m_displayCog.Sprite->GetSpriteId() != spriteId){
			m_displayCog.Sprite.reset(new DynamicSprite(spriteId));
			m_displayCog.Sprite->UpdateTextureColor(Cog_PlaceGraphicColor);
			m_displayCog.DisplayRotation = cog->GetRotation() + GEAR_TEETH_ROTATION_OFFSET;
		}

		//If anything changed about the selected cog, we need to update the inital rotation of the display cog.
		if (direction != m_displayCog.Direction || m_displayCog.ReferenceCog != cog)
		{
			m_displayCog.DisplayRotation = cog->GetRotation() + GEAR_TEETH_ROTATION_OFFSET;
		}

		if (cog->m_gearSpeed == 0.f)
		{
			m_displayCog.DisplayRotation = 0.f;
		}

		m_displayCog.ReferenceCog = cog;
		m_displayCog.Direction = direction;
		m_displayCog.DisplayOffset = Vec2{ 0.f };
	}


}
