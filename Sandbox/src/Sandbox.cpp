#include <iostream>
#include <Chonps/Chonps.h>

using namespace std::placeholders;

int main()
{
	Chonps::Log::Init();

	CHONPS_INFO("Initialized Log");

	Chonps::Window window("window", 800, 600);
	

	while (true)
	{
		window.OnUpdate();
	}

	return 0;
}