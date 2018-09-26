#include "TamOptions.h"
#include <iostream>

int TamOptions::circleRadius = 0;

int TamOptions::maxMapSize = 0;
char TamOptions::greyLvl = 0;
int TamOptions::minGrey = 0;
int TamOptions::maxGrey = 0;
int TamOptions::nbOfGreys = 0;
int TamOptions::radiusVariation = 0;

void TamOptions::SetOptions()
{
	std::cout << "Nb of greys you want : (3, 6, 9 ...)" << std::endl;
	do { std::cin >> nbOfGreys; if(nbOfGreys > 0 && nbOfGreys % 3 == 0) break; } while (true);

	std::cout << "Darkest grey value ( 5-255 ):" << std::endl;
	do { std::cin >> minGrey; if (minGrey >= 5 && minGrey <= 255) break; } while (true);

	std::cout << "Lightest grey value ( Darkest-255 ):" << std::endl;
	do { std::cin >> maxGrey; if (maxGrey > minGrey && maxGrey <= 255) break; } while (true);

	std::cout << "Map size ( > 16, POT ) :" << std::endl;
	do { std::cin >> maxMapSize; if (maxMapSize > 16 && (maxMapSize & (maxMapSize - 1)) == 0) break; } while (true);

	std::cout << "Dot Radius ( > 0 ):" << std::endl;
	do { std::cin >> circleRadius; if (circleRadius > 0) break; } while (true);

	std::cout << "Radius Variation ( in % ):" << std::endl;
	do { std::cin >> radiusVariation; if (radiusVariation >= 0) break; } while (true);
}