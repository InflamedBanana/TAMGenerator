#include "TamOptions.h"
#include <iostream>

int TamOptions::circleRadius = 0;

unsigned int TamOptions::maxMapSize = 0;
unsigned char TamOptions::greyLvl = 0;
unsigned int TamOptions::minGrey = 0;
unsigned int TamOptions::maxGrey = 0;
unsigned int TamOptions::nbOfGreys = 0;
int TamOptions::radiusVariation = 0;

void TamOptions::SetOptions()
{
	std::cout << "Nb of greys you want :" << std::endl;
	do { std::cin >> nbOfGreys; } while (nbOfGreys < 2 || nbOfGreys > 10);
	std::cout << "min grey value :" << std::endl;
	do { std::cin >> minGrey; } while (minGrey < 0 || minGrey > 255);
	std::cout << "max grey value :" << std::endl;
	do { std::cin >> maxGrey; } while (maxGrey < minGrey || maxGrey > 255);
	std::cout << "max map size :" << std::endl;
	do { std::cin >> maxMapSize; } while (maxMapSize < 16 || !(maxMapSize & (maxMapSize - 1)) == 0);
	std::cout << "Circle Radius :" << std::endl;
	do { std::cin >> circleRadius; } while (circleRadius < 1);
	std::cout << "Radius Variation ( in % ):" << std::endl;
	std::cin >> radiusVariation;
}