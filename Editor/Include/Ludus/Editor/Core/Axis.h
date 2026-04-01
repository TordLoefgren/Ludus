#pragma once

#include <array>
#include <stdexcept>
#include <string_view>

#include <Ludus/Engine/Core/Enums.h>

namespace Ludus::Editor::Core
{
	enum class Axis
	{
		X,
		Y,
		Z
	};

	constexpr std::string_view ToString(const Axis axis)
	{
		switch (axis)
		{
			case Axis::X:	return "X";
			case Axis::Y:	return "Y";
			case Axis::Z:	return "Z";
			default:		throw std::runtime_error("Unsupported axis.");
		}
	}
}

template <>
struct Ludus::Engine::Core::Enums::GetValues<Ludus::Editor::Core::Axis>
{
	static constexpr std::array<Ludus::Editor::Core::Axis, 3> Values {
		Ludus::Editor::Core::Axis::X,
		Ludus::Editor::Core::Axis::Y,
		Ludus::Editor::Core::Axis::Z
	};
};
