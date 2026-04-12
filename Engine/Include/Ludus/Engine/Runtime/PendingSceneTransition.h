#pragma once

#include <filesystem>
#include <utility>
#include <variant>

#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Engine::Runtime
{
	struct PendingSceneTransition
	{
		struct None { };
		struct Load { std::filesystem::path Path; };

		using Variant = std::variant<None, Load>;

		Variant Data;

		static PendingSceneTransition NoneState() { return PendingSceneTransition { None { } }; }
		static PendingSceneTransition LoadScene(const std::filesystem::path& path) { return PendingSceneTransition { Load { path } }; }
	};
}
