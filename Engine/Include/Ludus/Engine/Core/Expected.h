#pragma once

#include <type_traits>
#include <utility>

#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Core
{
	// Lightweight implmentation of the C++23 std::expected and std::unexpected class templates.
	// See: https://en.cppreference.com/w/cpp/utility/expected.html.

	template<typename E>
	struct Unexpected
	{
		E Error;

		template<typename U>
		static Unexpected<std::decay_t<U>> Create(U&& unexpected)
		{
			return Unexpected<std::decay_t<U>>(std::forward<U>(unexpected));
		}

		explicit Unexpected(E error) : Error(std::move(error)) { }
	};

	template<typename T, typename E>
	struct Expected
	{
	private:
		struct UnexpectTag { };
		static constexpr UnexpectTag Tag { };

		union ExpectedData
		{
			T Value;
			E Error;

			~ExpectedData() { }

			explicit ExpectedData(T value) : Value(std::move(value)) { }
			explicit ExpectedData(UnexpectTag tag, E error) : Error(std::move(error)) { }
		};

		ExpectedData m_Data;
		bool m_HasValue;

	public:
		~Expected()
		{
			if (m_HasValue)
			{
				m_Data.Value.~T();
			}
			else
			{
				m_Data.Error.~E();
			}
		}

		Expected(const T& t) : m_Data(t), m_HasValue(true) { }
		Expected(T&& t) : m_Data(std::move(t)), m_HasValue(true) { }
		Expected(const Unexpected<E>& e) : m_Data(Tag, e.Error), m_HasValue(false) { }
		Expected(Unexpected<E>&& e) : m_Data(Tag, std::move(e.Error)), m_HasValue(false) { }

		Expected(const Expected&) = delete;
		Expected& operator=(const Expected&) = delete;
		Expected(Expected&&) = delete;
		Expected& operator=(Expected&&) = delete;

		bool HasValue() const { return m_HasValue; }

		T& GetValue() { LUDUS_ASSERT(m_HasValue, "There must be a value for Expected."); return m_Data.Value; }
		const T& GetValue() const { LUDUS_ASSERT(m_HasValue, "There must be a value for Expected."); return m_Data.Value; }
		E& GetError() { LUDUS_ASSERT(!m_HasValue, "There cannot be an error for Unexpected."); return m_Data.Error; }
		const E& GetError() const { LUDUS_ASSERT(!m_HasValue, "There cannot be an error for Unexpected."); return m_Data.Error; }
	};
}
