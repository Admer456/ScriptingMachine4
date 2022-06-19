
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Library
{
	public static partial class EngineInterface
	{
		// Function pointer types
		public delegate uint ICreateEntity( int x, int y );
		public delegate void IRemoveEntity( uint entityIndex );
		public delegate void ISetRenderColor( byte red, byte green, byte blue );
		public delegate void IDrawLine( int x1, int y1, int x2, int y2 );
		public delegate void ISendEvent( EntityEvent ev );

		// Callbacks to the game
		// The equivalent can be found on the C++ side:
		// struct GameExports @ GameCallbacks.hpp
		[StructLayout(LayoutKind.Sequential)]
		public struct GameImports
		{
			public ICreateEntity CreateEntity;
			public IRemoveEntity RemoveEntity;
			public ISetRenderColor SetRenderColor;
			public IDrawLine DrawLine;
			public ISendEvent SendEvent;

			public GameExports ManagedGame;
		}

		// * ImportFunctions
		// The C++ side will call ImportFunctions with function pointers to various game functions
		// In the most layman terms ever, C++ gives some functions to C#, and C# gives some functions
		// to C++ in return. What's happening here is basically an API exchange
		public delegate void ImportFunctionsDelegate( ref GameImports gi );
		public static void ImportFunctions( ref GameImports gi )
		{
			// Import the API from the C++ game
			game.CreateEntity = gi.CreateEntity;
			game.RemoveEntity = gi.RemoveEntity;
			game.SetRenderColor = gi.SetRenderColor;
			game.DrawLine = gi.DrawLine;
			game.SendEvent = gi.SendEvent;

			// Export our API to the C++ game
			gi.ManagedGame.GameVersion = 100;
			gi.ManagedGame.ReceiveEvent = EngineInterface.ReceiveEvent;
			game.ManagedGame = gi.ManagedGame;

			Console.WriteLine( $"Library.GameSystem.ImportFunctions(), {gi}" );
		}

		// Below are functions that will be exported to the C++ side

		// * Game loop
		public delegate void GameLoopDelegate();
		public static void GameLoop()
		{
			foreach ( GameEntity ent in gameEntities )
			{
				ent.Think();
			}
		}

		// * Game init
		// Currently there are two entity lists, one on the C++ side, and one on the
		// C# side. It's not too hard to unify them into one, and I'll look into that later on
		public delegate void GameInitDelegate();
		public static void GameInit()
		{
			gameEntities = new List<GameEntity>();

			//for ( int i = 0; i < 700; i++ )
			//{
			//	CreateEntity<GameEntity>( 60 + i*2, 60 + i ).cycleSpeed = 0.333f * (i*i * 0.00005f);
			//}

			CreateEntity<GreenBarrel>(200, 300);
		}

		// * Game shutdown
		public delegate void GameShutdownDelegate();
		public static void GameShutdown()
		{
			gameEntities.Clear();
		}

		// Below are some utilities, not exported to the C++ side

		public static GameEntity CreateEntity<T>( int x = 0, int y = 0 ) where T: GameEntity, new()
		{
			uint entityIndex = game.CreateEntity( x, y );
			if ( entityIndex == 0 || entityIndex >= 1 << 16 )
			{
				Console.WriteLine( $"Null entity, bad index {entityIndex}" );
				return null;
			}

			GameEntity ent = new T();
		
			ent.index = entityIndex;
			ent.x = x;
			ent.y = y;

			gameEntities.Add( ent );

			Console.WriteLine( $"Created new entity with index {entityIndex}, entity array size is {gameEntities.Count}" );

			return ent;
		}

		public static void RemoveEntity( uint index )
		{
			game.RemoveEntity( index );
		}

		public static void DrawLine( int x1, int y1, int x2, int y2 )
		{
			game.DrawLine( x1, y1, x2, y2 );
		}

		public static List<GameEntity> gameEntities;
		public static GameImports game;
	}
}
