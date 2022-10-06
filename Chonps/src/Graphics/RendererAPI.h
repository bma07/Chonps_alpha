#ifndef RENDERER_API_H
#define RENDERER_API_H

namespace Chonps
{
	enum class API
	{
		None = 0,
		OpenGL,
		Vulkan,
		DirectX
	};

	bool renderInit(API api = API::OpenGL);

	API getGraphicsContext();
	std::string getGraphicsContextName();


	class RendererAPI
	{
	public:
		virtual void Init() = 0;
		virtual void SetClearColor(const float r, const float g, const float b, const float w) = 0;
		virtual void Clear() = 0;

		virtual void Draw(const size_t& count) = 0;

		virtual void UploadUniform1f(uint32_t shader, const char* uniform, float x) = 0;
		virtual void UploadUniform2f(uint32_t shader, const char* uniform, float x, float y) = 0;
		virtual void UploadUniform3f(uint32_t shader, const char* uniform, float x, float y, float z) = 0;
		virtual void UploadUniform4f(uint32_t shader, const char* uniform, float x, float y, float z, float w) = 0;

		virtual void UploadUniform1i(uint32_t shader, const char* uniform, int x) = 0;
		virtual void UploadUniform2i(uint32_t shader, const char* uniform, int x, int y) = 0;
		virtual void UploadUniform3i(uint32_t shader, const char* uniform, int x, int y, int z) = 0;
		virtual void UploadUniform4i(uint32_t shader, const char* uniform, int x, int y, int z, int w) = 0;

		virtual void UploadUniform1ui(uint32_t shader, const char* uniform, unsigned int x) = 0;
		virtual void UploadUniform2ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y) = 0;
		virtual void UploadUniform3ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z) = 0;
		virtual void UploadUniform4ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w) = 0;

		virtual void UploadUniform1fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) = 0;
		virtual void UploadUniform2fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) = 0;
		virtual void UploadUniform3fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) = 0;
		virtual void UploadUniform4fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) = 0;

		virtual void UploadUniform1iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) = 0;
		virtual void UploadUniform2iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) = 0;
		virtual void UploadUniform3iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) = 0;
		virtual void UploadUniform4iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) = 0;

		virtual void UploadUniform1uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) = 0;
		virtual void UploadUniform2uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) = 0;
		virtual void UploadUniform3uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) = 0;
		virtual void UploadUniform4uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) = 0;


		virtual void UploadUniform2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) = 0;
		virtual void UploadUniform3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) = 0;
		virtual void UploadUniform4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) = 0;

		virtual void UploadUniform2x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) = 0;
		virtual void UploadUniform3x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) = 0;
		virtual void UploadUniform2x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) = 0;
		virtual void UploadUniform4x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) = 0;
		virtual void UploadUniform3x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) = 0;
		virtual void UploadUniform4x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) = 0;
	};

	RendererAPI* createRendererAPI();

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