#ifndef CHONPS_RENDERER_API_H
#define CHONPS_RENDERER_API_H

#include "RendererBackends.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Shader.h"

namespace Chonps
{
	enum class GraphicsAPI
	{
		None = 0,
		OpenGL,
		Vulkan,
		DirectX
	};

	bool setRenderContext(GraphicsAPI api);

	GraphicsAPI getGraphicsAPI();
	std::string getGraphicsAPIName();


	class RendererAPI
	{
	public:
		virtual ~RendererAPI() {}
		virtual void Init() = 0;
		virtual void ClearColor(const float r, const float g, const float b, const float w) = 0;
		virtual void Clear() = 0;

		virtual void Draw(const uint32_t& count) = 0;
		virtual void Draw(VertexArray* vertexArray) = 0;
		virtual void DrawLine(VertexArray* vertexArray) = 0;

		virtual void BeginNextFrame() = 0;
		virtual void DrawSubmit() = 0;

		virtual void FrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height) = 0;

		virtual void EnableCullFace() = 0;
		virtual void DisableCullFace() = 0;
	};

	RendererAPI* getRendererAPI();
	bool createRendererAPI();
	void destroyRendererAPI();

	// Vulkan API Specific
	namespace vkSpec
	{
		// Set topology to default if no topology type needs to be specified
		// topology type will be copied from RendererAPI's topology type
		struct InputAssembly
		{
			RenderTopologyType topology;
			bool primitiveRestartEnable;
		};

		struct rec2d
		{
			int x, y;
		};

		// width and height are based on GLFW window framebuffer size
		// Note: GLFW framebuffer size and window pixel coordinates may not be the same on different systems
		// Set width and height to -1 if it does not need to be specified, width and height will then be automatically set to the framebuffer size
		struct Viewport
		{
			float x, y;
			float width, height;
			float minDepth, maxDepth;

			rec2d scissorsOffset;
		};

		struct Rasterizer
		{
			bool depthClampEnable;
			bool rasterizerDiscardEnable;
			float lineWidth;

			RenderCullFaceMode cullMode;
			RenderCullFrontFace frontFace;

			bool depthBiasEnable;
			float depthBiasConstantFactor = 0.0f; // Optional
			float depthBiasClamp = 0.0f; // Optional
			float depthBiasSlopeFactor = 0.0f; // Optional
		};

		enum Sample
		{
			SampleCount_1_Bit = 0x00000001,
			SampleCount_2_Bit = 0x00000002,
			SampleCount_4_Bit = 0x00000004,
			SampleCount_8_Bit = 0x00000008,
			SampleCount_16_Bit = 0x00000010,
			SampleCount_32_Bit = 0x00000020,
			SampleCount_64_Bit = 0x00000040,
			SampleCount_Max_Bit = 0x7FFFFFFF
		};

		struct MultiSampling
		{
			bool sampleShadingEnable;
			Sample rasterizationSamples;
			float minSampleShading = 1.0f; // Optional
			uint32_t* sampleMask = nullptr; // Optional
			bool alphaToCoverageEnable = false; // Optional
			bool alphaToOneEnable = false; // Optional
		};

		struct ColorBlend
		{
			bool blendEnable;
			bool logicOpEnable;
			float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // Optional
		};

		struct PipelineLayout
		{
			uint32_t setLayoutCount = 0; // Optional
			void* setLayouts = nullptr; // Optional
			uint32_t pushConstantRangeCount = 0; // Optional
			void* pushConstantRanges = nullptr; // Optional
		};

		struct PipelineSpecification
		{
			InputAssembly inputAssembly;
			Viewport viewport;
			Rasterizer rasterizer;
			MultiSampling multisampling;
			ColorBlend colorBlend;
			PipelineLayout pipelineLayout;
		};

	}

	namespace oglSpec
	{
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
}

#endif