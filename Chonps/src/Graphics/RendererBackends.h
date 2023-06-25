#ifndef CHONPS_RENDERER_BACKENDS_H
#define CHONPS_RENDERER_BACKENDS_H

namespace Chonps
{
	enum class RenderTopologyType
	{
		Point,
		Line,
		LineStrip,
		Triangle,
		TriangleStrip,
		Default
	};
	typedef RenderTopologyType TopoType;

	enum class RenderCullFaceMode
	{
		Front,
		Back,
		Both,
		Disable
	};
	typedef RenderCullFaceMode CullFaceMode;

	enum class RenderCullFrontFace
	{
		Clockwise,
		CounterClockwise,

		CW = Clockwise,
		CCW = CounterClockwise
	};
	typedef RenderCullFrontFace CullFrontFace;

	struct RendererBackends
	{
		bool					enableColorBlend;
		bool					enableGammaCorrection;
		float					gammaValue;
		RenderTopologyType		topologyType;
		bool					enableCullFace;
		RenderCullFaceMode		cullFaceMode;
		RenderCullFrontFace		cullFrontFace;

		RendererBackends()
		{
			enableColorBlend = false;
			enableCullFace = false;
			enableGammaCorrection = false;
			gammaValue = 2.2f;
			topologyType = RenderTopologyType::Triangle;
			cullFaceMode = RenderCullFaceMode::Both;
			cullFrontFace = RenderCullFrontFace::Clockwise;
		}
	};

	void renderEnableColorBlend(bool enable);
	
	void renderGammaCorrection(bool correct);
	
	bool renderGetGammaCorrection();
	
	void renderSetGamma(float gamma);
	
	float renderGetGamma();
	
	// When the topology type is set to Default it will automatically be set to Triangle
	// This is different from filling in a Vulkan pipeline where when the topology is
	// set to default it will be set to the GraphicsAPI's topology type
	void renderSetTopologyType(RenderTopologyType type);
	
	// Get the topology type for renderering primitive geometry data
	RenderTopologyType renderGetTopologyType();

	void renderEnableCullFace(bool enable);
	
	void renderSetCullFaceMode(RenderCullFaceMode cullFaceMode);
	
	RenderCullFaceMode renderGetCullFaceMode();
	
	void renderSetCullFrontFace(RenderCullFrontFace cullFrontFace);
	
	RenderCullFrontFace renderGetCullFrontFace();

}

#endif