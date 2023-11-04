#ifndef HG_CHONPS_FONT_H
#define HG_CHONPS_FONT_H

#include <map>
#include "Texture.h"
#include "Core/DataTypes.h"
#include "Core/QueueChain.h"

namespace Chonps
{
	struct FontMetrics
	{
		float emSize;
		float ascender, descender;
		float lineHeight;
		float underline, underlineThickness;
	};

	struct FontGlyph
	{
		typedef struct GlyphUV
		{
			float x0, y0, x1, y1;
		} GlyphUV;

		typedef struct GlyphVec2
		{
			float x, y;
		} GlyphVec2;

		uint32_t unicode;
		GlyphUV uv;
		GlyphVec2 size;
		GlyphVec2 bearing;
		int advance;
	};

	struct Charset
	{
		uint32_t first, count;
		static Charset ASCII;
	};

	struct Font
	{
		typedef struct FontTextureData
		{
			unsigned char* pixels;
			uint32_t width, height;
		} FontTextureData;

		FontTextureData atlas;
		uint32_t fontSize;
		std::map<uint32_t, FontGlyph> glyphs;
		Charset codepoints;
	};

	bool loadFontfromTTF(const char* filename, uint32_t fontSize, Font* font, Charset charset = Chonps::Charset::ASCII);

	class sdfFont
	{
	public:
		sdfFont() = default;
		// Creates a 4-channel MTSDF atlas map containing all ASCII characters
		sdfFont(const char* fontFilename, uint32_t channels, double setMinimumScale, double setPixelRange, double setMiterLimit = 1.0, double maxCornerAngle = 3.0);

		void Delete();
		Texture* getFontAtlas() { return m_FontAtlas; }
		std::map<uint32_t, FontGlyph> getGlyphs() { return m_Glyphs; }
		float getHighestGlyphLength() { return m_MaxHeight; }

		uint32_t id() { return m_ID; }

	private:
		void CreateFontAtlasMTSDF(const char* fontFilename, double setMinimumScale, double setPixelRange, double setMiterLimit, double maxCornerAngle);
		void CreateFontAtlasSDF(const char* fontFilename, double setMinimumScale, double setPixelRange, double setMiterLimit, double maxCornerAngle);

		Texture* m_FontAtlas = nullptr;
		FontMetrics m_Metrics;
		std::map<uint32_t, FontGlyph> m_Glyphs;
		uint32_t m_MaxHeight;

		float m_ID;
	};
}

#endif