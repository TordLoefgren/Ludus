#pragma once

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

	static float GetContactWeight(BodyType bodyType)
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

namespace std
{
	template <>
	struct formatter<Ludus::Engine::Physics::Core::BodyType> : formatter<string_view>
	{
		auto format(const Ludus::Engine::Physics::Core::BodyType& bodyType, format_context& context) const
		{
			return formatter<string_view>::format(Ludus::Engine::Physics::Core::ToString(bodyType), context);
		}
	};
}
