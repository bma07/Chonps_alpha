#include "cepch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

#include "OpenGLContext.h"
#include "OpenGLShader.h"

namespace Chonps
{
	GLFWwindow* s_OglCurrentWindowContext = nullptr;

	GLenum getCullFaceMode(CullFaceMode cullFaceMode)
	{
		switch (cullFaceMode)
		{
			case CullFaceMode::Disable: { return GL_NONE; }
			case CullFaceMode::Front: { return GL_FRONT; }
			case CullFaceMode::Back: { return GL_BACK; }
			case CullFaceMode::Both: { return GL_FRONT_AND_BACK; }
			default:
			{
				CHONPS_CORE_LOG_ERROR(OpenGL, "Cannot find matching cull face mode!");
				return GL_NONE;
			}
		}
	}

	GLenum getFrontFace(CullFrontFace frontFace)
	{
		switch (frontFace)
		{
			case CullFrontFace::Clockwise: { return GL_CW; }
			case CullFrontFace::CounterClockwise: { return GL_CCW; }
			default:
			{
				CHONPS_CORE_LOG_ERROR(OpenGL, "Cannot find matching cull face front mode!");
				return GL_CW;
			}
		}
	}

	GLenum getTopologyType(TopologyType topologyType)
	{
		switch (topologyType)
		{
			case TopologyType::None: { return GL_NONE; }
			case TopologyType::Point: { return GL_POINTS; }
			case TopologyType::Line: { return GL_LINES; }
			case TopologyType::LineStrip: { return GL_LINE_STRIP; }
			case TopologyType::Triangle: { return GL_TRIANGLES; }
			case TopologyType::TriangleStrip: { return GL_TRIANGLE_STRIP; }
			default:
			{
				CHONPS_CORE_LOG_ERROR(OpenGL, "Cannot find matching topology type!");
				return GL_NONE;
			}
		}
	}

	void OpenGLRendererAPI::Init()
	{
		m_RendererBackends = getRendererBackends();

		if (m_RendererBackends->enableMultiThreading)
			glEnable(GL_MULTISAMPLE);
		if (m_RendererBackends->enableDepthTesting)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
		}
		if (m_RendererBackends->enableStencilTesting)
			glEnable(GL_STENCIL_TEST);

