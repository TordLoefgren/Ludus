#pragma once

#include <format>
#include <string>

#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/ILoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/ISaveArchive.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>

namespace Ludus::Engine::Persistance::Serializers
{
	using Sprite = Ludus::Engine::Components::Sprite2DComponent;
	using ISaveArchive = Ludus::Engine::Persistance::Archives::ISaveArchive;
	using ILoadArchive = Ludus::Engine::Persistance::Archives::ILoadArchive;

	template <class TArchive>
	using ObjectScope = Ludus::Engine::Persistance::Archives::ObjectScope<TArchive>;
	template <class TArchive>
	using OptionalObjectScope = Ludus::Engine::Persistance::Archives::OptionalObjectScope<TArchive>;

	struct Sprite2DComponentSerializer
	{
		inline static void Save(ISaveArchive& archive, const Sprite& sprite)
		{
			ObjectScope spriteScope(archive, "Sprite2D");

			archive.WriteUint32("OwnerHandle", sprite.OwnerHandle);

			archive.WriteString("Shape", std::format("{}", sprite.Shape));

			{
				ObjectScope colorScope(archive, "Color");
				archive.WriteFloat("R", sprite.Color.R);
				archive.WriteFloat("G", sprite.Color.G);
				archive.WriteFloat("B", sprite.Color.B);
				archive.WriteFloat("A", sprite.Color.A);
			}

			// Texture field requires texture id instead of pointer.

			archive.WriteBool("Fill", sprite.Fill);
		}

		inline static Ludus::Engine::Core::Expected<Sprite, PersistanceException> Load(ILoadArchive& archive)
		{
			try
			{
				Sprite sprite(0);

				ObjectScope spriteScope(archive, "Sprite2D");

				archive.ReadUint32("OwnerHandle", sprite.OwnerHandle);

				std::string shape;
				if (archive.TryReadString("Shape", shape))
				{
					Ludus::Engine::Graphics::Shape parsed;
					if (Ludus::Engine::Graphics::TryParse(shape, parsed))
					{
						sprite.Shape = parsed;
					}
				}

				{
					OptionalObjectScope colorScope(archive, "Color");
					if (colorScope)
					{
						archive.ReadFloatOrDefault("R", sprite.Color.R, 1.0f);
						archive.ReadFloatOrDefault("G", sprite.Color.G, 1.0f);
						archive.ReadFloatOrDefault("B", sprite.Color.B, 1.0f);
						archive.ReadFloatOrDefault("A", sprite.Color.A, 1.0f);
					}
				}

				// Texture field requires texture id instead of pointer.

				archive.ReadBoolOrDefault("Fill", sprite.Fill, true);

				return sprite;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<Sprite, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
