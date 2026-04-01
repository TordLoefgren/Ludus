#pragma once

#include <cstdint>

namespace Ludus::Scripting::ABI
{
	struct Version
	{
		std::uint32_t Major;
		std::uint32_t Minor;
		std::uint32_t Patch;
	};

	inline constexpr Version CurrentAPIVersion { 0, 1, 0 };

	struct DisplayNameData
	{
		const char* Value;
	};

	struct Vector2F
	{
		float X = 0.0f;
		float Y = 0.0f;
	};

	enum class BodyType
	{
		Dynamic,
		Kinematic,
		Static
	};

	struct RigidBody2DData
	{
		Vector2F Velocity { };
		float GravityScale = 1.0f;
		float Mass = 0.0f;
		BodyType BodyType = BodyType::Dynamic;
	};

	struct Color
	{
		float R = 1.0f;
		float G = 1.0f;
		float B = 1.0f;
		float A = 1.0f;
	};

	enum class HorizontalTextAlignment
	{
		Left,
		Center,
		Right
	};

	struct Text2DData
	{
		const char* Text;
		Color Color;
		HorizontalTextAlignment HorizontalTextAlignment;
	};

	struct Transform2DData
	{
		Vector2F Position { };
		Vector2F Scale { 1.0f, 1.0f };
		float Rotation = 0.0f;
	};

	struct Collision2DData
	{
		std::uint64_t Other = 0;
		Vector2F LocalSelf { };
		Vector2F LocalOther { };
		Vector2F Normal { };
		float Penetration = 0.0f;
		bool IsTrigger = false;
	};

	using EntityHandle = std::uint64_t;
	using SceneHandle = std::uint64_t;

	struct ScriptAPI;
	struct ScriptContext;

	enum class Key : std::int32_t
	{
		Unknown = -1,

		Space = 32,
		Apostrophe = 39,
		Comma = 44,
		Minus = 45,
		Period = 46,
		Slash = 47,

		D0 = 48,
		D1 = 49,
		D2 = 50,
		D3 = 51,
		D4 = 52,
		D5 = 53,
		D6 = 54,
		D7 = 55,
		D8 = 56,
		D9 = 57,

		Semicolon = 59,
		Equal = 61,

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,
		Backslash = 92,
		RightBracket = 93,
		GraveAccent = 96,

		World1 = 161,
		World2 = 162,

		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		DeleteKey = 261,

		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,

		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,

		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,

		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		KP_0 = 320,
		KP_1 = 321,
		KP_2 = 322,
		KP_3 = 323,
		KP_4 = 324,
		KP_5 = 325,
		KP_6 = 326,
		KP_7 = 327,
		KP_8 = 328,
		KP_9 = 329,
		KP_Decimal = 330,
		KP_Divide = 331,
		KP_Multiply = 332,
		KP_Subtract = 333,
		KP_Add = 334,
		KP_Enter = 335,
		KP_Equal = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	};

	enum class MouseButton : std::int32_t
	{
		Left = 0,
		Right = 1,
		Middle = 2
	};

	using GetEntityByNameFn = bool(*)(ScriptContext* context, const char* name, EntityHandle* entityHandle);

	using DebugFn = void(*)(ScriptContext* context, const char* message);
	using PrintFn = void(*)(ScriptContext* context, const char* message);

	using GetKeyFn = bool(*)(ScriptContext* context, Key key);
	using GetKeyDownFn = bool(*)(ScriptContext* context, Key key);
	using GetKeyUpFn = bool(*)(ScriptContext* context, Key key);

	using GetMouseButtonFn = bool(*)(ScriptContext* context, MouseButton mouseButton);
	using GetMouseButtonDownFn = bool(*)(ScriptContext* context, MouseButton mouseButton);
	using GetMouseButtonUpFn = bool(*)(ScriptContext* context, MouseButton mouseButton);

	using GetDisplayNameFn = bool(*)(ScriptContext* context, EntityHandle entityHandle, DisplayNameData* displayName);
	using SetDisplayNameFn = bool(*)(ScriptContext* context, EntityHandle entityHandle, const DisplayNameData* displayName);

	using GetRigidBodyFn = bool(*)(ScriptContext* context, EntityHandle entityHandle, RigidBody2DData* rigidBody);
	using SetRigidBodyFn = bool(*)(ScriptContext* context, EntityHandle entityHandle, const RigidBody2DData* rigidBody);

	using GetTransformFn = bool(*)(ScriptContext* context, EntityHandle entityHandle, Transform2DData* transform);
	using SetTransformFn = bool(*)(ScriptContext* context, EntityHandle entityHandle, const Transform2DData* transform);

	using GetTextFn = bool(*)(ScriptContext* context, EntityHandle entityHandle, Text2DData* text);
	using SetTextFn = bool(*)(ScriptContext* context, EntityHandle entityHandle, const Text2DData* text);

	using OnCreateFn = void(*)(ScriptContext*, EntityHandle);
	using OnDestroyFn = void(*)(ScriptContext*, EntityHandle);
	using OnUpdateFn = void(*)(ScriptContext*, EntityHandle, float);
	using OnCollisionEnterFn = void(*)(ScriptContext*, EntityHandle, const Collision2DData*);
	using OnCollisionStayFn = void(*)(ScriptContext*, EntityHandle, const Collision2DData*);
	using OnCollisionExitFn = void(*)(ScriptContext*, EntityHandle, const Collision2DData*);

	struct ScriptDefinition
	{
		const char* Name = nullptr;
		OnCreateFn OnCreate = nullptr;
		OnDestroyFn OnDestroy = nullptr;
		OnUpdateFn OnUpdate = nullptr;
		OnCollisionEnterFn OnCollisionEnter = nullptr;
		OnCollisionStayFn OnCollisionStay = nullptr;
		OnCollisionExitFn OnCollisionExit = nullptr;
	};

	using AddScriptDefinitionFn = void(*)(void*, const ScriptDefinition*);

	struct Registry
	{
		void* Context = nullptr;
		AddScriptDefinitionFn AddFn = nullptr;
	};

	inline void Registry_Add(Registry* registry, const ScriptDefinition* definition)
	{
		if (!registry || !registry->AddFn || !registry->Context || !definition)
		{
			return;
		}

		registry->AddFn(registry->Context, definition);
	}

	using RegisterScriptsFn = void(*)(Registry*);

	inline constexpr const char* RegisterSymbolName = "RegisterScripts";
}
