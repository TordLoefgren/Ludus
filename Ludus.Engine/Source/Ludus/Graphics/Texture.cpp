#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include <Ludus/Engine/Utilities.h>
#include <Ludus/Graphics/Texture.h>

namespace Ludus::Graphics
{
	Texture::Texture(unsigned int handle, int width, int height)
		: m_Handle(handle), m_Width(width), m_Height(height)
	{ }

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_Handle);
	}

	Texture::Texture(Texture&& o) noexcept
		: m_Handle(o.m_Handle), m_Width(o.m_Width), m_Height(o.m_Height)
	{
		o.m_Handle = 0; o.m_Width = o.m_Height = 0;
	}

	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			if (m_Handle) glDeleteTextures(1, &m_Handle);
			m_Handle = other.m_Handle;
			m_Width = other.m_Width;
			m_Height = other.m_Height;

			other.m_Handle = 0;
			other.m_Width = other.m_Height = 0;
		}

		return *this;
	}

	Texture Texture::FromFile(const std::string& path)
	{
		unsigned int handle;
		int width, height, _;

		stbi_set_flip_vertically_on_load(1);
		auto m_Data = stbi_load(path.c_str(), &width, &height, &_, 4);
		if (!m_Data)
		{
			Ludus::Engine::Utilities::WriteLine("Failed to load texture: " + std::string(path));
			return Texture::White();
		}

		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);

		// The following parameters are not optional.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Data);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(m_Data);
		m_Data = nullptr;

		Texture texture(handle, width, height);
		return texture;
	}

	Texture Texture::FromMemory(int width, int height, const void* data)
	{
		unsigned int handle;
		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_R8,
			width,
			height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			data
		);

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

	Texture Texture::White()
	{
		unsigned int handle;

		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		unsigned char whitePixel[4] = { 255, 255, 255, 255 };
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

		glBindTexture(GL_TEXTURE_2D, 0);

		Texture texture(handle, 1, 1);
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
}
