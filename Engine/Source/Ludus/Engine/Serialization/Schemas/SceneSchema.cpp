#include "pch.h"

#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>
#include <Ludus/Engine/Serialization/Schemas/Camera2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Collider2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/DisplayNameComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/RigidBody2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/SceneSchema.h>
#include <Ludus/Engine/Serialization/Schemas/ScriptComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Sprite2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Text2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Transform2DComponentSchema.h>

namespace
{
	using Token = Ludus::Engine::Serialization::Core::Token;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;

	struct StagedComponents
	{
		std::optional<Ludus::Engine::Components::Camera2DComponent> Camera;
		std::optional<Ludus::Engine::Components::Collider2DComponent> Collider;
		std::optional<Ludus::Engine::Components::DisplayNameComponent> DisplayName;
		std::optional<Ludus::Engine::Components::RigidBody2DComponent> RigidBody;
		std::optional<Ludus::Engine::Components::ScriptComponent> Script;
		std::optional<Ludus::Engine::Components::Sprite2DComponent> Sprite;
		std::optional<Ludus::Engine::Components::Text2DComponent> Text;
		std::optional<Ludus::Engine::Components::Transform2DComponent> Transform;
	};

	template <class SlotT, class LoaderT>
	bool TryReadComponent(
		ITokenStreamReader& reader,
		SlotT& componentSlot,
		LoaderT loader,
		std::string_view componentName
	)
	{
		if (!Ludus::Engine::Serialization::Core::Is<Token::StartObject>(reader.Peek()))
		{
			Ludus::Engine::Serialization::Core::SkipValue(reader);
			return false;
		}

		const auto result = loader(reader);
		if (!result.HasValue())
		{
			LUDUS_LOG_WARN(
				std::string("SceneSchema: failed to read component ") +
				std::string(componentName) + ": " + result.GetError().what()
			);

			if (!Ludus::Engine::Serialization::Core::Is<Token::Key>(reader.Peek()) &&
				!Ludus::Engine::Serialization::Core::Is<Token::EndObject>(reader.Peek()) &&
				!Ludus::Engine::Serialization::Core::Is<Token::EndArray>(reader.Peek()))
			{
				Ludus::Engine::Serialization::Core::SkipValue(reader);
			}
			return false;
		}

		componentSlot = std::move(result.GetValue());
		return true;
	}
}

