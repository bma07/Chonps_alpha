#ifndef LAYER_H
#define LAYER_H

#include "Core.h"
#include "Events/Event.h"

namespace Chonps
{
	class CHONPS_API Layer
	{
	public:
		Layer(std::string name = "Layer") {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}
	};
}


#endif 