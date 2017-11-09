#include <CImg/CImg.h>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>

using namespace cimg_library;

struct tamOptions
{
	unsigned int mapSize;
	unsigned int maxMapSize;
	unsigned int minMapSize;
	unsigned char greyLvl;
};

struct Position
{
	float x;
	float y;
};

class Object
{
public :
	Object() : m_position(), m_opacity(1), m_hardness(1){}
	Object(Position _position) :m_position(_position), m_opacity(1), m_hardness(1) {}
	Object(float _opacity, float _hardness) :m_opacity(_opacity), m_hardness(_hardness){}
	Object(Position _position, float _opacity, float _hardness) : m_position(_position), m_opacity(_opacity), m_hardness(_hardness) {}

protected :
	float m_opacity;
	float m_hardness;
	Position m_position;


public:
	float opacity() const { return m_opacity; }
	float opacity(float value) { if (value > 1)m_opacity = 1; else if (value < 0) m_opacity = 0; else m_opacity = value; }
	float hardness() const { return m_hardness; }
	float hardness(float value){ if (value > 1) m_hardness= 1; else if (value < 0) m_hardness= 0; else m_hardness = value; }
	Position position() { return m_position; }
};

class Point : public Object
{
private:
	unsigned int m_radius;

public:
	int radius() const { return m_radius; }
	int radius(int value) { m_radius = value; }

	Point(unsigned int _radius): Object(), m_radius(_radius) {}
	Point(unsigned int _radius, float _opacity, float _hardness ): Object( _opacity,_hardness ), m_radius(_radius) {}
	Point(Position _position, unsigned int _radius, float _opacity, float _hardness) : Object(_position, _opacity, _hardness), m_radius(_radius) {}
};

class Line : public Object
{
private:
	unsigned int m_length;

public:
	unsigned int length() const { return m_length; }
	unsigned int length(unsigned int value) { m_length = value; }
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
	float opacity;
	float hardness;

	std::cout << "Radius :" << std::endl;
	std::cin >> radius;
	std::cout << "Opacity : [0-1]" << std::endl;
	std::cin >> opacity;
	std::cout << "Hardness : [0-1]" << std::endl;
	std::cin >> hardness;


	CImg<unsigned char> rsltImage(32, 32, 1, 1, 0);
	const unsigned char black[] = { 0,0,0 }, white[] = { 255,255,255 }, blue[] = { 0,0,255 };
	Position pos;
	pos.x = pos.y = .5f;
	
	Point p( pos, radius, opacity, 1.f-hardness);
	

	CImgDisplay rslt_display(256, 256, "Rslt")/*, opt_display( 500, 250, "Options" )*/;

	rsltImage.draw_circle((int)nearbyint(rsltImage.width() * p.position().x), (int)nearbyint(rsltImage.height() * p.position().y), p.radius(), white, p.opacity() ).blur(p.hardness()).display(rslt_display);
	
	int average = 0;

	for (const auto& pixel : rsltImage)
		if ((int)pixel > 0)
		{
			std::cout << (int)pixel << std::endl;
			average += (int)pixel;
		}

	average /= (int)nearbyint(rsltImage.width() * rsltImage.height());

	std::cout << "average value " << average << std::endl;

	tamOptions opt;

	while (!rslt_display.is_closed() /*&& !opt_display.is_closed() */)
	{

	}

	return 0;
}
