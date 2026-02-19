#pragma once

#include <string>
#include <variant>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Camera = Ludus::Engine::Components::Camera2DComponent;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct Camera2DComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const Camera& camera)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "OwnerHandle" });
			writer.Emit(Token::Uint { camera.OwnerHandle });

			writer.Emit(Token::Key { "OrthographicSize" });
			writer.Emit(Token::Double { camera.OrthographicSize });

			writer.Emit(Token::Key { "Priority" });
			writer.Emit(Token::Int { camera.Priority });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Camera, SerializationException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				Camera camera(0);

				bool hasOwner = false;

				Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
					{
						if (key == "OwnerHandle")
						{
							camera.OwnerHandle = Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader);
							hasOwner = true;
							return;
						}
						if (key == "OrthographicSize")
						{
							camera.OrthographicSize = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
							return;
						}
						if (key == "Priority")
						{
							camera.Priority = Ludus::Engine::Serialization::Core::ConsumeIntLike(reader);
							return;
						}

						Ludus::Engine::Serialization::Core::SkipValue(reader);
					});

				if (!hasOwner)
				{
					throw SerializationException("No owner handle found.");
				}

				return camera;
			}
			catch (const SerializationException& ex)
			{
				const auto error =
					Ludus::Engine::Serialization::Core::WithContext(ex, "Camera2DComponentSchema::Deserialize");
				return Ludus::Engine::Core::Expected<Camera, SerializationException>(
					Ludus::Engine::Core::Unexpected<SerializationException>::Create(error)
				);
			}
		}

	};
}
