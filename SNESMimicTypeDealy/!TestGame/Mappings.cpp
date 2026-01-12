#include "Mappings.h"
#include "ItemMapping.h"	
#include "../Components/Physics/GravityComponent.h"
#include "../Components/Physics/Collision/StaticBoxCollider.h"
#include "../Components/Physics/Collision/Helpers/ActiveBoxCollisionGravityObject.h"
#include "../Components/Physics/Collision/Helpers/StaticBoxCollisionObject.h"
#include "../Components/Physics/Collision/ActiveBoxCollider.h"
#include "../!TestGame/Objects/Environment/Water.h"
#include "../!TestGame/Objects/Environment/BGParallax.h"

#include "../!TestGame/Objects/Platforms/RotatingPlatform.h"
#include "../!TestGame/Objects/Platforms/FallingPlatform.h"

#include "../!TestGame/Objects/Environment/Buildings/SpinningObject.h"

#include "../Objects/Instantiate/LevelObjectHandler.h"

#include "../!TestGame/Items/FloorItem.h"
#include "../!TestGame/Items/Types/Tool.h"
#include "../!TestGame/Objects/Environment/Plants/Tree.h"
#include "../!TestGame/Items/Inventories/InventoryContainer.h"

#include "../!TestGame/Objects/Enemies/Types/LeftRightEnemy.h"
#include "../!TestGame/Objects/Enemies/Types/PassiveRabbit.h"
#include "../!TestGame/Objects/Enemies/Types/FeralRabbit.h"

#include "../!TestGame/Cutscenes/InnerThoughtScene.h"
#include "../!TestGame/Cutscenes/CutsceneManager.h"

#if _DEBUG
#include "../Debug/DebugLog.h"
#define DebugMapper(x) {std::string s = "[MAPPER] ";DEBUG::DebugLog::Log(s + x, true, ";35");}
#endif

#include "../Components/Physics/Collision/Helpers/PixelPerfectColliderStaticObject.h"
#include "../Objects/Particles/ParticleEmitter.h"

#include "./Objects/Environment/Buildings/FrontWall.h"
#include "./Objects/Environment/Buildings/Door.h"
#include "./Objects/Environment/Buildings/FrontDoor.h"
#include "./Objects/Environment/Buildings/Bench.h"
#include "./Objects/Environment/Buildings/SaggingObject.h"
#include "./Objects/Environment/Buildings/InnerUpperWall.h"

#include "./Objects/Environment/BreakableBlock.h"

#include "./Objects/Collectables/ToastCollectable.h"

#include "./Objects/Environment/ExplosiveObject.h"

#include "./Objects/Environment/ClimbableObject.h"

#include "./Objects/Environment/Effects/GlitchEffect.h"
#include "./Objects/Environment/Effects/GlitchedRegion.h"

#include "./Objects/Environment/Movement/Trampoline.h"
#include "Objects/Environment/Buildings/InteriorDoor.h"
#include "Objects/Environment/DialogueInteractable.h"

#include "Objects/LevelCompleteZone.h"


#include "Objects/Environment/Buildings/Electrical/ElectricalTransformer.h"

#include "./Objects/Environment/Waterfall.h"
#include "Objects/LayerFlipObject.h"

#define COMPONENT_MAPPINGS_SIZE 1	//How many component mappings there are
#define MAPPINGS_SIZE 20			//How many object mappings there are.

constexpr float GenesisTileSize = 8.f;

std::vector<std::shared_ptr<GAME_NAME::Objects::Environment::Buildings::BuildingZone>> BUILDING_createdBuildingZones;

inline Vec2 STOIVEC(std::string x, std::string y)
{
	float xcoord, ycoord;
	if (x.ends_with("t"))
	{
		xcoord = std::stof(x.erase(x.length() - 1)) * GenesisTileSize;
	}
	else { xcoord = std::stof(x); }

	if (y.ends_with("t"))
	{
		ycoord = std::stof(y.erase(y.length() - 1)) * GenesisTileSize;
	}
	else { ycoord = std::stof(y); }

	return GAME_NAME::MathUtils::Vec2(xcoord, ycoord);
}

using namespace GAME_NAME;

