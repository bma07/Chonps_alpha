#ifndef OPENGL_RENDERER_API_H
#define OPENGL_RENDERER_API_H

#include "Graphics/RendererAPI.h"

namespace Chonps
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual ~OpenGLRendererAPI() {}

		virtual void Init() override;
		virtual void ClearColor(const float r, const float g, const float b, const float w) override;
		virtual void Clear() override;

		virtual void Draw(const uint32_t& count) override;
		virtual void DrawLine() override;
		virtual void FrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height);

		virtual void EnableCullFace() override;
		virtual void DisableCullFace() override;

		virtual void UploadUniform1f(uint32_t shader, const char* uniform, float x) override;
		virtual void UploadUniform2f(uint32_t shader, const char* uniform, float x, float y) override;
		virtual void UploadUniform3f(uint32_t shader, const char* uniform, float x, float y, float z) override;
		virtual void UploadUniform4f(uint32_t shader, const char* uniform, float x, float y, float z, float w) override;

		virtual void UploadUniform1i(uint32_t shader, const char* uniform, int x) override;
		virtual void UploadUniform2i(uint32_t shader, const char* uniform, int x, int y) override;
		virtual void UploadUniform3i(uint32_t shader, const char* uniform, int x, int y, int z) override;
		virtual void UploadUniform4i(uint32_t shader, const char* uniform, int x, int y, int z, int w) override;

		virtual void UploadUniform1ui(uint32_t shader, const char* uniform, unsigned int x) override;
		virtual void UploadUniform2ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y) override;
		virtual void UploadUniform3ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z) override;
		virtual void UploadUniform4ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w) override;

		virtual void UploadUniform1fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) override;
		virtual void UploadUniform2fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) override;
		virtual void UploadUniform3fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) override;
		virtual void UploadUniform4fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) override;

		virtual void UploadUniform1iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) override;
		virtual void UploadUniform2iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) override;
		virtual void UploadUniform3iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) override;
		virtual void UploadUniform4iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) override;

		virtual void UploadUniform1uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) override;
		virtual void UploadUniform2uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) override;
		virtual void UploadUniform3uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) override;
		virtual void UploadUniform4uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) override;

		virtual void UploadUniform2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;

		virtual void UploadUniform2x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform3x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform2x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform4x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform3x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform4x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
	};
}

#endif