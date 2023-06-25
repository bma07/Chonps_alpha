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
		virtual void Draw(VertexArray* vertexArray) override;

		virtual void DrawLine(VertexArray* vertexArray) override;

		virtual void BeginNextFrame() override;
		virtual void DrawSubmit() override;

		virtual void FrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height);

		virtual void EnableCullFace() override;
		virtual void DisableCullFace() override;
	};

	namespace oglSpec
	{
		void oglImplUploadUniform1f(uint32_t shader, const char* uniform, float x);
		void oglImplUploadUniform2f(uint32_t shader, const char* uniform, float x, float y);
		void oglImplUploadUniform3f(uint32_t shader, const char* uniform, float x, float y, float z);
		void oglImplUploadUniform4f(uint32_t shader, const char* uniform, float x, float y, float z, float w);

		void oglImplUploadUniform1i(uint32_t shader, const char* uniform, int x);
		void oglImplUploadUniform2i(uint32_t shader, const char* uniform, int x, int y);
		void oglImplUploadUniform3i(uint32_t shader, const char* uniform, int x, int y, int z);
		void oglImplUploadUniform4i(uint32_t shader, const char* uniform, int x, int y, int z, int w);

		void oglImplUploadUniform1ui(uint32_t shader, const char* uniform, unsigned int x);
		void oglImplUploadUniform2ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y);
		void oglImplUploadUniform3ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z);
		void oglImplUploadUniform4ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w);

		void oglImplUploadUniform1fv(uint32_t shader, const char* uniform, unsigned int count, const float* v);
		void oglImplUploadUniform2fv(uint32_t shader, const char* uniform, unsigned int count, const float* v);
		void oglImplUploadUniform3fv(uint32_t shader, const char* uniform, unsigned int count, const float* v);
		void oglImplUploadUniform4fv(uint32_t shader, const char* uniform, unsigned int count, const float* v);

		void oglImplUploadUniform1iv(uint32_t shader, const char* uniform, unsigned int count, const int* v);
		void oglImplUploadUniform2iv(uint32_t shader, const char* uniform, unsigned int count, const int* v);
		void oglImplUploadUniform3iv(uint32_t shader, const char* uniform, unsigned int count, const int* v);
		void oglImplUploadUniform4iv(uint32_t shader, const char* uniform, unsigned int count, const int* v);

		void oglImplUploadUniform1uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v);
		void oglImplUploadUniform2uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v);
		void oglImplUploadUniform3uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v);
		void oglImplUploadUniform4uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v);


		void oglImplUploadUniform2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
		void oglImplUploadUniform3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
		void oglImplUploadUniform4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);

		void oglImplUploadUniform2x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
		void oglImplUploadUniform3x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
		void oglImplUploadUniform2x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
		void oglImplUploadUniform4x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
		void oglImplUploadUniform3x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
		void oglImplUploadUniform4x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
	}
}

#endif