#include <CImg/CImg.h>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <unordered_set>
#include <set>
#include <unordered_map>

using namespace cimg_library;

const unsigned char color_black[] = { 0,0,0 };
const unsigned char color_white[] = { 255,255,255 };

struct tamOptions
{
	//unsigned int mapSize;
	unsigned int maxMapSize;
	//unsigned int minMapSize;
	unsigned char greyLvl;
};

struct Position
{
	float x;
	float y;

	/*bool operator==(Position& b)
	{
		return (this->x == b.x && this->y == b.y);
	}*/
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

	/*bool operator==(Point& b)
	{
		return (this->m_position == b.m_position);
	}*/
};

class Circle : public Point
{
protected :

	void drawImg()
	{
		m_img.resize(m_radius * 6, m_radius * 6);
		/*const unsigned char black[] = { 0,0,0 };
		const unsigned char white[] = { 255,255,255 };*/
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

void ChangeRand()
{
	srand(time(NULL));
}

class Map
{
private:
	

	std::unordered_set<Circle*> m_circles;
	CImg<unsigned char> m_img;
	int m_greyLvl;
	int m_size;
	bool m_isGenerated;

	int CheckGreyLevel()
	{
		int average = 0;
		for (const auto& pixel : m_img)
			average += (int)pixel;
		return (int)nearbyint(average / ( m_img.width() * m_img.height() ) );
	}

	void AddCircle()
	{

	}

public:
	Map() {};
	Map(int _size, int _greylvl) :m_img(_size, _size, 0, 1, 0), m_size(_size), m_greyLvl(_greylvl) {};

	const std::unordered_set<Circle*>& circles() const { return m_circles; }
	const CImg<unsigned char>& img() const { return m_img; }
	int greyLvl() const { return m_greyLvl; }
	int size() const { return m_size; }
	bool isGenerated() const { return m_isGenerated; }

	void Generate(const Map* _precedingMap, const Map* _precedingToneMap )
	{
		if(_precedingMap != nullptr)
			m_circles.insert(_precedingMap->m_circles.begin(), _precedingMap->m_circles.end());
		if(_precedingToneMap != nullptr)
			m_circles.insert(_precedingToneMap->m_circles.begin(), _precedingToneMap->m_circles.end());

		/*if(_precedingMap != nullptr || _precedingToneMap != nullptr)
			for (const auto& circle : m_circles)
				m_img.draw_circle(circle.position().x, circle.position().y, circle.radius(), color_white, 1.).blur(circle.hardness());*/

		/*while ( CheckGreyLevel() < m_greyLvl )
		{

		}*/

		std::cout << "Map " << m_size << " tone " << m_greyLvl << " generated." << std::endl;
		m_isGenerated = true;
	}
};

class Tone
{
private :
	std::vector<Map> m_maps;
	int m_greylvl;

public :
	std::vector<Map> maps() const { return m_maps; }
	int greylvl() const { return m_greylvl; }

	void Generate( const int _maxMapSize, const Tone* _precedingTone )
	{
		for (int size = 16; size < _maxMapSize; size *= 2)
		{
			Map newMap(size, m_greylvl);
			m_maps.push_back(newMap);
		}

		int i(0);
		for (std::vector<Map>::iterator it = m_maps.begin(), previt = _precedingTone->maps().begin(); it != m_maps.end(); ++it, ++i)
		{
			if (it == m_maps.begin())
				it->Generate(nullptr, (_precedingTone == nullptr) ? nullptr : &(*previt) );
			else
				it->Generate( &(*(it - 1)) , (_precedingTone == nullptr) ? nullptr : &(*previt));
		}
		std::cout << "tone " << m_greylvl << " generated." << std::endl;
	}

	void Save()
	{

	}
};

int main() {
	/*CImg<unsigned char> image(500,400,1,3,50), visu(500, 400, 1, 3, 0);

	const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };

	image.blur(2.5);
	CImgDisplay main_disp(image, "Click a point"), draw_disp(visu, "Intensity profile");

	while (!main_disp.is_closed() && !draw_disp.is_closed()) {
		main_disp.wait();
		if (main_disp.button() && main_disp.mouse_y() >= 0) {
			const int y = main_disp.mouse_y();
			visu.fill(0).draw_graph(image.get_crop(0, y, 0, 0, image.width() - 1, y, 0, 0), red, 1, 1, 0, 255, 0);
			visu.draw_graph(image.get_crop(0, y, 0, 1, image.width() - 1, y, 0, 1), green, 1, 1, 0, 255, 0);
			visu.draw_graph(image.get_crop(0, y, 0, 2, image.width() - 1, y, 0, 2), blue, 1, 1, 0, 255, 0).display(draw_disp);
		}
	}*/

	int radius;
	float hardness;
	int nbOfGreys;
	int minGrey;
	int maxGrey;
	int maxMapSize;


	std::cout << "Nb of greys you want :" << std::endl;
	do { std::cin >> nbOfGreys; } while (nbOfGreys < 1 || nbOfGreys > 10);
	std::cout << nbOfGreys << std::endl;
	std::cout << "min grey value :" << std::endl;
	do { std::cin >> minGrey; } while (minGrey < 0 || minGrey > 255);
	std::cout << "max grey value :" << std::endl;
	do { std::cin >> maxGrey; } while (maxGrey < minGrey || maxGrey > 255);
	std::cout << "max map size :" << std::endl;
	do { std::cin >> maxMapSize; } while (maxMapSize < 16 || !(maxMapSize & (maxMapSize - 1)) == 0);
	std::cout << "Radius :" << std::endl;
	do { std::cin >> radius; } while (radius < 1);
	std::cout << "Hardness : [0-1]" << std::endl;
	do { std::cin >> hardness; } while (hardness < 0 || hardness > 1);
	
	std::vector<int> greyLvls;
	std::vector<Tone> tones;

	//int greyRange(255 - maxGrey);

	std::vector<CImg<unsigned char>> tests;
	std::vector<CImgDisplay> displays;

	for (int i = minGrey; i < maxGrey; i += (maxGrey / nbOfGreys) + minGrey)
	{//à l'envers ... du plus clair au plus foncé
		greyLvls.push_back(i);
		CImg<unsigned char> test(256, 256, 1, 1, i);
		tests.push_back(test);
		CImgDisplay display(256, 246, std::to_string(i).c_str());
		displays.push_back(display);
		tests.back().display(displays.back());
	}

	CImgDisplay rslt_display(256, 256, "Rslt");

	//tamOptions opt;

	while (!rslt_display.is_closed() /*&& !opt_display.is_closed() */)
	{

	}

	return 0;
}