/// <summary>
/// Maps a string array to a component. The first value in the inputted string array is the component type so it can be ignored.
/// </summary>
std::function<Components::IComponent* (std::vector<std::string>)> m_componentMappings[COMPONENT_MAPPINGS_SIZE]
{
	//Example template
	[](std::vector<std::string> componentData) {
#if _DEBUG
			DebugMapper("L [COMPONENTS] Loading Static BOX Collider...");
#endif
		return (Components::IComponent*)(new Components::Physics::Collision::StaticBoxCollider());
	}
};

/// <summary>
/// Takes in a string vector and spawns a GameObject.
/// </summary>
std::function<void (std::vector<std::string>, size_t line)> m_mappings[MAPPINGS_SIZE]
{
	//Basic Object (map,xPos,yPos,xScale,yScale,sprite[start with "sb_" to use SpriteBase() function],layer?,front?)
	[](std::vector<std::string> data, size_t n) {

#if _DEBUG
		DebugMapper(">>> Loading Basic Object");
#endif
		uint8_t layer = (data.size() > 5) ? std::stoi(data[5]) : 1;

		std::shared_ptr<Sprite> finalSprite = nullptr;

		if (data[4].starts_with("sb_"))
		{
			finalSprite = std::shared_ptr<Sprite>(Renderer::GetSprite(SpriteBase(std::stoi(data[4].substr(3)))));
		}
		else {
			finalSprite = std::shared_ptr<Sprite>(Renderer::GetSprite(std::stoi(data[4])));
		}

		Renderer::LoadObject(new Objects::GameObject(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), finalSprite), layer, (data.size() > 6) ? std::stoi(data[6]) > 0 : false);
	},

	//Component Object
	[](std::vector<std::string> data, size_t n)
	{
#if _DEBUG
			DebugMapper(">>> Loading Component Object");
#endif
		std::vector<Components::IComponent*> components;
		for (int i = 5; i < data.size(); i++)
		{
			std::stringstream ss(data[i]);
			std::string var;
			std::vector<std::string> out;

			while (std::getline(ss, var, '|'))
			{
				out.push_back(var);
			}

			components.push_back(m_componentMappings[std::stoi(out[0])](out));
		}

		Renderer::LoadObject(new Components::ComponentObject(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), components));
	},

	//StaticBoxCollisionObject
	[](std::vector<std::string> data, size_t n) {

#if _DEBUG
		DebugMapper(">>> Loading StaticBoxCollisionObject");
#endif
		std::cout << "GOT: " << std::stoi(data[4]) << std::endl;

		Sprite* finalSprite = nullptr;

		if (data[4].starts_with("sb_"))
		{
			finalSprite = Renderer::GetSprite(SpriteBase(std::stoi(data[4].substr(3))));
		}
		else {
			finalSprite = Renderer::GetSprite(std::stoi(data[4]));
		}

		Renderer::LoadObject(new GAME_NAME::Components::Physics::Collision::StaticBoxCollisionObject(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), finalSprite), (data.size() > 5 ? std::stoi(data[5]) : 1U));
	},

	//ActiveBoxCollisionGravityObject
	[](std::vector<std::string> data, size_t n) {

#if _DEBUG
		DebugMapper(">>> Loading ActiveBoxCollisionGravityObject");
#endif
		Renderer::LoadActiveObject(new GAME_NAME::Components::Physics::Collision::ActiveBoxCollisionGravityObject(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4]))));
	},

	//Water
	[](std::vector<std::string> data, size_t n) {

#if _DEBUG
		DebugMapper(">>> Loading Water");
#endif
		Renderer::LoadActiveObject(new GAME_NAME::Objects::Environment::Water(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3])), 1);
	},

	//5
	[](std::vector<std::string> data, size_t d)
	{

#if _DEBUG
		DebugMapper(">>> Loading PixelColliderOPbject");
#endif
	    using namespace GAME_NAME::Components::Physics::Collision;
		using namespace GAME_NAME::Components::Physics::Collision::Helpers;
		
		constexpr PixelPerfectCollider::PIXEL_COLLISION_FACE n = PixelPerfectCollider::PIXEL_COLLISION_FACE_NONE;
		constexpr PixelPerfectCollider::PIXEL_COLLISION_FACE t = PixelPerfectCollider::PIXEL_COLLISION_FACE_TOP;
		constexpr PixelPerfectCollider::PIXEL_COLLISION_FACE l = PixelPerfectCollider::PIXEL_COLLISION_FACE_LEFT;
		constexpr PixelPerfectCollider::PIXEL_COLLISION_FACE b = PixelPerfectCollider::PIXEL_COLLISION_FACE_BOTTOM;

		PixelPerfectCollider::PIXEL_COLLISION_FACE collisionFaces[PixelColliderSize * PixelColliderSize] {
			l,b,b,b,b,b,b,b,b,b,b,b,b,b,t,t,
			l,l,b,b,b,b,b,b,b,b,b,t,t,t,t,n,
			l,l,l,b,b,b,b,b,b,b,t,t,t,t,n,n,
			l,l,l,l,b,b,b,b,b,t,t,t,t,n,n,n,
			l,l,l,l,l,b,b,t,t,t,t,t,n,n,n,n,
			l,l,l,l,l,t,t,t,t,t,t,n,n,n,n,n,
			l,l,l,l,l,t,t,t,t,t,n,n,n,n,n,n,
			l,l,l,l,t,t,t,t,t,n,n,n,n,n,n,n,
			l,l,l,l,t,t,t,t,n,n,n,n,n,n,n,n,
			l,l,l,t,t,t,t,n,n,n,n,n,n,n,n,n,
			l,l,t,t,t,t,n,n,n,n,n,n,n,n,n,n,
			l,t,t,t,t,n,n,n,n,n,n,n,n,n,n,n,
			t,t,t,t,n,n,n,n,n,n,n,n,n,n,n,n,
			t,t,t,n,n,n,n,n,n,n,n,n,n,n,n,n,
			t,t,n,n,n,n,n,n,n,n,n,n,n,n,n,n,
			t,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n
		};

		PixelPerfectColliderStaticObject* ppcso = new PixelPerfectColliderStaticObject(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), collisionFaces);

		Renderer::LoadObject(ppcso, false);

	},

	//GAME
	//BGParallaxObject
	[](std::vector<std::string> data, size_t n) {

#if _DEBUG
		DebugMapper(">>> Loading ParallaxBGObject");
#endif
		Renderer::LoadActiveObject(new GAME_NAME::Objects::Environment::BGParallax(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), static_cast<float>(std::stoi(data[5])/100.f)), 0);
	},

	/*
		7: Platform (map,layer,positionX,positionY,scaleX,scaleY,sprite,type,...)
		TYPE:
			0: Rotating (originX,originY,speed*10,offset*pi/6)
				- A platform that rotates about a fixed point.
			1: Falling (fallDelay[float])
				- A platform that falls when it is stepped on.
	*/
	[](std::vector<std::string> data, size_t n) {
#if _DEBUG
		DebugMapper(">>> Loading Platform");
#endif
		switch (std::stoi(data[6]))
		{
			//ROTATING PLATFORM
		case 0:
			Renderer::LoadObject(new GAME_NAME::Objects::Platforms::RotatingPlatform(
				STOIVEC(data[1], data[2]),								//positionX,positionY
				STOIVEC(data[3], data[4]),								//scaleX,scaleY
				Renderer::GetSprite(std::stoi(data[5])),				//sprite
				STOIVEC(data[7], data[8]),								//originX,originY
				static_cast<double>(std::stoi(data[9]) / 10.0),			//speed*10
				M_PI * static_cast<double>(std::stoi(data[10]) / 6.0)	//offset * pi/6
			), std::stoi(data[0]));										//layer
			break;
			//FALLING PLATFORM
		case 1:
			Renderer::LoadObject(new Platforms::FallingPlatform(
				STOIVEC(data[1], data[2]),					//positionX,positionY
				STOIVEC(data[3], data[4]),					//scaleX,scaleY
				Renderer::GetSprite(std::stoi(data[5])),	//sprite
				std::stof(data[7])							//fallDelay[float]
			), std::stoi(data[0]));							//layer
		}
	},

	/*
		8: Floor Item (map,positionX,positionY,itemSerialized)
		- An item that can be picked up.
	*/
	[](std::vector<std::string> data, size_t n)
	{
#if _DEBUG
			DebugMapper(">>> Loading FloorItem(" + data[2] + ")");
#endif
			using namespace GAME_NAME::Items;

			InventoryItem* invItem = ItemMapping::DeSerialize(data[2]);
			FloorItem* newFloorItem = new FloorItem(STOIVEC(data[0], data[1]), invItem);

			Renderer::LoadActiveObject(newFloorItem);
	},

	/*
		9: Particle Emitter (map, positionX,positionY,particleLifetime*100,numParticles,spawnInterval,velX,velY,gravity, rot, posVarX, posVarY, levelsource, particle_1_objparent, particle_1_objname, ...)
	*/
	[](std::vector<std::string> data, size_t n)
	{
#if _DEBUG
		DebugMapper(">>> Loading ParticleEmitter");
#endif 

		using namespace Particles;

		ParticleEmitter* pe = new ParticleEmitter(STOIVEC(data[0], data[1]), static_cast<float>(std::stoi(data[2]))/100.f);

		for (int i = 12; i < data.size(); i += 2)
		{
			pe->RegisterParticle(Particle(Instantiate::LevelObjectHandler::GetLevelObject(data[i], data[i + 1], false, data[11])));
			Renderer::DestroyObject(Renderer::GetLastLoadedObject());
		}

		Renderer::LoadObject(pe);
		pe->SpawnParticlesLooping(std::stod(data[4]), std::stoi(data[3]), STOIVEC(data[5], data[6]), std::stof(data[7]), std::stof(data[8]), STOIVEC(data[9], data[10]));
	},

	/*
		10: Tree (map,positionX,positionY,scaleX,scaleY,sprite,chopSprite,layer)
	*/

	[](std::vector<std::string> data, size_t n)
	{
		Environment::Plants::Tree* tree = new Environment::Plants::Tree(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), n, Renderer::GetSprite(std::stoi(data[5])));
		Renderer::LoadActiveObject(tree, std::stoi(data[6]));
	},

	/*
		11: Container Inventory (map,positionX,positionY,scaleX,scaleY,sprite,inventoryName,inventorySize,layer,inventoryItems...)
	*/
	[](std::vector<std::string> data, size_t n)
	{
		Items::Inventories::InventoryContainer* container = new Items::Inventories::InventoryContainer(data[5], std::stoi(data[6]), STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), n);

		if (!container->GetWasLoadedFromState())
		{
			for (int i = 8; i < data.size(); i++)
			{
				Items::InventoryItem* ii = ItemMapping::DeSerialize(data[i]);
				container->AddItem(ii);
			}
		}
		
		Renderer::LoadActiveObject(container, std::stoi(data[7]));
	},

	/*
		12: Enemy (map,positionX,positionY,scaleX,scaleY,sprite,type,layer, ...)
		Type:
			0 - LeftRightEnemy (anchorLeftX,anchorLeftY,anchorRightX,anchorRightY)
			1 - PassiveRabbit (runningSprite)
			2 - FeralRabbit (runningSprite,asleep),
			3 - Dummy Enemy (health)
	*/
	[](std::vector<std::string> data, size_t n)
	{
using namespace Enemies;

#if _DEBUG
		DebugMapper("Loading Enemy... (Type: " + data[5] + ")");
#endif

		uint8_t type = std::stoi(data[5]);

		switch (type)
		{
			case 0:		//LeftRightEnemy
			{
				LeftRightEnemy* lre = new LeftRightEnemy(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), STOIVEC(data[7], data[8]), STOIVEC(data[9], data[10]), new LeftRightEnemy::LeftRightEnemyAttributes(), n);
				Renderer::LoadObject(lre, std::stoi(data[6]));
				break;
			}

			case 1:		//Passive Rabbit
			{
				PassiveRabbit* pr = new PassiveRabbit(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), std::stoi(data[7]), new PassiveRabbit::PassiveRabbitAttributes() , n);
				Renderer::LoadActiveObject(pr, std::stoi(data[6]));
				break;
			}

			case 2:		//Feral Rabbit
			{
				FeralRabbit* pr = new FeralRabbit(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), std::stoi(data[7]), new PassiveRabbit::PassiveRabbitAttributes(), std::stoi(data[8]) == 1, n);
				Renderer::LoadActiveObject(pr, std::stoi(data[6]));
				break;
			}

			case 3:     //Dummy Enemy
			{
				Enemy* e = new Enemy(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), new Enemy::EnemyAttributes(), n, std::stof(data[7]), false);
				Renderer::LoadActiveObject(e, std::stoi(data[6]));
				break;
			}

			default:
			{
#if _DEBUG
				DEBUG::DebugLog::LogError("Tried to load unknown enemy type. (Mappings.cpp : m_mappings:12)");
#endif
				break;
			}
		}
	},

	/*
		13: BuildingObject (map,type,positionX,positionY,scaleX,scaleY,layer,...)
		Type:
			0 - FrontWall (sprite,buildingZone_positionX,buildingZone_positionY,buildingZone_scaleX,buildingZone_scaleY)
			1 - Door (closeSprite,openSprite,openDistance[float]=DEFAULT_DOOR_OPEN_DISTANCE,rotation[float]=0)
			2 - FrontDoor (sprite, roomFile)
			3 - Bench (sprite)
			4 - Sagging Object (sprite, segmentCount, bouncy)
			5 - InteriorDoor (sprite,destination,playerExitX,playerExitY,shouldLoadOnlyObjects=false,shouldUseLocalState=false)
			6 - DialogueInteractable (sprite,dialogueKey)
			7 - SpinningObject (sprite, rotationSpeed, enabled)
			8 - ElectricalTransformer (sprite, zapRadius)
			9 - InnerUpperWall (sprite,fadeOffset) [Set layer to 4 to render infront]
	*/
	[](std::vector<std::string> data, size_t n)
	{
using namespace Objects::Environment::Buildings;

#if _DEBUG
		DebugMapper("Loading Building Structure (Type: " + data[0] + ")");
#endif

		uint8_t buildingType = std::stoi(data[0]);

		switch (buildingType)
		{
		//Front wall.
		case 0:
		{
			
			const Vec2 bZonePos = STOIVEC(data[7], data[8]);
			const Vec2 bZoneSca = STOIVEC(data[9], data[10]);

			std::shared_ptr<BuildingZone> newBz(nullptr);

			for (std::shared_ptr<BuildingZone> bz : BUILDING_createdBuildingZones)
			{
				if (Vec2::Distance(bz->Position, bZonePos) < 2.f)
				{
					newBz = bz;
					break;
				}
			}

			if (newBz == nullptr)
			{
				newBz = std::shared_ptr<BuildingZone>(new BuildingZone(bZonePos, bZoneSca));
			}

			FrontWall* frontWall = new FrontWall(STOIVEC(data[1], data[2]), STOIVEC(data[3], data[4]), Renderer::GetSprite(std::stoi(data[6])), newBz);
			Renderer::LoadObject(frontWall, std::stoi(data[5]));
			break;
		}
		case 1:
		{
			Door* door = new Door(STOIVEC(data[1], data[2]), STOIVEC(data[3], data[4]), Renderer::GetSprite(std::stoi(data[6])), Renderer::GetSprite(std::stoi(data[7])), (data.size() > 8 ? std::stof(data[8]) : DEFAULT_DOOR_OPEN_DISTANCE), (data.size() > 9) ? std::stof(data[9]) : 0.f);
			Renderer::LoadObject(door, std::stoi(data[5]));
			break;
		}
		case 2:
		{
			FrontDoor* frontDoor = new FrontDoor(STOIVEC(data[1], data[2]), STOIVEC(data[3], data[4]), Renderer::GetSprite(std::stoi(data[6])), data[7]);
			Renderer::LoadActiveObject(frontDoor, std::stoi(data[5]));
			break;
		}
		case 3:
		{
			Bench* bench = new Bench(STOIVEC(data[1], data[2]), STOIVEC(data[3], data[4]), Renderer::GetSprite(std::stoi(data[6])));
			Renderer::LoadObject(bench, std::stoi(data[5]));
			break;
		}
		case 4:
		{
			SaggingObject* sag = new SaggingObject(STOIVEC(data[1], data[2]), STOIVEC(data[3], data[4]), Renderer::GetSprite(std::stoi(data[6])), std::stoi(data[7]), std::stoi(data[8]) == 1);
			Renderer::LoadObject(sag, std::stoi(data[5]));
			break;
		}
		case 5:
		{
			InteriorDoor* interiorDoor = new InteriorDoor(STOIVEC(data[1], data[2]), STOIVEC(data[3], data[4]), Renderer::GetSprite(std::stoi(data[6])), data[7], STOIVEC(data[8], data[9]), data.size() >= 11 ? (std::stoi(data[10]) == 1) : false, data.size() >= 12 ? (std::stoi(data[11]) == 1) : false);
			Renderer::LoadObject(interiorDoor, std::stoi(data[5]));
			break;
		}
		case 6:
		{
			GAME_NAME::Objects::Environment::DialogueInteractable* di = new GAME_NAME::Objects::Environment::DialogueInteractable(STOIVEC(data[1], data[2]), STOIVEC(data[3], data[4]), Renderer::GetSprite(std::stoi(data[6])), data[7]);
			Renderer::LoadObject(di, std::stoi(data[5]));
			break;
		}
		case 7:
		{
			GAME_NAME::Objects::Environment::SpinningObject* so = new GAME_NAME::Objects::Environment::SpinningObject(STOIVEC(data[1], data[2]), STOIVEC(data[3], data[4]), Renderer::GetSprite(std::stoi(data[6])), std::stof(data[7]), std::stoi(data[8]) == 1);
			Renderer::LoadObject(so, std::stoi(data[5]));
			break;
		}
		case 8:
		{
			ElectricalTransformer* et = new ElectricalTransformer(STOIVEC(data[1], data[2]), STOIVEC(data[3], data[4]), Renderer::GetSprite(std::stoi(data[6])), std::stof(data[7]));
			Renderer::LoadObject(et, std::stoi(data[5]));
			break;
		}
		case 9:
		{
			InnerUpperWall* innerUpperWall = new InnerUpperWall(STOIVEC(data[1], data[2]), STOIVEC(data[3], data[4]), Renderer::GetSprite(std::stoi(data[6])), data.size() > 7 ? std::stof(data[7]) : 0);
			Renderer::LoadObject(innerUpperWall, std::stoi(data[5]) == 4 ? 2 : std::stoi(data[5]), std::stoi(data[5]) == 4);
			break;
		}

		default:
		{
#ifdef SHOW_WINDOWS_ERROR_POPUPS
			std::string message = "Tried to load a nonexistant building ID: ";
			message.append(std::to_string(buildingType));
			std::wstring stemp = std::wstring(message.begin(), message.end());
			MessageBox(nullptr, stemp.c_str(), TEXT("Mapping Error"), MB_OK);
#endif
			break;
		}
		}
	
	},

	/*
		14: Cutscene Trigger (map,type,positionX,positionY,radius*10,...)
		Type:
			0 - InnerThoughtCutscene (text)
	*/

	[](std::vector<std::string> data, size_t n)
	{
using namespace Cutscenes;

#if _DEBUG
		DebugMapper("Loading Cutscene (Type: " + data[0] + ")");
#endif

		uint8_t cutsceneType = std::stoi(data[0]);

		switch (cutsceneType)
		{
		//Inner thought cutscene.
		case 0:
			std::string textString = data[4];
			std::replace(textString.begin(), textString.end(), '-', ' ');
			std::shared_ptr<InnerThoughtScene> itc(new InnerThoughtScene(STOIVEC(data[1], data[2]), (float)std::stoi(data[3])/10.f, textString));
			CutsceneManager::RegisterCutscene(itc);
			break;
		}
	},

	/*
		15: Breakable Object (map,positionX,positionY,scaleX,scaleY,sprite,layer,breakSeconds[double],breakResistance,toolActionFlags=MINE)
	*/

	[](std::vector<std::string> data, size_t n)
	{
#if _DEBUG
		DebugMapper("Loading Breakable Object");
#endif

		BreakableBlock* bb = new BreakableBlock(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), n, std::stod(data[6]), std::stoi(data[7]), data.size() > 8 ? (Items::TOOL_ACTION)std::stoi(data[8], nullptr, 2) : Items::TOOL_ACTION::MINE);
		
		//ACTIVE?
		//TODO: NOT ACTIVE WOULD BE BETTER BUT DOES IT SCREW UP THE SAVE DATA.
		Renderer::LoadActiveObject(bb, std::stoi(data[5]));
	},

	/*
		16: Collectable Object (map,type,positionX,positionY,scaleX,scaleY,sprite,layer)
	*/
	[](std::vector<std::string> data, size_t n)
	{
#if _DEBUG
		DebugMapper("Loading Collectable Object");
#endif

		int type = std::stoi(data[0]);

		switch (type)
		{
			case 0:
			{
				Objects::Collectables::ToastCollectable* tc = new Objects::Collectables::ToastCollectable(STOIVEC(data[1], data[2]), STOIVEC(data[3], data[4]), Renderer::GetSprite(std::stoi(data[5])), n);
				Renderer::LoadObject(tc, std::stoi(data[6]));
				break;
			}
		}
	},

	/*
		17: Explosive Object (map,positionX,positionY,scaleX,scaleY,sprite,layer,explosionRadius,explosionPower,causeParams)
			{
			  causeParams:
				0001 - JUMPED_ON
			}
	*/
	[](std::vector<std::string> data, size_t n)
{
#if _DEBUG
	DebugMapper("Loading Explosive Object");
#endif

	Environment::ExplosiveObject* eo = new Environment::ExplosiveObject(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), std::stof(data[6]), std::stof(data[7]), (Environment::ExplosiveObject::EXPLOSION_REASON)(std::stoi(data[8])), n);
	Renderer::LoadObject(eo, std::stoi(data[5]));
},

	/*
	18: Climbable (map,positionX,positionY,scaleX,scaleY,sprite,layer)
	*/
	[](std::vector<std::string> data, size_t n)
{
#if _DEBUG
	DebugMapper("Loading Climbable Object");
#endif

	Environment::ClimbableObject* co = new Environment::ClimbableObject(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])));
	Renderer::LoadObject(co, std::stoi(data[5]));
},

	/*
	19: Glitch Effect (map,positionX,positionY,scaleX,scaleY,layer)
	*/
	[](std::vector<std::string> data, size_t n)
{
#if _DEBUG
	DebugMapper("Loading Glitch Object");
#endif

	Environment::Effects::GlitchEffect* ge = new Environment::Effects::GlitchEffect(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]));
	Renderer::LoadObject(ge, std::stoi(data[4]), true);
}
};

