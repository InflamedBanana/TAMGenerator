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

	bool operator==(Position& b)
	{
		return (this->x == b.x && this->y == b.y);
	}
};

class Point
{

protected :
	float m_hardness;
	Position m_position;
	CImg<unsigned char> m_img;

	virtual void drawImg() = 0;

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

	void drawImg()
	{
		m_img.resize(m_radius * 6, m_radius * 6);
		int center = (int)nearbyint(m_img.width() / 2.f);
		m_img.draw_circle(center, center, m_radius, color_black, 1.f).blur(m_hardness);

		int size(0);
		for ( const auto& pixel : m_img.get_column(center) )
			if ( (int)pixel < 235 )
				++size;

		m_img.crop( center - nearbyint(size / 2), center + nearbyint(size / 2), center + nearbyint(size / 2), center - nearbyint(size / 2) );
	}

private:
	unsigned int m_radius;

	
	
public:
	int radius() const { return m_radius; }
	int radius(int value) { m_radius = value; }

	Circle(unsigned int _radius) : Point(), m_radius(_radius) { drawImg(); }
	Circle(unsigned int _radius, float _hardness) : Point(_hardness), m_radius(_radius) { drawImg(); }
	Circle(Position _position, unsigned int _radius, float _hardness) : Point(_position, _hardness), m_radius(_radius) { drawImg(); }
};

class Line : public Point
{
private:
	unsigned int m_length;

public:
	unsigned int length() const { return m_length; }
	unsigned int length(unsigned int value) { m_length = value; }
};

/*void ChangeRand()
{
	srand(time(NULL));
}*/

class Map
{
private:
	std::unordered_set<Circle*> m_circles;
	CImg<unsigned char> m_img;
	int m_greyLvl;
	int m_size;
	bool m_isGenerated;

	

	void AddCircle()
	{

	}

public:
	Map() {};
	Map(int _size, int _greylvl) :m_img(_size, _size, 1, 3, 255), m_size(_size), m_greyLvl(_greylvl) {};

	const std::unordered_set<Circle*>& circles() const { return m_circles; }
	const CImg<unsigned char>& img() const { return m_img; }
	int greyLvl() const { return m_greyLvl; }
	int size() const { return m_size; }
	bool isGenerated() const { return m_isGenerated; }

	int CheckGreyLevel() const
	{
		int average = 0;
		for (const auto& pixel : m_img)
			average += (int)pixel;


		return (int)nearbyint(average / m_img.size());
	}

	void Generate(const Map* _precedingMap, const Map* _precedingToneMap )
	{
		if(_precedingMap != nullptr)
			for (const auto& point : _precedingMap->circles())
			{
				m_img.draw_circle((int)nearbyint(point->position().x * m_img.width()), (int)nearbyint(point->position().y * m_img.height()), point->radius(), color_black, 1.).blur(point->hardness());
				m_circles.insert(point);
			}

		if(_precedingToneMap != nullptr)
			for (const auto& point : _precedingToneMap->circles())
			{
				m_img.draw_circle((int)nearbyint(point->position().x * m_img.width()), (int)nearbyint(point->position().y * m_img.height()), point->radius(), color_black, 1.).blur(point->hardness());
				m_circles.insert(point);
			}

		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> uni(0 , m_size - 1);

		while ( CheckGreyLevel() > m_greyLvl )
		{
			Position pointPos;
			do
			{
				pointPos.x = (float)( uni(rng) / ( (float)m_size - 1.f ) );
				pointPos.y = (float)( uni(rng) / ( (float)m_size -1.f ) );
			} while ( (int)m_img.atXY((int)(m_img.width() * pointPos.x), (int)(m_img.height() * pointPos.y), 1, 0) < 200 );

			short int sign(1);
			short int variation(rand() % ( (int)nearbyint( options.circleRadius * ( options.radiusVariation / 100.f ) + 1 ) ) );
			if (options.radiusVariation > 0)
				sign = ( rand() % 2 + 1) < 1? -1 : 1;
			short int radius( options.circleRadius + variation * sign);

			std::cout << "VARIATION " << variation << " radius : " << radius  << std::endl;
			m_circles.insert(new Circle(pointPos, radius, options.hardness));

			m_img.draw_circle((int)nearbyint( pointPos.x * m_img.width() ), (int)nearbyint( pointPos.y * m_img.height() ), radius, color_black, 1.f)/*.blur( options.hardness )*/;

		}

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
	/*std::cout << "Hardness : [0-1]" << std::endl;
	do { std::cin >> options.hardness; } while (options.hardness < 0 || options.hardness > 1);*/
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
