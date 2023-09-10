#ifndef HG_CHONPS_REGISTRY_H
#define HG_CHONPS_REGISTRY_H

#include "Entity.h"
#include "ComponentManager.h"
#include "SystemManager.h"

namespace Chonps
{
	class Registry
	{
	public:
		Registry()
		{
			m_EntityManager = std::make_unique<EntityManager>();
			m_ComponentManager = std::make_unique<ComponentManager>();
			m_SystemManager = std::make_unique<SystemManager>();
		}

		Entity create_entity()
		{
			return m_EntityManager->create();
		}

		void destroy_entity(Entity entity)
		{
			m_EntityManager->destroy(entity);
			m_ComponentManager->entity_destroyed(entity);
			m_SystemManager->entity_destroyed(entity);
		}

		// Components
		template<typename T>
		void register_component()
		{
			m_ComponentManager->register_component<T>();
		}

		template<typename T>
		void add_component(Entity entity, T component)
		{
			m_ComponentManager->add<T>(entity, component);

			auto signature = m_EntityManager->get_signature(entity);
			signature->set(m_ComponentManager->type<T>(), true);

			m_SystemManager->signature_changed(entity, *signature);
		}

		template<typename T>
		void remove_component(Entity entity)
		{
			m_ComponentManager->remove<T>(entity);

			auto signature = m_EntityManager->get_signature(entity);
			signature->set(m_ComponentManager->type<T>(), false);

			m_SystemManager->signature_changed(entity, *signature);
		}

		template<typename T>
		void remove_component(Entity entity, T component)
		{
			m_ComponentManager->remove<T>(entity);

			auto signature = m_EntityManager->get_signature(entity);
			signature->set(m_ComponentManager->type<T>(), false);

			m_SystemManager->signature_changed(entity, *signature);
		}

		void remove_component(Entity entity, const type_info* compType)
		{
			m_ComponentManager->remove(entity, compType);

			auto signature = m_EntityManager->get_signature(entity);
			signature->set(m_ComponentManager->type(compType), false);

			m_SystemManager->signature_changed(entity, *signature);
		}

		template<typename T>
		T& get_component(Entity entity)
		{
			return m_ComponentManager->get<T>(entity);
		}

		std::vector<std::shared_ptr<BaseComponentArray>> get_all_components()
		{
			std::vector<std::shared_ptr<BaseComponentArray>> components;
			std::unordered_map<const type_info*, std::shared_ptr<BaseComponentArray>> componentArrays = m_ComponentManager->get_component_arrays();
			for (auto& component : componentArrays)
				components.emplace_back(component.second);
			return components;
		}

		template<typename T>
		ComponentType get_component_type()
		{
			return m_ComponentManager->type<T>();
		}

		template<typename T>
		bool has_component(Entity entity)
		{
			return m_ComponentManager->has<T>(entity);
		}

		bool has_component(Entity entity, const type_info* component)
		{
			return m_ComponentManager->has(entity, component);
		}

		// System
		template<typename T>
		std::shared_ptr<T> register_system()
		{
			return m_SystemManager->register_system<T>();
		}

		template<typename T>
		void set_signature(Signature signature)
		{
			m_SystemManager->set_signature<T>(signature);
		}

	private:
		std::unique_ptr<EntityManager> m_EntityManager;
		std::unique_ptr<ComponentManager> m_ComponentManager;
		std::unique_ptr<SystemManager> m_SystemManager;
	};
}

#endif