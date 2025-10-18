#pragma once

#include <Ludus/Engine/GameObject.h>
#include <Ludus/Engine/LayerMask.h>
#include <Ludus/Engine/Scene.h>
#include <Ludus/Engine/Utilities.h>
#include <Ludus/Graphics/Camera2D.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/GLContext.h>
#include <Ludus/Graphics/Renderer2D.h>
#include <Ludus/Graphics/Shader.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Physics/AABBOverlapInfo.h>
#include <Ludus/Physics/CollisionSystem2D.h>
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/Window.h>
#include <Ludus/Platform/WindowOptions.h>

#include <IDemo.h>

namespace Ludus::Lab
{
	struct AABBDemo1Info
	{
		AABBOverlapInfo OverlapInfo { };
		const Ludus::Graphics::Color CollisionColor = Ludus::Graphics::Colors::Red;
		const Ludus::Graphics::Color NonCollisionColor = Ludus::Graphics::Colors::Green;
		const int CursorLayerIndex = 1;
		const int BoxLayerIndex = 1;
		const std::string CursorLayerName = "Cursor";
		const std::string BoxLayerName = "Box";
		Ludus::Engine::GameObjectHandle CursorHandle = 0;
		Ludus::Engine::GameObjectHandle BoxHandle = 0;
		bool IsColliding = false;
		bool IsCircle = false;
	};

	class AABBDemo1 : IDemo
	{
	private:

		Ludus::Platform::Window& m_Window;
		int m_Width, m_Height;
		Ludus::Graphics::Shader m_Shader;
		Ludus::Graphics::Renderer2D m_Renderer;
		Ludus::Graphics::Camera2D m_Camera;
		AABBDemo1Info m_Info;
		Ludus::Engine::Scene m_Scene;
		Ludus::Physics::CollisionSystem2D m_CollisionSystem;

		void DrawCursor(Ludus::Graphics::Renderer2D& renderer, float x, float y, Ludus::Graphics::Color color);
		void DrawCursors(Ludus::Graphics::Renderer2D& renderer, const Ludus::Math::Transform2D& transform, Ludus::Graphics::Color color);

	public:
		AABBDemo1(Ludus::Platform::Window& window, int width, int height);
		virtual ~AABBDemo1() = default;

		virtual void Init();
		virtual void Update();
	};
}
