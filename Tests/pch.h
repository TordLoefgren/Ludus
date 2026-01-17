//
// pch.h
//

#pragma once

#include "gtest/gtest.h"

#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/EventBus.h>
#include <Ludus/Engine/Events/EventHandler.h>
#include <Ludus/Engine/Math/AABB.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Persistance/Archives/DomArchiveDiagnostics.h>
#include <Ludus/Engine/Persistance/Archives/DomLoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/DomNode.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>
#include <Ludus/Engine/Persistance/Serializers/SceneSerializer.h>
#include <Ludus/Engine/Persistance/Serializers/Sprite2DComponentSerializer.h>
#include <Ludus/Engine/Persistance/Serializers/Transform2DComponentSerializer.h>
