#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Ludus::Engine::Persistance::Archives
{
	template<class TArchive>
	class ObjectScope
	{
	private:
		TArchive& m_Archive;

	public:
		ObjectScope(TArchive& archive, std::string_view key) : m_Archive(archive)
		{
			m_Archive.BeginObject(key);
		}

		ObjectScope(const ObjectScope&) = delete;
		ObjectScope& operator=(const ObjectScope&) = delete;
		ObjectScope(ObjectScope&&) = delete;
		ObjectScope& operator=(ObjectScope&&) = delete;

		~ObjectScope() noexcept
		{
			m_Archive.EndObject();
		}
	};

	template<class TArchive>
	class ArrayScope
	{
	private:
		TArchive& m_Archive;

	public:
		ArrayScope(TArchive& archive, std::string_view key)
			: m_Archive(archive)
		{
			m_Archive.BeginArray(key);
		}

		ArrayScope(const ArrayScope&) = delete;
		ArrayScope& operator=(const ArrayScope&) = delete;
		ArrayScope(ArrayScope&&) = delete;
		ArrayScope& operator=(ArrayScope&&) = delete;

		~ArrayScope() noexcept
		{
			m_Archive.EndArray();
		}
	};

	template<class TArchive>
	class CountedArrayScope
	{
	private:
		TArchive& m_Archive;
		size_t m_Count = 0;

	public:
		CountedArrayScope(TArchive& archive, std::string_view key)
			: m_Archive(archive), m_Count(m_Archive.BeginArray(key))
		{ }

		CountedArrayScope(const CountedArrayScope&) = delete;
		CountedArrayScope& operator=(const CountedArrayScope&) = delete;
		CountedArrayScope(CountedArrayScope&&) = delete;
		CountedArrayScope& operator=(CountedArrayScope&&) = delete;

		size_t Count() const noexcept { return m_Count; }

		~CountedArrayScope() noexcept
		{
			m_Archive.EndArray();
		}
	};

	template<class TArchive>
	class ArrayElementScope
	{
	private:
		TArchive& m_Archive;

	public:
		ArrayElementScope(TArchive& archive, size_t index) : m_Archive(archive)
		{
			m_Archive.BeginArrayElement(index);
		}

		ArrayElementScope(TArchive& archive) : m_Archive(archive)
		{
			m_Archive.BeginArrayElement();
		}

		ArrayElementScope(const ArrayElementScope&) = delete;
		ArrayElementScope& operator=(const ArrayElementScope&) = delete;
		ArrayElementScope(ArrayElementScope&&) = delete;
		ArrayElementScope& operator=(ArrayElementScope&&) = delete;

		~ArrayElementScope() noexcept
		{
			m_Archive.EndArrayElement();
		}
	};

	template<class TArchive>
	class OptionalObjectScope
	{
	private:
		TArchive& m_Archive;
		bool m_Active;

	public:
		OptionalObjectScope(TArchive& archive, std::string_view key)
			: m_Archive(archive), m_Active(m_Archive.TryBeginObject(key))
		{ }

		explicit operator bool() const noexcept { return m_Active; }

		OptionalObjectScope(const OptionalObjectScope&) = delete;
		OptionalObjectScope& operator=(const OptionalObjectScope&) = delete;
		OptionalObjectScope(OptionalObjectScope&&) = delete;
		OptionalObjectScope& operator=(OptionalObjectScope&&) = delete;

		~OptionalObjectScope() noexcept
		{
			if (m_Active)
			{
				m_Archive.EndObject();
			}
		}
	};

	template<class TArchive>
	class OptionalArrayScope
	{
	private:
		TArchive& m_Archive;
		bool m_Active;
		size_t m_Count = 0;

	public:
		OptionalArrayScope(TArchive& archive, std::string_view key)
			: m_Archive(archive), m_Active(m_Archive.TryBeginArray(key, m_Count))
		{ }

		explicit operator bool() const noexcept { return m_Active; }

		size_t Count() const noexcept { return m_Count; }

		OptionalArrayScope(const OptionalArrayScope&) = delete;
		OptionalArrayScope& operator=(const OptionalArrayScope&) = delete;
		OptionalArrayScope(OptionalArrayScope&&) = delete;
		OptionalArrayScope& operator=(OptionalArrayScope&&) = delete;

		~OptionalArrayScope() noexcept
		{
			if (m_Active)
			{
				m_Archive.EndArray();
			}
		}
	};
}
