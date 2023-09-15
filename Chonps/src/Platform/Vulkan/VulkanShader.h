#ifndef HG_CHONPS_VULKAN_SHADER_H
#define HG_CHONPS_VULKAN_SHADER_H

#include "Core/File.h"
#include "Graphics/Shader.h"
#include "VulkanPipeline.h"

#include <vulkan/vulkan.h>

namespace Chonps
{
	struct VulkanDynamicStencil
	{
		bool enabled = false;
		VkStencilOp failOp = VK_STENCIL_OP_KEEP, depthFailOp = VK_STENCIL_OP_KEEP, passOp = VK_STENCIL_OP_KEEP;
		VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;
		uint32_t writeMask = 0x00, compareMask = 0x00, reference = 1;
	};

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& vertexFile, const std::string& fragmentFile, PipelineLayoutInfo* pipelineInfo);
		virtual ~VulkanShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() const override;
		
		virtual uint32_t id() const override { return m_ID; }

		operator uint32_t& () { return m_ID; }

		VkPipelineLayout getNativePipelineLayout() { return m_Pipeline.pipelineLayout; }

	private:
		uint32_t m_ID;
		PipelineLayoutInfo m_LayoutInfo;

		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;
		VulkanPipeline m_Pipeline;

		VulkanDynamicStencil m_DynamicStencil;

		VulkanPipelineShaderStages m_ShaderStages;

		void BindPipeline(PipelineLayoutInfo* pipelineLayout);
	};
}

#endif