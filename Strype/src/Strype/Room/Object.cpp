#include "stypch.h"
#include "Object.h"

#include "Components.h"

#define COPY_COMPONENTS(component) Room::CopyComponentIfExists<component>(src.m_Handle, src.m_Room->m_Registry, dest.m_Handle, dest.m_Room->m_Registry)

namespace Strype {

	Object Object::Copy(Object src, Ref<Room> targetroom)
	{
		Object newobj = targetroom->CreateObject();

		CopyInto(src, newobj);

		return newobj;
	}

	void Object::CopyInto(Object src, Object dest)
	{
		COPY_COMPONENTS(SpriteRenderer);
		COPY_COMPONENTS(ScriptComponent);
	}

}