		if (m_RendererBackends->enableColorBlend)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		if (m_RendererBackends->enableCullFace)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(getCullFaceMode(m_RendererBackends->cullFaceMode));
			glFrontFace(getFrontFace(m_RendererBackends->cullFrontFace));
		}

		m_TopologyType = getTopologyType(m_RendererBackends->topologyType);

		if (m_RendererBackends->enableMultiThreading)
			CHONPS_CORE_LOG_WARN(OpenGL, "OpenGL does not support multithreading; Multithreading capabilities will not be used!");

		ogls::initStandardOglPipelineSpecification();

		PipelineSpecification pipelineSpec = ogls::getStandardOglPipelineSpecification();
		pipelineSpec.rasterizer.cullMode = m_RendererBackends->cullFaceMode;
		pipelineSpec.rasterizer.frontFace = m_RendererBackends->cullFrontFace;
		pipelineSpec.depthstencil.enableDepth = m_RendererBackends->enableDepthTesting;
		pipelineSpec.depthstencil.enableStencil = m_RendererBackends->enableStencilTesting;
		pipelineSpec.depthstencil.depthOpCompare = m_RendererBackends->depthOpCompare;
		pipelineSpec.depthstencil.stencil.reference = m_RendererBackends->stencil.reference;
		pipelineSpec.depthstencil.stencil.compareMask = m_RendererBackends->stencil.compareMask;
		pipelineSpec.depthstencil.stencil.writeMask = m_RendererBackends->stencil.writeMask;
		pipelineSpec.depthstencil.stencil.compareOp = m_RendererBackends->stencil.compareOp;

		ogls::setStandardOglPipelineSpecification(pipelineSpec);
	}

	void OpenGLRendererAPI::ClearColor(const float r, const float g, const float b, const float w)
	{
		glClearColor(r, g, b, w);
	}

	void OpenGLRendererAPI::Clear()
	{
		glStencilMask(0xff);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::Draw(uint32_t vertexCount)
	{
		glDrawArrays(m_TopologyType, 0, vertexCount);
	}

	void OpenGLRendererAPI::DrawIndexed(uint32_t indexCount)
	{
		glDrawElements(m_TopologyType, indexCount, GL_UNSIGNED_INT, 0);
	}

	void OpenGLRendererAPI::DrawIndexed(VertexArray* vertexArray)
	{
		vertexArray->Bind();
		glDrawElements(m_TopologyType, vertexArray->GetIndexCount(), GL_UNSIGNED_INT, 0);
	}

	void OpenGLRendererAPI::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
	{
		glDrawArraysInstancedBaseInstance(m_TopologyType, 0, vertexCount, instanceCount, firstInstance);
	}

	void OpenGLRendererAPI::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
	{
		glDrawElementsInstancedBaseInstance(m_TopologyType, indexCount, GL_UNSIGNED_INT, 0, instanceCount, firstInstance);
	}

	void OpenGLRendererAPI::SetStencilReference(uint32_t reference)
	{
		GLint stencilFunc, stencilMask;
		glGetIntegerv(GL_STENCIL_FUNC, &stencilFunc);
		glGetIntegerv(GL_STENCIL_VALUE_MASK, &stencilMask);
		glStencilFunc(stencilFunc, reference, stencilMask);
	}

	void OpenGLRendererAPI::SetStencilMask(uint32_t compareMask, uint32_t writeMask)
	{
		GLint stencilFunc, stencilRef;
		glGetIntegerv(GL_STENCIL_FUNC, &stencilFunc);
		glGetIntegerv(GL_STENCIL_REF, &stencilRef);
		glStencilFunc(stencilFunc, stencilRef, compareMask);
		glStencilMask(writeMask);
	}

	void OpenGLRendererAPI::BeginNextFrame()
	{
		int width, height;
		glfwGetFramebufferSize(s_OglCurrentWindowContext, &width, &height);
		gladUpdateViewPort(s_OglCurrentWindowContext, width, height);
	}

	void OpenGLRendererAPI::DrawSubmit()
	{

	}

	void OpenGLRendererAPI::RenderPassBegin()
	{
		int width, height;
		glfwGetFramebufferSize(s_OglCurrentWindowContext, &width, &height);
		gladUpdateViewPort(s_OglCurrentWindowContext, width, height);
	}

	void OpenGLRendererAPI::RenderPassEnd()
	{

	}

	void OpenGLRendererAPI::BindBufferSet(Shader* shader, UniformBuffer* buffer, uint32_t setIndex)
	{

	}

	void setOglCurrentWindow(GLFWwindow* window)
	{
		s_OglCurrentWindowContext = window;
	}

	namespace ogls
	{
		uint32_t getBlendFactorType(ColorBlendFactor blendFactor)
		{
			switch (blendFactor)
			{
				case Chonps::ColorBlendFactor::Zero: { return GL_ZERO; }
				case Chonps::ColorBlendFactor::One: { return GL_ONE; }
				case Chonps::ColorBlendFactor::SrcColor: { return GL_SRC_COLOR; }
				case Chonps::ColorBlendFactor::OneMinusSrcColor: { return GL_ONE_MINUS_SRC_COLOR; }
				case Chonps::ColorBlendFactor::DstColor: { return GL_DST_COLOR; }
				case Chonps::ColorBlendFactor::OneMinusDstColor: { return GL_ONE_MINUS_DST_COLOR; }
				case Chonps::ColorBlendFactor::SrcAlpha: { return GL_SRC_ALPHA; }
				case Chonps::ColorBlendFactor::OneMinusSrcAlpha: { return GL_ONE_MINUS_SRC_ALPHA; }
				case Chonps::ColorBlendFactor::DstAlpha: { return GL_DST_ALPHA; }
				case Chonps::ColorBlendFactor::OneMinusDstAlpha: { return GL_ONE_MINUS_DST_ALPHA; }
				case Chonps::ColorBlendFactor::ConstantColor: { return GL_CONSTANT_COLOR; }
				case Chonps::ColorBlendFactor::OneMinusConstantColor: { return GL_ONE_MINUS_CONSTANT_COLOR; }
				case Chonps::ColorBlendFactor::ConstantAlpha: { return GL_CONSTANT_ALPHA; }
				case Chonps::ColorBlendFactor::OneMinusConstantAlpha: { return GL_ONE_MINUS_CONSTANT_ALPHA; }
				case Chonps::ColorBlendFactor::SrcAlphaSaturate: { return GL_SRC_ALPHA_SATURATE; }
				case Chonps::ColorBlendFactor::Src1Color: { return GL_SRC1_COLOR; }
				case Chonps::ColorBlendFactor::OneMinusSrc1Color: { return GL_ONE_MINUS_SRC1_COLOR; }
				case Chonps::ColorBlendFactor::Src1_Alpha: { return GL_SRC1_ALPHA; }
				case Chonps::ColorBlendFactor::OneMinusSrc1Alpha: { return GL_ONE_MINUS_SRC1_ALPHA; }
				default:
				{
					CHONPS_CORE_LOG_ERROR(OpenGL, "Could not find matching color blend factor!");
					return GL_ZERO;
				}
			}
		}

		uint32_t getPipelineCullFaceMode(CullFaceMode cullFaceMode)
		{
			switch (cullFaceMode)
				{
				case CullFaceMode::Disable: { return GL_NONE; }
				case CullFaceMode::Front: { return GL_FRONT; }
				case CullFaceMode::Back: { return GL_BACK; }
				case CullFaceMode::Both: { return GL_FRONT_AND_BACK; }
				default:
				{
					CHONPS_CORE_LOG_ERROR(OpenGL, "Cannot find matching cull face mode!");
					return GL_NONE;
				}
			}
		}

		uint32_t getPipelineFrontFace(CullFrontFace frontFace)
		{
			switch (frontFace)
			{
				case CullFrontFace::Clockwise: { return GL_CW; }
				case CullFrontFace::CounterClockwise: { return GL_CCW; }
				default:
				{
					CHONPS_CORE_LOG_ERROR(OpenGL, "Cannot find matching cull face front mode!");
					return GL_CW;
				}
			}
		}

		uint32_t getPipelineCompareOp(CompareOperation compare)
		{
			switch (compare)
				{
				case CompareOperation::Never: { return GL_NEVER; }
				case CompareOperation::Less: { return GL_LESS; }
				case CompareOperation::Equal: { return GL_EQUAL; }
				case CompareOperation::LessOrEqual: { return GL_LEQUAL; }
				case CompareOperation::Greater: { return GL_GREATER; }
				case CompareOperation::NotEqual: { return GL_NOTEQUAL; }
				case CompareOperation::GreaterOrEqual: { return GL_GEQUAL; }
				case CompareOperation::Always: { return GL_ALWAYS; }
				default:
				{
					CHONPS_CORE_LOG_ERROR(OpenGL, "Cannot find matching compare operation!");
					return GL_NEVER;
			}
			}
		}

		uint32_t getPipelineStencilOp(StencilOperation stencilOp)
		{
			switch (stencilOp)
			{
				case StencilOperation::Keep: { return GL_KEEP; }
				case StencilOperation::Zero: { return GL_ZERO; }
				case StencilOperation::Replace: { return GL_REPLACE; }
				case StencilOperation::IncrementAndClamp: { return GL_INCR_WRAP; }
				case StencilOperation::DecrementAndClamp: { return GL_DECR_WRAP; }
				case StencilOperation::Invert: { return GL_INVERT; }
				case StencilOperation::IncrementAndWrap: { return GL_INCR_WRAP; }
				case StencilOperation::DecrementAndWrap: { return GL_DECR_WRAP; }
				default:
				{
					CHONPS_CORE_LOG_ERROR(OpenGL, "Cannot find matching stencil operation!");
					return GL_KEEP;
				}
			}
		}
	}
}