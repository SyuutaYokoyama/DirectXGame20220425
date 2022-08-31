#include"Vector3.h"
#include <cmath>     //sqrt

Vector3::Vector3()
	:x(0), y(0), z(0)
{
}

Vector3::Vector3(float x, float y, float z)
	: x(x), y(y), z(z)
{
}

float Vector3::length()const
{
	return sqrt((x * x) + (y * y) + (z * z));
}

Vector3& Vector3::normalize()
{
	float len = length();
	if (len != 0)
	{
		return *this /= len;
	}
	return *this;
}

float Vector3::dot(const Vector3& v)const
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

float Vector3::cross(const Vector3& v)const
{
	return  (v.y * v.z) - (v.z * v.y),(v.z * v.x) - (v.x * v.z) ,(v.x * v.y) - (v.y * v.z) ;
}

Vector3 Vector3::operator+() const
{
	return *this;
}

Vector3 Vector3::operator-()const
{
	return (*this)* -1;
}

Vector3& Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3& Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3& Vector3::operator/= (float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

//Vector2ÉNÉâÉXÇ…ëÆÇ≥Ç»Ç¢ä÷êîåQ
//ìÒçÄââéZéq
const Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3& v)
{
	return v * s;
}

const Vector3 operator/(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp /= s;
}