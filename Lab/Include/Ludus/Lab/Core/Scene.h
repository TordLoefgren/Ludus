#pragma once

#include <Ludus/Engine/Core/Cooldown.h>
#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Core/State.h>
#include <Ludus/Engine/Core/SystemPredicate.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/Shape.h>

namespace Ludus::Lab::Core
{
	struct AABBDemo2Info
	{
		const Ludus::Engine::Graphics::Color CollisionColor = Ludus::Engine::Graphics::Colors::Orange.WithAlpha(0.5f);
		const Ludus::Engine::Graphics::Color NonCollisionColor = Ludus::Engine::Graphics::Colors::LightBlue.WithAlpha(0.5f);
		Ludus::Engine::Graphics::Color CursorColor = Ludus::Engine::Graphics::Colors::Gray;
		const int CursorLayerIndex = 1;
		const int QuadLayerIndex = 2;
		const int MaxCount = 120;
		const std::string CursorLayerName = "Cursor";
		const std::string QuadLayerName = "Quad";
		Ludus::Engine::Core::EntityHandle CursorHandle = 0;
		bool IsColliding = false;
	};

	struct FallingQuad
	{
		Ludus::Engine::Core::EntityHandle Handle;
		float Speed;

		bool operator==(const FallingQuad& other) const { return Handle == other.Handle; }
	};

	class Scene : public Ludus::Engine::Core::ISystem
	{
		AABBDemo2Info m_Info;
		Ludus::Engine::Core::Random m_Random;
		Ludus::Engine::Core::Cooldown m_Cooldown;
		std::vector<FallingQuad> m_FallingQuads;

		FallingQuad CreateQuad();

	public:
		Scene();

		virtual void OnAttachImpl() override;
		virtual void UpdateImpl(float deltaTime) override;
	};
}
