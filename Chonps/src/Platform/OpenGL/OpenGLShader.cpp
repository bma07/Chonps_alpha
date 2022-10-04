#include "cepch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

namespace Chonps
{
	GLenum get_shader_type(std::string type)
	{
		std::transform(type.begin(), type.end(), type.begin(), [](unsigned char c) { return std::tolower(c); });
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		CHONPS_CORE_ERROR("ERROR: SHADER: Unkown shader type: {0}", type);
		return 0;
	}

	void compileErrors(unsigned int shader, GLenum type, const char* shaderFile = " ")
	{
		GLint hasCompiled;
		char infoLog[1024];
		if (type != GL_PROGRAM)
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				std::string shaderType;
				if (type == GL_VERTEX_SHADER)
					shaderType = "VERTEX";
				else if (type == GL_FRAGMENT_SHADER)
					shaderType = "FRAGMENT";
				glGetShaderInfoLog(shader, 1024, 0, infoLog);
				CHONPS_CORE_ERROR("ERROR: SHADER: {0}: {1} - SHADER_COMPILATION_ERROR!\nInfolog: {2}", shaderType, shaderFile, infoLog);
				glDeleteShader(shader);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetShaderInfoLog(shader, 1024, 0, infoLog);
				CHONPS_CORE_ERROR("ERROR: SHADER: {0}: {1} - SHADER_LINKING_ERROR!\nInfolog: {2}", "PROGRAM", shaderFile, infoLog);
				glDeleteProgram(shader);
			}
		}
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexFile, const std::string& fragmentFile)
		: Shader(name, vertexFile, fragmentFile), m_Name(name)
	{
		std::string vertexCode = get_file_contents(vertexFile.c_str());
		std::string fragmentCode = get_file_contents(fragmentFile.c_str());

		const char* vertexSource = vertexCode.c_str();
		const char* fragmentSource = fragmentCode.c_str();

		// Create Vertex Shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, 0);
		glCompileShader(vertexShader);
		compileErrors(vertexShader, GL_VERTEX_SHADER, vertexFile.c_str());

		// Create Fragment Shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, 0);
		glCompileShader(fragmentShader);
		compileErrors(fragmentShader, GL_FRAGMENT_SHADER, fragmentFile.c_str());

		// Create Shader Program
		m_ID = glCreateProgram();

		// Bind Shaders
		glAttachShader(m_ID, vertexShader);
		glAttachShader(m_ID, fragmentShader);
		glLinkProgram(m_ID);
		compileErrors(m_ID, GL_PROGRAM);

		glDetachShader(m_ID, vertexShader);
		glDetachShader(m_ID, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: Shader(filepath)
	{
		m_ID = glCreateProgram();
		std::string source;
		
		if (get_file_contents(filepath.c_str(), source))
		{
			std::unordered_map<GLenum, std::string> shaders;

			// Gets shaders from single file, shader token needed to define shader type
			// Finds '#shader' token in the file with the corresponding shader type
			const char* shaderToken = "#shader";
			size_t shaderTokenLength = strlen(shaderToken);
			size_t shaderTokenPos = source.find(shaderToken, 0);
			while (shaderTokenPos != std::string::npos)
			{
				size_t eol = source.find_first_of("\r\n", shaderTokenPos);
				CHONPS_CORE_ASSERT(eol != std::string::npos, "Syntax error");
				std::string findWS = source.substr(shaderTokenPos + shaderTokenLength);
				int WhiteSpace = 0;
				for (auto ws : findWS) // Searches for whitespace between token and shader type
				{
					if (ws != ' ') break;
					WhiteSpace++;
				}
				size_t begin = shaderTokenPos + shaderTokenLength + WhiteSpace;
				std::string type = source.substr(begin, eol - begin); // type of shader
				CHONPS_CORE_ASSERT(get_shader_type(type), "Invalid shader type specification!");

				// Gets the source of the shader type
				// This will create a substring from the shader token until the next shader token is found
				size_t nextLinePos = source.find_first_not_of("\r\n", eol);
				shaderTokenPos = source.find(shaderToken, nextLinePos);
				std::string shaderSource = source.substr(nextLinePos, shaderTokenPos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

				shaders[get_shader_type(type)] = shaderSource;
			}

			std::vector<GLuint> shaderID;

			for (auto& st : shaders)
			{
				GLenum type = st.first;
				const std::string& shaderSource = st.second;

				// Create Shaders
				GLuint shader = glCreateShader(type);
				const char* shaderSourceCstr = shaderSource.c_str();
				glShaderSource(shader, 1, &shaderSourceCstr, 0);
				glCompileShader(shader);
				compileErrors(shader, type, filepath.c_str());
				glAttachShader(m_ID, shader);
				shaderID.push_back(shader); // Store shaders to be deleted later
			}

			// Link shaders to program
			glLinkProgram(m_ID);
			compileErrors(m_ID, GL_PROGRAM, filepath.c_str());

			// Detach shaders from program
			for (auto id : shaderID)
			{
				glDetachShader(m_ID, id);
				glDeleteShader(id);
			}

			// Sets name of the shader to the name of the file
			auto lastSlash = filepath.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = filepath.rfind('.');
			auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
			m_Name = filepath.substr(lastSlash, count);
		}
		else
			CHONPS_CORE_ERROR("ERROR: SHADER: Could not create shader with specified path: {0}", filepath);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_ID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::Delete() const
	{
		glDeleteProgram(m_ID);
	}
}