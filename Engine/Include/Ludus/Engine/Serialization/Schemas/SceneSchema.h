#pragma once

#include <optional>
#include <string_view>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>
#include <Ludus/Engine/Serialization/Schemas/Camera2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Collider2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/DisplayNameComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/RigidBody2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Sprite2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Text2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Transform2DComponentSchema.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Scene = Ludus::Engine::Core::Scene;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct SceneSchema
	{
	private:
		static constexpr std::string_view DisplayNameString = "DisplayName";
		static constexpr std::string_view Camera2DString = "Camera2D";
		static constexpr std::string_view Collider2DString = "Collider2D";
		static constexpr std::string_view RigidBody2DString = "RigidBody2D";
		static constexpr std::string_view Sprite2DString = "Sprite2D";
		static constexpr std::string_view Text2DString = "Text2D";
		static constexpr std::string_view Transform2DString = "Transform2D";

		struct StagedComponents
		{
			std::optional<Ludus::Engine::Components::Camera2DComponent> Camera;
			std::optional<Ludus::Engine::Components::Collider2DComponent> Collider;
			std::optional<Ludus::Engine::Components::DisplayNameComponent> DisplayName;
			std::optional<Ludus::Engine::Components::RigidBody2DComponent> RigidBody;
			std::optional<Ludus::Engine::Components::Sprite2DComponent> Sprite;
			std::optional<Ludus::Engine::Components::Text2DComponent> Text;
			std::optional<Ludus::Engine::Components::Transform2DComponent> Transform;
		};

		template <class SlotT, class LoaderT>
		static inline bool TryReadComponent(ITokenStreamReader& reader, SlotT& componentSlot, LoaderT loader)
		{
			if (!Ludus::Engine::Serialization::Core::Is<Token::StartObject>(reader.Peek()))
			{
				Ludus::Engine::Serialization::Core::SkipValue(reader);
				return false;
			}

			const auto result = loader(reader);
			if (!result.HasValue())
			{
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

	public:
		inline static void Serialize(ITokenStreamWriter& writer, const Scene& scene)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "Handle" });
			writer.Emit(Token::Uint32 { scene.Handle });

			writer.Emit(Token::Key { "Entities" });
			writer.Emit(Token::StartArray { });

			auto& ecs = scene.EntityComponentSystem;
			for (const auto& entity : ecs.View())
			{
				writer.Emit(Token::StartObject { });

				writer.Emit(Token::Key { "Handle" });
				writer.Emit(Token::Uint32 { entity.Handle });

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

		inline static Ludus::Engine::Core::Expected<Scene, SerializationException> Deserialize(ITokenStreamReader& reader)
		{
			Scene scene;

			try
			{
				bool hasHandle = false;

				Ludus::Engine::Serialization::Core::ReadObject(reader,
					[&](std::string_view key)
					{
						if (key == "Handle")
						{
							scene.Handle = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Uint32>(reader).Data;
							hasHandle = true;
							return;
						}
						if (key == "Entities")
						{
							Ludus::Engine::Serialization::Core::ConsumeAs<Token::StartArray>(reader);

							while (!Ludus::Engine::Serialization::Core::Is<Token::EndArray>(reader.Peek()))
							{
								StagedComponents stagedComponents;
								uint32_t entityHandle = 0;
								bool entityValid = true;
								bool hasEntityHandle = false;

								Ludus::Engine::Serialization::Core::ReadObject(reader,
									[&](std::string_view entityKey)
									{
										if (entityKey == "Handle")
										{
											entityHandle = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Uint32>(reader).Data;
											hasEntityHandle = true;
											return;
										}
										if (entityKey == Camera2DString)
										{
											if (!TryReadComponent(reader, stagedComponents.Camera, Camera2DComponentSchema::Deserialize))
											{
												entityValid = false;
											}
											return;
										}
										if (entityKey == Collider2DString)
										{
											if (!TryReadComponent(reader, stagedComponents.Collider, Collider2DComponentSchema::Deserialize))
											{
												entityValid = false;
											}
											return;
										}
										if (entityKey == DisplayNameString)
										{
											if (!TryReadComponent(reader, stagedComponents.DisplayName, DisplayNameComponentSchema::Deserialize))
											{
												entityValid = false;
											}
											return;
										}
										if (entityKey == RigidBody2DString)
										{
											if (!TryReadComponent(reader, stagedComponents.RigidBody, RigidBody2DComponentSchema::Deserialize))
											{
												entityValid = false;
											}
											return;
										}
										if (entityKey == Sprite2DString)
										{
											if (!TryReadComponent(reader, stagedComponents.Sprite, Sprite2DComponentSchema::Deserialize))
											{
												entityValid = false;
											}
											return;
										}
										if (entityKey == Text2DString)
										{
											if (!TryReadComponent(reader, stagedComponents.Text, Text2DComponentSchema::Deserialize))
											{
												entityValid = false;
											}
											return;
										}
										if (entityKey == Transform2DString)
										{
											if (!TryReadComponent(reader, stagedComponents.Transform, Transform2DComponentSchema::Deserialize))
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

								scene.EntityComponentSystem.AddEntityWithHandle(entityHandle);

								if (stagedComponents.Camera.has_value())
								{
									scene.EntityComponentSystem.AttachCamera(stagedComponents.Camera.value());
								}

								if (stagedComponents.Collider.has_value())
								{
									scene.EntityComponentSystem.AttachCollider(stagedComponents.Collider.value());
								}

								if (stagedComponents.DisplayName.has_value())
								{
									scene.EntityComponentSystem.AttachDisplayName(stagedComponents.DisplayName.value());
								}

								if (stagedComponents.RigidBody.has_value())
								{
									scene.EntityComponentSystem.AttachRigidBody(stagedComponents.RigidBody.value());
								}

								if (stagedComponents.Sprite.has_value())
								{
									scene.EntityComponentSystem.AttachSprite(stagedComponents.Sprite.value());
								}

								if (stagedComponents.Text.has_value())
								{
									scene.EntityComponentSystem.AttachText(stagedComponents.Text.value());
								}

								if (stagedComponents.Transform.has_value())
								{
									scene.EntityComponentSystem.AttachTransform(stagedComponents.Transform.value());
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
				return Ludus::Engine::Core::Expected<Scene, SerializationException>(
					Ludus::Engine::Core::Unexpected<SerializationException>::Create(ex)
				);
			}

			return scene;
		}
	};
}
