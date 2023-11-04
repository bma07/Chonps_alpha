#include "cepch.h"
#include "Font.h"

#include <msdfgen.h>
#include <msdfgen-ext.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include <stb_truetype.h>
#include <stb_image_write.h>
#include <freetype/freetype.h>

#include "RendererBackends.h"
#include "Core/Math.h"

namespace Chonps
{
	Charset Charset::ASCII = { 32, 95 };

	bool loadFontfromTTF(const char* filename, uint32_t fontSize, Font* font, Charset charset)
	{
		FT_Library ft;
		FT_Face face;

		if (FT_Init_FreeType(&ft))
		{
			CHONPS_CORE_LOG_ERROR(Font:ttf, "Failed to load FreeType library!");
			return false;
		}

		if (FT_New_Face(ft, filename, 0, &face))
		{
			CHONPS_CORE_LOG_ERROR(Font:ttf, "Failed to load font face!");
			return false;
		}

		FT_Set_Char_Size(face, 0, fontSize << 6, 96, 96);

		int max_dim = (1 + (face->size->metrics.height >> 6)) * ceilf(sqrtf(charset.count));
		int tex_width = 1;
		while (tex_width < max_dim) tex_width <<= 1;
		int tex_height = tex_width;

		// render glyphs to atlas
		unsigned char* pixels = (unsigned char*)calloc(tex_width * tex_height, 1);
		int pen_x = 0, pen_y = 0;

		for (uint32_t i = charset.first; i < charset.first + charset.count; i++)
		{
			FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
			FT_Bitmap* bmp = &face->glyph->bitmap;

			if (pen_x + bmp->width >= tex_width)
			{
				pen_x = 0;
				pen_y += ((face->size->metrics.height >> 6) + 1);
			}

			for (unsigned int row = 0; row < bmp->rows; row++)
			{
				for (unsigned int col = 0; col < bmp->width; col++)
				{
					int x = pen_x + col;
					int y = pen_y + row;
					pixels[y * tex_width + x] = bmp->buffer[row * bmp->pitch + col];
				}
			}

			font->glyphs[i].uv.x0 = (float)pen_x / (float)tex_width;
			font->glyphs[i].uv.y0 = (float)pen_y / (float)tex_height;
			font->glyphs[i].uv.x1 = (float)(pen_x + bmp->width) / (float)tex_width;
			font->glyphs[i].uv.y1 = (float)(pen_y + bmp->rows) / (float)tex_height;

			font->glyphs[i].size.x = bmp->width;
			font->glyphs[i].size.y = bmp->rows;
			font->glyphs[i].bearing.x = face->glyph->bitmap_left;
			font->glyphs[i].bearing.y = face->glyph->bitmap_top;
			font->glyphs[i].advance = face->glyph->advance.x >> 6;

			pen_x += bmp->width + 1;
		}

		FT_Done_FreeType(ft);

		font->atlas.pixels = pixels;
		font->atlas.width = tex_width;
		font->atlas.height = tex_height;
		font->fontSize = fontSize;
		font->codepoints = charset;

		return true;
	}

	void sdfFont::Delete()
	{
		RendererBackends* backends = getRendererBackends();
		backends->fontIDsQueue.push(m_ID);
		m_FontAtlas->Delete();
	}

	sdfFont::sdfFont(const char* fontFilename, uint32_t channels, double setMinimumScale, double setPixelRange, double setMiterLimit, double madvanceCornerAngle)
	{
		RendererBackends* backends = getRendererBackends();
		m_ID = backends->fontIDsQueue.take_next();

		switch (channels)
		{
		case 1:
			CreateFontAtlasSDF(fontFilename, setMinimumScale, setPixelRange, setMiterLimit, madvanceCornerAngle);
			break;
		case 4:
			CreateFontAtlasMTSDF(fontFilename, setMinimumScale, setPixelRange, setMiterLimit, madvanceCornerAngle);
			break;
		default:
			CHONPS_CORE_LOG_ERROR(Font, "Cannot find font SDF generator that matches the channel given: {0} | channels are only supported in (1) and (4) format!", channels);
			break;
		}
	}

