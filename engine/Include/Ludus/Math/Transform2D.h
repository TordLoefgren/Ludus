#pragma once

#include <cmath>
#include <cstdint>

#include <Ludus\Engine\Gameobject.h>  
#include <Ludus\Math\Numeric.h>  
#include <Ludus\Math\Vector2D.h>

namespace Ludus::Math
{
	using Ludus::Engine::GameObjectHandle;
	using TransformHandle = uint32_t;

	struct Transform2D
	{
	private:
		inline static TransformHandle s_NextHandle = 1;

	public:
		TransformHandle  Handle;
		GameObjectHandle OwnerHandle;

		Vector2D Position { 0.0f, 0.0f };
		Vector2D Scale { 1.0f, 1.0f };
		float Rotation { 0.0f };

		explicit Transform2D(
			GameObjectHandle ownerHandle,
			Vector2D position = { 0.0f, 0.0f },
			Vector2D scale = { 1.0f, 1.0f },
			float rotation = 0.0f
		)
			: Handle(s_NextHandle++), OwnerHandle(ownerHandle), Position(position), Scale(scale), Rotation(rotation)
		{ }

		// Move-only.
		Transform2D(const Transform2D&) = delete;
		Transform2D& operator=(const Transform2D&) = delete;
		Transform2D(Transform2D&&) noexcept = default;
		Transform2D& operator=(Transform2D&&) noexcept = default;
		~Transform2D() = default;

		bool operator==(const Transform2D& other) const { return Handle == other.Handle; }


		/// <summary>
		/// Computes the forward unit vector from the transform's rotation.
		/// </summary>
		/// <returns>A unit vector pointing in the forward direction.</returns>
		Vector2D Forward() const
		{
			const float r = Numeric::DegreesToRadians(Rotation);
			return { std::cos(r), std::sin(r) };
		}
	};
}
