#pragma once

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Ludus::Engine::Core::Strings
{
	inline const std::string_view DefaultEllipsis = "...";

	inline std::string ReplaceAll(std::string value, std::string_view from, std::string_view to)
	{
		if (from.empty())
		{
			return value;
		}

		auto position = value.find(from, size_t { });
		while (position != std::string::npos)
		{
			value.replace(position, from.length(), to);
			position = value.find(from, position + to.length());
		}

		return value;
	}

	inline void UpsertLineInRegion(
		std::string& text,
		std::string_view beginMarker,
		std::string_view endMarker,
		std::string_view line
	)
	{
		const auto begin = text.find(beginMarker);
		const auto end = text.find(endMarker);

		if (end < begin)
		{
			throw std::runtime_error("End marker appears before begin marker.");
		}

		if (begin == std::string::npos || end == std::string::npos)
		{
			throw std::runtime_error("Region markers were not found or malformed.");
		}

		const auto contentStart = begin + beginMarker.length();
		const auto contentLength = end - contentStart;

		std::string region = text.substr(contentStart, contentLength);
		const std::string fullLine = std::string(line) + '\n';

		if (!region.empty() && region.back() != '\n')
		{
			region.push_back('\n');
		}

		if (region.find(fullLine) == std::string::npos)
		{
			region += fullLine;
		}

		text.replace(contentStart, contentLength, region);
	}

	inline std::string NormalizeFileExtension(std::string_view extension)
	{
		while (!extension.empty() && (extension.front() == '*' || extension.front() == '.'))
		{
			extension.remove_prefix(1);
		}

		return std::string(extension);
	}

	inline std::string EllipsizeAt(
		std::string_view text,
		std::size_t leftCount,
		std::size_t rightCount = 0,
		std::string_view ellipsis = DefaultEllipsis
	)
	{
		if (text.empty())
		{
			return std::string(text);
		}

		leftCount = std::min(leftCount, text.size());
		rightCount = std::min(rightCount, text.size() - leftCount);

		if (leftCount + rightCount >= text.size())
		{
			return std::string(text);
		}

		if (ellipsis.empty())
		{
			return std::string(text.substr(0, leftCount)) + std::string(text.substr(text.size() - rightCount));
		}

		std::string result;
		result.reserve(leftCount + ellipsis.size() + rightCount);
		result.append(text.substr(0, leftCount));
		result.append(ellipsis);

		if (rightCount > 0)
		{
			result.append(text.substr(text.size() - rightCount));
		}

		return result;
	}
}