	void sdfFont::CreateFontAtlasMTSDF(const char* fontFilename, double setMinimumScale, double setPixelRange, double setMiterLimit, double madvanceCornerAngle)
	{
		if (msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype())
		{
			if (msdfgen::FontHandle* font = msdfgen::loadFont(ft, fontFilename))
			{
				std::vector<msdf_atlas::GlyphGeometry> glyphs;
				msdf_atlas::FontGeometry fontGeometry(&glyphs);

				fontGeometry.loadCharset(font, 1.0, msdf_atlas::Charset::ASCII);

				for (msdf_atlas::GlyphGeometry& glyph : glyphs)
					glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, madvanceCornerAngle, 0);

				msdf_atlas::TightAtlasPacker packer;
				packer.setDimensionsConstraint(msdf_atlas::TightAtlasPacker::DimensionsConstraint::POWER_OF_TWO_SQUARE);
				packer.setMinimumScale(setMinimumScale);
				packer.setPixelRange(setPixelRange);
				packer.setMiterLimit(setMiterLimit);
				packer.pack(glyphs.data(), static_cast<int>(glyphs.size()));

				int width = 0, height = 0;
				packer.getDimensions(width, height);
				msdf_atlas::ImmediateAtlasGenerator<float, 4, msdf_atlas::mtsdfGenerator, msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 4>> generator(width, height);

				msdf_atlas::GeneratorAttributes attributes;
				generator.setAttributes(attributes);
				generator.setThreadCount(4);

				// Generate atlas bitmap
				// The atlas bitmap can now be retrieved via atlasStorage as a BitmapConstRef.
				// The glyphs array (or fontGeometry) contains positioning data for typesetting text.
				generator.generate(glyphs.data(), static_cast<int>(glyphs.size()));

				const uint8_t* pixels = generator.atlasStorage().operator msdfgen::BitmapConstRef<msdf_atlas::byte, 4>().pixels;

				m_MaxHeight = 0.0f;

				msdfgen::FontMetrics metrics = fontGeometry.getMetrics();
				m_Metrics.emSize = metrics.emSize;
				m_Metrics.ascender = metrics.ascenderY;
				m_Metrics.descender = metrics.descenderY;
				m_Metrics.lineHeight = metrics.lineHeight;
				m_Metrics.underline = metrics.underlineY;
				m_Metrics.underlineThickness = metrics.underlineThickness;

				for (auto& glyph : glyphs)
				{
					FontGlyph fontGlyph;
					fontGlyph.unicode = glyph.getCodepoint();

					int x, y, w, h;
					glyph.getBoxRect(x, y, w, h);

					msdfgen::Shape::Bounds bounds = glyph.getShape().getBounds();

					fontGlyph.uv.x0 = (float)x / (float)width;
					fontGlyph.uv.y0 = (float)y / (float)height;
					fontGlyph.uv.x1 = ((float)x + (float)w) / (float)width;
					fontGlyph.uv.y1 = ((float)y + (float)h) / (float)height;
					fontGlyph.size = { ((float)bounds.r - (float)bounds.l), ((float)bounds.t - (float)bounds.b) };
					fontGlyph.bearing = { (float)bounds.l, (float)bounds.t };
					fontGlyph.advance = glyph.getAdvance() / fontGeometry.getGeometryScale();

					m_MaxHeight = m_MaxHeight < (bounds.t - bounds.b) ? (bounds.t - bounds.b) : m_MaxHeight;

					m_Glyphs[fontGlyph.unicode] = fontGlyph;
				}

				m_FontAtlas = createTexture(width, height, 4, pixels, TexType::Diffuse, { TexFilter::Linear, TexFilter::Linear }, TexWrap::ClampToEdge);

				// Cleanup
				msdfgen::destroyFont(font);
			}
			msdfgen::deinitializeFreetype(ft);
		}
	}

	void sdfFont::CreateFontAtlasSDF(const char* fontFilename, double setMinimumScale, double setPixelRange, double setMiterLimit, double madvanceCornerAngle)
	{
		if (msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype())
		{
			if (msdfgen::FontHandle* font = msdfgen::loadFont(ft, fontFilename))
			{
				std::vector<msdf_atlas::GlyphGeometry> glyphs;
				msdf_atlas::FontGeometry fontGeometry(&glyphs);

				fontGeometry.loadCharset(font, 1.0, msdf_atlas::Charset::ASCII);

				for (msdf_atlas::GlyphGeometry& glyph : glyphs)
					glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, madvanceCornerAngle, 0);

				msdf_atlas::TightAtlasPacker packer;
				packer.setDimensionsConstraint(msdf_atlas::TightAtlasPacker::DimensionsConstraint::POWER_OF_TWO_SQUARE);
				packer.setMinimumScale(setMinimumScale);
				packer.setPixelRange(setPixelRange);
				packer.setMiterLimit(setMiterLimit);
				packer.pack(glyphs.data(), static_cast<int>(glyphs.size()));

				int width = 0, height = 0;
				packer.getDimensions(width, height);
				msdf_atlas::ImmediateAtlasGenerator<float, 1, msdf_atlas::sdfGenerator, msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 1>> generator(width, height);

				msdf_atlas::GeneratorAttributes attributes;
				generator.setAttributes(attributes);
				generator.setThreadCount(4);

				// Generate atlas bitmap
				// The atlas bitmap can now be retrieved via atlasStorage as a BitmapConstRef.
				// The glyphs array (or fontGeometry) contains positioning data for typesetting text.
				generator.generate(glyphs.data(), static_cast<int>(glyphs.size()));

				msdfgen::savePng(generator.atlasStorage().operator msdfgen::BitmapConstRef<msdf_atlas::byte, 1>(), "outputSDF.png");

				const uint8_t* pixels = generator.atlasStorage().operator msdfgen::BitmapConstRef<msdf_atlas::byte, 1>().pixels;

				m_MaxHeight = 0.0f;

				msdfgen::FontMetrics metrics = fontGeometry.getMetrics();
				m_Metrics.emSize = metrics.emSize;
				m_Metrics.ascender = metrics.ascenderY;
				m_Metrics.descender = metrics.descenderY;
				m_Metrics.lineHeight = metrics.lineHeight;
				m_Metrics.underline = metrics.underlineY;
				m_Metrics.underlineThickness = metrics.underlineThickness;

				for (auto& glyph : glyphs)
				{
					FontGlyph fontGlyph;
					fontGlyph.unicode = glyph.getCodepoint();

					int x, y, w, h;
					glyph.getBoxRect(x, y, w, h);

					msdfgen::Shape::Bounds bounds = glyph.getShape().getBounds();

					fontGlyph.uv.x0 = (float)x / (float)width;
					fontGlyph.uv.y0 = (float)y / (float)height;
					fontGlyph.uv.x1 = ((float)x + (float)w) / (float)width;
					fontGlyph.uv.y1 = ((float)y + (float)h) / (float)height;
					fontGlyph.size = { ((float)bounds.r - (float)bounds.l), ((float)bounds.t - (float)bounds.b) };
					fontGlyph.bearing = { (float)bounds.l, (float)bounds.t };
					fontGlyph.advance = glyph.getAdvance() / fontGeometry.getGeometryScale();

					m_MaxHeight = m_MaxHeight < (bounds.t - bounds.b) ? (bounds.t - bounds.b) : m_MaxHeight;

					m_Glyphs[fontGlyph.unicode] = fontGlyph;
				}

				m_FontAtlas = createTexture(width, height, 1, pixels, TexType::Diffuse, { TexFilter::Nearest, TexFilter::Nearest }, TexWrap::ClampToEdge);

				// Cleanup
				msdfgen::destroyFont(font);
			}
			msdfgen::deinitializeFreetype(ft);
		}
	}

}