namespace Ludus::Engine::Serialization::Schemas
{
	void SceneSchema::Serialize(ITokenStreamWriter& writer, const Scene& scene)
	{
		writer.Emit(Token::StartObject { });

		writer.Emit(Token::Key { "Handle" });
		writer.Emit(Token::Uint { scene.Handle });

		writer.Emit(Token::Key { "Name" });
		writer.Emit(Token::String { scene.Name });

		writer.Emit(Token::Key { "Entities" });
		writer.Emit(Token::StartArray { });

		auto& ecs = scene.EntityComponentSystem;
		for (const auto& entity : ecs.View())
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "Handle" });
			writer.Emit(Token::Uint { entity.Handle });

			if (const auto* camera = ecs.Cameras.TryGetByOwner(entity.Handle))
			{
				writer.Emit(Token::Key { Camera2DString });
				Camera2DComponentSchema::Serialize(writer, *camera);
			}

			if (const auto* collider = ecs.Colliders.TryGetByOwner(entity.Handle))
			{
				writer.Emit(Token::Key { Collider2DString });
				Collider2DComponentSchema::Serialize(writer, *collider);
			}

			if (const auto* displayName = ecs.DisplayNames.TryGetByOwner(entity.Handle))
			{
				writer.Emit(Token::Key { DisplayNameString });
				DisplayNameComponentSchema::Serialize(writer, *displayName);
			}

			if (const auto* rigidBody = ecs.RigidBodies.TryGetByOwner(entity.Handle))
			{
				writer.Emit(Token::Key { RigidBody2DString });
				RigidBody2DComponentSchema::Serialize(writer, *rigidBody);
			}

			if (const auto* script = ecs.Scripts.TryGetByOwner(entity.Handle))
			{
				writer.Emit(Token::Key { ScriptString });
				ScriptComponentSchema::Serialize(writer, *script);
			}

			if (const auto* sprite = ecs.Sprites.TryGetByOwner(entity.Handle))
			{
				writer.Emit(Token::Key { Sprite2DString });
				Sprite2DComponentSchema::Serialize(writer, *sprite);
			}

			if (const auto* text = ecs.Texts.TryGetByOwner(entity.Handle))
			{
				writer.Emit(Token::Key { Text2DString });
				Text2DComponentSchema::Serialize(writer, *text);
			}

			if (const auto* transform = ecs.Transforms.TryGetByOwner(entity.Handle))
			{
				writer.Emit(Token::Key { Transform2DString });
				Transform2DComponentSchema::Serialize(writer, *transform);
			}

			writer.Emit(Token::EndObject { });
		}

		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
	}

	Ludus::Engine::Core::Expected<Scene, SerializationException> SceneSchema::Deserialize(ITokenStreamReader& reader)
	{
		Scene scene { 0 };

		try
		{
			bool hasHandle = false;

			Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
			{
				if (key == "Handle")
				{
					scene.Handle = Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader);
					hasHandle = true;
					return;
				}
				if (key == "Name")
				{
					scene.Name = Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data;
					return;
				}
				if (key == "Entities")
				{
					Ludus::Engine::Serialization::Core::ConsumeAs<Token::StartArray>(reader);

					while (!Ludus::Engine::Serialization::Core::Is<Token::EndArray>(reader.Peek()))
					{
						StagedComponents stagedComponents;
						uint64_t entityHandle = 0;
						bool entityValid = true;
						bool hasEntityHandle = false;

						Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view entityKey)
						{
							if (entityKey == "Handle")
							{
								entityHandle = Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader);
								hasEntityHandle = true;
								return;
							}
							if (entityKey == Camera2DString)
							{
								if (!TryReadComponent(reader, stagedComponents.Camera, Camera2DComponentSchema::Deserialize, "Camera2D"))
								{
									entityValid = false;
								}
								return;
							}
							if (entityKey == Collider2DString)
							{
								if (!TryReadComponent(reader, stagedComponents.Collider, Collider2DComponentSchema::Deserialize, "Collider2D"))
								{
									entityValid = false;
								}
								return;
							}
							if (entityKey == DisplayNameString)
							{
								if (!TryReadComponent(reader, stagedComponents.DisplayName, DisplayNameComponentSchema::Deserialize, "DisplayName"))
								{
									entityValid = false;
								}
								return;
							}
							if (entityKey == RigidBody2DString)
							{
								if (!TryReadComponent(reader, stagedComponents.RigidBody, RigidBody2DComponentSchema::Deserialize, "RigidBody2D"))
								{
									entityValid = false;
								}
								return;
							}
							if (entityKey == ScriptString)
							{
								if (!TryReadComponent(reader, stagedComponents.Script, ScriptComponentSchema::Deserialize, "Script"))
								{
									entityValid = false;
								}
								return;
							}
							if (entityKey == Sprite2DString)
							{
								if (!TryReadComponent(reader, stagedComponents.Sprite, Sprite2DComponentSchema::Deserialize, "Sprite2D"))
								{
									entityValid = false;
								}
								return;
							}
							if (entityKey == Text2DString)
							{
								if (!TryReadComponent(reader, stagedComponents.Text, Text2DComponentSchema::Deserialize, "Text2D"))
								{
									entityValid = false;
								}
								return;
							}
							if (entityKey == Transform2DString)
							{
								if (!TryReadComponent(reader, stagedComponents.Transform, Transform2DComponentSchema::Deserialize, "Transform2D"))
								{
									entityValid = false;
								}
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});

						if (!hasEntityHandle)
						{
							continue;
						}

						if (stagedComponents.Camera && stagedComponents.Camera->OwnerHandle != entityHandle)
						{
							entityValid = false;
						}
						if (stagedComponents.Collider && stagedComponents.Collider->OwnerHandle != entityHandle)
						{
							entityValid = false;
						}
						if (stagedComponents.DisplayName && stagedComponents.DisplayName->OwnerHandle != entityHandle)
						{
							entityValid = false;
						}
						if (stagedComponents.RigidBody && stagedComponents.RigidBody->OwnerHandle != entityHandle)
						{
							entityValid = false;
						}
						if (stagedComponents.Script && stagedComponents.Script->OwnerHandle != entityHandle)
						{
							entityValid = false;
						}
						if (stagedComponents.Sprite && stagedComponents.Sprite->OwnerHandle != entityHandle)
						{
							entityValid = false;
						}
						if (stagedComponents.Text && stagedComponents.Text->OwnerHandle != entityHandle)
						{
							entityValid = false;
						}
						if (stagedComponents.Transform && stagedComponents.Transform->OwnerHandle != entityHandle)
						{
							entityValid = false;
						}

						if (!entityValid)
						{
							continue;
						}

						scene.EntityComponentSystem.RestoreEntity(entityHandle);

						if (stagedComponents.Camera)
						{
							scene.EntityComponentSystem.AttachCamera(*stagedComponents.Camera);
						}

						if (stagedComponents.Collider)
						{
							scene.EntityComponentSystem.AttachCollider(*stagedComponents.Collider);
						}

						if (stagedComponents.DisplayName)
						{
							scene.EntityComponentSystem.AttachDisplayName(*stagedComponents.DisplayName);
						}

						if (stagedComponents.RigidBody)
						{
							scene.EntityComponentSystem.AttachRigidBody(*stagedComponents.RigidBody);
						}

						if (stagedComponents.Script)
						{
							scene.EntityComponentSystem.AttachScript(*stagedComponents.Script);
						}

						if (stagedComponents.Sprite)
						{
							scene.EntityComponentSystem.AttachSprite(*stagedComponents.Sprite);
						}

						if (stagedComponents.Text)
						{
							scene.EntityComponentSystem.AttachText(*stagedComponents.Text);
						}

						if (stagedComponents.Transform)
						{
							scene.EntityComponentSystem.AttachTransform(*stagedComponents.Transform);
						}
					}

					Ludus::Engine::Serialization::Core::ConsumeAs<Token::EndArray>(reader);
					return;
				}

				Ludus::Engine::Serialization::Core::SkipValue(reader);
			});

			if (!hasHandle)
			{
				throw SerializationException("No scene handle found.");
			}
		}
		catch (const SerializationException& ex)
		{
			const auto error = Ludus::Engine::Serialization::Core::WithContext(
				ex, "SceneSchema::Deserialize"
			);

			return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
		}

		return scene;
	}
}
