#pragma once

#ifndef DEF_TAM_OPTIONS
#define DEF_TAM_OPTIONS

class TamOptions
{
private:
	TamOptions();

public:
	static unsigned int maxMapSize;
	static unsigned char greyLvl;
	static unsigned int minGrey;
	static unsigned int maxGrey;
	static unsigned int nbOfGreys;
	static int circleRadius;
	static int radiusVariation;

	static void SetOptions(); 
};

typedef TamOptions options;

#endif // !DEF_TAM_OPTIONS
