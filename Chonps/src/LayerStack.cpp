#include "cepch.h"
#include "LayerStack.h"

namespace Chonps
{
	void LayerStack::add(Layer* layer)
	{
		Layers.emplace(Layers.begin() + m_LayerInsertIndex, layer);
		layer->OnAttach();
	}

	void LayerStack::add_overlay(Layer* layer)
	{
		Layers.emplace_back(layer);
		layer->OnAttach();
		m_LayerInsertIndex++;
	}

	void LayerStack::insert(Layer* layer, unsigned int index)
	{
		if (index > m_LayerInsertIndex)
		{
			CHONPS_CORE_WARN("Cannot add layers into overlay layers: (index: {0} must be less than {1})", index, m_LayerInsertIndex);
			return;
		}

		Layers.emplace(Layers.begin() + index, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::remove(Layer* layer)
	{
		auto it = std::find(Layers.begin(), Layers.end(), layer);
		if (it != Layers.end())
		{
			layer->OnDetach();
			Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::delete_layer(Layer* layer)
	{
		auto it = std::find(Layers.begin(), Layers.end(), layer);
		if (it != Layers.end())
		{
			Layers.erase(it);
			layer->~Layer();
			delete layer;
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::delete_all()
	{
		for (Layer* layer : Layers)
		{
			delete layer;
		}
	}
}