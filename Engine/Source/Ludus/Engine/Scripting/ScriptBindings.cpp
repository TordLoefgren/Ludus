#include "pch.h"

#include <stdexcept>
#include <string>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>
#include <Ludus/Engine/Runtime/SceneRuntimeState.h>
#include <Ludus/Engine/Scripting/ScriptBindings.h>
#include <Ludus/Engine/Windowing/Input.h>
#include <Ludus/Scripting/ABI/ScriptContext.h>
#include <Ludus/Scripting/ABI/Types.h>

namespace Ludus::Engine::Scripting
{
	using ScriptContext = Ludus::Scripting::ABI::ScriptContext;
	using ScriptAPI = Ludus::Scripting::ABI::ScriptAPI;
	using ABIEntityId = Ludus::Scripting::ABI::EntityHandle;
	using Key = Ludus::Scripting::ABI::Key;
	using MouseButton = Ludus::Scripting::ABI::MouseButton;
	using DisplayNameData = Ludus::Scripting::ABI::DisplayNameData;
	using RigidBody2DData = Ludus::Scripting::ABI::RigidBody2DData;
	using Text2DData = Ludus::Scripting::ABI::Text2DData;
	using Transform2DData = Ludus::Scripting::ABI::Transform2DData;

	struct ScriptHost
	{
		const Ludus::Engine::Runtime::RuntimeManifest& RuntimeManifest;
		Ludus::Engine::Core::SceneRegistry& SceneRegistry;
		Ludus::Engine::Runtime::SceneRuntimeState& SceneRuntimeState;
		Ludus::Engine::Windowing::Input& Input;
		Ludus::Engine::Core::SceneId ContextSceneId;
	};

	struct ScriptBindingsState
	{
		ScriptHost Host;
		ScriptAPI API;
	};

	namespace
	{
		ScriptHost& ResolveHost(ScriptContext* context)
		{
			auto* host = reinterpret_cast<ScriptHost*>(Ludus::Scripting::ABI::GetScriptHost(context));
			if (!host)
			{
				throw std::runtime_error("Scripting host could not be resolved.");
			}

			return *host;
		}

		Ludus::Engine::Core::Scene& ResolveScene(const ScriptHost& host)
		{
			return host.SceneRegistry.GetScene(host.ContextSceneId);
		}

		Ludus::Engine::Core::EntityId FromABI(ABIEntityId id)
		{
			return Ludus::Engine::Core::EntityId { id };
		}

		ABIEntityId ToABI(Ludus::Engine::Core::EntityId id)
		{
			return id.Value;
		}

#pragma region Entity and Scene bindings

		static bool GetEntityByName(ScriptContext* context, const char* name, ABIEntityId* entityId)
		{
			LUDUS_ASSERT(entityId != nullptr, "Script binding output entity id must not be null.");
			if (!entityId)
			{
				return false;
			}

			if (!name || *name == '\0')
			{
				LUDUS_LOG_WARN("GetEntityByName requires a non-empty entity name.");
				return false;
			}

			auto& host = ResolveHost(context);
			const auto& scene = ResolveScene(host);

			for (const auto& displayName : scene.EntityComponentSystem.DisplayNames.View())
			{
				if (displayName.Name == name)
				{
					*entityId = ToABI(displayName.OwnerId);
					return true;
				}
			}

			return false;
		}

		static bool LoadSceneByName(ScriptContext* context, const char* name)
		{
			auto& host = ResolveHost(context);
			if (!name || *name == '\0')
			{
				LUDUS_LOG_WARN("LoadSceneByName requires a non-empty scene name.");
				return false;
			}

			std::filesystem::path scenePath;
			for (const auto& reference : host.RuntimeManifest.Scenes)
			{
				if (reference.Name == name)
				{
					scenePath = reference.Path;
					break;
				}
			}

			if (scenePath.empty())
			{
				LUDUS_LOG_WARN("LoadSceneByName could not find scene: " + std::string(name));
				return false;
			}

			host.SceneRuntimeState.PendingTransition = Ludus::Engine::Runtime::PendingSceneTransition::LoadScene(scenePath);

			return true;
		}

#pragma endregion

#pragma region Logging bindings

