#pragma once

#include <string>

#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Core/Enums/EnumTraits.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Id.h>
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

			const std::string shape = Ludus::Engine::Core::Enums::GetDisplayName(sprite.Shape);
			writer.Emit(Token::Key { "Shape" });
			writer.Emit(Token::String { shape });

			{
				writer.Emit(Token::Key { "Color" });
				writer.Emit(Token::StartObject { });
				writer.Emit(Token::Key { "R" });
				writer.Emit(Token::Double { sprite.Color.R });
				writer.Emit(Token::Key { "G" });
				writer.Emit(Token::Double { sprite.Color.G });
				writer.Emit(Token::Key { "B" });
				writer.Emit(Token::Double { sprite.Color.B });
				writer.Emit(Token::Key { "A" });
				writer.Emit(Token::Double { sprite.Color.A });
				writer.Emit(Token::EndObject { });
			}

			writer.Emit(Token::Key { "TextureId" });
			writer.Emit(Token::Uint { sprite.TextureId.Value });

			{
				writer.Emit(Token::Key { "SourceRect" });
				writer.Emit(Token::StartObject { });
				writer.Emit(Token::Key { "X" });
				writer.Emit(Token::Int { sprite.SourceRect.X });
				writer.Emit(Token::Key { "Y" });
				writer.Emit(Token::Int { sprite.SourceRect.Y });
				writer.Emit(Token::Key { "Width" });
				writer.Emit(Token::Int { sprite.SourceRect.Width });
				writer.Emit(Token::Key { "Height" });
				writer.Emit(Token::Int { sprite.SourceRect.Height });
				writer.Emit(Token::EndObject { });
			}

			writer.Emit(Token::Key { "Fill" });
			writer.Emit(Token::Bool { sprite.Fill });

			writer.Emit(Token::Key { "FlipX" });
			writer.Emit(Token::Bool { sprite.FlipX });

			writer.Emit(Token::Key { "FlipY" });
			writer.Emit(Token::Bool { sprite.FlipY });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Sprite, SerializationException> Deserialize(
			ITokenStreamReader& reader,
			Ludus::Engine::Core::EntityId ownerId
		)
		{
			try
			{
				Sprite sprite;
				sprite.OwnerId = ownerId;

				Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
				{
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
						Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view colorKey)
						{
							if (colorKey == "R")
							{
								sprite.Color.R = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
								return;
							}
							if (colorKey == "G")
							{
								sprite.Color.G = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
								return;
							}
							if (colorKey == "B")
							{
								sprite.Color.B = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
								return;
							}
							if (colorKey == "A")
							{
								sprite.Color.A = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});
						return;
					}
					if (key == "TextureId")
					{
						sprite.TextureId = { Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader) };
						return;
					}
					if (key == "SourceRect")
					{
						Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view sourceRectKey)
						{
							if (sourceRectKey == "X")
							{
								sprite.SourceRect.X = Ludus::Engine::Serialization::Core::ConsumeIntLike(reader);
								return;
							}
							if (sourceRectKey == "Y")
							{
								sprite.SourceRect.Y = Ludus::Engine::Serialization::Core::ConsumeIntLike(reader);
								return;
							}
							if (sourceRectKey == "Width")
							{
								sprite.SourceRect.Width = Ludus::Engine::Serialization::Core::ConsumeIntLike(reader);
								return;
							}
							if (sourceRectKey == "Height")
							{
								sprite.SourceRect.Height = Ludus::Engine::Serialization::Core::ConsumeIntLike(reader);
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
					if (key == "FlipX")
					{
						sprite.FlipX = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Bool>(reader).Data;
						return;
					}
					if (key == "FlipY")
					{
						sprite.FlipY = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Bool>(reader).Data;
						return;
					}

					Ludus::Engine::Serialization::Core::SkipValue(reader);
				});

				return sprite;
			}
			catch (const SerializationException& ex)
			{
				const auto error = Ludus::Engine::Serialization::Core::WithContext(
					ex, "Sprite2DComponentSchema::Deserialize"
				);

				return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
			}
		}
	};
}
