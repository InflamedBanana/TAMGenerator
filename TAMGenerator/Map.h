#pragma once

#ifndef DEF_MAP
#define DEF_MAP

#include <CImg/CImg.h>
#include <memory>
#include <vector>

#include <array>

#include "Position.h"
#include "Shapes.h"

typedef std::shared_ptr<cimg_library::CImg<unsigned char>> CImgSharedPtr;
namespace CILib = cimg_library;

struct Tone
{
	std::vector<Circle> circles;
	int greyLevel;

	Tone() : circles( 0 ), greyLevel( 0 ) {}
};

struct MipMap
{
	CImgSharedPtr image;
	int size;
	std::vector<Tone> redTone;
	std::vector<Tone> greenTone;
	std::vector<Tone> blueTone;

	MipMap() : image( nullptr ), size( 0 ), redTone( 0 ), greenTone( 0 ), blueTone( 0 ) {}
};

class Map
{
private:
	MipMap m_mainImage;
	std::vector<MipMap> m_mipMaps;

	Map();
	~Map();

	MipMap GenerateMipMap( int _size );
	//void GenerateTone();
	bool CheckTone();

public:
	const MipMap getMainImage() const { return m_mainImage; };
	const MipMap getMipMap( const int _index ) const { return m_mipMaps[ _index ]; }
	const std::vector<MipMap> getMipMaps() const { return m_mipMaps; }

	Map Create( const int _size, const int _firstGreyLevel, const int _secondGreyLevel, const int _thirdGreyLevel );
};

#endif // !DEF_MAP
