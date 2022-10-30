#include "cepch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Chonps
{
	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}

	void OpenGLRendererAPI::SetClearColor(const float r, const float g, const float b, const float w)
	{
		glClearColor(r, g, b, w);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	bool OpenGLRendererAPI::GetGammaCorrectiom()
	{
		return m_GammaCorrection;
	}
	
	void OpenGLRendererAPI::Draw(const size_t& count)
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
	}

	void OpenGLRendererAPI::FrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFBO);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	void OpenGLRendererAPI::UploadUniform1f(uint32_t shader, const char* uniform, float x)
	{
		glUniform1f(glGetUniformLocation(shader, uniform), x);
	}

	void OpenGLRendererAPI::UploadUniform2f(uint32_t shader, const char* uniform, float x, float y)
	{
		glUniform2f(glGetUniformLocation(shader, uniform), x, y);
	}

	void OpenGLRendererAPI::UploadUniform3f(uint32_t shader, const char* uniform, float x, float y, float z)
	{
		glUniform3f(glGetUniformLocation(shader, uniform), x, y, z);
	}

	void OpenGLRendererAPI::UploadUniform4f(uint32_t shader, const char* uniform, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(shader, uniform), x, y, z, w);
	}

	void OpenGLRendererAPI::UploadUniform1i(uint32_t shader, const char* uniform, int x)
	{
		glUniform1i(glGetUniformLocation(shader, uniform), x);
	}

	void OpenGLRendererAPI::UploadUniform2i(uint32_t shader, const char* uniform, int x, int y)
	{
		glUniform2i(glGetUniformLocation(shader, uniform), x, y);
	}

	void OpenGLRendererAPI::UploadUniform3i(uint32_t shader, const char* uniform, int x, int y, int z)
	{
		glUniform3i(glGetUniformLocation(shader, uniform), x, y, z);
	}

	void OpenGLRendererAPI::UploadUniform4i(uint32_t shader, const char* uniform, int x, int y, int z, int w)
	{
		glUniform4i(glGetUniformLocation(shader, uniform), x, y, z, w);
	}

	void OpenGLRendererAPI::UploadUniform1ui(uint32_t shader, const char* uniform, unsigned int x)
	{
		glUniform1ui(glGetUniformLocation(shader, uniform), x);
	}

	void OpenGLRendererAPI::UploadUniform2ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y)
	{
		glUniform2ui(glGetUniformLocation(shader, uniform), x, y);
	}

	void OpenGLRendererAPI::UploadUniform3ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z)
	{
		glUniform3ui(glGetUniformLocation(shader, uniform), x, y, z);
	}

	void OpenGLRendererAPI::UploadUniform4ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w)
	{
		glUniform4ui(glGetUniformLocation(shader, uniform), x, y, z, w);
	}

	void OpenGLRendererAPI::UploadUniform1fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{
		glUniform1fv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform2fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{
		glUniform2fv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform3fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{
		glUniform3fv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform4fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{
		glUniform4fv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform1iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{
		glUniform1iv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform2iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{
		glUniform2iv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform3iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{
		glUniform3iv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform4iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{
		glUniform4iv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform1uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{
		glUniform1uiv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform2uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{
		glUniform2uiv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform3uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{
		glUniform3uiv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform4uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{
		glUniform4uiv(glGetUniformLocation(shader, uniform), count, v);
	}

	void OpenGLRendererAPI::UploadUniform2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		glUniformMatrix2fv(glGetUniformLocation(shader, uniform), count, transpose, v);
	}

	void OpenGLRendererAPI::UploadUniform3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		glUniformMatrix3fv(glGetUniformLocation(shader, uniform), count, transpose, v);
	}

	void OpenGLRendererAPI::UploadUniform4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		glUniformMatrix4fv(glGetUniformLocation(shader, uniform), count, transpose, v);
	}

	void OpenGLRendererAPI::UploadUniform2x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		glUniformMatrix2x3fv(glGetUniformLocation(shader, uniform), count, transpose, v);
	}

	void OpenGLRendererAPI::UploadUniform3x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		glUniformMatrix3x2fv(glGetUniformLocation(shader, uniform), count, transpose, v);
	}

	void OpenGLRendererAPI::UploadUniform2x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		glUniformMatrix2x4fv(glGetUniformLocation(shader, uniform), count, transpose, v);
	}

	void OpenGLRendererAPI::UploadUniform4x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		glUniformMatrix4x2fv(glGetUniformLocation(shader, uniform), count, transpose, v);
	}

	void OpenGLRendererAPI::UploadUniform3x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		glUniformMatrix3x4fv(glGetUniformLocation(shader, uniform), count, transpose, v);
	}

	void OpenGLRendererAPI::UploadUniform4x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		glUniformMatrix4x3fv(glGetUniformLocation(shader, uniform), count, transpose, v);
	}

}