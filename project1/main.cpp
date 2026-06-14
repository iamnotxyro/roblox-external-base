#include <chrono>
#include <thread>
#include <memory/memory.hpp>
#include <rbx/offsets/offsets.hpp>
#include <rbx/sdk/sdk.hpp>
#include <settings.hpp>
#include <rbx/cache/cache.hpp>
#include <render/render.hpp>

/* ts is my base i built my own memory and cache system from scratch not copied from random paste dumps or anything like that its all structured the way i needed it to work for ts project even if its not perfect or industry clean
i only used a couple external render classes (stackz stuff) bc im not wasting time reinventing imgui wrappers when i already understand the basics everything else is mine including how the memory flow and caching is handled
the whole idea here is simple: keep memory access stable reduce unnecessary reads and cache what matters so it doesnt spam the system every frame its not some overengineered shitty framework just a practical setup that works for what i needed at the time
ts project wasnt meant to be some full feature cheat or anything massive no aimbot no extra noise just basic structure memory handling and rendering pipeline testing
ive been working on it for a few days mostly iterating and fixing mistakes as they show up its not perfect but its functional enough that i can build on top of it later if needed */

#define BINARY_NAME "RobloxPlayerBeta.exe"

int main()
{
	memory::pid = memory::get_process(BINARY_NAME);

	if (!memory::pid)
	{
		printf("unable to find Roblox!\n");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return 1;
	}

	memory::EnsureSyscallInit();

	memory::base_address = memory::get_base();

	if (!memory::process)
	{
		printf("unable to attach to Roblox!\n");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return 1;
	}

	if (!memory::base_address)
	{
		printf("unable to find main module address!\n");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return 1;
	}

	printf("base -> 0x%llx\n", memory::get_base());

	std::uint64_t fake_datamodel{ memory::read<std::uint64_t>(memory::get_base() + Offsets::FakeDataModel::Pointer) };
	game::datamodel = sdk::instance_t(memory::read<std::uint64_t>(fake_datamodel + Offsets::FakeDataModel::RealDataModel));
	printf("datamodel -> 0x%llx\n", game::datamodel.address);

	game::visengine = { memory::read<std::uint64_t>(memory::get_base() + Offsets::VisualEngine::Pointer) };
	printf("visengine -> 0x%llx\n", game::visengine.address);

	game::players = { game::datamodel.find_first_child_by_class("Players") };
	printf("players -> 0x%llx\n", game::players.address);

	game::lighting = { game::datamodel.find_first_child("Lighting") };
	printf("lighting -> 0x%llx\n", game::lighting.address);

	std::thread(cache::run).detach();

	if (!render->create_window())
	{
		printf("failed to create window\n");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return 1;
	}

	if (!render->create_device())
	{
		printf("failed to create device\n");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return 1;
	}
	
	if (!render->create_imgui())
	{
		printf("failed to create imgui\n");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return 1;
	}

	while (true)
	{
		render->start_render();

		render->render_visuals();

		if (render->running)
		{
			render->render_menu();
		}

		render->end_render();
	}
}