#ifndef CHONPS_LAYER_STACK_H
#define CHONPS_LAYER_STACK_H

#include <vector>

#include "Layer.h"

namespace Chonps
{
	class LayerStack
	{
	public:
		void add(Layer* layer);
		void add_overlay(Layer* layer);
		
		void insert(Layer* layer, unsigned int index);
		void remove(Layer* layer);

		void delete_layer(Layer* layer);
		void delete_all();

		size_t size() { return Layers.size(); }
		std::vector<Layer*>::iterator begin() { return Layers.begin(); }
		std::vector<Layer*>::iterator end() { return Layers.end(); }

		// Use GetLayer function instead to avoid manipulating Layers data
		Layer* get(int index) { return Layers[index]; }

		std::vector<Layer*> Layers;

	private:
		unsigned int m_LayerInsertIndex = 0;
	};
}


#endif