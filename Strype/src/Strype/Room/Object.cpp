#include "stypch.h"
#include "Object.h"

#include "Components.h"

#define COPY_COMPONENTS(component) src.m_Room->CopyComponentIfExists<component>(newobj, targetroom->m_Registry, src)

namespace Strype {

	Object Object::Copy(Object src, Ref<Room> targetroom)
	{
		Object newobj = targetroom->CreateObject();

		COPY_COMPONENTS(TagComponent);
		COPY_COMPONENTS(Transform);
		COPY_COMPONENTS(SpriteRenderer);

		return newobj;
	}

}