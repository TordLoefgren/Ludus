#pragma once

#include <any>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <Ludus/Engine/Core/ITransitionableResource.h>

namespace Ludus::Engine::Core
{
	struct ResourceRegistry
	{
	private:
		std::unordered_map<std::type_index, std::any> m_Registry;
		std::vector<ITransitionableResource*> m_TransitionableResources;

	public:
		template <typename T>
		void Add(T resource)
		{
			using Decayed = std::remove_cvref_t<T>;

			auto& slot = m_Registry[typeid(Decayed)];
			slot = std::any(std::forward<T>(resource));

			if constexpr (std::is_base_of_v<ITransitionableResource, Decayed>)
			{
				auto& stored = std::any_cast<Decayed&>(slot);
				m_TransitionableResources.push_back(&stored);
			}
		}

		template <typename T>
		T& Get()
		{
			auto iter = m_Registry.find(typeid(T));
			if (iter == m_Registry.end())
			{
				throw std::runtime_error("Resource not found.");
			}
			return std::any_cast<T&>(iter->second);
		}

		std::vector<ITransitionableResource*> GetTransitionableResources()
		{
			return m_TransitionableResources;
		}
	};
}