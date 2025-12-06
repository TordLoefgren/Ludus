#pragma once

#include <Ludus/Editor/Core/InspectorHelpers.h>
#include <Ludus/Editor/Core/PanelSystem.h>
#include <Ludus/Editor/Panels/Constants.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Graphics/Sprite2D.h>
#include <Ludus/Engine/Graphics/Text2D.h>
#include <Ludus/Engine/Math/Transform2D.h>
#include <Ludus/Engine/Physics/Core/Collider2D.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>
#include <Ludus/Engine/Physics/Core/RigidBody2D.h>
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
