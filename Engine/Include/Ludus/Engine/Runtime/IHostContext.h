#pragma once

#include <string_view>

#include <Ludus/Engine/Core/FlagSet.h>
#include <Ludus/Engine/Math/Size.h>

#pragma region Forward Declarations

struct GLFWwindow;

namespace Ludus::Engine::Graphics
{
	struct RenderTarget;
}

namespace Ludus::Engine::Runtime
{
	class RuntimeInstance;
}

namespace Ludus::Engine::Windowing
{
	class Input;
}

#pragma endregion

namespace Ludus::Engine::Runtime
{
	class IHostContext
	{
	public:
		virtual ~IHostContext() = default;

		virtual void AttachRuntime(RuntimeInstance* runtime) = 0;
		virtual void DetachRuntime() = 0;

		virtual Ludus::Engine::Math::Size<int> GetFramebufferSize() const = 0;
		virtual Ludus::Engine::Math::Size<int> GetWindowSize() const = 0;
		virtual GLFWwindow* GetWindowHandle() const = 0;
		virtual Ludus::Engine::Graphics::RenderTarget& GetMainRenderTarget() = 0;
		virtual Ludus::Engine::Core::FlagSet& GetExecutionFlags() = 0;
		virtual Ludus::Engine::Windowing::Input& GetInput() = 0;

		virtual void SetWindowTitle(std::string_view title) = 0;
	};
}
