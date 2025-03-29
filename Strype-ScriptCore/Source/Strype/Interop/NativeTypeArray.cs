using System;
using System.Collections.Generic;
using Coral.Managed.Interop;

namespace Strype.Interop
{

	public struct NativeTypeArray
	{
		private NativeArray<ReflectionType> m_Array;

		public static implicit operator NativeTypeArray(Type[] types)
		{
			List<ReflectionType> list = [];
			foreach (var type in types)
			{
				list.Add(type);
			}

			return new() { m_Array = list.ToArray() };
		}
	}

}