		static void OnDebugImpl(ScriptContext* context, const char* message)
		{
			LUDUS_LOG_DEBUG(message ? message : "");
		}

		static void OnPrintImpl(ScriptContext* context, const char* message)
		{
			LUDUS_LOG_INFO(message ? message : "");
		}

#pragma endregion

#pragma region Input bindings

		static bool OnGetKey(ScriptContext* context, Key key)
		{
			return ResolveHost(context).Input.GetKey(static_cast<Ludus::Engine::Windowing::Key>(key));
		}

		static bool OnGetKeyDown(ScriptContext* context, Key key)
		{
			return ResolveHost(context).Input.GetKeyDown(static_cast<Ludus::Engine::Windowing::Key>(key));
		}

		static bool OnGetKeyUp(ScriptContext* context, Key key)
		{
			return ResolveHost(context).Input.GetKeyUp(static_cast<Ludus::Engine::Windowing::Key>(key));
		}

		static bool OnGetMouseButton(ScriptContext* context, MouseButton mouseButton)
		{
			return ResolveHost(context).Input.GetMouseButton(static_cast<Ludus::Engine::Windowing::MouseButton>(mouseButton));
		}

		static bool OnGetMouseButtonDown(ScriptContext* context, MouseButton mouseButton)
		{
			return ResolveHost(context).Input.GetMouseButtonDown(static_cast<Ludus::Engine::Windowing::MouseButton>(mouseButton));
		}

		static bool OnGetMouseButtonUp(ScriptContext* context, MouseButton mouseButton)
		{
			return ResolveHost(context).Input.GetMouseButtonUp(static_cast<Ludus::Engine::Windowing::MouseButton>(mouseButton));
		}

#pragma endregion

#pragma region Display Name bindings

		Ludus::Scripting::ABI::DisplayNameData ToABI(const Ludus::Engine::Components::DisplayNameComponent& displayName)
		{
			return DisplayNameData
			{
				.Name = displayName.Name.c_str(),
			};
		}

		void ApplyFromABI(
			Ludus::Engine::Components::DisplayNameComponent& destination,
			const Ludus::Scripting::ABI::DisplayNameData& source
		)
		{
			destination.Name = source.Name;
		}

		static bool OnGetDisplayName(ScriptContext* context, ABIEntityId entityId, DisplayNameData* displayNameOut)
		{
			LUDUS_ASSERT(displayNameOut != nullptr, "Script binding output display name must not be null.");
			if (!displayNameOut)
			{
				return false;
			}

			auto& host = ResolveHost(context);
			const auto& scene = ResolveScene(host);

			const auto* displayName = scene.EntityComponentSystem.DisplayNames.TryGetByOwner(FromABI(entityId));
			if (!displayName)
			{
				return false;
			}

			*displayNameOut = ToABI(*displayName);

			return true;
		}

		static bool OnSetDisplayName(ScriptContext* context, ABIEntityId entityId, const DisplayNameData* displayNameIn)
		{
			LUDUS_ASSERT(displayNameIn != nullptr, "Script binding input display name must not be null.");
			if (!displayNameIn)
			{
				return false;
			}

			auto& host = ResolveHost(context);
			auto& scene = ResolveScene(host);

			auto* displayName = scene.EntityComponentSystem.DisplayNames.TryGetByOwnerMutable(FromABI(entityId));
			if (!displayName)
			{
				return false;
			}

			ApplyFromABI(*displayName, *displayNameIn);

			return true;
		}

#pragma endregion

#pragma region RigidBody bindings

		Ludus::Scripting::ABI::RigidBody2DData ToABI(const Ludus::Engine::Components::RigidBody2DComponent& rigidBody)
		{
			return RigidBody2DData
			{
				.Velocity = { rigidBody.Velocity.X, rigidBody.Velocity.Y },
				.GravityScale = rigidBody.GravityScale,
				.Mass = rigidBody.Mass,
				.BodyType = static_cast<Ludus::Scripting::ABI::BodyType>(rigidBody.BodyType)
			};
		}

