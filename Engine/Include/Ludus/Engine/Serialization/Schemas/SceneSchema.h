#pragma once

#include <string_view>

#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Scene = Ludus::Engine::Core::Scene;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;

	struct SceneSchema
	{
	private:
		static constexpr std::string_view DisplayNameString = "DisplayName";
		static constexpr std::string_view Camera2DString = "Camera2D";
		static constexpr std::string_view Collider2DString = "Collider2D";
		static constexpr std::string_view RigidBody2DString = "RigidBody2D";
		static constexpr std::string_view ScriptString = "Script";
		static constexpr std::string_view Sprite2DString = "Sprite2D";
		static constexpr std::string_view Text2DString = "Text2D";
		static constexpr std::string_view Transform2DString = "Transform2D";

	public:
		static void Serialize(ITokenStreamWriter& writer, const Scene& scene);
		static Ludus::Engine::Core::Expected<Scene, SerializationException> Deserialize(ITokenStreamReader& reader);
	};
}
