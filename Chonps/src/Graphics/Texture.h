#ifndef CHONPS_TEXTURE_H
#define CHONPS_TEXTURE_H

#include "Shader.h"

namespace Chonps
{
	enum class TexType
	{
		NA = 0, 
		Diffuse, Specular, Metallic, Roughness, Normal, Emissive, Occlusion,
		MetallicRoughness
	};

	enum class TexFilter
	{
		Linear, Nearest,
		Nearest_Mipmap_Nearest, Nearest_Mipmap_Linear, 
		Linear_Mipmap_Linear, Linear_Mipmap_Nearest
	};

	enum class TexWrap
	{
		Default = 0, Repeat, MirroredRepeat, ClampEdge
	};

	typedef TexType TexT;
	typedef TexFilter TexF;
	typedef TexWrap TexW;

	struct TexFilterPair
	{
		TexFilter min;
		TexFilter mag;
	};

	typedef TexFilterPair TexFP;

	class Texture
	{
	public:
		Texture(const std::string& filepath, TexType texType = TexType::Diffuse, TexFilterPair texFilter = { TexFilter::Linear, TexFilter::Nearest }, TexWrap texWrap = TexWrap::Repeat) {}
		Texture(uint32_t width, uint32_t height, void* data, uint32_t size) {}

		virtual void TexUnit(Shader* shader, const char* uniform, uint32_t unit) = 0;

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

	Texture* createTexture(const std::string& filepath, TexType texType = TexType::Diffuse, TexFilterPair texFilter = { TexFilter::Linear, TexFilter::Nearest }, TexWrap texWrap = TexWrap::Repeat);
	Texture* createTexture(uint32_t width, uint32_t height, void* data, uint32_t size);
}

#endif