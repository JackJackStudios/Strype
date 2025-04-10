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
                // Remove this entity from the cache if it's been indexed either with the ID or Tag hash code
                s_ObjectCache.Remove(entity.ID);

                InternalCalls.Room_DestroyObject(entity.ID);
            }
        }

        private static Dictionary<ulong, Object?> s_ObjectCache = new Dictionary<ulong, Object?>(50);

        public static Object? FindObjectByID(ulong entityID)
        {
            unsafe
            {
                if (s_ObjectCache.ContainsKey(entityID) && s_ObjectCache[entityID] != null)
                {
                    var entity = s_ObjectCache[entityID];

                    if (!InternalCalls.Room_ObjectExists(entity!.ID))
                    {
                        s_ObjectCache.Remove(entityID);
                        entity = null;
                    }

                    return entity;
                }

                if (!InternalCalls.Room_ObjectExists(entityID))
                    return null;
            }

            var newObject = new Object(entityID);
            s_ObjectCache[entityID] = newObject;
            return newObject;
        }

    }
}
