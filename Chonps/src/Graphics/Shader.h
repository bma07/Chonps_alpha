#ifndef SHADER_H
#define SHADER_H

#include "Core.h"

namespace Chonps
{
	class Shader
	{
	public:
		Shader(const std::string& name, const std::string& vertex, const std::string& fragment) {}
		Shader(const std::string& filepath) {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() const = 0;

		virtual std::string GetName() const = 0;

		virtual uint32_t GetID() const = 0;
	};

	// Creates Shader from two seperate vertex and fragment files
	Shader* createShader(const std::string& vertex, const std::string& fragment, const std::string& name = "Shader");

	// Creates Shader from single file containing all shaders
	Shader* createShader(const std::string& filepath);
}


#endif