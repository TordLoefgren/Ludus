#pragma once

#include <Ludus/Engine/Utilities.h>
#include <Ludus/Graphics/Vertex.h>

namespace Ludus::Graphics
{
	class FramebufferObject
	{
	private:
		unsigned int m_Handle = 0;
		unsigned int m_RenderBufferHandle = 0;
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
	};
}
