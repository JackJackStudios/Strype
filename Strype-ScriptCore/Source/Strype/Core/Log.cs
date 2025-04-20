namespace Strype
{
	public static class Log
	{
		internal enum LogLevel
		{
			Trace = 1 << 0,
			Info = 1 << 2,
			Warn = 1 << 3,
			Error = 1 << 4,
		}

		public static void Trace(string format, params object[] parameters)
		{
			unsafe { InternalCalls.Log_LogMessage(LogLevel.Trace, FormatUtils.Format(format, parameters)); }
		}

		public static void Info(string format, params object[] parameters)
		{
			unsafe { InternalCalls.Log_LogMessage(LogLevel.Info, FormatUtils.Format(format, parameters)); }
		}

		public static void Warn(string format, params object[] parameters)
		{
			unsafe { InternalCalls.Log_LogMessage(LogLevel.Warn, FormatUtils.Format(format, parameters)); }
		}

		public static void Error(string format, params object[] parameters)
		{
			unsafe { InternalCalls.Log_LogMessage(LogLevel.Error, FormatUtils.Format(format, parameters)); }
		}

		public static void Trace(object value)
		{
			unsafe { InternalCalls.Log_LogMessage(LogLevel.Trace, FormatUtils.Format(value)); }
		}

		public static void Info(object value)
		{
			unsafe { InternalCalls.Log_LogMessage(LogLevel.Info, FormatUtils.Format(value)); }
		}

		public static void Warn(object value)
		{
			unsafe { InternalCalls.Log_LogMessage(LogLevel.Warn, FormatUtils.Format(value)); }
		}

		public static void Error(object value)
		{
			unsafe { InternalCalls.Log_LogMessage(LogLevel.Error, FormatUtils.Format(value)); }
		}

	}
}
