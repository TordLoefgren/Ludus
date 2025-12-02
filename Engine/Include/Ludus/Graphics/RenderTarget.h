#pragma once

#include <string>

#include <Ludus/Graphics/FramebufferObject.h>
#include <Ludus/Graphics/Texture.h>

namespace Ludus::Graphics
{
	struct RenderTarget
	{
		std::string TargetId;
		Texture ColorTexture;
		FramebufferObject Framebuffer;

		explicit RenderTarget(std::string id, int width, int height)
			: TargetId(id), ColorTexture(Texture::FramebufferTexture(width, height)), Framebuffer(ColorTexture.Handle(), width, height)
		{ }

		explicit RenderTarget(int width, int height)
			: TargetId("Default"), ColorTexture(Texture::Empty(width, height)), Framebuffer(FramebufferObject::Default(width, height))
		{ }
	};
}
