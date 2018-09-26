#pragma once

#ifndef DEF_STRUCT_POSITION
#define DEF_STRUCT_POSITION

#include <cmath>

struct Position
{
	float x = .0f;
	float y = .0f;

	Position() {};
	Position(float _x, float _y) : x(_x), y(_y) {};
	Position(const Position& _pos) : x(_pos.x), y(_pos.y) {};

	bool operator==(const Position& b) { return (this->x == b.x && this->y == b.y); }

	Position Normalized(const Position& _pos, const float _max)
	{
		Position p(_pos);
		p.Normalize(_max);
		return p;
	}
	void Normalize(const float _max) { x = x / (_max - 1.f); y = y / (_max - 1.f); }

	void ToSize(const int _size, const int _offset = -1.f)
	{
		x = nearbyint(x * (_size - 1)) + _offset;
		y = nearbyint(y * (_size - 1)) + _offset;
	}

	Position Sized(const Position& _pos, const int _size, const int _offset = -1.f)
	{
		Position p(_pos);
		p.ToSize(_size, _offset);
		return p;
	}

	int xInt() { return static_cast<int>(x); }
	int yInt() { return static_cast<int>(y); }
};

#define POS_X0Y1 Position{0,1}
#define POS_X1Y0 Position{1,0}
#define POS_ZERO Position{0,0}

#endif // !DEF_STRUCT_POSITION
