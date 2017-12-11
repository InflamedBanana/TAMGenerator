#pragma once

#ifndef DEF_TAM
#define DEF_TAM

#include <unordered_set>
#include <vector>
#include "Position.h"
#include <CImg/CImg.h>

namespace ci = cimg_library;

const unsigned char color_black[] = { 0x1, 0x1, 0x1 };
const unsigned char color_white[] = { 255, 255, 255 };
const unsigned char color_red[] = { 255, 0, 0 };

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

class Map
{
private:
	Map();

	std::unordered_set<Circle*> m_circles;
	ci::CImg<unsigned char>* m_img;
	int m_greyLvl;
	int m_size;
	bool m_isGenerated;
	int m_tileOffset;

	void TilePoint( const Position& _pos, const int _radius );
	int CheckGreyLevel() const;

public:
	~Map();
	Map( int _size, int _greylvl );
	//Map(const Map& _map);

	const std::unordered_set<Circle*>& circles() const { return m_circles; }
	const ci::CImg<unsigned char>& img() const { return *m_img; }
	//void img(const ci::CImg<unsigned char>& _img) { m_img = _img; }

	int greyLvl() const { return m_greyLvl; }
	int size() const { return m_size; }
	bool isGenerated() const { return m_isGenerated; }

	void Resize( const float _resizeValue, const ci::CImg<unsigned char>& _higherImg );
	void Generate( const Map* _precedingMap, const Map* _precedingToneMap );
	void SaveMap();
};

class Tone
{
private:
	std::vector<Map> m_maps;
	int m_greylvl;

	void ComputeLowerMipMaps();

public:
	Tone( int _greyLvl );
	std::vector<Map> maps() const { return m_maps; }
	int greylvl() const { return m_greylvl; }

	void Generate( const int _maxMapSize, const Tone* _precedingTone = nullptr );

	void Save();
};

#endif // !DEF_TAM
