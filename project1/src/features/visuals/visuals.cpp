#include <features/visuals/visuals.hpp>

#include <vector>
#include <cmath>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <settings.hpp>
#include <rbx/cache/cache.hpp>

namespace helper
{
	__forceinline void box(ImVec2& c1, ImVec2& c2, ImU32 color)
	{
		c1.x = std::round(c1.x);
		c1.y = std::round(c1.y);
		c2.x = std::round(c2.x);
		c2.y = std::round(c2.y);

		ImDrawList* draw = ImGui::GetBackgroundDrawList();
		draw->Flags &= ImDrawListFlags_AntiAliasedLines;

		ImRect rect(c1.x, c1.y, c1.x + c2.x, c1.y + c2.y);

		draw->AddRect(rect.Min, rect.Max, IM_COL32(0, 0, 0, color >> 24));
		draw->AddRect({ rect.Min.x - 1.f, rect.Min.y - 1.f }, { rect.Max.x + 1.f, rect.Max.y + 1.f }, color);
		draw->AddRect({ rect.Min.x - 2.f, rect.Min.y - 2.f }, { rect.Max.x + 2.f, rect.Max.y + 2.f }, IM_COL32(0, 0, 0, color >> 24));
	}

	__forceinline void rat(sdk::part_t& part, const math::vector2& dims, const math::matrix4& view, sdk::visualengine_t& visengine, float& left, float& top, float& right, float& bottom, bool& valid)
	{
		static constexpr math::vector3 corners[8] =
		{
			{-1,-1,-1}, { 1,-1,-1}, {-1, 1,-1}, { 1, 1,-1},
			{-1,-1, 1}, { 1,-1, 1}, {-1, 1, 1}, { 1, 1, 1}
		};

		sdk::primitive_t prim = part.get_primitive();
		if (!prim.address) return;

		const auto pos  = prim.get_position();
		const auto size = prim.get_size();
		if (size.x == 0.f || size.y == 0.f || size.z == 0.f) return;

		const auto rot = prim.get_rotation();
		const float hx = size.x * 0.5f;
		const float hy = size.y * 0.5f;
		const float hz = size.z * 0.5f;

		for (const auto& c : corners)
		{
			const math::vector3 world = pos + rot * math::vector3{ c.x * hx, c.y * hy, c.z * hz };

			math::vector2 screen{};
			if (visengine.world_to_screen(world, screen, dims, view))
			{
				valid  = true;
				left = min(left, screen.x);
				top = min(top, screen.y);
				right = max(right, screen.x);
				bottom = max(bottom, screen.y);
			}
		}
	}
}

void esp::run()
{
	const math::vector2 dims = game::visengine.get_dimensions();
	const math::matrix4 view = game::visengine.get_viewmatrix();

	std::vector<cache::entity_t> snapshot;
	{
		std::lock_guard<std::mutex> lock(cache::mtx);
		snapshot = cache::cached_players;
	}

	const ImU32 box_color = ImGui::ColorConvertFloat4ToU32({
		settings::visuals::box_color[0],
		settings::visuals::box_color[1],
		settings::visuals::box_color[2],
		settings::visuals::box_color[3]
	});

	for (const cache::entity_t& entity : snapshot)
	{
		if (!entity.instance.address || entity.body_parts.empty())
		continue;

		bool valid = false;
		float l = FLT_MAX, t = FLT_MAX;
		float r = -FLT_MAX, b = -FLT_MAX; // thats ai??? aori??? nigga?? retard???!?! AORI???

		for (sdk::part_t part : entity.body_parts)
		{
			helper::rat(part, dims, view, game::visengine, l, t, r, b, valid);
		}

		if (!valid || l >= r || t >= b)
		continue;

		ImVec2 c1(l, t);
		ImVec2 c2(r - l, b - t);

		if (settings::visuals::box) 
		helper::box(c1, c2, box_color);
	}
}
