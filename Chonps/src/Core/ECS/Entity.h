#ifndef CHONPS_ENTITY_H
#define CHONPS_ENTITY_H

#include "Core/Log.h"
#include "Core/Config.h"
#include <bitset>
#include <queue>
#include <unordered_map>
#include <any>

#define CHONPS_MAX_ENTITIES Chonps::s_MaxEntities

namespace Chonps
{
	using Entity = std::uint32_t;

	static size_t s_MaxEntities = 5000;
	static bool s_EnableEntityLimit = false;

	static void ecsChangeMaxEntityCount(size_t count) { s_MaxEntities = count; }
	static void ecsEnableEntityLimit(bool enable) { s_EnableEntityLimit = false; }

	using ComponentType = std::uint16_t;
	using ComponentTypeInfo = const std::type_info*;

	using Signature = std::bitset<64>; // Signature of components so that entities can be divided into groups with the same signitures

	static size_t s_MaxComponents = 1024;
	static bool s_EnableComponentLimit = false;

	// Change the limit of components that can be created
	static void ecsChangeMaxComponentCount(size_t count) { s_MaxComponents = count; }
	// Enables or disables the max limit for components
	static void ecsEnableComponentLimit(bool enable) { s_EnableComponentLimit = enable; }

	class EntityManager
	{
	public:
		EntityManager()
		{
			for (int i = 1; i < s_MaxEntities; i++)
			{
				m_AvailableEntities.push(i);
			}
		}

		Entity create()
		{
			if (s_EnableEntityLimit) CHONPS_CORE_ASSERT((m_ExistingEntities < s_MaxEntities), "Entity limit reached! Too many entities created");

			if ((m_AvailableEntities.size() == 1) && !s_EnableEntityLimit)
				m_AvailableEntities.push(++m_AvailableEntities.front());

			Entity id = m_AvailableEntities.front();
			m_AvailableEntities.pop();
			m_ExistingEntities += 1;

			return id;
		}

		void destroy(Entity entity)
		{
			if (s_EnableEntityLimit) CHONPS_CORE_ASSERT((entity < s_MaxEntities), "Entity out of bound!");

			m_EntitySignatures[entity].reset();

			m_AvailableEntities.push(entity);
			m_ExistingEntities -= 1;
		}

		void set_signature(Entity entity, Signature signature)
		{
			if (s_EnableEntityLimit) CHONPS_CORE_ASSERT((entity < s_MaxEntities), "Entity out of bound!");
			m_EntitySignatures[entity] = signature;
		}

		Signature* get_signature(Entity entity)
		{
			//CHONPS_CORE_ASSERT(m_EntitySignatures.find(entity) != m_EntitySignatures.end(), "Entity sigature not found!");
			return &m_EntitySignatures[entity];
		}

	private:
		std::queue<Entity> m_AvailableEntities;
		uint32_t m_ExistingEntities;
		std::unordered_map<Entity, Signature> m_EntitySignatures;
	};
}

#endif