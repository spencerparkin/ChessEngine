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

	double Dot(const Vector& vector) const;
	double Cross(const Vector& vector) const;
	double Length() const;

	double x, y;
};

Vector operator+(const Vector& leftVector, const Vector& rightVector);
Vector operator-(const Vector& leftVector, const Vector& rightVector);
Vector operator*(const Vector& leftVector, double rightScalar);
Vector operator*(double leftScalar, const Vector& rightVector);

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