		void ApplyFromABI(
			Ludus::Engine::Components::RigidBody2DComponent& destination,
			const Ludus::Scripting::ABI::RigidBody2DData& source
		)
		{
			destination.Velocity = { source.Velocity.X, source.Velocity.Y };
			destination.GravityScale = source.GravityScale;
			destination.Mass = source.Mass;
			destination.BodyType = static_cast<Ludus::Engine::Physics::Core::BodyType>(source.BodyType);
		}

		static bool OnGetRigidBody(ScriptContext* context, ABIEntityId entityId, RigidBody2DData* rigidBodyOut)
		{
			LUDUS_ASSERT(rigidBodyOut != nullptr, "Script binding output rigid body must not be null.");
			if (!rigidBodyOut)
			{
				return false;
			}

			auto& host = ResolveHost(context);
			const auto& scene = ResolveScene(host);

			const auto* rigidBody = scene.EntityComponentSystem.RigidBodies.TryGetByOwner(FromABI(entityId));
			if (!rigidBody)
			{
				return false;
			}

			*rigidBodyOut = ToABI(*rigidBody);

			return true;
		}

		static bool OnSetRigidBody(ScriptContext* context, ABIEntityId entityId, const RigidBody2DData* rigidBodyIn)
		{
			LUDUS_ASSERT(rigidBodyIn != nullptr, "Script binding input rigid body must not be null.");
			if (!rigidBodyIn)
			{
				return false;
			}

			auto& host = ResolveHost(context);
			auto& scene = ResolveScene(host);

			auto* rigidBody = scene.EntityComponentSystem.RigidBodies.TryGetByOwnerMutable(FromABI(entityId));
			if (!rigidBody)
			{
				return false;
			}

			ApplyFromABI(*rigidBody, *rigidBodyIn);

			return true;
		}

#pragma endregion

#pragma region Text bindings

		Ludus::Scripting::ABI::Text2DData ToABI(const Ludus::Engine::Components::Text2DComponent& text)
		{
			return Text2DData
			{
				.Text = text.Text.c_str(),
				.Color = { text.Color.R, text.Color.G, text.Color.B, text.Color.A },
				.HorizontalTextAlignment = static_cast<Ludus::Scripting::ABI::HorizontalTextAlignment>(text.HorizontalTextAlignment)
			};
		}

		void ApplyFromABI(
			Ludus::Engine::Components::Text2DComponent& destination,
			const Ludus::Scripting::ABI::Text2DData& source
		)
		{
			destination.Text = source.Text;
			destination.Color = { source.Color.R, source.Color.G, source.Color.B, source.Color.A };
			destination.HorizontalTextAlignment = static_cast<Ludus::Engine::Graphics::HorizontalTextAlignment>(source.HorizontalTextAlignment);
		}

		static bool OnGetText(ScriptContext* context, ABIEntityId entityId, Text2DData* textOut)
		{
			LUDUS_ASSERT(textOut != nullptr, "Script binding output text must not be null.");
			if (!textOut)
			{
				return false;
			}

			auto& host = ResolveHost(context);
			const auto& scene = ResolveScene(host);

			const auto* text = scene.EntityComponentSystem.Texts.TryGetByOwner(FromABI(entityId));
			if (!text)
			{
				return false;
			}

			*textOut = ToABI(*text);

			return true;
		}

		static bool OnSetText(ScriptContext* context, ABIEntityId entityId, const Text2DData* textIn)
		{
			LUDUS_ASSERT(textIn != nullptr, "Script binding input text must not be null.");
			if (!textIn)
			{
				return false;
			}

			auto& host = ResolveHost(context);
			auto& scene = ResolveScene(host);

			auto* text = scene.EntityComponentSystem.Texts.TryGetByOwnerMutable(FromABI(entityId));
			if (!text)
			{
				return false;
			}

			ApplyFromABI(*text, *textIn);

			return true;
		}

#pragma endregion

#pragma region Transform bindings

