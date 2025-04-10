using Coral.Managed.Interop;
using System;

namespace Strype
{
	internal static unsafe class InternalCalls
	{
        internal static delegate* unmanaged<ulong, ReflectionType, void> Object_CreateComponent;
        internal static delegate* unmanaged<ulong, ReflectionType, bool> Object_HasComponent;
        internal static delegate* unmanaged<ulong, ReflectionType, bool> Object_RemoveComponent;

        internal static delegate* unmanaged<ulong, bool> Room_ObjectExists;
        internal static delegate* unmanaged<NativeString, ulong> Room_CreateObject;
        internal static delegate* unmanaged<ulong, void> Room_DestroyObject;

        internal static delegate* unmanaged<ulong, Vector2*, void> Transform_GetPosition;
        internal static delegate* unmanaged<ulong, Vector2*, void> Transform_SetPosition;
        internal static delegate* unmanaged<ulong, float*, void> Transform_GetRotation;
        internal static delegate* unmanaged<ulong, float*, void> Transform_SetRotation;
        internal static delegate* unmanaged<ulong, Vector2*, void> Transform_GetScale;
        internal static delegate* unmanaged<ulong, Vector2*, void> Transform_SetScale;

		internal static delegate* unmanaged<KeyCode, Bool32> Input_IsKeyPressed;
		internal static delegate* unmanaged<KeyCode, Bool32> Input_IsKeyHeld;
		internal static delegate* unmanaged<KeyCode, Bool32> Input_IsKeyDown;
		internal static delegate* unmanaged<KeyCode, Bool32> Input_IsKeyReleased;
		internal static delegate* unmanaged<MouseButton, bool> Input_IsMouseButtonPressed;
		internal static delegate* unmanaged<MouseButton, bool> Input_IsMouseButtonHeld;
		internal static delegate* unmanaged<MouseButton, bool> Input_IsMouseButtonDown;
		internal static delegate* unmanaged<MouseButton, bool> Input_IsMouseButtonReleased;
		internal static delegate* unmanaged<Vector2*, void> Input_GetMousePosition;

        internal static delegate* unmanaged<Log.LogLevel, NativeString, void> Log_LogMessage;
    }
}
