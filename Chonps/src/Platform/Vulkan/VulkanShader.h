#ifndef HG_CHONPS_VULKAN_SHADER_H
#define HG_CHONPS_VULKAN_SHADER_H

#include "Core/File.h"
#include "Graphics/Shader.h"
#include "VulkanPipeline.h"

#include <vulkan/vulkan.h>

namespace Chonps
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& vertexFile, const std::string& fragmentFile);
		virtual ~VulkanShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() const override;
		
		virtual uint32_t id() const override { return m_ID; }

		operator uint32_t& () { return m_ID; }

		void BindPipeline(PipelineLayoutInfo* pipelineLayout);

	private:

		uint32_t m_ID;
		PipelineLayoutInfo m_LayoutInfo;

		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;
		VulkanPipeline m_Pipeline;
		bool m_TextureArray = false, m_FrameBuffer = false, m_Cubemap = false;
		uint32_t m_TextureArrayIndex, m_FrameBufferIndex, m_CubemapIndex;

		VulkanPipelineShaderStages m_ShaderStages;
	};
}

#endif