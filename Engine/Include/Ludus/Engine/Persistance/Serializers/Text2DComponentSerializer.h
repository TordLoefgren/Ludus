#pragma once

#include <format>
#include <string>

#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/ILoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/ISaveArchive.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>

namespace Ludus::Engine::Persistance::Serializers
{
	using Text = Ludus::Engine::Components::Text2DComponent;
	using ISaveArchive = Ludus::Engine::Persistance::Archives::ISaveArchive;
	using ILoadArchive = Ludus::Engine::Persistance::Archives::ILoadArchive;

	template <class TArchive>
	using ObjectScope = Ludus::Engine::Persistance::Archives::ObjectScope<TArchive>;
	template <class TArchive>
	using OptionalObjectScope = Ludus::Engine::Persistance::Archives::OptionalObjectScope<TArchive>;

	struct Text2DComponentSerializer
	{
		inline static void Save(ISaveArchive& archive, const Text& text)
		{
			ObjectScope textScope(archive, "Text2D");

			archive.WriteUint32("OwnerHandle", text.OwnerHandle);

			archive.WriteString("Text", std::format("{}", text.Text));

			{
				ObjectScope colorScope(archive, "Color");
				archive.WriteFloat("R", text.Color.R);
				archive.WriteFloat("G", text.Color.G);
				archive.WriteFloat("B", text.Color.B);
				archive.WriteFloat("A", text.Color.A);
			}

			archive.WriteString("HorizontalAlignment", std::format("{}", text.HorizontalAlignment));
		}

		inline static Ludus::Engine::Core::Expected<Text, PersistanceException> Load(ILoadArchive& archive)
		{
			try
			{
				Text text(0);

				ObjectScope textScope(archive, "Text2D");

				archive.ReadUint32("OwnerHandle", text.OwnerHandle);

				archive.ReadStringOrDefault("Text", text.Text, "");

				{
					OptionalObjectScope colorScope(archive, "Color");
					if (colorScope)
					{
						archive.ReadFloatOrDefault("R", text.Color.R, 1.0f);
						archive.ReadFloatOrDefault("G", text.Color.G, 1.0f);
						archive.ReadFloatOrDefault("B", text.Color.B, 1.0f);
						archive.ReadFloatOrDefault("A", text.Color.A, 1.0f);
					}
				}

				std::string horizontalAlignment;
				if (archive.TryReadString("HorizontalAlignment", horizontalAlignment))
				{
					Ludus::Engine::Graphics::HorizontalTextAlignment parsed;
					if (Ludus::Engine::Graphics::TryParse(horizontalAlignment, parsed))
					{
						text.HorizontalAlignment = parsed;
					}
				}

				return text;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<Text, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
