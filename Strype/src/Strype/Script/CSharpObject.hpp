#pragma once

#include <Coral/ManagedObject.hpp>

namespace Strype {

	class CSharpObject
	{
	public:
		template<typename... TArgs>
		void Invoke(std::string_view methodName, TArgs&&... args)
		{
			m_Handle->InvokeMethod(methodName, std::forward<TArgs>(args)...);
		}

		bool IsValid() const { return m_Handle != nullptr; }

		Coral::ManagedObject* GetHandle() const { return m_Handle; }

	private:
		Coral::ManagedObject* m_Handle = nullptr;

		friend class ScriptEngine;
	};

}
