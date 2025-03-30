using Coral.Managed.Interop;
using System;

namespace Strype
{
	internal static unsafe class InternalCalls
	{
#pragma warning disable CS0649 // Variable is never assigned to

		#region AssetHandle

		internal static delegate* unmanaged<AssetHandle, bool> AssetHandle_IsValid;

		#endregion

		#region Application

		internal static delegate* unmanaged<void> Application_Quit;
		internal static delegate* unmanaged<uint> Application_GetWidth;
		internal static delegate* unmanaged<uint> Application_GetHeight;

		#endregion

		#region Transform

		internal static delegate* unmanaged<Transform*, Transform*, Transform*, void> TransformMultiply_Native;

        #endregion

        #region Object

        internal static delegate* unmanaged<ulong, ReflectionType, void> Object_CreateComponent;
        internal static delegate* unmanaged<ulong, ReflectionType, bool> Object_HasComponent;
        internal static delegate* unmanaged<ulong, ReflectionType, bool> Object_RemoveComponent;

        #endregion

        #region Room

        internal static delegate* unmanaged<NativeString, ulong> Room_FindObjectByTag;
        internal static delegate* unmanaged<ulong, bool> Room_IsObjectValid;
        internal static delegate* unmanaged<NativeString, ulong> Room_CreateObject;
        internal static delegate* unmanaged<ulong, void> Room_DestroyObject;

        internal static delegate* unmanaged<NativeArray<ulong>> Room_GetObjects;

        #endregion

        #region Entity

        internal static delegate* unmanaged<ulong, ReflectionType, void> Entity_CreateComponent;
        internal static delegate* unmanaged<ulong, ReflectionType, bool> Entity_HasComponent;
        internal static delegate* unmanaged<ulong, ReflectionType, bool> Entity_RemoveComponent;

        #endregion

        #region TagComponent

        internal static delegate* unmanaged<ulong, NativeString> TagComponent_GetTag;
        internal static delegate* unmanaged<ulong, NativeString, void> TagComponent_SetTag;

        #endregion

        #region TransformComponent

        internal static delegate* unmanaged<ulong, Vector2*, void> TransformComponent_GetPosition;
        internal static delegate* unmanaged<ulong, Vector2*, void> TransformComponent_SetPosition;
        internal static delegate* unmanaged<ulong, float*, void> TransformComponent_GetRotation;
        internal static delegate* unmanaged<ulong, float*, void> TransformComponent_SetRotation;
        internal static delegate* unmanaged<ulong, Vector2*, void> TransformComponent_GetScale;
        internal static delegate* unmanaged<ulong, Vector2*, void> TransformComponent_SetScale;

        #endregion

        #region ScriptComponent

        internal static delegate* unmanaged<ulong, NativeInstance<object>> ScriptComponent_GetInstance;

        #endregion

        #region Noise

        internal static delegate* unmanaged<int, IntPtr> Noise_Constructor;
		internal static delegate* unmanaged<IntPtr, void> Noise_Destructor;
		internal static delegate* unmanaged<IntPtr, float> Noise_GetFrequency;
		internal static delegate* unmanaged<IntPtr, float, void> Noise_SetFrequency;
		internal static delegate* unmanaged<IntPtr, int> Noise_GetFractalOctaves;
		internal static delegate* unmanaged<IntPtr, int, void> Noise_SetFractalOctaves;
		internal static delegate* unmanaged<IntPtr, float> Noise_GetFractalLacunarity;
		internal static delegate* unmanaged<IntPtr, float, void> Noise_SetFractalLacunarity;
		internal static delegate* unmanaged<IntPtr, float> Noise_GetFractalGain;
		internal static delegate* unmanaged<IntPtr, float, void> Noise_SetFractalGain;
		internal static delegate* unmanaged<IntPtr, float, float, float> Noise_Get;

		internal static delegate* unmanaged<int, void> Noise_SetSeed;
		internal static delegate* unmanaged<float, float, float> Noise_Perlin;

		#endregion

		#region Matrix4

		internal static delegate* unmanaged<Vector3*, Vector3*, Vector3*, Matrix4*, void> Matrix4_LookAt;

		#endregion

		#region Log

		internal static delegate* unmanaged<Log.LogLevel, NativeString, void> Log_LogMessage;

		#endregion

		#region Input

		internal static delegate* unmanaged<KeyCode, Bool32> Input_IsKeyPressed;
		internal static delegate* unmanaged<KeyCode, Bool32> Input_IsKeyHeld;
		internal static delegate* unmanaged<KeyCode, Bool32> Input_IsKeyDown;
		internal static delegate* unmanaged<KeyCode, Bool32> Input_IsKeyReleased;
		internal static delegate* unmanaged<MouseButton, bool> Input_IsMouseButtonPressed;
		internal static delegate* unmanaged<MouseButton, bool> Input_IsMouseButtonHeld;
		internal static delegate* unmanaged<MouseButton, bool> Input_IsMouseButtonDown;
		internal static delegate* unmanaged<MouseButton, bool> Input_IsMouseButtonReleased;
		internal static delegate* unmanaged<Vector2*, void> Input_GetMousePosition;

		#endregion

#pragma warning restore CS0649
	}
}
