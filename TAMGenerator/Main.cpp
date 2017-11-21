#include <CImg/CImg.h>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <unordered_set>
#include <random>

using namespace cimg_library;

const unsigned char color_black[] = { 0,0,0 };
const unsigned char color_white[] = { 255,255,255 };
const unsigned char color_red[] = { 255, 0, 0 };

struct tamOptions
{
	unsigned int maxMapSize;
	unsigned char greyLvl;
	unsigned int minGrey;
	unsigned int maxGrey;
	unsigned int nbOfGreys;
	float hardness;
	int circleRadius;
	int radiusVariation;

}options;

struct Position
{
	float x;
	float y;

	Position() {};
	Position(float _x, float _y) : x(_x), y(_y) {};
	Position(const Position& _pos) : x(_pos.x), y(_pos.y) {};

	bool operator==(Position& b){ return (this->x == b.x && this->y == b.y); }

	Position Normalized(const Position& _pos, const int _max)
	{
		Position p(_pos);
		p.Normalize(_max);
		return p;
	}
	void Normalize(const int _max) { x = x / (float)(_max - 1.f); y = y / (float)(_max - 1.f); }
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
};

class Point
{

protected :
	float m_hardness;
	Position m_position;
	CImg<unsigned char> m_img;

	/*virtual void drawImg() = 0;*/

public:
	Point() : m_hardness(1), m_img(1, 1, 1, 1, 255) { }
	Point(Position _position) :m_position(_position), m_hardness(1), m_img(1, 1, 1, 1, 255) { }
	Point(float _hardness) : m_hardness(_hardness), m_img(1, 1, 1, 1, 255) {}
	Point(Position _position, float _hardness) : m_position(_position), m_hardness(_hardness), m_img(1, 1, 1, 1, 255) { }
	~Point(){}

	Position position() const { return m_position; }
	float hardness() const { return m_hardness; }
	float hardness(float value){ if (value > 1) m_hardness= 1; else if (value < 0) m_hardness= 0; else m_hardness = value; }
	CImg<unsigned char>& img() { return m_img; }

	bool operator==(Point& b)
	{
		return (this->m_position == b.m_position);
	}
};

class Circle : public Point
{
protected :

private:
	unsigned int m_radius;

	
	
public:
	int radius() const { return m_radius; }
	int radius(int value) { m_radius = value; }

	Circle(unsigned int _radius) : Point(), m_radius(_radius) {  }
	Circle(unsigned int _radius, float _hardness) : Point(_hardness), m_radius(_radius) { }
	Circle(Position _position, unsigned int _radius, float _hardness) : Point(_position, _hardness), m_radius(_radius) { }
};

class Line : public Point
{
private:
	unsigned int m_length;

public:
	unsigned int length() const { return m_length; }
	unsigned int length(unsigned int value) { m_length = value; }
};

class Map
{
private:
	std::unordered_set<Circle*> m_circles;
	CImg<unsigned char> m_img;
	int m_greyLvl;
	int m_size;
	bool m_isGenerated;
	int m_tileOffset;

	void AddCircle()
	{

	}

public:
	Map() {};
	Map(int _size, int _greylvl) :
		m_tileOffset(((options.circleRadius * 2) + 1) * ((options.radiusVariation > 0) ?
		(options.circleRadius * (options.radiusVariation / 100.f) + 1) : 1)),
		m_size(_size), m_greyLvl(_greylvl)
	{
		CImg<unsigned char> img(_size + m_tileOffset * 2, _size + m_tileOffset * 2, 1, 1, 255);
		m_img = img;
	}

	const std::unordered_set<Circle*>& circles() const { return m_circles; }
	const CImg<unsigned char>& img() const { return m_img; }
	int greyLvl() const { return m_greyLvl; }
	int size() const { return m_size; }
	bool isGenerated() const { return m_isGenerated; }

	void TilePoint(const Position& _pos, const int _radius)
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

