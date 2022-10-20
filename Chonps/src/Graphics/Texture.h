#ifndef TEXTURE_H
#define TEXTURE_H

#include "Shader.h"

namespace Chonps
{
	enum class TexType
	{
		NA = 0, 
		Diffuse, Specular, Metallic, Roughness, Emissive, Occlusion
	};

	enum class TexFilter
	{
		Linear, Nearest
	};

	enum class TexWrap
	{
		Repeat, MirroredRepeat, ClampEdge
	};

	typedef TexType TexT;
	typedef TexFilter TexF;
	typedef TexWrap TexW;

	class Texture
	{
	public:
		Texture(const std::string& filepath, TexF texFilter = TexF::Linear, TexW texWrap = TexW::Repeat) {}

		virtual void TexUnit(Shader* shader, const char* uniform, uint32_t unit) = 0;
		virtual void SetTexType(TexT textureType) = 0;

		virtual void Bind(uint32_t unit) const = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;

		virtual uint32_t GetID() const = 0;
		virtual uint32_t GetUnit() const = 0;
		virtual TexType GetTexType() const = 0;

	};

	Texture* createTexture(const std::string& filepath, TexF texFilter = TexF::Linear, TexW texWrap = TexW::Repeat);
}

#endif