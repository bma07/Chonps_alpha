#ifndef LAYER_STACK_H
#define LAYER_STACK_H

#include <vector>

#include "Layer.h"

namespace Chonps
{
	class LayerStack
	{
	public:
		void AddLayer(Layer* layer);
		void InsertLayer(Layer* layer, unsigned int index);
		void RemoveLayer(Layer* layer);
		void DeleteLayer(Layer* layer);
		void DeleteAll();

		size_t size() { return Layers.size(); }

		// Use GetLayer function instead to avoid manipulating Layers data
		Layer* GetLayer(int index) { return Layers[index]; }

		std::vector<Layer*> Layers;
	};
}


#endif