#pragma once

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Graphics/Texture.h>
#include <Ludus/Engine/Math/RectInt.h>

namespace Ludus::Engine::Components
{
	struct Sprite2DComponent
	{
	public:
		Ludus::Engine::Core::EntityId OwnerId { Ludus::Engine::Core::EntityId::Invalid() };
		Ludus::Engine::Graphics::Shape Shape = Ludus::Engine::Graphics::Shape::Quad;
		Ludus::Engine::Graphics::Color Color = Ludus::Engine::Graphics::Colors::White;
		Ludus::Engine::Core::AssetId TextureId { Ludus::Engine::Core::AssetId::Invalid() };
		Ludus::Engine::Math::RectInt SourceRect = Ludus::Engine::Math::RectInt::Empty();
		bool Fill = true;
		bool FlipX = false;
		bool FlipY = false;

		Sprite2DComponent() = default;

		explicit Sprite2DComponent(
			Ludus::Engine::Graphics::Shape shape,
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Core::AssetId textureId = { Ludus::Engine::Core::AssetId::Invalid() },
			Ludus::Engine::Math::RectInt sourceRect = Ludus::Engine::Math::RectInt::Empty(),
			bool fill = true,
			bool flipX = false,
			bool flipY = false
		) :
			Shape(shape),
			Color(color),
			TextureId(textureId),
			SourceRect(sourceRect),
			Fill(fill),
			FlipX(flipX),
			FlipY(flipY)
		{}

		Sprite2DComponent(
			Ludus::Engine::Core::EntityId owner,
			Ludus::Engine::Graphics::Shape shape = Ludus::Engine::Graphics::Shape::Quad,
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Core::AssetId textureId = { Ludus::Engine::Core::AssetId::Invalid() },
			Ludus::Engine::Math::RectInt sourceRect = Ludus::Engine::Math::RectInt::Empty(),
			bool fill = true,
			bool flipX = false,
			bool flipY = false
		) :
			OwnerId(owner),
			Shape(shape),
			Color(color),
			TextureId(textureId),
			SourceRect(sourceRect),
			Fill(fill),
			FlipX(flipX),
			FlipY(flipY)
		{}

		~Sprite2DComponent() = default;

		bool operator==(const Sprite2DComponent& other) const { return OwnerId == other.OwnerId; }
	};
}

