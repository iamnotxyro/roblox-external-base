#pragma once
#include <rbx/sdk/sdk.hpp>

namespace game
{
	inline sdk::instance_t datamodel{};
	inline sdk::visualengine_t visengine{};
	inline sdk::instance_t players{};
	inline sdk::instance_t lighting{}; /* im sure its instance */
}

namespace globals
{
	inline bool streamproof = false;
}

namespace settings
{
	namespace visuals
	{
		inline bool box = false;
		inline float box_color[4] = { 1.f, 1.f, 1.f, 1.f };
	}
}