		Ludus::Scripting::ABI::Transform2DData ToABI(const Ludus::Engine::Components::Transform2DComponent& transform)
		{
			return Transform2DData
			{
				.Position = { transform.Position.X, transform.Position.Y },
				.Scale = { transform.Scale.X, transform.Scale.Y },
				.Rotation = transform.Rotation
			};
		}

		void ApplyFromABI(
			Ludus::Engine::Components::Transform2DComponent& destination,
			const Ludus::Scripting::ABI::Transform2DData& source
		)
		{
			destination.Position = { source.Position.X, source.Position.Y };
			destination.Scale = { source.Scale.X, source.Scale.Y };
			destination.Rotation = source.Rotation;
		}

		static bool OnGetTransform(ScriptContext* context, ABIEntityId entityId, Transform2DData* transformOut)
		{
			LUDUS_ASSERT(transformOut != nullptr, "Script binding output transform must not be null.");
			if (!transformOut)
			{
				return false;
			}

			auto& host = ResolveHost(context);
			const auto& scene = ResolveScene(host);

			const auto* transform = scene.EntityComponentSystem.Transforms.TryGetByOwner(FromABI(entityId));
			if (!transform)
			{
				return false;
			}

			*transformOut = ToABI(*transform);

			return true;
		}

		static bool OnSetTransform(ScriptContext* context, ABIEntityId entityId, const Transform2DData* transformIn)
		{
			LUDUS_ASSERT(transformIn != nullptr, "Script binding input transform must not be null.");
			if (!transformIn)
			{
				return false;
			}

			auto& host = ResolveHost(context);
			auto& scene = ResolveScene(host);

			auto* transform = scene.EntityComponentSystem.Transforms.TryGetByOwnerMutable(FromABI(entityId));
			if (!transform)
			{
				return false;
			}

			ApplyFromABI(*transform, *transformIn);

			return true;
		}

#pragma endregion

	}

	ScriptBindingsState* CreateScriptBindingsState(
		const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest,
		Ludus::Engine::Core::SceneRegistry& sceneRegistry,
		Ludus::Engine::Runtime::SceneRuntimeState& sceneRuntimeState,
		Ludus::Engine::Windowing::Input& input
	)
	{
		auto* state = new ScriptBindingsState
		{
			.Host = { runtimeManifest, sceneRegistry, sceneRuntimeState, input, sceneRuntimeState.Presentation.CurrentSceneId },
			.API = {
				.Version = Ludus::Scripting::ABI::CurrentAPIVersion,
				.GetEntityByName = &GetEntityByName,
				.LoadSceneByName = &LoadSceneByName,
				.Debug = &OnDebugImpl,
				.Print = &OnPrintImpl,
				.GetKey = &OnGetKey,
				.GetKeyDown = &OnGetKeyDown,
				.GetKeyUp = &OnGetKeyUp,
				.GetMouseButton = &OnGetMouseButton,
				.GetMouseButtonDown = &OnGetMouseButtonDown,
				.GetMouseButtonUp = &OnGetMouseButtonUp,
				.GetDisplayName = &OnGetDisplayName,
				.SetDisplayName = &OnSetDisplayName,
				.GetRigidBody = &OnGetRigidBody,
				.SetRigidBody = &OnSetRigidBody,
				.GetText = &OnGetText,
				.SetText = &OnSetText,
				.GetTransform = &OnGetTransform,
				.SetTransform = &OnSetTransform }
		};

		return state;
	}

	void DestroyScriptBindingsState(ScriptBindingsState* state)
	{
		delete state;
	}

	void SetContextScene(ScriptBindingsState* state, Ludus::Engine::Core::SceneId sceneId)
	{
		state->Host.ContextSceneId = sceneId;
	}

	const ScriptAPI* GetScriptAPI(const ScriptBindingsState* state)
	{
		return &state->API;
	}

	void* GetScriptHost(ScriptBindingsState* state)
	{
		return &state->Host;
	}
}
