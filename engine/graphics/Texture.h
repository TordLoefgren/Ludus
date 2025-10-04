#pragma once

#include <filesystem>

#include "../core/utils.h"

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

	static Texture FromFile(const std::string& path);
	static Texture FromMemory(int width, int height, const void* data);
	static Texture White();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	unsigned int Handle() const;
};

