#ifndef HG_CHONPS_SYSTEM_MANAGER_H
#define HG_CHONPS_SYSTEM_MANAGER_H

#include "Entity.h"
#include <set>

namespace Chonps
{
	class System
	{
	public:
		std::set<Entity> Entities;
	};

	class SystemManager
	{
	public:
		template<typename T>
		std::shared_ptr<T> register_system()
		{
			const type_info* systemName = &typeid(T);
			CHONPS_CORE_ASSERT(m_Systems.find(systemName) == m_Systems.end(), "System registered more than once!");

			auto system = std::make_shared<T>();
			m_Systems.insert({ systemName, system });
			return system;
		}

		template<typename T>
		void set_signature(Signature signature)
		{
			const type_info* systemName = &typeid(T);
			CHONPS_CORE_ASSERT(m_Systems.find(systemName) != m_Systems.end(), "No system of that type was found! System may not be registered first");

			// set the signature for this system
			m_Signatures.insert({ systemName, signature });
		}

		void entity_destroyed(Entity entity)
		{
			// erase entity from all systems that include it
			for (auto const& pair : m_Systems)
			{
				auto const& system = pair.second;
				system->Entities.erase(entity);
			}
		}

		void signature_changed(Entity entity, Signature entitySignature)
		{
			// update the entity's signiture in each system
			for (auto const& pair : m_Systems)
			{
				auto const& type = pair.first;
				auto const& system = pair.second;
				auto const& systemSignature = m_Signatures[type];

				// entity signature matches system signature
				if ((entitySignature & systemSignature) == systemSignature)
					system->Entities.insert(entity);
				else // entity signature does not match system signature
					system->Entities.erase(entity);
			}
		}

	private:
		std::unordered_map<const type_info*, Signature> m_Signatures{};
		std::unordered_map<const type_info*, std::shared_ptr<System>> m_Systems{};
	};
}

#endif