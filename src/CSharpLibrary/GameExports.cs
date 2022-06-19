
using System.Runtime.InteropServices;

/*
 * This part exposes some of the C# game API to the native engine.
 * Currently, only ReceiveEvent is exported, so that the C++ game can let
 * the C# game "know" about ongoing events.
 */

namespace Library
{
	public static partial class EngineInterface
	{
		public delegate void IReceiveEvent(EntityEvent ev);
		
		// The equivalent thing on the C++ side can be found at:
		// struct GameImports @ GameCallbacks.hpp
		[StructLayout(LayoutKind.Sequential)]
		public struct GameExports
		{
			public int GameVersion;
			public IReceiveEvent ReceiveEvent;
		}

		public static void ReceiveEvent( EntityEvent ev )
		{
			foreach ( var entity in gameEntities )
			{
				entity.ReceiveEvent(ev);
			}
		}
	}
}
