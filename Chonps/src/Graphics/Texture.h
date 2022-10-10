#ifndef TEXTURE_H
#define TEXTURE_H

#include "Shader.h"

namespace Chonps
{
	enum class TexType
	{
		BaseColor, Metallic, Roughness, Specular, Emissive, Occlusion
	};

	class Texture
	{
	public:
		Texture(const std::string& filepath) {}

		virtual void TexUnit(Shader* shader, const char* uniform, uint32_t unit) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

		virtual uint32_t GetID() const = 0;
		virtual uint32_t GetUnit() const = 0;
		virtual TexType GetTexType() const = 0;
	};

	typedef Texture Texture2D;

	Texture* createTexture(const std::string& filepath);
}

#endif