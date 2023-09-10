#ifndef HG_CHONPS_LAYER_STACK_H
#define HG_CHONPS_LAYER_STACK_H

#include <vector>

#include "Layer.h"

namespace Chonps
{
	class LayerStack
	{
	public:
		void push(Layer* layer);
		void push_back(Layer* layer);
		
		void pop();
		void pop_back();

		void insert(Layer* layer, unsigned int index);
		
		// Revomes the layer from the stack but does not delete it
		void remove(Layer* layer);

		// Removes the layer from the stack and also deletes it
		void destroy(Layer* layer);
		
		// Clears layer stack from layers
		void clear();

		size_t size() { return layers.size(); }
		std::vector<Layer*>::iterator begin() { return layers.begin(); }
		std::vector<Layer*>::iterator end() { return layers.end(); }

		// Use GetLayer function instead to avoid manipulating layers data
		Layer* Get(int index) { return layers[index]; }

		std::vector<Layer*> layers;

	private:
		unsigned int m_LayerInsertIndex = 0;
	};
}


#endif