using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Strype
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Transform
	{
		public Vector3 Position;
		public Vector3 Rotation;
		public Vector3 Scale;

		public Transform(Vector3 position, Vector3 rotation, Vector3 scale)
		{
			Position = position;
			Rotation = rotation;
			Scale = scale;
		}
		public override bool Equals(object? obj) => obj is Transform other && Equals(other);
		public bool Equals(Transform right) => Position.X == right.Position.X && Position.Y == right.Position.Y && Position.Z == right.Position.Z && Rotation.X == right.Rotation.X && Rotation.Y == right.Rotation.Y && Rotation.Z == right.Rotation.Z && Scale.X == right.Scale.X && Scale.Y == right.Scale.Y && Scale.Z == right.Scale.Z;
		public override int GetHashCode() => (Position.X, Position.Y, Position.Z, Rotation.X, Rotation.Y, Rotation.Z, Scale.X, Scale.Y, Scale.Z).GetHashCode();
		public static bool operator ==(Transform left, Transform right) => left.Equals(right);
		public static bool operator !=(Transform left, Transform right) => !(left == right);

	}
}
