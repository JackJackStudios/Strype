using Strype;

namespace ExampleProject
{
    public class TestObject : Object
    {
        protected override void OnCreate()
        {

        }

        protected override void OnUpdate(float ts)
        {
            Log.Info("Hello C#");
        }
    }
}
