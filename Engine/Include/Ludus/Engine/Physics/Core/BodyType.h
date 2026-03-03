#pragma once

#include <array>
#include <format>
#include <string_view>

#include <Ludus/Engine/Core/Enums.h>

namespace Ludus::Engine::Physics::Core
{
	enum class BodyType
	{
		Dynamic,
		Kinematic,
		Static
	};

	constexpr float GetContactWeight(BodyType bodyType)
	{
		switch (bodyType)
		{
			case BodyType::Dynamic:
				return 1.0f;
			case BodyType::Kinematic:
				return 0.0;
			case BodyType::Static:
				return 0.0;
		}

		return 0.0f;
	}

	constexpr std::string_view ToString(BodyType type)
	{
		switch (type)
		{
			case BodyType::Dynamic:		return "Dynamic";
			case BodyType::Kinematic:	return "Kinematic";
			case BodyType::Static:		return "Static";
			default:					return "Unknown";
		}
	}

	constexpr bool TryParse(std::string_view text, BodyType& out)
	{
		if (text == "Dynamic")
		{
			out = BodyType::Dynamic;
			return true;
		}

		if (text == "Kinematic")
		{
			out = BodyType::Kinematic;
			return true;
		}

		if (text == "Static")
		{
			out = BodyType::Static;
			return true;
		}

		return false;
	}
}

template <>
struct Ludus::Engine::Core::Enums::GetValues<Ludus::Engine::Physics::Core::BodyType>
{
	static constexpr std::array<Ludus::Engine::Physics::Core::BodyType, 3> Values {
		Ludus::Engine::Physics::Core::BodyType::Dynamic,
		Ludus::Engine::Physics::Core::BodyType::Kinematic,
		Ludus::Engine::Physics::Core::BodyType::Static
	};
};
