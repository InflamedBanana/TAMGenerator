#pragma once

#ifndef DEF_SHAPES
#define DEF_SHAPES

#include "Position.h"

class Point
{

protected:
	Position m_position;

public:
	Point();
	Point( Position _position );
	~Point();

	Position position() const { return m_position; }

	bool operator==( Point& b )
	{
		return ( this->m_position == b.m_position );
	}
};

class Circle : public Point
{

private:
	unsigned int m_radius;

public:
	int radius() const { return m_radius; }
	int radius( int value ) { m_radius = value; }

	Circle( unsigned int _radius );
	Circle( Position _position, unsigned int _radius );
};

class Line : public Point
{
private:
	unsigned int m_length;

public:
	unsigned int length() const { return m_length; }
	unsigned int length( unsigned int value ) { m_length = value; }
};

#endif // !DEF_SHAPES

