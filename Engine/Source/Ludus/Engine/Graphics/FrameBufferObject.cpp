#include "pch.h"

#include <glad/glad.h>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Graphics/FramebufferObject.h>

namespace Ludus::Engine::Graphics
{
	FramebufferObject::FramebufferObject(unsigned int textureHandle, int width, int height)
		: m_Handle(0), m_ColorTextureHandle(textureHandle), m_Width(width), m_Height(height)
	{
		glGenFramebuffers(1, &m_Handle);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTextureHandle, 0);

		glGenRenderbuffers(1, &m_RenderBufferHandle);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferHandle);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferHandle);

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
		if (m_RenderBufferHandle)
		{
			glDeleteRenderbuffers(1, &m_RenderBufferHandle);
		}

		if (m_Handle)
		{
			glDeleteFramebuffers(1, &m_Handle);
		}
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

		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferHandle);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferHandle);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
