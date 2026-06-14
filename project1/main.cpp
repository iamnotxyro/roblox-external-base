#include <chrono>
#include <thread>
#include <memory/memory.hpp>
#include <rbx/offsets/offsets.hpp>
#include <rbx/sdk/sdk.hpp>
#include <settings.hpp>
#include <rbx/cache/cache.hpp>
#include <render/render.hpp>

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
