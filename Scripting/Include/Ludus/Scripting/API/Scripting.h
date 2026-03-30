#pragma once

#include "../ABI/Types.h"

// Public entry point and user API for scripting.

namespace Ludus::Scripting::API
{
	using ABI::BodyType;
	using ABI::Collision2DData;
	using ABI::Color;
	using ABI::DisplayNameData;
	using ABI::EntityHandle;
	using ABI::HorizontalTextAlignment;
	using ABI::Key;
	using ABI::MouseButton;
	using ABI::Registry;
	using ABI::RegisterScriptsFn;
	using ABI::RigidBody2DData;
	using ABI::ScriptContext;
	using ABI::ScriptDefinition;
	using ABI::Transform2DData;
	using ABI::Text2DData;
	using ABI::Vector2F;

	bool GetEntityByName(ScriptContext* context, const char* name, EntityHandle* entityHandle);

	void Debug(ScriptContext* context, const char* message);
	void Print(ScriptContext* context, const char* message);

	bool GetKey(ScriptContext* context, Key key);
	bool GetKeyDown(ScriptContext* context, Key key);
	bool GetKeyUp(ScriptContext* context, Key key);

	bool GetMouseButton(ScriptContext* context, MouseButton mouseButton);
	bool GetMouseButtonDown(ScriptContext* context, MouseButton mouseButton);
	bool GetMouseButtonUp(ScriptContext* context, MouseButton mouseButton);

	bool GetDisplayName(ScriptContext* context, EntityHandle entityHandle, DisplayNameData* displayName);
	bool SetDisplayName(ScriptContext* context, EntityHandle entityHandle, const DisplayNameData* displayName);

	bool GetRigidBody(ScriptContext* context, EntityHandle entityHandle, RigidBody2DData* rigidBody);
	bool SetRigidBody(ScriptContext* context, EntityHandle entityHandle, const RigidBody2DData* rigidBody);

	bool GetText(ScriptContext* context, EntityHandle entityHandle, Text2DData* text);
	bool SetText(ScriptContext* context, EntityHandle entityHandle, const Text2DData* text);

	bool GetTransform(ScriptContext* context, EntityHandle entityHandle, Transform2DData* transform);
	bool SetTransform(ScriptContext* context, EntityHandle entityHandle, const Transform2DData* transform);
}

namespace Ludus
{
	namespace Scripts = Scripting::API;
}

#define LUDUS_FORWARD_DECLARE_SCRIPT(SymbolName)                                            \
    void Add_##SymbolName(::Ludus::Scripts::Registry* registry)

#define LUDUS_DEFINE_SCRIPT(SymbolName, CreateFn, DestroyFn, UpdateFn)                      \
    static const ::Ludus::Scripts::ScriptDefinition Definition_##SymbolName                 \
    {                                                                                       \
        #SymbolName,                                                                        \
        CreateFn,                                                                           \
        DestroyFn,                                                                          \
        UpdateFn,                                                                           \
        nullptr,                                                                            \
        nullptr,                                                                            \
        nullptr                                                                             \
    };                                                                                      \
                                                                                            \
    void Add_##SymbolName(::Ludus::Scripts::Registry* registry)                             \
    {                                                                                       \
        ::Ludus::Scripting::ABI::Registry_Add(registry, &Definition_##SymbolName);          \
    }

#define LUDUS_DEFINE_SCRIPT_EX(                                                             \
    SymbolName,                                                                             \
    CreateFn,                                                                               \
    DestroyFn,                                                                              \
    UpdateFn,                                                                               \
    CollisionEnterFn,                                                                       \
    CollisionStayFn,                                                                        \
    CollisionExitFn                                                                         \
)                                                                                           \
    static const ::Ludus::Scripts::ScriptDefinition Definition_##SymbolName                 \
    {                                                                                       \
        #SymbolName,                                                                        \
        CreateFn,                                                                           \
        DestroyFn,                                                                          \
        UpdateFn,                                                                           \
        CollisionEnterFn,                                                                   \
        CollisionStayFn,                                                                    \
        CollisionExitFn                                                                     \
    };                                                                                      \
                                                                                            \
    void Add_##SymbolName(::Ludus::Scripts::Registry* registry)                             \
    {                                                                                       \
        ::Ludus::Scripting::ABI::Registry_Add(registry, &Definition_##SymbolName);          \
    }
