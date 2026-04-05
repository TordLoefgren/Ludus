#pragma once

#include <string>

#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Text = Ludus::Engine::Components::Text2DComponent;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct Text2DComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const Text& text)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "Text" });
			writer.Emit(Token::String { text.Text });

			{
				writer.Emit(Token::Key { "Color" });
				writer.Emit(Token::StartObject { });
				writer.Emit(Token::Key { "R" });
				writer.Emit(Token::Double { text.Color.R });
				writer.Emit(Token::Key { "G" });
				writer.Emit(Token::Double { text.Color.G });
				writer.Emit(Token::Key { "B" });
				writer.Emit(Token::Double { text.Color.B });
				writer.Emit(Token::Key { "A" });
				writer.Emit(Token::Double { text.Color.A });
				writer.Emit(Token::EndObject { });
			}

			const std::string horizontalTextAlignment = Ludus::Engine::Core::Enums::GetDisplayName(text.HorizontalTextAlignment);
			writer.Emit(Token::Key { "HorizontalTextAlignment" });
			writer.Emit(Token::String { horizontalTextAlignment });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Text, SerializationException> Deserialize(
			ITokenStreamReader& reader,
			Ludus::Engine::Core::EntityId ownerId
		)
		{
			try
			{
				Text text;
				text.OwnerId = ownerId;

				Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
				{
					if (key == "Text")
					{
						text.Text = std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
						return;
					}
					if (key == "Color")
					{
						Ludus::Engine::Serialization::Core::ReadObject(reader,
							[&](std::string_view colorKey)
						{
							if (colorKey == "R")
							{
								text.Color.R = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
								return;
							}
							if (colorKey == "G")
							{
								text.Color.G = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
								return;
							}
							if (colorKey == "B")
							{
								text.Color.B = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
								return;
							}
							if (colorKey == "A")
							{
								text.Color.A = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});
						return;
					}
					if (key == "HorizontalTextAlignment")
					{
						std::string horizontalTextAlignment = std::string(
							Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
						Ludus::Engine::Graphics::HorizontalTextAlignment parsed;
						if (Ludus::Engine::Graphics::TryParse(horizontalTextAlignment, parsed))
						{
							text.HorizontalTextAlignment = parsed;
						}
						return;
					}

					Ludus::Engine::Serialization::Core::SkipValue(reader);
				});

				return text;
			}
			catch (const SerializationException& ex)
			{
				const auto error = Ludus::Engine::Serialization::Core::WithContext(
					ex, "Text2DComponentSchema::Deserialize"
				);

				return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
			}
		}
	};
}
