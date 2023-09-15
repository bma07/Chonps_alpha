#ifndef HG_CHONPS_TEXTURE_H
#define HG_CHONPS_TEXTURE_H

#include "Shader.h"
#include <map>

namespace Chonps
{
	enum class TexFormat
	{
		None = 0,
		Auto = None,
		RGB8, RGBA8, 
		RGB16, RGBA16,
		RGB32F, RGBA32F, RGB32I, RGBA32I, RGB32UI, RGBA32UI,
		RGB64F, RGBA64F, RGB64I, RGBA64I, RGB64UI, RGBA64UI
	};

	enum class TexType
	{
		None = 0, 
		Albedo, Diffuse, Specular, Metallic, Roughness, Normal, Emissive, Occlusion,
		Displacement, Bump,
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
		Default = 0, Repeat, MirroredRepeat, ClampToEdge
	};

	struct TexFilterPair
	{
		TexFilter min;
		TexFilter mag;
	};

	class Texture
	{
	public:
		Texture(const std::string& filepath,
			TexType texType, 
			TexFilterPair texFilter = { TexFilter::Linear, TexFilter::Nearest }, 
			TexWrap texWrap = TexWrap::Repeat) {}

		Texture(uint32_t width, uint32_t height, const void* data,
			TexType texType = TexType::Diffuse, 
			TexFilterPair texFilter = { TexFilter::Nearest, TexFilter::Nearest }, 
			TexWrap texWrap = TexWrap::Repeat) {}

		virtual void Delete() = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;

		virtual uint32_t id() = 0;
		virtual TexType GetTexType() const = 0;
	};

	struct TextureCreateInfo
	{
		Texture* texture;
		uint32_t slot;
	};

	class TextureLayout
	{
	public:
		TextureLayout() = default;
		TextureLayout(TextureCreateInfo* pTextures, uint32_t textureCount, uint32_t setIndex) {};

		virtual void insert(Texture* texture, uint32_t slot) = 0;
		virtual void erase(uint32_t slot) = 0;

		virtual void Bind(Shader* shader) const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

		virtual std::map<uint32_t, Texture*> GetTextures() = 0;
	};

	// Note: Textures format may vary between Render APIs due to different support of formats. The TexFormat::Auto
	// indicator is recommended so that the format can be automatically chosen based on the texture
	Texture* createTexture(const std::string& filepath,
		TexType texType, 
		TexFilterPair texFilter = { TexFilter::Linear, TexFilter::Nearest }, 
		TexWrap texWrap = TexWrap::Repeat);
	
	Texture* createTexture(uint32_t width, uint32_t height, const void* data,
		TexType texType = TexType::Diffuse,
		TexFilterPair texFilter = { TexFilter::Nearest, TexFilter::Nearest },
		TexWrap texWrap = TexWrap::Repeat);

	std::shared_ptr<Texture> createTextureSp(const std::string& filepath,
		TexType texType,
		TexFilterPair texFilter = { TexFilter::Linear, TexFilter::Nearest },
		TexWrap texWrap = TexWrap::Repeat);

	std::shared_ptr<Texture> createTextureSp(uint32_t width, uint32_t height, const void* data,
		TexType texType = TexType::Diffuse, 
		TexFilterPair texFilter = { TexFilter::Nearest, TexFilter::Nearest }, 
		TexWrap texWrap = TexWrap::Repeat);

	TextureLayout* createTextureLayout(TextureCreateInfo* pTextures, uint32_t textureCount, uint32_t setIndex);
	std::shared_ptr<TextureLayout> createTextureLayoutSp(TextureCreateInfo* pTextures, uint32_t textureCount, uint32_t setIndex);
}

#endif