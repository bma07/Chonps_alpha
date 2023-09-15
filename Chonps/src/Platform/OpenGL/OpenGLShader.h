#ifndef HG_CHONPS_OPENGL_SHADER_H
#define HG_CHONPS_OPENGL_SHADER_H

#include "Graphics/Pipeline.h"
#include "Graphics/Shader.h"
#include "Core/File.h"

namespace Chonps
{
	struct OpenGLPipelines
	{
		bool enableDepthTest = false, enableStencilTest = false, enableBlending = false, enableCullMode = false;
		uint32_t srcBlendFactor, dstBlendFactor;
		uint32_t depthCompare;
		uint32_t stencilCompareMask, stencilWriteMask, stencilReference, stencilFailOp, stencilPassOp, stencilDepthFailOp, stencilCompareOp;
		uint32_t cullmode, cullModeFrontFace;
	};

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexFile, const std::string& fragmentFile, PipelineLayoutInfo* pipelineInfo);
		virtual ~OpenGLShader() {}

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() const override;

		virtual uint32_t id() const override { return m_ID; }

		operator uint32_t& () { return m_ID; }


	private:
		uint32_t m_ID;
		PipelineLayoutInfo m_LayoutInfo;
		OpenGLPipelines m_Pipeline;

		void BindPipeline(PipelineLayoutInfo* pipelineLayout);
	};

	namespace ogls
	{
		void initStandardOglPipelineSpecification();

		PipelineSpecification getStandardOglPipelineSpecification();

		void setStandardOglPipelineSpecification(PipelineSpecification pipelineSpecification);

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