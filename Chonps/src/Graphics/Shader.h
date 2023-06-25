#ifndef CHONPS_SHADER_H
#define CHONPS_SHADER_H

namespace Chonps
{
	class Shader
	{
	public:
		Shader(const std::string& name, const std::string& vertex, const std::string& fragment) {}
		Shader(const std::string& filepath) {}
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() const = 0;

		virtual std::string GetName() const = 0;

		virtual uint32_t GetID() const = 0;
	};

	// Creates Shader from two seperate vertex and fragment files
	std::shared_ptr<Shader> createShaderSp(const std::string& vertex, const std::string& fragment, const std::string& name = "Shader");

	// Creates Shader from single file containing all shaders
	std::shared_ptr<Shader> createShaderSp(const std::string& filepath);

	// Creates Shader from two seperate vertex and fragment files
	Shader* createShader(const std::string& vertex, const std::string& fragment, const std::string& name = "Shader");

	// Creates Shader from single file containing all shaders
	Shader* createShader(const std::string& filepath);

	class ShaderLibrary
	{
	public:

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
	};
}


#endif