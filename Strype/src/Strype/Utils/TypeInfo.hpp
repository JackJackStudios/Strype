#pragma once

#include <type_traits>
#include <string>

#include "Strype/Core/Hash.hpp"

namespace Strype {

	using TypeNameString = std::string_view;

	template<typename T, bool ExcludeNamespace>
	struct TypeInfoBase
	{
	protected:
		TypeNameString DemangleTypeName(std::string_view typeName) const
		{
			size_t spacePos = typeName.find(' ');
			if (spacePos != std::string_view::npos)
				typeName.remove_prefix(spacePos + 1);

			if constexpr (ExcludeNamespace)
			{
				size_t namespacePos = typeName.find("::");
				if (namespacePos != std::string_view::npos)
					typeName.remove_prefix(namespacePos + 2);
			}

			return typeName;
		}
	};

	template<typename T, bool ExcludeNamespace = false>
	struct TypeInfo : TypeInfoBase<T, ExcludeNamespace>
	{
	public:
		using Base = TypeInfoBase<T, ExcludeNamespace>;

	public:
		TypeInfo()
			: m_DemangledName(Base::DemangleTypeName(typeid(T).name()))
		{
		}

		TypeNameString Name() { return m_DemangledName; }
		const TypeNameString& Name() const { return m_DemangledName; }
		uint32_t HashCode() const { return Hash::GenerateFNVHash(m_DemangledName.data()); }

	private:
		TypeNameString m_DemangledName;
	};
}
