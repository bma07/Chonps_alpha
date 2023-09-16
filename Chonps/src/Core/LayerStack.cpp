#include "cepch.h"
#include "LayerStack.h"

namespace Chonps
{
	void LayerStack::push(Layer* layer)
	{
		layers.emplace(layers.begin() + m_LayerInsertIndex, layer);
		layer->OnAttach();
	}

	void LayerStack::push_back(Layer* layer)
	{
		layers.emplace_back(layer);
		layer->OnAttach();
		m_LayerInsertIndex++;
	}

	void LayerStack::pop()
	{
		if (!layers.empty())
		{
			layers.front()->OnDetach();
			layers.erase(layers.begin());
		}
	}

	void LayerStack::pop_back()
	{
		if (!layers.empty())
		{
			layers.back()->OnDetach();
			layers.erase(layers.end());
		}
	}

	void LayerStack::insert(Layer* layer, unsigned int index)
	{
		if (index > m_LayerInsertIndex)
		{
			CHONPS_CORE_LOG_WARN(Layer, "Cannot add layers into overlay layers: (index: {0} must be less than {1})", index, m_LayerInsertIndex);
			return;
		}

		layers.emplace(layers.begin() + index, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::remove(Layer* layer)
	{
		auto it = std::find(layers.begin(), layers.end(), layer);
		if (it != layers.end())
		{
			layer->OnDetach();
			layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::destroy(Layer* layer)
	{
		auto it = std::find(layers.begin(), layers.end(), layer);
		if (it != layers.end())
		{
			layer->OnDetach();
			layers.erase(it);
			layer->~Layer();
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::clear()
	{
		for (Layer* layer : layers)
			layer->OnDetach();

		layers.clear();
	}
}