#include "pch.h"

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Graphics/FramebufferObject.h>
#include <Ludus/Engine/Graphics/GL.h>

namespace Ludus::Engine::Graphics
{
	FramebufferObject::FramebufferObject(unsigned int textureHandle, int width, int height)
		: m_Handle(0), m_ColorTextureHandle(textureHandle), m_Width(width), m_Height(height)
	{
		glGenFramebuffers(1, &m_Handle);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTextureHandle, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			LUDUS_LOG_ERROR("Frame buffer is not complete.");
		}
		else
		{
			LUDUS_LOG_INFO("Frame buffer is complete.");
		}

		Unbind();
	}

	FramebufferObject::~FramebufferObject()
	{
		if (m_Handle)
		{
			glDeleteFramebuffers(1, &m_Handle);
		}
	}

	FramebufferObject::FramebufferObject(FramebufferObject&& other) noexcept
		: m_Handle(other.m_Handle),
		m_ColorTextureHandle(other.m_ColorTextureHandle),
		m_Width(other.m_Width),
		m_Height(other.m_Height),
		m_IsDefault(other.m_IsDefault)
	{
		other.m_Handle = 0;
		other.m_ColorTextureHandle = 0;
		other.m_Width = 0;
		other.m_Height = 0;
		other.m_IsDefault = false;
	}

	FramebufferObject& FramebufferObject::operator=(FramebufferObject&& other) noexcept
	{
		if (this != &other)
		{
			if (m_Handle)
			{
				glDeleteFramebuffers(1, &m_Handle);
			}

			m_Handle = other.m_Handle;
			m_ColorTextureHandle = other.m_ColorTextureHandle;
			m_Width = other.m_Width;
			m_Height = other.m_Height;
			m_IsDefault = other.m_IsDefault;

			other.m_Handle = 0;
			other.m_ColorTextureHandle = 0;
			other.m_Width = 0;
			other.m_Height = 0;
			other.m_IsDefault = false;
		}

		return *this;
	}

	FramebufferObject FramebufferObject::Default(int width, int height)
	{
		FramebufferObject framebuffer;
		framebuffer.m_Handle = 0;
		framebuffer.m_Width = width;
		framebuffer.m_Height = height;
		framebuffer.m_IsDefault = true;
		return framebuffer;
	}

	void FramebufferObject::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
		glViewport(0, 0, m_Width, m_Height);
	}

	void FramebufferObject::Unbind() const
	{
		if (!m_IsDefault && m_Handle != 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	void FramebufferObject::Resize(int width, int height)
	{
		if (m_Width == width && m_Height == height)
		{
			return;
		}

		if (m_IsDefault)
		{
			m_Width = width;
			m_Height = height;
			return;
		}

		m_Width = width;
		m_Height = height;

		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);

		glBindTexture(GL_TEXTURE_2D, m_ColorTextureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTextureHandle, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
