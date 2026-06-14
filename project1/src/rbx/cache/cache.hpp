#pragma once
#include <string>
#include <mutex>
#include <vector>
#include <rbx/sdk/sdk.hpp>

namespace rig
{
	constexpr std::uint8_t r6  = 0;
	constexpr std::uint8_t r15 = 1;
}

namespace cache
{
	inline std::mutex mtx;

	struct entity_t final
	{
		sdk::instance_t instance;
		std::string name;
		std::uint8_t rig_type{ rig::r15 };

		sdk::humanoid_t humanoid;

		sdk::part_t head;
		sdk::part_t humanoidrootpart;

		/* r6 */
		sdk::part_t torso;
		sdk::part_t leftarm;
		sdk::part_t rightarm;
		sdk::part_t leftleg;
		sdk::part_t rightleg;

		/* r15 */
		sdk::part_t uppertorso;
		sdk::part_t lowertorso;
		sdk::part_t leftupperarm;
		sdk::part_t leftlowerarm;
		sdk::part_t lefthand;
		sdk::part_t rightupperarm;
		sdk::part_t rightlowerarm;
		sdk::part_t righthand;
		sdk::part_t leftupperleg;
		sdk::part_t leftlowerleg;
		sdk::part_t leftfoot;
		sdk::part_t rightupperleg;
		sdk::part_t rightlowerleg;
		sdk::part_t rightfoot;

		std::vector<sdk::part_t> body_parts;
	};

	inline cache::entity_t cached_local_player;
	inline std::vector<cache::entity_t> cached_players;

	void run();
}
