namespace Strype
{

	public enum MouseButton
	{
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Left = Button0,
		Right = Button1,
		Middle = Button2
	}

	public static class Input
	{
		/// <summary>
		/// Returns true the first frame that the key represented by the given KeyCode is pressed down
		/// </summary>
		public static bool IsKeyPressed(KeyCode keycode)
		{
			unsafe { return InternalCalls.Input_IsKeyPressed(keycode); }
		}

		/// <summary>
		/// Returns true every frame after the key was initially pressed (returns false when <see cref="Input.IsKeyPressed(KeyCode)"/> returns true)
		/// </summary>
		public static bool IsKeyHeld(KeyCode keycode)
		{
			unsafe { return InternalCalls.Input_IsKeyHeld(keycode); }
		}

		/// <summary>
		/// Returns true every frame that the key is down. Equivalent to doing <code>Input.IsKeyPressed(key) || Input.IsKeyHeld(key)</code>
		/// </summary>
		public static bool IsKeyDown(KeyCode keycode)
		{
			unsafe { return InternalCalls.Input_IsKeyDown(keycode); }
		}

		/// <summary>
		/// Returns true during the frame that the key was released
		/// </summary>
		public static bool IsKeyReleased(KeyCode keycode)
		{
			unsafe { return InternalCalls.Input_IsKeyReleased(keycode); }
		}

		/// <summary>
		/// Returns true the first frame that the button represented by the given MouseButton is pressed down
		/// </summary>
		public static bool IsMouseButtonPressed(MouseButton button)
		{
			unsafe { return InternalCalls.Input_IsMouseButtonPressed(button); }
		}

		/// <summary>
		/// Returns true every frame after the button was initially pressed (returns false when <see cref="Input.IsMouseButtonPressed(MouseButton)"/> returns true)
		/// </summary>
		public static bool IsMouseButtonHeld(MouseButton button)
		{
			unsafe { return InternalCalls.Input_IsMouseButtonHeld(button); }
		}

		/// <summary>
		/// Returns true every frame that the button is down. Equivalent to doing <code>Input.IsMouseButtonPressed(key) || Input.IsMouseButtonHeld(key)</code>
		/// </summary>
		public static bool IsMouseButtonDown(MouseButton button)
		{
			unsafe { return InternalCalls.Input_IsMouseButtonDown(button); }
		}

		/// <summary>
		/// Returns true during the frame that the button was released
		/// </summary>
		public static bool IsMouseButtonReleased(MouseButton button)
		{
			unsafe { return InternalCalls.Input_IsMouseButtonReleased(button); }
		}

		public static Vector2 GetMousePosition()
		{
			Vector2 position;
			unsafe { InternalCalls.Input_GetMousePosition(&position); }
			return position;
		}
	}

}
