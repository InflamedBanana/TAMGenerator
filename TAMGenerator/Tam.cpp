#include "Tam.h"
#include <random>
#include <iostream>
#include <string>
#include "TamOptions.h"
#include <cmath>

using namespace std;

Point::Point() { }
Point::Point(Position _position) : m_position(_position) { }
Point::~Point(){}


Circle::Circle(unsigned int _radius) : Point(), m_radius(_radius) {}
Circle::Circle(Position _position, unsigned int _radius) 
	: Point(_position), m_radius(_radius) { }


Map::Map(): m_size(1), m_greyLvl(0){}
Map::Map(int _size, int _greylvl ) :
	m_tileOffset( ( (options::circleRadius * 2) + 1 ) * ( ( options::radiusVariation > 0) ?
	(options::circleRadius* (options::radiusVariation / 100.f ) + 1 ) : 1 ) ),
	m_size(_size), m_greyLvl(_greylvl)
{
	ci::CImg<unsigned char> img(_size + m_tileOffset * 2, _size + m_tileOffset * 2, 1, 1, 255);
	m_img = img;
}
//Map::Map(const Map& _map) :m_size(_map.size()),m_greyLvl(_map.greyLvl()),
//m_img(_map.img(),false), m_isGenerated(_map.m_isGenerated){}


void Map::TilePoint(const Position& _pos, const int _radius)
{
	const int lowerTile(m_tileOffset + _radius + 1);
	const int upperTile(m_tileOffset + m_size - _radius - 1);
	bool isTiled(false);

	if ((int)_pos.x >= lowerTile && (int)_pos.y >= lowerTile &&
		(int)_pos.x <= upperTile && (int)_pos.y <= lowerTile)
		return;

	Position tiledPos(0.f, 0.f);

	if ((int)_pos.x < lowerTile)
		m_img.draw_circle((int)_pos.x + m_size, (int)_pos.y, _radius, color_black, 1.f);
	else if ((int)_pos.x > upperTile)
		m_img.draw_circle((int)_pos.x - m_size, (int)_pos.y, _radius, color_black, 1.f);

	if ((int)_pos.y < lowerTile)
		m_img.draw_circle((int)_pos.x, (int)_pos.y + m_size, _radius, color_black, 1.f);
	else if ((int)_pos.y > upperTile)
		m_img.draw_circle((int)_pos.x, (int)_pos.y - m_size, _radius, color_black, 1.f);

	if ((int)_pos.x < lowerTile && (int)_pos.y < lowerTile)
		m_img.draw_circle((int)_pos.x + m_size, (int)_pos.y + m_size, _radius, color_black, 1.f);
	else if ((int)_pos.x > upperTile && (int)_pos.y < lowerTile)
		m_img.draw_circle((int)_pos.x - m_size, (int)_pos.y + m_size, _radius, color_black, 1.f);
	else if ((int)_pos.x < lowerTile && (int)_pos.y > upperTile)
		m_img.draw_circle((int)_pos.x + m_size, (int)_pos.y - m_size, _radius, color_black, 1.f);
	else if ((int)_pos.x > upperTile && (int)_pos.y > upperTile)
		m_img.draw_circle((int)_pos.x - m_size, (int)_pos.y - m_size, _radius, color_black, 1.f);
}

int Map::CheckGreyLevel() const
{
	int average = 0;

	for (int x = m_tileOffset + 1; x < m_size + 1 + m_tileOffset; ++x)
		for (int y = m_tileOffset + 1; y < m_size + 1 + m_tileOffset; ++y)
			average += (int)m_img.atXY(x, y, 0, 0);

	return (int)nearbyint(average / (m_size *  m_size));
}

void Map::Generate(const Map* _precedingMap, const Map* _precedingToneMap)
{
	if (_precedingMap != nullptr)
		m_circles.insert(_precedingMap->circles().begin(), _precedingMap->circles().end());
	
	if (_precedingToneMap != nullptr)
		m_circles.insert(_precedingToneMap->circles().begin(), _precedingToneMap->circles().end());
	
	for (const auto& point : m_circles)
	{
		Position pos(point->position());
		pos.ToSize(m_size, m_tileOffset);
		m_img.draw_circle((int)pos.x, (int)pos.y, point->radius(), color_black, 1.f);
		TilePoint(pos, point->radius());
	}

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> uni(0, m_size - 1);

	while (CheckGreyLevel() > m_greyLvl)
	{
		Position pointPos;
		do
		{
			pointPos.x = (float)(uni(rng));
			pointPos.y = (float)(uni(rng));
		} while ( (int)m_img.atXY( (int)pointPos.x + m_tileOffset, (int)pointPos.y + m_tileOffset, 0, 0) < 240 );

		short int sign(1);
		short int variation(rand() % ((int)nearbyint(options::circleRadius * (options::radiusVariation / 100.f) + 1)));

		if (options::radiusVariation > 0)
			sign = (rand() % 2 + 1) < 1 ? -1 : 1;

		short int radius(options::circleRadius + variation * sign);

		m_img.draw_circle((int)pointPos.x + m_tileOffset, (int)pointPos.y + m_tileOffset, radius, color_black, 1.f);

		Position tilePos(pointPos.x + m_tileOffset, pointPos.y + m_tileOffset);
		TilePoint(tilePos, radius);

		pointPos.Normalize(m_size);

		m_circles.insert(new Circle(pointPos, radius));
	}

	int center = m_size / 2;
	m_img.crop(m_tileOffset + 1, m_tileOffset + 1, m_tileOffset + m_size, m_tileOffset + m_size);

	SaveMap();

	cout << "Map " << m_size << " tone " << m_greyLvl << " generated." << endl;
	m_isGenerated = true;
}

void Map::Resize(const float _resizeValue)
{
	m_img.resize(_resizeValue * -100, _resizeValue * -100, -100,-100,3);
	m_size *= _resizeValue;
}

void Map::SaveMap()
{
	string file("");
	file.append(to_string(m_greyLvl));
	file.append("_");
	file.append(to_string(m_size));
	file.append(".bmp");

	m_img.save(file.c_str());

	cout << "Map " << m_size << " tone " << m_greyLvl << " saved !" << endl;
}

Tone::Tone(int _greyLvl) : m_greylvl(_greyLvl) {};

void Tone::Generate(const int _maxMapSize, const Tone* _precedingTone)
{
	for (int size = 1; size < _maxMapSize * 2; size *= 2)
	{
		Map newMap(size, m_greylvl);
		m_maps.push_back(newMap);
	}

	int i(0);
	for (vector<Map>::iterator it = m_maps.begin() + 4; it != m_maps.end(); ++it, ++i)
	{
		if ( i == 0 )
			it->Generate(nullptr, (_precedingTone == nullptr) ? nullptr : &_precedingTone->maps()[i]);
		else
			it->Generate(&(*(it - 1)), (_precedingTone == nullptr) ? nullptr : &_precedingTone->maps()[i]);
	}

	ComputeLowerMipMaps();

	cout << "tone " << m_greylvl << " generated with " << m_maps.size() << " maps." << endl;
}

void Tone::Save()
{
	for (auto& map : m_maps)
		map.SaveMap();
}

void Tone::ComputeLowerMipMaps()
{
	for (int i = 3; i >= 0; i --)
	{
		ci::CImg<unsigned char> img(m_maps[4].img(), false);
		//Map lower(m_maps[4]);
		int mapSize(m_maps[4].size() * 1 / pow(2, 4 - i));
		Map lower(mapSize, m_greylvl);
		//lower.img(img);
		//lower.Resize(1/pow(2,4-i));
		lower.SaveMap();
	}
	
}