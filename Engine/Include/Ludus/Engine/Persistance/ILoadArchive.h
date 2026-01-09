#pragma once

namespace Ludus::Engine::Persistance
{
	class ILoadArchive
	{
	public:
		virtual ~ILoadArchive() = default;

		virtual void BeginObject(std::string_view key) = 0;
		virtual void EndObject() = 0;

		virtual size_t BeginArray(std::string_view key) = 0;
		virtual void EndArray() = 0;

		virtual void BeginArrayElement(size_t index) = 0;
		virtual void EndArrayElement() = 0;

		virtual bool Has(std::string_view key) const = 0;

		virtual bool TryReadBool(std::string_view key, bool& outValue) const = 0;
		virtual bool TryReadDouble(std::string_view key, double& outValue) const = 0;
		virtual bool TryReadFloat(std::string_view key, float& outValue) const = 0;
		virtual bool TryReadInt(std::string_view key, int& outValue) const = 0;
		virtual bool TryReadString(std::string_view key, std::string& outValue) const = 0;
	};
}
