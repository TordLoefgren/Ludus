#pragma once

#include <cstdint>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Math/Numeric.h>
#include <Ludus/Engine/Math/Rect.h>

namespace Ludus::Engine::Graphics
{
	using Camera2DComponentHandle = uint32_t;

	struct Camera2DComponent
	{
	private:
		inline static Camera2DComponentHandle s_NextHandle = 1;

		float WorldWidth = 0.0f;
		float WorldHeight = 0.0f;

	public:
		Camera2DComponentHandle Handle;
		Ludus::Engine::Core::EntityHandle OwnerHandle;
		float OrthographicSize;


		Camera2DComponent(
			Ludus::Engine::Core::EntityHandle owner,
			float OrthographicSize = 10.0f
		) :
			Handle(s_NextHandle++),
			OwnerHandle(owner),
			OrthographicSize(OrthographicSize)
		{ }

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

		Camera2DComponent(const Camera2DComponent&) = delete;
		Camera2DComponent& operator=(const Camera2DComponent&) = delete;
		Camera2DComponent(Camera2DComponent&&) noexcept = default;
		Camera2DComponent& operator=(Camera2DComponent&&) noexcept = default;
		~Camera2DComponent() = default;

		bool operator==(const Camera2DComponent& other) const { return Handle == other.Handle; }
	};
}
