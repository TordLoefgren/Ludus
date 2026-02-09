#pragma once

#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamReader.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamWriter.h>
#include <Ludus/Engine/Persistance/Serialization/TokenRead.h>

namespace Ludus::Engine::Persistance::Schemas
{
	using Transform = Ludus::Engine::Components::Transform2DComponent;
	using ITokenStreamWriter = Ludus::Engine::Persistance::Serialization::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Persistance::Serialization::ITokenStreamReader;
	using Token = Ludus::Engine::Persistance::Serialization::Token;

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

		inline static Ludus::Engine::Core::Expected<Transform, PersistanceException> Deserialize(ITokenStreamReader& reader)
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

				Ludus::Engine::Persistance::Serialization::ReadObject(reader,
					[&](std::string_view key)
					{
						if (key == "OwnerHandle")
						{
							transform.OwnerHandle = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Uint32>(reader).Data;
							hasOwner = true;
							return;
						}
						if (key == "Position")
						{
							Ludus::Engine::Persistance::Serialization::ReadObject(reader,
								[&](std::string_view positionKey)
								{
									if (positionKey == "X")
									{
										transform.Position.X = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Float>(reader).Data;
										return;
									}
									if (positionKey == "Y")
									{
										transform.Position.Y = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Float>(reader).Data;
										return;
									}

									Ludus::Engine::Persistance::Serialization::SkipValue(reader);
								});
							return;
						}
						if (key == "Scale")
						{
							Ludus::Engine::Persistance::Serialization::ReadObject(reader,
								[&](std::string_view scaleKey)
								{
									if (scaleKey == "X")
									{
										transform.Scale.X = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Float>(reader).Data;
										return;
									}
									if (scaleKey == "Y")
									{
										transform.Scale.Y = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Float>(reader).Data;
										return;
									}

									Ludus::Engine::Persistance::Serialization::SkipValue(reader);
								});
							return;
						}
						if (key == "Rotation")
						{
							transform.Rotation = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Float>(reader).Data;
							return;
						}

						Ludus::Engine::Persistance::Serialization::SkipValue(reader);
					});

				if (!hasOwner)
				{
					throw PersistanceException("No owner handle found.");
				}

				return transform;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<Transform, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
