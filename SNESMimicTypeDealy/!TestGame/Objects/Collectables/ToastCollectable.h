#pragma once

#include "BasicCollectable.h"

namespace GAME_NAME::Objects::Collectables
{
	class ToastCollectable
		: public BasicCollectable
	{
	public:

		ToastCollectable(Vec2 position, Vec2 scale, uint8_t toastID, size_t saveID);
	protected:
		void onCollect() override;

	private:
		const uint8_t m_toastID;
	};
}