#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory/memory.hpp>
#include <rbx/offsets/offsets.hpp>
#include <rbx/math/math.hpp>
#include <memory> 

namespace sdk
{
	class instance_t;
	class primitive_t;
	class model_instance_t;

	struct addressable_t
	{
		std::uint64_t address;

		addressable_t() : address(0) {}
		addressable_t(std::uint64_t address) : address(address) {}
	};

	struct nameable_t : public addressable_t
	{
		using addressable_t::addressable_t;

		std::string get_name();
		std::string get_class_name();
	};

	struct interface_t
	{
		template <typename T>
		std::vector<T> get_children();

		std::vector<sdk::instance_t> get_children();
		sdk::instance_t find_first_child(std::string_view str);
		sdk::instance_t find_first_child_by_class(std::string_view str);
	};

	struct instance_t : public nameable_t, public interface_t
	{
		using nameable_t::nameable_t;
	};

	struct player_t final : public instance_t
	{
		using instance_t::instance_t;

		sdk::model_instance_t get_model_instance();
	};

	struct model_instance_t final : public instance_t
	{
		using instance_t::instance_t;
	};
	
	struct humanoid_t final : public addressable_t
	{
		using addressable_t::addressable_t;

		std::uint8_t get_rig_type();
	};

	struct part_t : public instance_t
	{
		using instance_t::instance_t;

		sdk::primitive_t get_primitive();
	};

	struct primitive_t final : public addressable_t
	{
		using addressable_t::addressable_t;

		math::vector3 get_size();
		math::vector3 get_position();
		math::matrix3 get_rotation();
	};

	struct visualengine_t final : public addressable_t
	{
		math::vector2 get_dimensions();
		math::matrix4 get_viewmatrix();
		bool world_to_screen(const math::vector3& world, math::vector2& out, const math::vector2& dims, const math::matrix4& view);
	};
}

template <typename T>
std::vector<T> sdk::interface_t::get_children()
{
	sdk::instance_t* base = static_cast<sdk::instance_t*>(this);

	std::uint64_t start = memory::read<std::uint64_t>(base->address + Offsets::Instance::ChildrenStart);
	std::uint64_t end = memory::read<std::uint64_t>(start + Offsets::Instance::ChildrenEnd);

	std::vector<T> children;
	children.reserve(32);

	for (std::uint64_t instance = memory::read<std::uint64_t>(start); instance != end; instance += sizeof(std::shared_ptr<void*>))
	{
		children.emplace_back(memory::read<std::uint64_t>(instance));
	}

	return children;
}