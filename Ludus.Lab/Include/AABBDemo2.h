#pragma once

#include <vector>

#include <Ludus/Engine/Cooldown.h>
#include <Ludus/Engine/GameObject.h>
#include <Ludus/Engine/LayerMask.h>
#include <Ludus/Engine/Random.h>
#include <Ludus/Engine/Scene.h>
#include <Ludus/Engine/TimeStep.h>
#include <Ludus/Engine/Utilities.h>
#include <Ludus/Graphics/Camera2D.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/GLContext.h>
#include <Ludus/Graphics/Renderer2D.h>
#include <Ludus/Graphics/Shader.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/AABBOverlapInfo.h>
#include <Ludus/Physics/CollisionSystem2D.h>
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/Window.h>
#include <Ludus/Platform/WindowOptions.h>

#include <IDemo.h>

namespace Ludus::Lab
{
	struct AABBDemo2Info
	{
		AABBOverlapInfo OverlapInfo { };
		const Ludus::Graphics::Color CollisionColor = Ludus::Graphics::Colors::Orange.WithAlpha(0.5f);
		const Ludus::Graphics::Color NonCollisionColor = Ludus::Graphics::Colors::LightBlue.WithAlpha(0.5f);
		Ludus::Graphics::Color CursorColor = Ludus::Graphics::Colors::Gray;
		const int CursorLayerIndex = 1;
		const int QuadLayerIndex = 2;
		const int MaxCount = 120;
		const std::string CursorLayerName = "Cursor";
		const std::string QuadLayerName = "Quad";
		Ludus::Engine::GameObjectHandle CursorHandle = 0;
		bool IsColliding = false;
		bool IsCircle = false;
	};

	struct FallingQuad
	{
		Ludus::Engine::GameObjectHandle Handle;
		float Speed;
		Ludus::Graphics::Color Color;

		bool operator==(const FallingQuad& other) const { return Handle == other.Handle; }
	};

	/// <summary>
	/// Interactive demo showcasing AABB collisions. Original concept from: https://www.jeffreythompson.org/collision-detection/.
	/// </summary>
	class AABBDemo2 : IDemo
	{
	private:

		Ludus::Platform::Window& m_Window;
		int m_Width, m_Height;
		Ludus::Graphics::Shader m_Shader;
		Ludus::Graphics::Renderer2D m_Renderer;
		Ludus::Graphics::Camera2D m_Camera;
		AABBDemo2Info m_Info;
		Ludus::Engine::Scene m_Scene;
		Ludus::Engine::Random m_Random;
		Ludus::Engine::TimeStep m_TimeStep;
		Ludus::Engine::Cooldown m_Cooldown;
		Ludus::Physics::CollisionSystem2D m_CollisionSystem;
		std::vector<FallingQuad> m_FallingQuads;

	public:
		AABBDemo2(Ludus::Platform::Window& window, int width, int height);
		virtual ~AABBDemo2() = default;

		virtual void Init();
		virtual void Update();

		FallingQuad CreateQuad();
	};
}
