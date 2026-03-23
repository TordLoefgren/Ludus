#pragma once

#include <string>
#include <utility>

#include <Ludus/Engine/Graphics/FramebufferObject.h>
#include <Ludus/Engine/Graphics/Texture.h>

namespace Ludus::Engine::Graphics
{
	struct RenderTarget
	{
		std::string TargetId;
		Texture ColorTexture;
		FramebufferObject Framebuffer;

		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;

	public:
		RenderTarget(RenderTarget&&) noexcept = default;
		RenderTarget& operator=(RenderTarget&&) noexcept = default;

	private:
		RenderTarget(std::string id, int width, int height)
			: TargetId(id), ColorTexture(Texture::FramebufferTexture(width, height)), Framebuffer(ColorTexture.Handle(), width, height)
		{ }

		RenderTarget(int width, int height)
			: TargetId("Default"), ColorTexture(Texture::Empty(width, height)), Framebuffer(FramebufferObject::Default(width, height))
		{ }

	public:
		static RenderTarget Create(std::string id, int width, int height)
		{
			return RenderTarget(std::move(id), width, height);
		}

		static RenderTarget CreateDefault(int width, int height)
		{
			return RenderTarget(width, height);
		}
	};
}
