using System;
using System.Collections.Generic;
using System.ComponentModel;
using Coral.Managed.Interop;

namespace Strype
{

    public class Object : IEquatable<Object>
    {
        private Dictionary<Type, Component> m_ComponentCache = new Dictionary<Type, Component>();
        private TransformComponent m_TransformComponent;

        protected Object() { ID = 0; }

        internal Object(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;
        public string Tag => GetComponent<TagComponent>()?.Tag ?? "";
        public TransformComponent Transform
        {
            get
            {
                if (m_TransformComponent == null)
                    m_TransformComponent = GetComponent<TransformComponent>();

                return m_TransformComponent;
            }
        }

        public Vector2 Position
        {
            get => Transform.Position;
            set => Transform.Position = value;
        }
        public Vector2 Scale
        {
            get => Transform.Scale;
            set => Transform.Scale = value;
        }

        public float Rotation
        {
            get => Transform.Rotation;
            set => Transform.Rotation = value;
        }


        protected virtual void OnCreate() { }
        protected virtual void OnUpdate(float ts) { }
        protected virtual void OnLateUpdate(float ts) { }
        protected virtual void OnPhysicsUpdate(float ts) { }
        protected virtual void OnDestroy() { }

        public T? CreateComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
                return GetComponent<T>();

            unsafe { InternalCalls.Object_CreateComponent(ID, typeof(T)); }
            var component = new T { Object = this };
            m_ComponentCache.Add(typeof(T), component);
            return component;
        }

        public bool HasComponent<T>() where T : Component
        {
            unsafe { return InternalCalls.Object_HasComponent(ID, typeof(T)); }
        }

        public bool HasComponent(Type type)
        {
            unsafe { return InternalCalls.Object_HasComponent(ID, type); }
        }

        public T? GetComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);

            if (!HasComponent<T>())
            {
                m_ComponentCache.Remove(componentType);
                return null;
            }

            if (!m_ComponentCache.ContainsKey(componentType))
            {
                var component = new T { Object = this };
                m_ComponentCache.Add(componentType, component);
                return component;
            }

            return m_ComponentCache[componentType] as T;
        }

        public bool RemoveComponent<T>() where T : Component
        {
            Type componentType = typeof(T);
            bool removed;

            unsafe { removed = InternalCalls.Object_RemoveComponent(ID, componentType); }

            if (removed && m_ComponentCache.ContainsKey(componentType))
                m_ComponentCache.Remove(componentType);

            return removed;
        }

        /// <summary>
        /// Checks if this entity is a script entity of type T
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <returns>True if this entity is a script entity of type T</returns>
        public bool Is<T>() where T : Object
        {
            var sc = GetComponent<ScriptComponent>();
            if (sc == null)
                return false;

            if (sc.Instance.Get() == null)
                return false;

            return sc.Instance.Get() is T;
        }

        /// <summary>
        /// Returns the script instance as type T if this entity is of the given type, otherwise null
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        public T? As<T>() where T : Object
        {
            var sc = GetComponent<ScriptComponent>();
            return sc?.Instance.Get() as T;
        }

        // Destroys the calling Object
        public void Destroy() => Room.DestroyObject(this);
        public void Destroy(Object other) => Room.DestroyObject(other);

        private void OnDestroyInternal()
        {
            OnDestroy();
            m_ComponentCache.Clear();
        }

        public override bool Equals(object? obj) => obj is Object other && Equals(other);

        // NOTE(Peter): Implemented according to Microsofts official documentation:
        // https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/statements-expressions-operators/how-to-define-value-equality-for-a-type
        public bool Equals(Object? other)
        {
            if (other is null)
                return false;

            if (ReferenceEquals(this, other))
                return true;

            return ID == other.ID;
        }

        private static bool IsValid(Object? entity)
        {
            if (entity is null)
                return false;

            unsafe { return InternalCalls.Room_IsObjectValid(entity.ID); }
        }

        public override int GetHashCode() => (int)ID;

        public static bool operator ==(Object? entityA, Object? entityB) => entityA is null ? entityB is null : entityA.Equals(entityB);
        public static bool operator !=(Object? entityA, Object? entityB) => !(entityA == entityB);

        public static implicit operator bool(Object entity) => IsValid(entity);

    }
}
