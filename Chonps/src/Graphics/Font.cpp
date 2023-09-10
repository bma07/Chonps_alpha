#include "cepch.h"
#include "Font.h"

#include <msdfgen.h>
#include <msdfgen-ext.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include "RendererBackends.h"

namespace Chonps
{
	void Font::Delete()
	{
		RendererBackends* backends = getRendererBackends();
		backends->fontIDsQueue.push(m_ID);
		m_FontAtlas->Delete();
	}

	Font::Font(const char* fontFilename, double setMinimumScale, double setPixelRange, double setMiterLimit, double maxCornerAngle)
	{
		RendererBackends* backends = getRendererBackends();
		m_ID = backends->fontIDsQueue.take_next();

		if (msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype())
		{
			if (msdfgen::FontHandle* font = msdfgen::loadFont(ft, fontFilename))
			{
				std::vector<msdf_atlas::GlyphGeometry> glyphs;
				msdf_atlas::FontGeometry fontGeometry(&glyphs);

				fontGeometry.loadCharset(font, 1.0, msdf_atlas::Charset::ASCII);

				for (msdf_atlas::GlyphGeometry& glyph : glyphs)
					glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);

				msdf_atlas::TightAtlasPacker packer;
				packer.setDimensionsConstraint(msdf_atlas::TightAtlasPacker::DimensionsConstraint::POWER_OF_TWO_SQUARE);
				packer.setMinimumScale(setMinimumScale);
				packer.setPixelRange(setPixelRange);
				packer.setMiterLimit(setMiterLimit);
				packer.pack(glyphs.data(), static_cast<int>(glyphs.size()));

				int width = 0, height = 0;
				packer.getDimensions(width, height);
				msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>> generator(width, height);

				msdf_atlas::GeneratorAttributes attributes;
				generator.setAttributes(attributes);
				generator.setThreadCount(4);

				// Generate atlas bitmap
				// The atlas bitmap can now be retrieved via atlasStorage as a BitmapConstRef.
				// The glyphs array (or fontGeometry) contains positioning data for typesetting text.
				generator.generate(glyphs.data(), static_cast<int>(glyphs.size()));

				const uint8_t* pixels = generator.atlasStorage().operator msdfgen::BitmapConstRef<msdf_atlas::byte, 3>().pixels;

				for (auto& glyph : glyphs)
				{
					FontGlyph fontGlyph;
					fontGlyph.unicode = glyph.getCodepoint();

					int x, y, w, h;
					glyph.getBoxRect(x, y, w, h);

					fontGlyph.bl = { (float)x / (float)width, (float)y / (float)height };
					fontGlyph.br = { ((float)x + (float)w) / (float)width, (float)y / (float)height };
					fontGlyph.tl = { (float)x / width, ((float)y + (float)h) / (float)height };
					fontGlyph.tr = { ((float)x + (float)w) / (float)width, ((float)y + (float)h) / (float)height};

					msdfgen::FontMetrics metrics = fontGeometry.getMetrics();
					fontGlyph.metrics.size = metrics.emSize;
					fontGlyph.metrics.ascender = metrics.ascenderY;
					fontGlyph.metrics.descender = metrics.descenderY;
					fontGlyph.metrics.lineHeight = metrics.lineHeight;
					fontGlyph.metrics.underline = metrics.underlineY;
					fontGlyph.metrics.underlineThickness = metrics.underlineThickness;
					fontGlyph.metrics.advance = glyph.getAdvance();

					m_Glyphs[fontGlyph.unicode] = fontGlyph;
				}

				m_FontAtlas = createTexture(width, height, pixels, TexType::Diffuse, { TexFilter::Linear, TexFilter::Linear }, TexWrap::Repeat);

				// Cleanup
				msdfgen::destroyFont(font);
			}
			msdfgen::deinitializeFreetype(ft);
		}
	}
}