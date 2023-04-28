#include "Math.h"
#include <math.h>

//---------------------------------------- Vector ----------------------------------------

Vector::Vector()
{
	this->x = 0.0;
	this->y = 0.0;
}

Vector::Vector(double x, double y)
{
	this->x = x;
	this->y = y;
}

Vector::Vector(const Vector& vector)
{
	this->x = vector.x;
	this->y = vector.y;
}

/*virtual*/ Vector::~Vector()
{
}

void Vector::operator=(const Vector& vector)
{
	this->x = vector.x;
	this->y = vector.y;
}

void Vector::operator+=(const Vector& vector)
{
	this->x += vector.x;
	this->y += vector.y;
}

void Vector::operator-=(const Vector& vector)
{
	this->x -= vector.x;
	this->y -= vector.y;
}

void Vector::operator*=(double scalar)
{
	this->x *= scalar;
	this->y *= scalar;
}

void Vector::operator/=(double scalar)
{
	this->x /= scalar;
	this->y /= scalar;
}

double Vector::Dot(const Vector& vector) const
{
	return this->x * vector.x + this->y * vector.y;
}

double Vector::Cross(const Vector& vector) const
{
	return this->x* vector.y - this->y * vector.x;
}

double Vector::Length() const
{
	return ::sqrt(this->Dot(*this));
}

Vector Vector::Rotated90CCW() const
{
	return Vector(-this->y, this->x);
}

bool Vector::Normalize()
{
	double length = this->Length();
	if (length == 0.0)
		return false;

	double scale = 1.0 / length;
	this->x *= scale;
	this->y *= scale;
	return true;
}

Vector operator+(const Vector& leftVector, const Vector& rightVector)
{
	return Vector(leftVector.x + rightVector.x, leftVector.y + rightVector.y);
}

Vector operator-(const Vector& leftVector, const Vector& rightVector)
{
	return Vector(leftVector.x - rightVector.x, leftVector.y - rightVector.y);
}

Vector operator*(const Vector& leftVector, double rightScalar)
{
	return Vector(leftVector.x * rightScalar, leftVector.y * rightScalar);
}

Vector operator*(double leftScalar, const Vector& rightVector)
{
	return Vector(rightVector.x * leftScalar, rightVector.y * leftScalar);
}

//---------------------------------------- Transform ----------------------------------------

Transform::Transform()
{
	this->xAxis.x = 1.0;
	this->xAxis.y = 0.0;
	this->yAxis.x = 0.0;
	this->yAxis.y = 1.0;
}

Transform::Transform(const Transform& transform)
{
	this->xAxis = transform.xAxis;
	this->yAxis = transform.yAxis;
}

/*virtual*/ Transform::~Transform()
{
}

void Transform::operator=(const Transform& transform)
{
	this->xAxis = transform.xAxis;
	this->yAxis = transform.yAxis;
}

Vector Transform::operator()(const Vector& vector) const
{
	return this->TransformPoint(vector);
}

Vector Transform::TransformVector(const Vector& vector) const
{
	return this->xAxis* vector.x + this->yAxis * vector.y;
}

Vector Transform::TransformPoint(const Vector& point) const
{
	return this->TransformVector(point) + this->translation;
}

Transform operator*(const Transform& leftTransform, const Transform& rightTransform)
{
	Transform product;
	product.xAxis = leftTransform.TransformVector(rightTransform.xAxis);
	product.yAxis = leftTransform.TransformVector(rightTransform.yAxis);
	product.translation = leftTransform.TransformVector(rightTransform.translation) + leftTransform.translation;
	return product;
}

//---------------------------------------- Box ----------------------------------------

Box::Box()
{
}

Box::Box(const Box& box)
{
	this->min = box.min;
	this->max = box.max;
}

Box::Box(const Vector& min, const Vector& max)
{
	this->min = min;
	this->max = max;
}

/*virtual*/ Box::~Box()
{
}

void Box::operator+=(const Vector& vector)
{
	this->min += vector;
	this->max += vector;
}

void Box::operator-=(const Vector& vector)
{
	this->min -= vector;
	this->max -= vector;
}

bool Box::IsValid() const
{
	return this->min.x <= this->max.x && this->min.y <= this->max.y;
}

void Box::PointToUVs(const Vector& point, Vector& uvs) const
{
	uvs.x = (point.x - this->min.x) / (this->max.x - this->min.x);
	uvs.y = (point.y - this->min.y) / (this->max.y - this->min.y);
}

void Box::PointFromUVs(Vector& point, const Vector& uvs) const
{
	point.x = this->min.x + uvs.x * (this->max.x - this->min.x);
	point.y = this->min.y + uvs.y * (this->max.y - this->min.y);
}

bool Box::ContainsPoint(const Vector& point) const
{
	if (this->min.x <= point.x && point.x <= this->max.x &&
		this->min.y <= point.y && point.y <= this->max.y)
	{
		return true;
	}

	return false;
}

Vector Box::Center() const
{
	return (this->min + this->max) * 0.5;
}

double Box::Width() const
{
	return this->max.x - this->min.x;
}

double Box::Height() const
{
	return this->max.y - this->min.y;
}

double Box::AspectRatio() const
{
	return this->Width() / this->Height();
}

void Box::ScaleHorizontallyToMatchAspectRatio(double aspectRatio)
{
	double delta = (aspectRatio * this->Height() - this->Width()) / 2.0;
	this->min.x -= delta;
	this->max.x += delta;
}

void Box::ScaleVerticallyToMatchAspectRatio(double aspectRatio)
{
	double delta = ((1.0 / aspectRatio) * this->Width() - this->Height()) / 2.0;
	this->min.y -= delta;
	this->max.y += delta;
}

void Box::ScaleToMatchAspectRatio(double aspectRatio)
{
	if (aspectRatio > this->AspectRatio())
		this->ScaleHorizontallyToMatchAspectRatio(aspectRatio);
	else
		this->ScaleVerticallyToMatchAspectRatio(aspectRatio);
}