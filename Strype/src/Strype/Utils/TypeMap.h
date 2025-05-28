#pragma once

#include "TypeInfo.h"

#include <any>

namespace Strype {

	template<typename TValue>
	class TypeMap 
	{
	public:
		TypeMap() = default;

		template<typename T>
		TValue Set(TValue value)
		{
			m_Map[TypeInfo<T, false>().HashCode()] = value;
			return value;
		}

		template<typename T>
		bool Has() const 
		{
			return m_Map.find(TypeInfo<T, false>().HashCode()) != m_Map.end();
		}

		template<typename T>
		TValue& Get()
		{
			auto it = m_Map.find(TypeInfo<T, false>().HashCode());
			return it->second;
		}

		template<typename T>
		const TValue& Get() const
		{
			auto it = m_Map.find(TypeInfo<T, false>().HashCode());
			return it->second;
		}

		template<typename T>
		void Erase() 
		{
			m_Map.erase(TypeInfo<T, false>().HashCode());
		}

		void Clear() 
		{
			m_Map.clear();
		}

		std::unordered_map<uint32_t, TValue>::const_iterator begin() const { return m_Map.begin(); }
		std::unordered_map<uint32_t, TValue>::const_iterator end()	const { return m_Map.end(); }
		std::unordered_map<uint32_t, TValue>::iterator begin() { return m_Map.begin(); }
		std::unordered_map<uint32_t, TValue>::iterator end() { return m_Map.end(); }
	private:
		std::unordered_map<uint32_t, TValue> m_Map;
	};

}