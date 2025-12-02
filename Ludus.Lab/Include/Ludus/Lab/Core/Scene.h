#pragma once

#include <Ludus/Core/ISystem.h>
#include <Ludus/Core/State.h>
#include <Ludus/Core/SystemPredicate.h>
#include <Ludus/Engine/Cooldown.h>
#include <Ludus/Engine/Random.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/Shape.h>

namespace Ludus::Lab::Core
{
	struct AABBDemo2Info
	{
		const Ludus::Graphics::Color CollisionColor = Ludus::Graphics::Colors::Orange.WithAlpha(0.5f);
		const Ludus::Graphics::Color NonCollisionColor = Ludus::Graphics::Colors::LightBlue.WithAlpha(0.5f);
		Ludus::Graphics::Color CursorColor = Ludus::Graphics::Colors::Gray;
		const int CursorLayerIndex = 1;
		const int QuadLayerIndex = 2;
		const int MaxCount = 120;
		const std::string CursorLayerName = "Cursor";
		const std::string QuadLayerName = "Quad";
		Ludus::Engine::EntityHandle CursorHandle = 0;
		bool IsColliding = false;
	};

	struct FallingQuad
	{
		Ludus::Engine::EntityHandle Handle;
		float Speed;

		bool operator==(const FallingQuad& other) const { return Handle == other.Handle; }
	};

	class Scene : public Ludus::Core::ISystem
	{
		AABBDemo2Info m_Info;
		Ludus::Engine::Random m_Random;
		Ludus::Engine::Cooldown m_Cooldown;
		std::vector<FallingQuad> m_FallingQuads;

		FallingQuad CreateQuad();

	public:
		Scene();

		virtual void OnAttachImpl() override;
		virtual void UpdateImpl(float deltaTime) override;
	};
}
