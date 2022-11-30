#include "math.h"
#include <math.h>

// 内積(3D)
float Dot3D(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return x1 * x2 + y1 * y2 + z1 * z2;
}

// 外積(3D)
void Cross3D(float x1, float y1, float z1, float x2, float y2, float z2, float vOut[3])
{
	vOut[0] = y1 * z2 - z1 * y2;
	vOut[1] = z1 * x2 - x1 * z2;
	vOut[2] = x1 * y2 - y1 * x2;
}

// 正規化(3D)
void Normalize3D(float x, float y, float z, float vOut[3])
{
	float fLength;
	fLength = Length3D(x, y, z);
	vOut[0] = x / fLength;
	vOut[1] = y / fLength;
	vOut[2] = z / fLength;
}

// ベクトルの長さ(3D)
float Length3D(float x, float y, float z)
{
	return (float)sqrt(x * x + y * y + z * z);
}


//ベクトルの足し算
Vector3D operator +(const Vector3D &lhs, const Vector3D &rhs)
{
	Vector3D vec;

	vec.x = lhs.x + rhs.x;
	vec.y = lhs.y + rhs.y;
	vec.z = lhs.z + rhs.z;

	return vec;
}
//ベクトルの引き算
Vector3D operator -(const Vector3D &lhs, const Vector3D &rhs)
{
	Vector3D vec;

	vec.x = lhs.x - rhs.x;
	vec.y = lhs.y - rhs.y;
	vec.z = lhs.z - rhs.z;

	return vec;
}
//ベクトルのスカラー倍
Vector3D operator *(float lhs, const Vector3D &rhs)
{
	Vector3D vec;

	vec.x = rhs.x * lhs;
	vec.y = rhs.y * lhs;
	vec.z = rhs.z * lhs;

	return vec;
}
//ベクトルのスカラー倍
Vector3D operator *(const Vector3D &lhs, float rhs)
{
	Vector3D vec;

	vec.x = lhs.x * rhs;
	vec.y = lhs.y * rhs;
	vec.z = lhs.z * rhs;

	return vec;
}
//ベクトルの割り算
Vector3D operator /(const Vector3D &lhs, float rhs)
{
	Vector3D vec;

	if (rhs == 0.f) return lhs;

	vec.x = lhs.x / rhs;
	vec.y = lhs.y / rhs;
	vec.z = lhs.z / rhs;

	return vec;
}
//ベクトルの長さを求める
float Vector3D::Length3D()
{
	float r = 0.0f;
	r = x * x + y * y + z * z;
	return (float)(sqrt(r));
}

//ベクトルの正規化
Vector3D Vector3D::Normalize3D()
{
	Vector3D vec;
	float this_vector_len;

	this_vector_len = Length3D();
	if (this_vector_len < 0.f)
		*this;

	vec = *this / this_vector_len;

	return vec;
}

//内積
float Vector3D::Dot3D(const Vector3D &v)
{
	return this->x * v.x + this->y * v.y + this->z * v.z;
}

// 戻り値 : const Vector2D &
// 関数名 : operator +=
// 引  数 : const Vecotr2D &rhs
//ベクトルの足し算
const Vector3D &Vector3D::operator +=(const Vector3D &rhl)
{
	this->x += rhl.x;
	this->y += rhl.y;

	return *this;
}
//ベクトルの引き算
const Vector3D &Vector3D::operator -=(const Vector3D &rhl)
{
	this->x -= rhl.x;
	this->y -= rhl.y;

	return *this;
}
//ベクトルのスカラー倍
const Vector3D &Vector3D::operator *=(float rhl)
{
	this->x *= rhl;
	this->y *= rhl;
	this->z *= rhl;

	return *this;
}
//ベクトルの割り算
const Vector3D &Vector3D::operator /=(float rhl)
{
	this->x /= rhl;
	this->y /= rhl;
	this->z /= rhl;

	return *this;
}
//ベクトルとベクトルが同じか
bool Vector3D::operator == (const Vector3D &rhs)
{
	if (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z)
	{
		return true;
	}

	return false;
}
//ベクトルとベクトルが違うか
bool Vector3D::operator != (const Vector3D &rhs)
{
	if (this->x != rhs.x || this->y != rhs.y || this->z != rhs.z)
	{
		return true;
	}

	return false;
}

