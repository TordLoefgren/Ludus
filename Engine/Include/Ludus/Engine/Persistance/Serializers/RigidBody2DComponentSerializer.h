#pragma once

#include <format>
#include <string>

#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/ILoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/ISaveArchive.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>

namespace Ludus::Engine::Persistance::Serializers
{
	using RigidBody = Ludus::Engine::Components::RigidBody2DComponent;
	using ISaveArchive = Ludus::Engine::Persistance::Archives::ISaveArchive;
	using ILoadArchive = Ludus::Engine::Persistance::Archives::ILoadArchive;

	template <class TArchive>
	using ObjectScope = Ludus::Engine::Persistance::Archives::ObjectScope<TArchive>;
	template <class TArchive>
	using OptionalObjectScope = Ludus::Engine::Persistance::Archives::OptionalObjectScope<TArchive>;

	struct RigidBody2DComponentSerializer
	{
		inline static void Save(ISaveArchive& archive, const RigidBody& rigidBody)
		{
			ObjectScope rigidBodyScope(archive, "RigidBody2D");

			archive.WriteUint32("OwnerHandle", rigidBody.OwnerHandle);

			{
				ObjectScope velocityScope(archive, "Velocity");
				archive.WriteFloat("X", rigidBody.Velocity.X);
				archive.WriteFloat("Y", rigidBody.Velocity.Y);
			}

			archive.WriteFloat("GravityScale", rigidBody.GravityScale);

			archive.WriteFloat("Mass", rigidBody.Mass);

			archive.WriteString("Type", std::format("{}", rigidBody.Type));
		}

		inline static Ludus::Engine::Core::Expected<RigidBody, PersistanceException> Load(ILoadArchive& archive)
		{
			try
			{
				RigidBody rigidBody(0);

				ObjectScope rigidBodyScope(archive, "RigidBody2D");

				archive.ReadUint32("OwnerHandle", rigidBody.OwnerHandle);

				{
					OptionalObjectScope velocityScope(archive, "Velocity");
					if (velocityScope)
					{
						archive.ReadFloatOrDefault("X", rigidBody.Velocity.X, 0.0f);
						archive.ReadFloatOrDefault("Y", rigidBody.Velocity.Y, 0.0f);
					}
				}

				archive.ReadFloatOrDefault("GravityScale", rigidBody.GravityScale, 1.0f);

				archive.ReadFloatOrDefault("Mass", rigidBody.Mass, 1.0f);

				std::string type;
				if (archive.TryReadString("Type", type))
				{
					Ludus::Engine::Physics::Core::BodyType parsed;
					if (Ludus::Engine::Physics::Core::TryParse(type, parsed))
					{
						rigidBody.Type = parsed;
					}
				}

				return rigidBody;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<RigidBody, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
