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
		double size;
		double ascender, descender;
		double lineHeight;
		double underline, underlineThickness;
		double advance;
	};

	struct FontGlyph
	{
		uint32_t unicode;
		glm::vec2 bl, br, tl, tr;
		FontMetrics metrics;
	};

	class Font
	{
	public:
		Font() = default;
		Font(const char* fontFilename, double setMinimumScale = 24.0, double setPixelRange = 2.0, double setMiterLimit = 1.0, double maxCornerAngle = 3.0);

		void Delete();
		Texture* getFontAtlas() { return m_FontAtlas; }

		uint32_t id() { return m_ID; }

	private:
		Texture* m_FontAtlas = nullptr;
		std::map<uint32_t, FontGlyph> m_Glyphs;

		uint32_t m_ID;
	};
}

#endif