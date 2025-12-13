#pragma once

#include <Ludus/Engine/Core/Utilities.h>
#include <Ludus/Engine/Graphics/Vertex.h>
#include <Ludus/Engine/Math/Size.h>

namespace Ludus::Engine::Graphics
{
	class FramebufferObject
	{
	private:
		unsigned int m_Handle = 0;
		unsigned int m_ColorTextureHandle = 0;
		int m_Width = 0;
		int m_Height = 0;
		bool m_IsDefault = false;

	public:
		FramebufferObject() = default;
		FramebufferObject(unsigned int textureHandle, int width, int height);
		~FramebufferObject();

		static FramebufferObject Default(int width, int height);

		void Bind() const;
		void Unbind() const;

		void Resize(int width, int height);

		int GetWidth() { return m_Width; }
		int GetHeight() { return m_Height; }

		Ludus::Engine::Math::Size<int> GetSize() { return { m_Width, m_Height }; }
	};
}
