namespace Strype
{
	public static class Application
	{
		public static uint Width
		{
			get
			{
				unsafe { return InternalCalls.Application_GetWidth(); }
			}
		}

		public static uint Height
		{
			get
			{
				unsafe { return InternalCalls.Application_GetHeight(); }
			}
		}

		public static float AspectRatio => Width / (float)Height;

		public static void Quit()
 		{
 			unsafe { InternalCalls.Application_Quit(); }
 		}

	}

}
