#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include "Graphics/Shader.h"
#include "Graphics/File.h"


namespace Chonps
{
	enum class ShaderType
	{
		None = 0,
		Vertex,
		Fragment
	};

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertexFile, const std::string& fragmentFile);
		OpenGLShader(const std::string& filepath);


		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() const override;

		virtual std::string GetName() const override { return m_Name; }

		virtual uint32_t GetID() const override { return m_ID; }

		operator uint32_t& () { return m_ID; }

	private:
		std::string m_Name;
		uint32_t m_ID;
	};
}

#endif