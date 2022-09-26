#include <iostream>
#include <Chonps.h>

class Layer1 : public Chonps::Layer
{
public:
	Layer1(std::string name)
		: m_Name(name)
	{
		m_Window = std::unique_ptr<Chonps::Window>(Chonps::createWindow("window1", 800, 600));
	}

	virtual void OnUpdate() override
	{
		m_Window->OnUpdate();
		if (Chonps::keyPressed(&(*m_Window), CHONPS_KEY_0))
			CHONPS_INFO("{0}", CHONPS_KEY_0);

		if (!m_Window->WindowIsOpen())
		{
			m_Window->Delete();
			m_UseLayer = false;
		}
	}

	virtual bool LayerStatus() override
	{
		return m_UseLayer;
	}

private:
	std::unique_ptr<Chonps::Window> m_Window;
	std::string m_Name;
	bool m_UseLayer = true;
};

class Layer2 : public Chonps::Layer
{
public:
	Layer2(std::string name)
		: m_Name(name)
	{
		m_Window = std::unique_ptr<Chonps::Window>(Chonps::createWindow("window2", 400, 400));
	}

	virtual void OnUpdate() override
	{
		m_Window->OnUpdate();
		m_Window->LogEvents(true);

		if (!m_Window->WindowIsOpen())
		{
			m_Window->Delete();
			m_UseLayer = false;
		}
	}

	virtual bool LayerStatus() override
	{
		return m_UseLayer;
	}

private:
	std::unique_ptr<Chonps::Window> m_Window;
	std::string m_Name;
	bool m_UseLayer = true;
};


int main()
{
	Chonps::Log::Init();

	CHONPS_INFO("Initialized Log");
	
	Chonps::initWindowAPI();

	Chonps::LayerStack ls;
	ls.AddLayer(new Layer1("l1"));
	ls.AddLayer(new Layer2("l2"));

	while (true)
	{
		for (int i = 0; i < ls.size(); i++)
		{
			ls.GetLayer(i)->OnUpdate();
			if (!ls.GetLayer(i)->LayerStatus())
				ls.RemoveLayer(ls.GetLayer(i));
		}
	}

	Chonps::terminateWindowAPI();

	return 0;
}

