#include <iostream>
#include <Chonps.h>

class ExampleLayer : public Chonps::Layer
{
public:
	ExampleLayer()
	{
		std::cout << "Hello\n";
	}

	virtual void OnUpdate() override
	{
		std::cout << "Hello\n";
	}
};

class OtherLayer : public Chonps::Layer
{
public:
	virtual void OnUpdate() override
	{
		CHONPS_CRITICAL("oh no");
	}

	void Print()
	{
		CHONPS_CRITICAL("oh no");
	}
};

int main()
{
	Chonps::Log::Init();

	CHONPS_INFO("Initialized Log");

	Chonps::Window window("window", 800, 600);

	ExampleLayer* el = new ExampleLayer;
	OtherLayer* ol = new OtherLayer;

	Chonps::LayerStack ls;
	ls.AddLayer(el);
	ls.AddLayer(ol);

	while (true)
	{
		window.OnUpdate();

		for (int i = 0; i < ls.size(); i++)
		{
			ls.GetLayer(i)->OnUpdate();
		}
	}

	return 0;
}

