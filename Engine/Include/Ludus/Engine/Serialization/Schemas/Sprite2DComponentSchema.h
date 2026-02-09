#pragma once

#include <format>
#include <string>

#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Sprite = Ludus::Engine::Components::Sprite2DComponent;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct Sprite2DComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const Sprite& sprite)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "OwnerHandle" });
			writer.Emit(Token::Uint32 { sprite.OwnerHandle });

			const std::string shape = std::format("{}", sprite.Shape);
			writer.Emit(Token::Key { "Shape" });
			writer.Emit(Token::String { shape });

			{
				writer.Emit(Token::Key { "Color" });
				writer.Emit(Token::StartObject { });
				writer.Emit(Token::Key { "R" });
				writer.Emit(Token::Float { sprite.Color.R });
				writer.Emit(Token::Key { "G" });
				writer.Emit(Token::Float { sprite.Color.G });
				writer.Emit(Token::Key { "B" });
				writer.Emit(Token::Float { sprite.Color.B });
				writer.Emit(Token::Key { "A" });
				writer.Emit(Token::Float { sprite.Color.A });
				writer.Emit(Token::EndObject { });
			}

			// Texture field requires texture id instead of pointer.

			writer.Emit(Token::Key { "Fill" });
			writer.Emit(Token::Bool { sprite.Fill });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Sprite, SerializationException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				Sprite sprite(0);
				bool hasOwner = false;
				sprite.Color.R = 1.0f;
				sprite.Color.G = 1.0f;
				sprite.Color.B = 1.0f;
				sprite.Color.A = 1.0f;
				sprite.Fill = true;

				Ludus::Engine::Serialization::Core::ReadObject(reader,
					[&](std::string_view key)
					{
						if (key == "OwnerHandle")
						{
							sprite.OwnerHandle = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Uint32>(reader).Data;
							hasOwner = true;
							return;
						}
						if (key == "Shape")
						{
							std::string shapeValue = std::string(
								Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
							Ludus::Engine::Graphics::Shape parsed;
							if (Ludus::Engine::Graphics::TryParse(shapeValue, parsed))
							{
								sprite.Shape = parsed;
							}
							return;
						}
						if (key == "Color")
						{
							Ludus::Engine::Serialization::Core::ReadObject(reader,
								[&](std::string_view colorKey)
								{
									if (colorKey == "R")
									{
										sprite.Color.R = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}
									if (colorKey == "G")
									{
										sprite.Color.G = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}
									if (colorKey == "B")
									{
										sprite.Color.B = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}
									if (colorKey == "A")
									{
										sprite.Color.A = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}

									Ludus::Engine::Serialization::Core::SkipValue(reader);
								});
							return;
						}
						if (key == "Fill")
						{
							sprite.Fill = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Bool>(reader).Data;
							return;
						}

						Ludus::Engine::Serialization::Core::SkipValue(reader);
					});

				if (!hasOwner)
				{
					throw SerializationException("No owner handle found.");
				}

				return sprite;
			}
			catch (const SerializationException& ex)
			{
				return Ludus::Engine::Core::Expected<Sprite, SerializationException>(
					Ludus::Engine::Core::Unexpected<SerializationException>::Create(ex)
				);
			}
		}
	};
}
