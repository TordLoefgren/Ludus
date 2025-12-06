#pragma once

#include <cstdint>

#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Engine::Graphics
{
	using Camera2DComponentHandle = uint32_t;

	struct Camera2DComponent
	{
	private:
		inline static Camera2DComponentHandle s_NextHandle = 1;

	public:
		Camera2DComponentHandle Handle;
		Ludus::Engine::Core::EntityHandle OwnerHandle;
		float OrthographicSize;
		bool IsMainCamera;

		Camera2DComponent(
			Ludus::Engine::Core::EntityHandle owner,
			float OrthographicSize = 10.0f,
			bool isMainCamera = false
		) :
			Handle(s_NextHandle++),
			OwnerHandle(owner),
			OrthographicSize(OrthographicSize),
			IsMainCamera(isMainCamera)
		{ }

		Camera2DComponent(const Camera2DComponent&) = delete;
		Camera2DComponent& operator=(const Camera2DComponent&) = delete;
		Camera2DComponent(Camera2DComponent&&) noexcept = default;
		Camera2DComponent& operator=(Camera2DComponent&&) noexcept = default;
		~Camera2DComponent() = default;

		bool operator==(const Camera2DComponent& other) const { return Handle == other.Handle; }
	};
}
