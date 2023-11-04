#include "cepch.h"
#include "Pipeline.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanPipeline.h"

namespace Chonps
{
	void setStandardPipelineSpecification(const PipelineSpecification& pipelineSpecification)
	{
		GraphicsContext api = getGraphicsContext();

		if (api == GraphicsContext::OpenGL)
			ogls::setStandardOglPipelineSpecification(pipelineSpecification);
		else if (api == GraphicsContext::Vulkan)
			vks::vkImplSetStandardVulkanPipelineSpecification(pipelineSpecification);
	}

	PipelineSpecification getStandardPipelineSpecification()
	{
		GraphicsContext api = getGraphicsContext();

		if (api == GraphicsContext::OpenGL)
			return ogls::getStandardOglPipelineSpecification();
		else if (api == GraphicsContext::Vulkan)
			return vks::vkImplGetStandardVulkanPipelineSpecification();

		return PipelineSpecification();
	}
}