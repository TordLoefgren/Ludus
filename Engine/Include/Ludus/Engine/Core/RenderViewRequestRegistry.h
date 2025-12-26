#pragma once

#include <span>
#include <vector>

#include <Ludus/Engine/Graphics/RenderViewRequest2D.h>

namespace Ludus::Engine::Core
{
	struct RenderViewRequestRegistry
	{
	private:
		std::vector<Ludus::Engine::Graphics::RenderViewRequest2D> m_RenderViewRequests;

	public:
		std::span<const Ludus::Engine::Graphics::RenderViewRequest2D> View() const { return m_RenderViewRequests; }

		void Register(const Ludus::Engine::Graphics::RenderViewRequest2D& renderViewRequest)
		{
			m_RenderViewRequests.push_back(renderViewRequest);
		}

		void Clear() { return m_RenderViewRequests.clear(); }

		bool Empty() const { return m_RenderViewRequests.empty(); }

		size_t GetSize() const { return m_RenderViewRequests.size(); }
	};
}
