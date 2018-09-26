#pragma once

#ifndef DEF_TAM_OPTIONS
#define DEF_TAM_OPTIONS

class TamOptions
{
private:
	TamOptions();

public:
	static int maxMapSize;
	static char greyLvl;
	static int minGrey;
	static int maxGrey;
	static int nbOfGreys;
	static int circleRadius;
	static int radiusVariation;

	static void SetOptions(); 
};

typedef TamOptions options;

#endif // !DEF_TAM_OPTIONS