void GAME_NAME::Mappings::LoadObjectsWithDefaultMapping(const char* levelPath)
{
#if _DEBUG
	DebugMapper("!=== LOADING OBJECT DATA ===!");
#endif
	Resources::AssetManager::LoadObjectData(levelPath, m_mappings);

	BUILDING_createdBuildingZones.clear();
}

GameObject* GAME_NAME::Mappings::LoadObjectWithDefaultMapping(std::string objectCode)
{
	
	Resources::AssetManager::loadObjectDataThread(objectCode, 0, m_mappings);

	BUILDING_createdBuildingZones.clear();

	return Renderer::GetLastLoadedObject();
}

void GAME_NAME::Mappings::LoadOver20Switch(int index, std::vector<std::string> data, size_t saveIndex)
{
	switch (index)
	{
		/*
			20: Trampoline (map,positionX,positionY,scaleX,scaleY,sprite,layer,bounceFactor)
		*/
	case 20:
	{
#if _DEBUG
		DebugMapper("Loading Trampoline");
#endif

		Environment::Trampoline* t = new Environment::Trampoline(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), std::stof(data[6]));
		Renderer::LoadObject(t, std::stoi(data[5]), true);
		break;
	}

	/*
		21: Waterfall (map, positionX, positionY, scaleX, scaleY, sprite1, sprite2, sprite3, segments, layer)
	*/
	case 21:
	{
		Environment::Waterfall* w = new Environment::Waterfall(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), Renderer::GetSprite(std::stoi(data[5])), Renderer::GetSprite(std::stoi(data[6])), std::stoi(data[7]));
		Renderer::LoadObject(w, std::stoi(data[8]));
		break;
	}
	/*
		22: LevelCompleteZone(map, positionX, positionY, scaleX, scaleY, sprite, levelDestination, layer)
	*/
	case 22:
	{
		LevelCompleteZone* z = new LevelCompleteZone(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), data[5]);
		Renderer::LoadObject(z, std::stoi(data[6]));
		break;
	}

	/*
		23: GlitchedRegion(map, positionX, positionY, scaleX, scaleY)
	*/
	case 23:
	{
		//TODO: Broken...
		GlitchedRegion* region = new GlitchedRegion(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]));
		Renderer::LoadObject(region);
		break;
	}

	/*
	24: LayerFlip(map, positionX, positionY, scaleX, scaleY, sprite, layer, flipOffset = 0)
	*/
	case 24:
	{
		LayerFlipObject* layerFlip = new LayerFlipObject(STOIVEC(data[0], data[1]), STOIVEC(data[2], data[3]), Renderer::GetSprite(std::stoi(data[4])), data.size() > 6 ? std::stof(data[6]) : 0.f);
		Renderer::LoadObject(layerFlip, std::stoi(data[5]));
		break;
	}
	}
}
