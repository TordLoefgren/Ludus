#pragma once

#include <stdexcept>
#include <string_view>

#include <Ludus/Engine/Core/Enums/EnumTraits.h>

namespace Ludus::Engine::Core
{
	enum class AssetType { Unknown, Texture2D };

	constexpr std::string_view ToString(AssetType assetType)
	{
		switch (assetType)
		{
			case AssetType::Unknown:	return "Unknown";
			case AssetType::Texture2D:	return "Texture2D";
			default:					throw std::runtime_error("Unsupported asset type.");
		}
	}

	constexpr bool TryParse(std::string_view text, AssetType& out)
	{
		if (text == "Unknown")
		{
			out = AssetType::Unknown;
			return true;
		}

		if (text == "Texture2D")
		{
			out = AssetType::Texture2D;
			return true;
		}

		return false;
	}
}
