#pragma once

namespace Ludus::Physics::Core
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
}
