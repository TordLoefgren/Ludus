#pragma once

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Math/Numeric.h>
#include <Ludus/Engine/Math/Rect.h>

namespace Ludus::Engine::Components
{
	struct Camera2DComponent
	{
	private:
		float WorldWidth = 0.0f;
		float WorldHeight = 0.0f;

	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle;
		float OrthographicSize;


		Camera2DComponent(
			Ludus::Engine::Core::EntityHandle owner,
			float OrthographicSize = 10.0f
		) :
			OwnerHandle(owner),
			OrthographicSize(OrthographicSize)
		{ }

		~Camera2DComponent() = default;

		bool operator==(const Camera2DComponent& other) const { return OwnerHandle == other.OwnerHandle; }

		float HalfWorldWidth()  const { return WorldWidth * 0.5f; }
		float HalfWorldHeight() const { return WorldHeight * 0.5f; }

		Ludus::Engine::Math::Rect GetViewportRect() { return { { 0.0f, 0.0f}, { WorldWidth, WorldHeight} }; }

		void SetWorldFromViewport(float width, float height)
		{
			auto viewportWidth = Ludus::Engine::Math::Numeric::Max(1.0f, width);
			auto viewportHeight = Ludus::Engine::Math::Numeric::Max(1.0f, height);

			auto ratio = viewportWidth / viewportHeight;
			auto worldHeight = OrthographicSize * 2.0f;
			auto worldWidth = worldHeight * ratio;

			WorldWidth = worldWidth;
			WorldHeight = worldHeight;
		}
	};
}
