#include "cepch.h"
#include "Pipeline.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanPipeline.h"

namespace Chonps
{
	void setStandardPipelineSpecification(const PipelineSpecification& pipelineSpecification)
	{
		GraphicsAPI api = getGraphicsAPI();

		if (api == GraphicsAPI::OpenGL)
			ogls::setStandardOglPipelineSpecification(pipelineSpecification);
		else if (api == GraphicsAPI::Vulkan)
			vks::vkImplSetStandardVulkanPipelineSpecification(pipelineSpecification);
	}

	PipelineSpecification getStandardPipelineSpecification()
	{
		GraphicsAPI api = getGraphicsAPI();

		if (api == GraphicsAPI::OpenGL)
			return ogls::getStandardOglPipelineSpecification();
		else if (api == GraphicsAPI::Vulkan)
			return vks::vkImplGetStandardVulkanPipelineSpecification();

		return PipelineSpecification();
	}
}