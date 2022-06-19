
using System;

namespace Library
{
	public static class MainClass
	{
		// The native host will call this Entry method first
		// Look at GameSystem.ImportFunctions
		public static int Entry( IntPtr args, int argLength )
		{
			Console.WriteLine( "Library.MainClass: HELLO THERE" );

			return 0;
		}

		// Just in case some silly compiles this as a console application
		public static void Main()
		{
			Console.WriteLine( "You shouldn't be here :)" );
		}
	}
}
