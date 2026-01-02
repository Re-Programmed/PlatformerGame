#pragma once
#include "../../Objects/GameObject.h"
#include "../../Objects/Particles/ParticleEmitter.h"

namespace GAME_NAME::Objects
{
	class LevelCompleteZone
		: public GameObject
	{
	public:
		LevelCompleteZone(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, std::string destination);

		void Update(GLFWwindow* window) override;

	private:

		const std::string m_levelDestination;

		class LevelCompleteScene
		{
		public:
			virtual void Update() = 0;
			virtual bool IsDone() = 0;
		};

		class ConfettiLevelCompleteScene
			: public LevelCompleteScene
		{
		public:
			ConfettiLevelCompleteScene();
			~ConfettiLevelCompleteScene();
			
			void Update() override;
			bool IsDone() override;


		private:
			Particles::ParticleEmitter* m_confettiEmitter;
			double m_elapsedTime;
		};

		class BalloonLevelCompleteScene
			: public LevelCompleteScene
		{
		public:
			BalloonLevelCompleteScene();
			~BalloonLevelCompleteScene();

			void Update() override;
			bool IsDone() override;

		private:
			
			GameObject* m_balloonObject;
		};

		std::unique_ptr<LevelCompleteScene> m_currentScene = nullptr;
	};
}