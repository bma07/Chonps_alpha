#include <iostream>
#include <Chonps.h>

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
		//CHONPS_INFO("Layer update: {0}", m_UseLayer);
	}

	virtual void OnEvent(Chonps::Event& e) override
	{
		CHONPS_INFO(e.ToString());
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
	
	Chonps::initWindowAPI();

	Chonps::Application app("Chonps", 800, 600);

	app.add_layer(new Layer1("l1"));

	app.Run();

	Chonps::terminateWindowAPI();

	return 0;
}

