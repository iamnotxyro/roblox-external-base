#include <rbx/cache/cache.hpp>
#include <thread>
#include <settings.hpp>

static void push_part(std::vector<sdk::part_t>& out, const sdk::part_t& part)
{
	if (part.address) out.push_back(part);
}

static void cache_entity(sdk::player_t& player, cache::entity_t& entity)
{
	entity.instance = { player.address };
	entity.name = player.get_name();

	if (entity.name.empty() || entity.name == "unknown")
	{
		return;
	}

	sdk::model_instance_t model = player.get_model_instance();
	{
		if (!model.address)
			return;
	}

	sdk::instance_t humanoid_inst = model.find_first_child("Humanoid");
	entity.humanoid = { humanoid_inst.address };

	bool found_uppertorso = false;
	bool found_torso = false;

	for (sdk::part_t& part : model.get_children<sdk::part_t>())
	{
		const std::string class_name = part.get_class_name();
		if (class_name != "Part" && class_name != "MeshPart" && class_name != "UnionOperation")
        continue;

		const std::string n = part.get_name();

		if (n == "Head") entity.head = part;
		else if (n == "HumanoidRootPart") entity.humanoidrootpart = part;
		else if (n == "Torso") {
			entity.torso = part; found_torso = true; 
		}
		else if (n == "Left Arm") entity.leftarm   = part;
		else if (n == "Right Arm") entity.rightarm  = part;
		else if (n == "Left Leg") entity.leftleg   = part;
		else if (n == "Right Leg") entity.rightleg  = part;
		else if (n == "UpperTorso") {
			entity.uppertorso = part; found_uppertorso = true; 
		}
		else if (n == "LowerTorso") entity.lowertorso = part;
		else if (n == "LeftUpperArm") entity.leftupperarm = part;
		else if (n == "LeftLowerArm") entity.leftlowerarm = part;
		else if (n == "LeftHand") entity.lefthand = part;
		else if (n == "RightUpperArm") entity.rightupperarm = part;
		else if (n == "RightLowerArm") entity.rightlowerarm = part;
		else if (n == "RightHand") entity.righthand = part;
		else if (n == "LeftUpperLeg") entity.leftupperleg = part;
		else if (n == "LeftLowerLeg") entity.leftlowerleg = part;
		else if (n == "LeftFoot") entity.leftfoot = part;
		else if (n == "RightUpperLeg") entity.rightupperleg = part;
		else if (n == "RightLowerLeg") entity.rightlowerleg = part;
		else if (n == "RightFoot") entity.rightfoot = part;
	}

	if (found_uppertorso)
		entity.rig_type = rig::r15;
	else if (found_torso)
		entity.rig_type = rig::r6;
	else if (entity.humanoid.address)
		entity.rig_type = entity.humanoid.get_rig_type();

	entity.body_parts.clear();

	if (entity.rig_type == rig::r6)
	{
		entity.body_parts.reserve(7);
		push_part(entity.body_parts, entity.humanoidrootpart);
		push_part(entity.body_parts, entity.head);
		push_part(entity.body_parts, entity.torso);
		push_part(entity.body_parts, entity.leftarm);
		push_part(entity.body_parts, entity.rightarm);
		push_part(entity.body_parts, entity.leftleg);
		push_part(entity.body_parts, entity.rightleg);
	}
	else
	{
		entity.body_parts.reserve(16);
		push_part(entity.body_parts, entity.humanoidrootpart);
		push_part(entity.body_parts, entity.head);
		push_part(entity.body_parts, entity.uppertorso);
		push_part(entity.body_parts, entity.lowertorso);
		push_part(entity.body_parts, entity.leftupperarm);
		push_part(entity.body_parts, entity.leftlowerarm);
		push_part(entity.body_parts, entity.lefthand);
		push_part(entity.body_parts, entity.rightupperarm);
		push_part(entity.body_parts, entity.rightlowerarm);
		push_part(entity.body_parts, entity.righthand);
		push_part(entity.body_parts, entity.leftupperleg);
		push_part(entity.body_parts, entity.leftlowerleg);
		push_part(entity.body_parts, entity.leftfoot);
		push_part(entity.body_parts, entity.rightupperleg);
		push_part(entity.body_parts, entity.rightlowerleg);
		push_part(entity.body_parts, entity.rightfoot);
	}
}

void cache::run()
{
	while (true)
	{
		std::vector<sdk::player_t> players = game::players.get_children<sdk::player_t>();

		std::vector<cache::entity_t> temp_cache;
		temp_cache.reserve(players.size());

		for (sdk::player_t& player : players)
		{
			cache::entity_t entity{};
			cache_entity(player, entity);

			if (entity.instance.address && !entity.name.empty())
				temp_cache.push_back(std::move(entity));
		}

		{
			std::lock_guard<std::mutex> lock(mtx);
			cached_players = std::move(temp_cache);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