		if((int)_pos.x < lowerTile && (int)_pos.y < lowerTile)
			m_img.draw_circle((int)_pos.x + m_size, (int)_pos.y + m_size, _radius, color_black, 1.f);
		else if((int)_pos.x > upperTile && (int)_pos.y < lowerTile)
			m_img.draw_circle((int)_pos.x - m_size, (int)_pos.y + m_size, _radius, color_black, 1.f);
		else if((int)_pos.x < lowerTile && (int)_pos.y > upperTile)
			m_img.draw_circle((int)_pos.x + m_size, (int)_pos.y - m_size, _radius, color_black, 1.f);
		else if((int)_pos.x > upperTile && (int)_pos.y > upperTile)
			m_img.draw_circle((int)_pos.x - m_size, (int)_pos.y - m_size, _radius, color_black, 1.f);
	}

	int CheckGreyLevel() const
	{
		int average = 0;

		for( int x = m_tileOffset + 1; x < m_size + 1 + m_tileOffset; ++x )
			for (int y = m_tileOffset + 1; y < m_size + 1 + m_tileOffset; ++y)
				average += (int)m_img.atXY(x,y,0,0);
			
		return (int)nearbyint( average / (m_size *  m_size) );
	}

	void Generate(const Map* _precedingMap, const Map* _precedingToneMap )
	{
		if (_precedingMap != nullptr)
			m_circles.insert(_precedingMap->circles().begin(), _precedingMap->circles().end());

		if (_precedingToneMap != nullptr)
			m_circles.insert(_precedingToneMap->circles().begin(), _precedingToneMap->circles().end());

		for (const auto& point : m_circles)
		{
			Position pos(point->position());
			pos.ToSize(m_size, m_tileOffset);
			m_img.draw_circle((int)pos.x , (int)pos.y, point->radius(), color_black, 1.f);
			TilePoint(pos, point->radius());
		}

		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> uni(0 , m_size - 1);
		
		while ( CheckGreyLevel() > m_greyLvl)
		{
			Position pointPos;
			do
			{
				pointPos.x = (float)(uni(rng) );
				pointPos.y = (float)(uni(rng) );
			} while ( (int)m_img.atXY((int)pointPos.x + m_tileOffset, (int)pointPos.y + m_tileOffset, 0, 0) < 240 );

			short int sign(1);
			short int variation(rand() % ( (int)nearbyint( options.circleRadius * ( options.radiusVariation / 100.f ) + 1 ) ) );

			if (options.radiusVariation > 0)
				sign = ( rand() % 2 + 1) < 1? -1 : 1;

			short int radius( options.circleRadius + variation * sign);
			
			m_img.draw_circle((int)pointPos.x + m_tileOffset, (int)pointPos.y + m_tileOffset, radius, color_black, 1.f);
			
			Position tilePos(pointPos.x + m_tileOffset, pointPos.y + m_tileOffset);
			TilePoint(tilePos, radius);

			pointPos.Normalize(m_size);

			m_circles.insert(new Circle(pointPos, radius, options.hardness));
		}

		int center = m_size / 2;
		m_img.crop(m_tileOffset+1,m_tileOffset+1, m_tileOffset + m_size, m_tileOffset + m_size);

		SaveMap();

		std::cout << "Map " << m_size << " tone " << m_greyLvl << " generated." << std::endl;
		m_isGenerated = true;
	}

	void SaveMap()
	{
		std::string file("");
		file.append(std::to_string(m_greyLvl));
		file.append("_");
		file.append(std::to_string(m_size));
		file.append(".bmp");
		 
		m_img.save(file.c_str());

		std::cout << "Map " << m_size << " tone " << m_greyLvl << " saved !" << std::endl;
	}
};

class Tone
{
private :
	std::vector<Map> m_maps;
	int m_greylvl;

public :
	Tone(int _greyLvl) : m_greylvl(_greyLvl) {};
	std::vector<Map> maps() const { return m_maps; }
	int greylvl() const { return m_greylvl; }

	void Generate( const int _maxMapSize, const Tone* _precedingTone )
	{
		for (int size = 16; size < _maxMapSize * 2 ; size *= 2)
		{
			Map newMap(size, m_greylvl);
			m_maps.push_back(newMap);
		}

		int i(0);
		for (std::vector<Map>::iterator it = m_maps.begin(); it != m_maps.end(); ++it, ++i)
		{
			if (it == m_maps.begin())
				it->Generate(nullptr, (_precedingTone == nullptr) ? nullptr : &_precedingTone->maps()[i] );
			else
				it->Generate( &(*(it - 1)) , (_precedingTone == nullptr) ? nullptr : &_precedingTone->maps()[i] );
		}
		std::cout << "tone " << m_greylvl << " generated with "<< m_maps.size() << " maps." << std::endl;
	}

	void Save()
	{
		for (auto& map : m_maps)
			map.SaveMap();
	}
};

int main()
{
	srand(time(0));

	std::cout << "Nb of greys you want :" << std::endl;
	do { std::cin >> options.nbOfGreys; } while (options.nbOfGreys < 2 || options.nbOfGreys > 10);
	std::cout << "min grey value :" << std::endl;
	do { std::cin >> options.minGrey; } while (options.minGrey < 0 || options.minGrey > 255);
	std::cout << "max grey value :" << std::endl;
	do { std::cin >> options.maxGrey; } while (options.maxGrey < options.minGrey || options.maxGrey > 255);
	std::cout << "max map size :" << std::endl;
	do { std::cin >> options.maxMapSize; } while (options.maxMapSize < 16 || !(options.maxMapSize & (options.maxMapSize - 1)) == 0);
	std::cout << "Circle Radius :" << std::endl;
	do { std::cin >> options.circleRadius; } while (options.circleRadius < 1);
	std::cout << "Radius Variation ( in % ):" << std::endl;
	std::cin >> options.radiusVariation;
	options.hardness = 0;
	std::vector<CImgDisplay> displays;

	std::vector<Tone> tones;
	 
	for ( int indice = 0, greyValue = options.maxGrey; indice < (int)options.nbOfGreys; ++indice, greyValue -= ( ( options.maxGrey - options.minGrey ) / ( options.nbOfGreys - 1 ) ) )
	{
		std::cout << "tone grey value " << greyValue << std::endl;
		Tone tone(greyValue);
		tones.push_back(tone);
	}

	for (std::vector<Tone>::iterator toneIt = tones.begin(); toneIt != tones.end(); ++toneIt)
	{
		if ( toneIt == tones.begin() )
			toneIt->Generate( options.maxMapSize, nullptr );
		else
			toneIt->Generate(options.maxMapSize, &(*(toneIt - 1)));
	}

	for(const auto& tone : tones)
		for (const auto& map : tone.maps())
		{
			std::string mapName(std::to_string(map.greyLvl()));
			mapName.append("  ");
			mapName.append(std::to_string(map.size()));
			CImgDisplay display( 256, 256, mapName.c_str(), 0 );
			displays.push_back( display );
			map.img().display( displays.back() );
		}

	std::cout << "FINISHED" << std::endl;
	system("PAUSE");

	return 0;
}
