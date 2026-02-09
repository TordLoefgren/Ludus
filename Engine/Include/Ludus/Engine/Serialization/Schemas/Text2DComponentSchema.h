#pragma once

#include <format>
#include <string>

#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Expected.h>
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

			writer.Emit(Token::Key { "OwnerHandle" });
			writer.Emit(Token::Uint32 { text.OwnerHandle });

			writer.Emit(Token::Key { "Text" });
			writer.Emit(Token::String { text.Text });

			{
				writer.Emit(Token::Key { "Color" });
				writer.Emit(Token::StartObject { });
				writer.Emit(Token::Key { "R" });
				writer.Emit(Token::Float { text.Color.R });
				writer.Emit(Token::Key { "G" });
				writer.Emit(Token::Float { text.Color.G });
				writer.Emit(Token::Key { "B" });
				writer.Emit(Token::Float { text.Color.B });
				writer.Emit(Token::Key { "A" });
				writer.Emit(Token::Float { text.Color.A });
				writer.Emit(Token::EndObject { });
			}

			const std::string horizontalAlignment = std::format("{}", text.HorizontalAlignment);
			writer.Emit(Token::Key { "HorizontalAlignment" });
			writer.Emit(Token::String { horizontalAlignment });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Text, SerializationException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				Text text(0);
				bool hasOwner = false;
				text.Text = "";
				text.Color.R = 1.0f;
				text.Color.G = 1.0f;
				text.Color.B = 1.0f;
				text.Color.A = 1.0f;

				Ludus::Engine::Serialization::Core::ReadObject(reader,
					[&](std::string_view key)
					{
						if (key == "OwnerHandle")
						{
							text.OwnerHandle = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Uint32>(reader).Data;
							hasOwner = true;
							return;
						}
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
										text.Color.R = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}
									if (colorKey == "G")
									{
										text.Color.G = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}
									if (colorKey == "B")
									{
										text.Color.B = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}
									if (colorKey == "A")
									{
										text.Color.A = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}

									Ludus::Engine::Serialization::Core::SkipValue(reader);
								});
							return;
						}
						if (key == "HorizontalAlignment")
						{
							std::string horizontalAlignment = std::string(
								Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
							Ludus::Engine::Graphics::HorizontalTextAlignment parsed;
							if (Ludus::Engine::Graphics::TryParse(horizontalAlignment, parsed))
							{
								text.HorizontalAlignment = parsed;
							}
							return;
						}

						Ludus::Engine::Serialization::Core::SkipValue(reader);
					});

				if (!hasOwner)
				{
					throw SerializationException("No owner handle found.");
				}

				return text;
			}
			catch (const SerializationException& ex)
			{
				return Ludus::Engine::Core::Expected<Text, SerializationException>(
					Ludus::Engine::Core::Unexpected<SerializationException>::Create(ex)
				);
			}
		}
	};
}
