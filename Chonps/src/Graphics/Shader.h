#ifndef SHADER_H
#define SHADER_H

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


	void uploadUniform1f(uint32_t shader, const char* uniform, float x);
	void uploadUniform2f(uint32_t shader, const char* uniform, float x, float y);
	void uploadUniform3f(uint32_t shader, const char* uniform, float x, float y, float z);
	void uploadUniform4f(uint32_t shader, const char* uniform, float x, float y, float z, float w);

	void uploadUniform1i(uint32_t shader, const char* uniform, int x);
	void uploadUniform2i(uint32_t shader, const char* uniform, int x, int y);
	void uploadUniform3i(uint32_t shader, const char* uniform, int x, int y, int z);
	void uploadUniform4i(uint32_t shader, const char* uniform, int x, int y, int z, int w);

	void uploadUniform1ui(uint32_t shader, const char* uniform, unsigned int x);
	void uploadUniform2ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y);
	void uploadUniform3ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z);
	void uploadUniform4ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w);

	void uploadUniform1fv(uint32_t shader, const char* uniform, unsigned int count, const float* v);
	void uploadUniform2fv(uint32_t shader, const char* uniform, unsigned int count, const float* v);
	void uploadUniform3fv(uint32_t shader, const char* uniform, unsigned int count, const float* v);
	void uploadUniform4fv(uint32_t shader, const char* uniform, unsigned int count, const float* v);

	void uploadUniform1iv(uint32_t shader, const char* uniform, unsigned int count, const int* v);
	void uploadUniform2iv(uint32_t shader, const char* uniform, unsigned int count, const int* v);
	void uploadUniform3iv(uint32_t shader, const char* uniform, unsigned int count, const int* v);
	void uploadUniform4iv(uint32_t shader, const char* uniform, unsigned int count, const int* v);

	void uploadUniform1uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v);
	void uploadUniform2uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v);
	void uploadUniform3uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v);
	void uploadUniform4uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v);


	void uploadUniform2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
	void uploadUniform3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
	void uploadUniform4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);

	void uploadUniform2x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
	void uploadUniform3x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
	void uploadUniform2x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
	void uploadUniform4x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
	void uploadUniform3x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
	void uploadUniform4x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v);
}


#endif