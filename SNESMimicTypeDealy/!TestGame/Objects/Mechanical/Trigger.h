#pragma once

#include "../Triggerable.h"

#include "../../../Objects/Tags/ObjectTagManager.h"

namespace GAME_NAME::Objects::Mechanical
{
	class Trigger
	{
	public:
		Trigger(Triggerable* effect = nullptr)
			: m_effect(effect)
		{
			
		}

		inline Triggerable* GetEffect()
		{
			return m_effect;
		}

		inline void SetEffect(Triggerable* effect)
		{
			m_effect = effect;
		}

		/// <summary>
		/// Set the effect to the object with the given tag.
		/// </summary>
		/// <param name="objectTag"></param>
		inline void SetEffect(std::string objectTag)
		{
			Triggerable* triggerable = dynamic_cast<Triggerable*>(Tags::ObjectTagManager::GetObjectWithTag(objectTag));
			if (triggerable != nullptr)
			{
				m_effect = triggerable;
			}
		}

	protected:
		void trigger(bool reverse, float intensity = 1.f)
		{
			m_effect->Trigger(reverse, intensity);
		}

	private:
		Triggerable* m_effect;
	};
}