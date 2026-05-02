#include "pch.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <optional>
#include <vector>

#include <stb_image/stb_image.h>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Graphics/GL.h>
#include <Ludus/Engine/Graphics/Texture.h>

namespace Ludus::Engine::Graphics
{
	namespace
	{
		unsigned char ToByte(float value)
		{
			value = std::clamp(value, 0.0f, 1.0f);
			return static_cast<unsigned char>(std::round(value * 255));
		}
	}

	Texture::Texture(
		unsigned int handle,
		int width,
		int height
	) :
		m_Handle(handle),
		m_Width(width),
		m_Height(height)
	{}

	Texture::~Texture()
	{
		if (m_Handle)
		{
			glDeleteTextures(1, &m_Handle);
		}
	}

	Texture::Texture(Texture&& other) noexcept
		: m_Handle(other.m_Handle), m_Width(other.m_Width), m_Height(other.m_Height)
	{
		other.m_Handle = 0; other.m_Width = other.m_Height = 0;
	}

	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			if (m_Handle)
			{
				glDeleteTextures(1, &m_Handle);
			}
			m_Handle = other.m_Handle;
			m_Width = other.m_Width;
			m_Height = other.m_Height;

			other.m_Handle = 0;
			other.m_Width = other.m_Height = 0;
		}

		return *this;
	}

	Texture Texture::Empty(int width, int height)
	{
		return Texture(0, width, height);
	}

	Texture Texture::FramebufferTexture(int width, int height)
	{
		unsigned int handle;
		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		return Texture(handle, width, height);
	}

	Texture Texture::White()
	{
		return FromColor(1, 1, Colors::White);
	}

	Texture Texture::Missing()
	{
		return FromColor(2, 2, { Colors::Black, Colors::Magenta, Colors::Magenta, Colors::Black });
	}

	Texture Texture::FromR8(int width, int height, const void* data)
	{
		LUDUS_ASSERT(width > 0 && height > 0, "Texture dimensions must be positive.");
		LUDUS_ASSERT(data != nullptr, "Cannot create R8 texture from null data.");

		unsigned int handle;
		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);

		glBindTexture(GL_TEXTURE_2D, 0);

		return Texture(handle, width, height);
	}

	Texture Texture::FromRGBA(int width, int height, const void* data)
	{
		LUDUS_ASSERT(width > 0 && height > 0, "Texture dimensions must be positive.");
		LUDUS_ASSERT(data != nullptr, "Cannot create RGBA texture from null data.");

		unsigned int handle;

		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D, 0);

		return Texture(handle, width, height);
	}

	Texture Texture::FromColor(int width, int height, const Color& color)
	{
		LUDUS_ASSERT(width > 0 && height > 0, "Texture dimensions must be positive.");

		const auto pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);

		std::vector<unsigned char> pixels;
		pixels.resize(pixelCount * 4);

		const auto r = ToByte(color.R);
		const auto g = ToByte(color.G);
		const auto b = ToByte(color.B);
		const auto a = ToByte(color.A);

		for (size_t i = 0; i < pixelCount; i++)
		{
			pixels[i * 4 + 0] = r;
			pixels[i * 4 + 1] = g;
			pixels[i * 4 + 2] = b;
			pixels[i * 4 + 3] = a;
		}

		return FromRGBA(width, height, pixels.data());
	}

	Texture Texture::FromColor(int width, int height, std::initializer_list<Color> colors)
	{
		LUDUS_ASSERT(width > 0 && height > 0, "Texture dimensions must be positive.");

		const auto pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
		LUDUS_ASSERT(colors.size() == pixelCount, "The texture dimensions do not match the size of the color list.");

		std::vector<unsigned char> bytes;
		bytes.reserve(pixelCount * 4);

		for (const auto& color : colors)
		{
			bytes.push_back(ToByte(color.R));
			bytes.push_back(ToByte(color.G));
			bytes.push_back(ToByte(color.B));
			bytes.push_back(ToByte(color.A));
		}

		return FromRGBA(width, height, bytes.data());
	}

	std::optional<Texture> Texture::TryFromFile(const std::filesystem::path& path)
	{
		int width, height, _;
		auto data = stbi_load(path.string().c_str(), &width, &height, &_, 4);
		if (!data)
		{
			return std::nullopt;
		}

		auto texture = FromRGBA(width, height, data);

		stbi_image_free(data);

		return texture;
	}

	void Texture::Bind(unsigned int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_Handle);
	}

	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int Texture::Handle() const
	{
		return m_Handle;
	}

	int Texture::Width() const
	{
		return m_Width;
	}

	int Texture::Height() const
	{
		return m_Height;
	}
}
