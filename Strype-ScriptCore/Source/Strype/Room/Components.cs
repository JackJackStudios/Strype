using System;
using Coral.Managed.Interop;

namespace Strype
{
    public abstract class Component
    {
        public Object Object { get; internal set; }
    }

    public class Transform : Component
    {
        public Vector2 Position
        {
            get
            {
                Vector2 result;
                unsafe { InternalCalls.Transform_GetPosition(Object.ID, &result); }
                return result;
            }

            set
            {
                unsafe { InternalCalls.Transform_SetPosition(Object.ID, &value); }
            }
        }

        public float Rotation
        {
            get
            {
                float result;
                unsafe { InternalCalls.Transform_GetRotation(Object.ID, &result); }
                return result;
            }

            set
            {
                unsafe { InternalCalls.Transform_SetRotation(Object.ID, &value); }
            }
        }

        public Vector2 Scale
        {
            get
            {
                Vector2 result;
                unsafe { InternalCalls.Transform_GetScale(Object.ID, &result); }
                return result;
            }

            set
            {
                unsafe { InternalCalls.Transform_SetScale(Object.ID, &value); }
            }
        }
    }

}
