#pragma once

#include <filesystem>
#include <initializer_list>
#include <optional>

#include <Ludus/Engine/Graphics/Color.h>

namespace Ludus::Engine::Graphics
{
	class Texture
	{
	private:
		unsigned int m_Handle;
		int m_Width;
		int m_Height;

	public:
		Texture(unsigned int handle, int width, int height);
		~Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(Texture&&) noexcept;
		Texture& operator=(Texture&&) noexcept;

		static Texture Empty(int width, int height);
		static Texture FramebufferTexture(int width, int height);

		static Texture White();
		static Texture Missing();

		static Texture FromR8(int width, int height, const void* data);
		static Texture FromRGBA(int width, int height, const void* data);
		static Texture FromColor(int width, int height, const Color& color = Colors::White);
		static Texture FromColor(int width, int height, std::initializer_list<Color> colors);

		static std::optional<Texture> TryFromFile(const std::filesystem::path& path);

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;

		unsigned int Handle() const;
		int Width() const;
		int Height() const;
	};
}
