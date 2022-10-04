#include <iostream>
#include <Chonps.h>

#include <Imgui/imgui.h>

class Layer1 : public Chonps::Layer
{
public:
	Layer1(std::string name)
		: m_Name(name)
	{
		CHONPS_INFO("Layer created: {0}", name);
	}

	virtual ~Layer1() override
	{
		
	}

	virtual void OnAttach() override
	{
		CHONPS_INFO("Layer attached");
	}

	virtual void OnDetach() override
	{

	}

	virtual void OnUpdate() override
	{

	}

	virtual void OnEvent(Chonps::Event& e) override
	{
		
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Console");
		ImGui::Text("oh no");
		ImGui::End();
	}

	virtual bool LayerStatus() override
	{
		return m_UseLayer;
	}

private:
	std::string m_Name;
	bool m_UseLayer = true;
};


int main()
{
	Chonps::Log::Init();

	CHONPS_INFO("Initialized Log");
	
	Chonps::windowInitAPI(Chonps::WindowAPI::Glfw);

	Chonps::Application app("Chonps", 800, 600);

	Layer1 l1("l1");

	app.add_layer(&l1);

	app.Run();

	Chonps::windowTerminateAPI();

	return 0;
}