Matrix4x4::Matrix4x4(float f11, float f12, float f13, float f14,
	float f21, float f22, float f23, float f24,
	float f31, float f32, float f33, float f34,
	float f41, float f42, float f43, float f44)
{
	_11 = f11; _12 = f12; _13 = f13; _14 = f14;
	_21 = f21; _22 = f22; _23 = f23; _24 = f24;
	_31 = f31; _32 = f32; _33 = f33; _34 = f34;
	_41 = f41; _42 = f42; _43 = f43; _44 = f44;
}

// 初期化
void Matrix4x4::Identity()
{
	_11 = 1.f; _12 = 0.f; _13 = 0.f; _14 = 0.f;
	_21 = 0.f; _22 = 1.f; _23 = 0.f; _24 = 0.f;
	_31 = 0.f; _32 = 0.f; _33 = 1.f; _34 = 0.f;
	_41 = 0.f; _42 = 0.f; _43 = 0.f; _44 = 1.f;
}

// 座標変換をする
// 頂点座標
Vector3D Matrix4x4::TransformPointVector3D(const Vector3D &vPoint)
{
	Vector3D v;

	v.x = vPoint.x * _11 + vPoint.y * _21 + vPoint.z * _31 + _41;
	v.y = vPoint.x * _12 + vPoint.y * _22 + vPoint.z * _32 + _42;
	v.z = vPoint.x * _13 + vPoint.y * _23 + vPoint.z * _33 + _43;

	return v;
}

// 法線用
Vector3D Matrix4x4::TransformNormalVector3D(const Vector3D &vPoint)
{
	Vector3D v;

	v.x = vPoint.x * _11 + vPoint.y * _21 + vPoint.z * _31;
	v.y = vPoint.x * _12 + vPoint.y * _22 + vPoint.z * _32;
	v.z = vPoint.x * _13 + vPoint.y * _23 + vPoint.z * _33;

	return v;
}

void Matrix4x4::Multiply(const Matrix4x4& mMatrix)
{
	Matrix4x4 temp = *this;

	_11 = temp._11 * mMatrix._11 + temp._12 * mMatrix._21 + temp._13 * mMatrix._31 + temp._14 * mMatrix._41;
	_12 = temp._11 * mMatrix._12 + temp._12 * mMatrix._22 + temp._13 * mMatrix._32 + temp._14 * mMatrix._42;
	_13 = temp._11 * mMatrix._13 + temp._12 * mMatrix._23 + temp._13 * mMatrix._33 + temp._14 * mMatrix._43;
	_14 = temp._11 * mMatrix._14 + temp._12 * mMatrix._24 + temp._13 * mMatrix._34 + temp._14 * mMatrix._44;

	_21 = temp._21 * mMatrix._11 + temp._22 * mMatrix._21 + temp._23 * mMatrix._31 + temp._24 * mMatrix._41;
	_22 = temp._21 * mMatrix._12 + temp._22 * mMatrix._22 + temp._23 * mMatrix._32 + temp._24 * mMatrix._42;
	_23 = temp._21 * mMatrix._13 + temp._22 * mMatrix._23 + temp._23 * mMatrix._33 + temp._24 * mMatrix._43;
	_24 = temp._21 * mMatrix._14 + temp._22 * mMatrix._24 + temp._23 * mMatrix._34 + temp._24 * mMatrix._44;
	
	_31 = temp._31 * mMatrix._11 + temp._32 * mMatrix._21 + temp._33 * mMatrix._31 + temp._34 * mMatrix._41;
	_32 = temp._31 * mMatrix._12 + temp._32 * mMatrix._22 + temp._33 * mMatrix._32 + temp._34 * mMatrix._42;
	_33 = temp._31 * mMatrix._13 + temp._32 * mMatrix._23 + temp._33 * mMatrix._33 + temp._34 * mMatrix._43;
	_34 = temp._31 * mMatrix._14 + temp._32 * mMatrix._24 + temp._33 * mMatrix._34 + temp._34 * mMatrix._44;

	_41 = temp._41 * mMatrix._11 + temp._42 * mMatrix._21 + temp._43 * mMatrix._31 + temp._44 * mMatrix._41;
	_42 = temp._41 * mMatrix._12 + temp._42 * mMatrix._22 + temp._43 * mMatrix._32 + temp._44 * mMatrix._42;
	_43 = temp._41 * mMatrix._13 + temp._42 * mMatrix._23 + temp._43 * mMatrix._33 + temp._44 * mMatrix._43;
	_44 = temp._41 * mMatrix._14 + temp._42 * mMatrix._24 + temp._43 * mMatrix._34 + temp._44 * mMatrix._44;
}