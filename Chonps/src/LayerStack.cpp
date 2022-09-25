#include "cepch.h"
#include "LayerStack.h"

namespace Chonps
{
	void LayerStack::AddLayer(Layer* layer)
	{
		Layers.emplace_back(layer);
		layer->OnAttach();
	}

	void LayerStack::InsertLayer(Layer* layer, unsigned int index)
	{
		Layers.insert(Layers.begin() + index, layer);
	}

	void LayerStack::RemoveLayer(Layer* layer)
	{
		auto it = std::find(Layers.begin(), Layers.end(), layer);
		if (it != Layers.end())
		{
			layer->OnDetach();
			Layers.erase(it);
		}
	}

	void LayerStack::DeleteLayer(Layer* layer)
	{
		auto it = std::find(Layers.begin(), Layers.end(), layer);
		if (it != Layers.end())
		{
			Layers.erase(it);
			delete layer;
		}
	}

	void LayerStack::DeleteAll()
	{
		for (Layer* layer : Layers)
		{
			delete layer;
		}
	}
}