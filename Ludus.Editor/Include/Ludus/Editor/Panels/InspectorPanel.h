#pragma once

#include <Ludus/Editor/Core/InspectorHelpers.h>
#include <Ludus/Editor/Core/PanelSystem.h>
#include <Ludus/Editor/Panels/Constants.h>
#include <Ludus/Engine/Entity.h>
#include <Ludus/Engine/LayerMask.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Graphics/Shape.h>
#include <Ludus/Graphics/Sprite2D.h>
#include <Ludus/Graphics/Text2D.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Physics/Core/Collider2D.h>
#include <Ludus/Physics/Core/RigidBody2D.h>
#include <Ludus/UI/Containers.h>
#include <Ludus/UI/Widgets.h>

namespace Ludus::Editor::Panels
{
	class InspectorPanel final : public Ludus::Editor::Core::PanelSystem
	{
	public:
		virtual void DrawPanel() override;
	};
}
