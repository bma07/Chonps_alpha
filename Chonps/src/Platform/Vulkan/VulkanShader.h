#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include "Core/File.h"
#include "Graphics/Shader.h"
#include "VulkanPipeline.h"

#include <vulkan/vulkan.h>

namespace Chonps
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& name, const std::string& vertexFile, const std::string& fragmentFile);
		VulkanShader(const std::string& filepath);
		virtual ~VulkanShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() const override;

		virtual std::string GetName() const override { return m_Name; }

		virtual uint32_t GetID() const override { return m_ID; }

		operator uint32_t& () { return m_ID; }

	private:
		std::string m_Name;
		uint32_t m_ID;

		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;

		VulkanPipelineShaderStages m_ShaderStages;
	};
}

#endif