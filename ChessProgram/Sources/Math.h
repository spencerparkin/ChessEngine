#pragma once

class Vector
{
public:
	Vector();
	Vector(double x, double y);
	Vector(const Vector& vector);
	virtual ~Vector();

	void operator=(const Vector& vector);
	void operator+=(const Vector& vector);
	void operator-=(const Vector& vector);
	void operator*=(double scalar);
	void operator/=(double scalar);

	double Dot(const Vector& vector) const;
	double Cross(const Vector& vector) const;
	double Length() const;
	Vector Rotated90CCW() const;
	bool Normalize();

	double x, y;
};

Vector operator+(const Vector& leftVector, const Vector& rightVector);
Vector operator-(const Vector& leftVector, const Vector& rightVector);
Vector operator*(const Vector& leftVector, double rightScalar);
Vector operator*(double leftScalar, const Vector& rightVector);

class Transform
{
public:
	Transform();
	Transform(const Transform& transform);
	virtual ~Transform();

	void operator=(const Transform& transform);
	Vector operator()(const Vector& vector) const;

	Vector TransformVector(const Vector& vector) const;
	Vector TransformPoint(const Vector& point) const;

	Vector xAxis, yAxis;
	Vector translation;
};

Transform operator*(const Transform& leftTransform, const Transform& rightTransform);

class Box
{
public:
	Box();
	Box(const Box& box);
	Box(const Vector& min, const Vector& max);
	virtual ~Box();

	void operator+=(const Vector& vector);
	void operator-=(const Vector& vector);

	bool IsValid() const;

	void PointToUVs(const Vector& point, Vector& uvs) const;
	void PointFromUVs(Vector& point, const Vector& uvs) const;

	Vector Center() const;

	bool ContainsPoint(const Vector& point) const;

	double Width() const;
	double Height() const;
	double AspectRatio() const;

	void ScaleHorizontallyToMatchAspectRatio(double aspectRatio);
	void ScaleVerticallyToMatchAspectRatio(double aspectRatio);

	void ScaleToMatchAspectRatio(double aspectRatio);

	// Union()
	// Intersection()

	Vector min, max;
};