#pragma once

#include <array>
#include <format>
#include <string_view>

#include <Ludus/Engine/Core/Enums.h>

namespace Ludus::Editor::Core
{
	enum class ViewportDisplayMode
	{
		Editor,
		Simulation
	};

	constexpr std::string_view ToString(ViewportDisplayMode mode)
	{
		switch (mode)
		{
			case ViewportDisplayMode::Editor:		return "Editor";
			case ViewportDisplayMode::Simulation:	return "Simulation";
			default:								return "Unknown";
		}
	}
}

template <>
struct Ludus::Engine::Core::Enums::GetValues<Ludus::Editor::Core::ViewportDisplayMode>
{
	static constexpr std::array<Ludus::Editor::Core::ViewportDisplayMode, 2> Values {
		Ludus::Editor::Core::ViewportDisplayMode::Editor,
		Ludus::Editor::Core::ViewportDisplayMode::Simulation
	};
};

namespace std
{
	template <>
	struct formatter<Ludus::Editor::Core::ViewportDisplayMode> : formatter<string_view>
	{
		auto format(const Ludus::Editor::Core::ViewportDisplayMode& mode, format_context& context) const
		{
			return formatter<string_view>::format(Ludus::Editor::Core::ToString(mode), context);
		}
	};
}
