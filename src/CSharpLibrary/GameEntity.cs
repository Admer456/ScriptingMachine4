
using System;
using System.Runtime.InteropServices;

namespace Library
{
	[Flags]
	enum EntityFlags : byte
	{
		Remove = 1 << 0,
		NoThink = 1 << 1,
		NoPresent = 1 << 2,
		NoEvents = 1 << 3
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct EntityEvent
	{
		public EntityEvents eventId;
		public int x, y;
	}

	public enum EntityEvents : int
	{
		Explosion,
		MouseClick
	}

	public class GameEntity
	{
		public GameEntity()
		{
			red = 255;
			green = 255;
			blue = 255;
			alpha = 255;
			x = 0.0f;
			y = 0.0f;
			radius = 25.0f;
			innerRadius = 5.0f;
			flags = 0;

			Console.WriteLine("GameEntity created in C#");
		}

		// index is obtained from the native game
		public GameEntity(uint index)
			: this()
		{
			this.index = index;

			Console.WriteLine($"GameEntity ctor, index {index}");
		}

		~GameEntity()
		{
			Console.WriteLine($"GameEntity dtor, index {index}");
			EngineInterface.RemoveEntity(index);
		}

		public virtual void Think()
		{
			cycle += 0.016f * cycleSpeed;

			ox = x + MathF.Cos(cycle) * 40.0f;
			oy = y + MathF.Sin(cycle) * 40.0f;

			EngineInterface.DrawLine((int)ox, (int)oy, (int)ox + 5, (int)oy + 5);
			EngineInterface.DrawLine((int)ox, (int)oy, (int)ox + -5, (int)oy + 5);
			EngineInterface.DrawLine((int)ox, (int)oy, (int)ox + 5, (int)oy + -5);
			EngineInterface.DrawLine((int)ox, (int)oy, (int)ox + -5, (int)oy + -5);
		}

		public virtual void ReceiveEvent(EntityEvent ev)
		{

		}

		public float DistanceTo(float otherX, float otherY)
		{
			float dx = otherX - x;
			float dy = otherY - y;
			return MathF.Sqrt(dx * dx + dy * dy);
		}

		public byte red, green, blue, alpha;
		public uint index;
		public byte flags;

		public float yaw;
		public float x, y;

		public float ox = 0.0f, oy = 0.0f;

		public float radius;
		public float innerRadius;

		public float cycle = 0.0f;
		public float cycleSpeed = 1.0f;
	}

	public class GreenBarrel : GameEntity
	{
		public GreenBarrel()
			: base()
		{
			radius = 50.0f;
			innerRadius = 35.0f;
		}

		public override void Think()
		{
			EngineInterface.game.SetRenderColor(64, 255, 64);

			const float step = MathF.Tau / 12.0f;
			for (float phase = 0.0f; phase < MathF.Tau; phase += step)
			{
				float polarX = MathF.Cos(phase) * radius + x;
				float polarY = MathF.Sin(phase) * radius + y;
				float nextPolarX = MathF.Cos(phase + step) * radius + x;
				float nextPolarY = MathF.Sin(phase + step) * radius + y;

				EngineInterface.DrawLine((int)polarX, (int)polarY, (int)nextPolarX, (int)nextPolarY);
			}
		}

		public override void ReceiveEvent(EntityEvent ev)
		{
			if (DistanceTo(ev.x, ev.y) > radius)
			{
				return;
			}

			if (ev.eventId == EntityEvents.MouseClick)
			{
				radius -= 0.25f;
				innerRadius -= 0.25f;
			}
		}
	}
}
