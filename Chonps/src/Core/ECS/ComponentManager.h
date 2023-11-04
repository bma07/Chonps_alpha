#ifndef HG_CHONPS_COMPONENT_MANAGER_H
#define HG_CHONPS_COMPONENT_MANAGER_H

#include "Entity.h"

#define CHONPS_MAX_COMPONENTS Chonps::s_MaxComponents

namespace Chonps
{
	struct BaseComponentArray
	{
		virtual ~BaseComponentArray() = default;
		virtual void entity_destroyed(Entity entity) = 0;
		virtual void remove(Entity entity) = 0;
		virtual bool has(Entity entity) const = 0;
		virtual const char* name() const = 0;
		virtual const std::type_info* type() const = 0;
	};

	template <typename T>
	struct ComponentArray : public BaseComponentArray
	{
		ComponentArray() { components.reserve(s_MaxComponents); } // Space will be allocated before filled in

		std::vector<T> components;

		void add(Entity entity, T comp) // Add a component to the entity ID
		{
			if (s_EnableComponentLimit) CHONPS_CORE_ASSERT(components.size() <= s_MaxComponents, "Component limit reached!");
			CHONPS_CORE_ASSERT(m_EntityIndex.find(entity) == m_EntityIndex.end(), "Entity alrady has component!");

			components.push_back(comp);
			m_EntityIndex[entity] = components.size() - 1;
			m_IndexOfEntity[components.size() - 1] = entity;
		}

		virtual void remove(Entity entity) override // remove the component from the entity ID
		{
			CHONPS_CORE_ASSERT(m_EntityIndex.find(entity) != m_EntityIndex.end(), "No entity was found with component!");
			if (!components.empty())
			{
				size_t index = m_EntityIndex[entity]; // Get index of the entity to be deleted
				Entity lastEntity = m_IndexOfEntity[components.size() - 1]; // Get last entity from array
				
				m_EntityIndex[lastEntity] = index; // Update last entity's index to index of deleted entity
				m_IndexOfEntity[index] = lastEntity; // Update index to last entity

				m_EntityIndex.erase(entity);
				m_IndexOfEntity.erase(components.size() - 1);

				std::swap(components[index], components[components.size() - 1]);
				components.pop_back();
			}
		}

		T& get(Entity entity) // get the component from the entity ID
		{
			CHONPS_CORE_ASSERT(m_EntityIndex.find(entity) != m_EntityIndex.end(), "No entity was found with component!");
			size_t index = m_EntityIndex[entity];
			return components[index];
		}

		virtual bool has(Entity entity) const override
		{
			bool entityHasComp;
			m_EntityIndex.find(entity) != m_EntityIndex.end() ? entityHasComp = true : entityHasComp = false;
			return entityHasComp;
		}

		virtual void entity_destroyed(Entity entity) override
		{
			if (m_EntityIndex.find(entity) != m_EntityIndex.end())
				remove(entity);
		}

		virtual const char* name() const override
		{
			return typeid(T).name();
		}

		virtual const std::type_info* type() const override
		{
			return &typeid(T);
		}

	private:
		std::unordered_map<Entity, size_t> m_EntityIndex;
		std::unordered_map<size_t, Entity> m_IndexOfEntity;
	};

	class ComponentManager
	{
	public:

		template <typename T>
		void register_component() // to add a new non-exsisting component, a component must be registered first
		{
			const std::type_info* compType = &typeid(T);
			CHONPS_CORE_ASSERT(m_ComponentTypes.find(compType) == m_ComponentTypes.end(), "Component type already exists within registy!");

			m_ComponentTypes.insert({ compType, m_ComponentTypesAmount });
			m_ComponentArrays.insert({ compType, std::make_shared<ComponentArray<T>>() });
			m_ComponentTypesAmount++;
		}

		template<typename T>
		void add(Entity entity, T component) // add a component to an entity, the component type will be stored in an array with the index of the entity
		{
			const std::type_info* compType = &typeid(T);

			if (m_ComponentTypes.find(compType) == m_ComponentTypes.end())
				register_component<T>();

			GetComponentArray<T>()->add(entity, component);
		}

		template<typename T>
		void remove(Entity entity) // remove a component type from an entity
		{
			const std::type_info* compType = &typeid(T);
			CHONPS_CORE_ASSERT(m_ComponentTypes.find(compType) != m_ComponentTypes.end(), "Component type not found! Componant may not be registered");

			GetComponentArray<T>()->remove(entity);
		}

		void remove(Entity entity, const std::type_info* compType) // remove a component type with std::type_info from an entity
		{
			CHONPS_CORE_ASSERT(m_ComponentTypes.find(compType) != m_ComponentTypes.end(), "Component type not found! Componant may not be registered");

			m_ComponentArrays[compType]->remove(entity);
		}

		template<typename T>
		T& get(Entity entity) // get the component from an entity if the component exists
		{
			return GetComponentArray<T>()->get(entity);
		}

		std::unordered_map<const std::type_info*, std::shared_ptr<BaseComponentArray>> get_component_arrays()
		{
			return m_ComponentArrays;
		}

		template<typename T>
		bool has(Entity entity) // returns true if the entity has the component type. Note: components entered need to be registered first
		{
			const std::type_info* compType = &typeid(T);
			if (m_ComponentTypes.find(compType) != m_ComponentTypes.end())
				return m_ComponentArrays[compType]->has(entity);
			return false;
		}

		bool has(Entity entity, const std::type_info* component) // returns true if the entity has the component type. Note: components entered need to be registered first
		{
			if (m_ComponentTypes.find(component) != m_ComponentTypes.end())
				return m_ComponentArrays[component]->has(entity);
			return false;
		}

		template<typename T>
		ComponentType type() // find the component type from an entity if the component exists
		{
			const std::type_info* compType = &typeid(T);
			CHONPS_CORE_ASSERT(m_ComponentTypes.find(compType) != m_ComponentTypes.end(), "Component type not found! Componant may not be registered");

			return m_ComponentTypes[compType];
		}

		ComponentType type(const std::type_info* compType) // find the component type with std::type_info from an entity if the component exists
		{
			CHONPS_CORE_ASSERT(m_ComponentTypes.find(compType) != m_ComponentTypes.end(), "Component type not found! Componant may not be registered");
			return m_ComponentTypes[compType];
		}

		void entity_destroyed(Entity entity) // remove the components of the entity upon deletion
		{
			for (auto const& pair : m_ComponentArrays)
			{
				auto const& component = pair.second;
				component->entity_destroyed(entity);
			}
		}

	private:
		std::unordered_map<const std::type_info*, ComponentType> m_ComponentTypes;
		std::unordered_map<const std::type_info*, std::shared_ptr<BaseComponentArray>> m_ComponentArrays;

		ComponentType m_ComponentTypesAmount = 0;

		template<typename T>
		std::shared_ptr<ComponentArray<T>> GetComponentArray() // Retrieves the component array which stores the specific components types for each entity
		{
			const std::type_info* compType = &typeid(T);

			CHONPS_CORE_ASSERT(m_ComponentTypes.find(compType) != m_ComponentTypes.end(), "Component type not found! Componant may not be registered first");

			return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[compType]);
		}
	};
}

#endif