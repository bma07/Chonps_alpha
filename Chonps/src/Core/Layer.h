#ifndef CHONPS_LAYER_H
#define CHONPS_LAYER_H

#include "Core.h"
#include "Events/Event.h"

namespace Chonps
{
	class Layer
	{
	public:
		Layer(std::string name = "Layer") {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}
		virtual bool LayerStatus() { return true; }
	};
}


#endif 