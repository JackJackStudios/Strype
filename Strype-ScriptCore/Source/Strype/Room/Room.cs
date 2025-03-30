using System;
using System.Collections.Generic;
using System.Linq;

namespace Strype
{
    public sealed class Room : Asset<Room>
    {
        public static Object CreateObject(string tag = "Unnamed")
        {
            unsafe { return new Object(InternalCalls.Room_CreateObject(tag)); }
        }

        public static void DestroyObject(Object entity)
        {
            if (entity == null)
                return;

            unsafe
            {
                if (!InternalCalls.Room_IsObjectValid(entity.ID))
                    return;

                // Remove this entity from the cache if it's been indexed either with the ID or Tag hash code
                if (!s_ObjectCache.Remove(entity.ID))
                    s_ObjectCache.Remove((ulong)entity.Tag.GetHashCode());

                InternalCalls.Room_DestroyObject(entity.ID);
            }
        }

        private static Dictionary<ulong, Object?> s_ObjectCache = new Dictionary<ulong, Object?>(50);

        public static Object? FindObjectByTag(string tag)
        {
            ulong hashCode = (ulong)tag.GetHashCode();

            Object? result = null;

            unsafe
            {
                if (s_ObjectCache.ContainsKey(hashCode) && s_ObjectCache[hashCode] != null)
                {
                    result = s_ObjectCache[hashCode];

                    if (!InternalCalls.Room_IsObjectValid(result!.ID))
                    {
                        s_ObjectCache.Remove(hashCode);
                        result = null;
                    }
                }

                if (result == null)
                {
                    ulong entityID = InternalCalls.Room_FindObjectByTag(tag);
                    var newObject = entityID != 0 ? new Object(entityID) : null;
                    s_ObjectCache[hashCode] = newObject;

                    result = newObject;
                }
            }

            return result;
        }

        public static Object? FindObjectByID(ulong entityID)
        {
            unsafe
            {
                if (s_ObjectCache.ContainsKey(entityID) && s_ObjectCache[entityID] != null)
                {
                    var entity = s_ObjectCache[entityID];

                    if (!InternalCalls.Room_IsObjectValid(entity!.ID))
                    {
                        s_ObjectCache.Remove(entityID);
                        entity = null;
                    }

                    return entity;
                }

                if (!InternalCalls.Room_IsObjectValid(entityID))
                    return null;
            }

            var newObject = new Object(entityID);
            s_ObjectCache[entityID] = newObject;
            return newObject;
        }

        public static Object[] GetEntities()
        {
            Object[] entities;

            unsafe
            {
                using var entityIDs = InternalCalls.Room_GetObjects();
                entities = new Object[entityIDs.Length];
                for (int i = 0; i < entityIDs.Length; i++)
                {
                    entities[i] = new Object(entityIDs[i]);
                }
            }

            return entities;
        }

        private static void OnObjectDestroyed(Object entity)
        {
            // Remove this entity from the cache if it's been indexed either with the ID or Tag hash code
            if (!s_ObjectCache.Remove(entity.ID))
                s_ObjectCache.Remove((ulong)entity.Tag.GetHashCode());
        }

    }
}
