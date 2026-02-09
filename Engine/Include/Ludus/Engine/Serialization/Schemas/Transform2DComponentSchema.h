#pragma once

#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Transform = Ludus::Engine::Components::Transform2DComponent;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct Transform2DComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const Transform& transform)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "OwnerHandle" });
			writer.Emit(Token::Uint32 { transform.OwnerHandle });

			{
				writer.Emit(Token::Key { "Position" });
				writer.Emit(Token::StartObject { });
				writer.Emit(Token::Key { "X" });
				writer.Emit(Token::Float { transform.Position.X });
				writer.Emit(Token::Key { "Y" });
				writer.Emit(Token::Float { transform.Position.Y });
				writer.Emit(Token::EndObject { });
			}

			{
				writer.Emit(Token::Key { "Scale" });
				writer.Emit(Token::StartObject { });
				writer.Emit(Token::Key { "X" });
				writer.Emit(Token::Float { transform.Scale.X });
				writer.Emit(Token::Key { "Y" });
				writer.Emit(Token::Float { transform.Scale.Y });
				writer.Emit(Token::EndObject { });
			}

			writer.Emit(Token::Key { "Rotation" });
			writer.Emit(Token::Float { transform.Rotation });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Transform, SerializationException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				Transform transform(0);
				bool hasOwner = false;
				transform.Position.X = 0.0f;
				transform.Position.Y = 0.0f;
				transform.Scale.X = 1.0f;
				transform.Scale.Y = 1.0f;
				transform.Rotation = 0.0f;

				Ludus::Engine::Serialization::Core::ReadObject(reader,
					[&](std::string_view key)
					{
						if (key == "OwnerHandle")
						{
							transform.OwnerHandle = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Uint32>(reader).Data;
							hasOwner = true;
							return;
						}
						if (key == "Position")
						{
							Ludus::Engine::Serialization::Core::ReadObject(reader,
								[&](std::string_view positionKey)
								{
									if (positionKey == "X")
									{
										transform.Position.X = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}
									if (positionKey == "Y")
									{
										transform.Position.Y = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}

									Ludus::Engine::Serialization::Core::SkipValue(reader);
								});
							return;
						}
						if (key == "Scale")
						{
							Ludus::Engine::Serialization::Core::ReadObject(reader,
								[&](std::string_view scaleKey)
								{
									if (scaleKey == "X")
									{
										transform.Scale.X = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}
									if (scaleKey == "Y")
									{
										transform.Scale.Y = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
										return;
									}

									Ludus::Engine::Serialization::Core::SkipValue(reader);
								});
							return;
						}
						if (key == "Rotation")
						{
							transform.Rotation = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Float>(reader).Data;
							return;
						}

						Ludus::Engine::Serialization::Core::SkipValue(reader);
					});

				if (!hasOwner)
				{
					throw SerializationException("No owner handle found.");
				}

				return transform;
			}
			catch (const SerializationException& ex)
			{
				return Ludus::Engine::Core::Expected<Transform, SerializationException>(
					Ludus::Engine::Core::Unexpected<SerializationException>::Create(ex)
				);
			}
		}
	};
}
