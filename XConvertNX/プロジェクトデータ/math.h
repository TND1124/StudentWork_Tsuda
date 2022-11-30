#pragma once

// 外積(3D)
void Cross3D(float x1, float y1, float z1, float x2, float y2, float z2, float vOut[3]);
// 内積(3D)
float Dot3D(float x1, float y1, float z1, float x2, float y2, float z2);
// 正規化(3D)
void Normalize3D(float x, float y, float z, float vOut[3]);
// ベクトルの長さ(3D)
float Length3D(float x, float y, float z);

// 3Dベクトル
struct Vector3D
{
	float x, y, z;

	Vector3D(void)
		: x(0.f), y(0.f), z(0.f)
	{}

	Vector3D(float fx, float fy, float fz)
		: x(fx), y(fy), z(fz)
	{}

	Vector3D(float* pf) : x(pf[0]), y(pf[1]), z(pf[2])
	{}

	void Zero(void)
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}

	bool isZero(void)
	{
		return (x * x + y * y + z * z) < 0.00001f;
	}

	const Vector3D &operator +=(const Vector3D &rhl); //this += rhs
	const Vector3D &operator -=(const Vector3D &rhl); //this -= rhs
	const Vector3D &operator *=(float rhl); //this *= rhs(スカラー)
	const Vector3D &operator /=(float rhl); //this /= rhs(スカラー)

	bool operator == (const Vector3D &rhs);	// this == rhs
	bool operator != (const Vector3D &rhs); // this != rhs

	float Length3D();
	Vector3D Normalize3D();
	float Dot3D(const Vector3D &v);
};
//------------------------------------------------------------
Vector3D operator +(const Vector3D &lhs, const Vector3D &rhs);
Vector3D operator -(const Vector3D &lhs, const Vector3D &rhs);
Vector3D operator *(float lhs, const Vector3D &rhs);
Vector3D operator *(const Vector3D &lhs, float rhs);
Vector3D operator /(const Vector3D &lhs, float rhs);

//------------------------------------------------------------

// 行列
struct Matrix4x4
{
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;

	Matrix4x4() {}
	Matrix4x4(float f11, float f12, float f13, float f14,
		float f21, float f22, float f23, float f24,
		float f31, float f32, float f33, float f34,
		float f41, float f42, float f43, float f44);

	void Identity();

	// 座標変換をする
	// 頂点座標
	Vector3D TransformPointVector3D(const Vector3D &vPoint);
	// 法線用
	Vector3D TransformNormalVector3D(const Vector3D &vPoint);

	void Multiply(const Matrix4x4& mMatrix);
};