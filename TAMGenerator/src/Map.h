#pragma once

#ifndef DEF_MAP
#define DEF_MAP

#include <CImg/CImg.h>
#include <memory>
#include <vector>

#include "Position.h"
#include "Shapes.h"

typedef std::shared_ptr<cimg_library::CImg<unsigned char>> CImgSharedPtr;

#define CImgMakeShared std::make_shared<CILib::CImg<unsigned char>>

namespace CILib = cimg_library;

///*const*/ unsigned char color_black[] = { 1, 1, 1 };
///*const*/ unsigned char color_white[] = { 255, 255, 255 };
///*const*/ unsigned char color_red[] = { 255, 1, 1 };
///*const*/ unsigned char color_green[] = { 1, 255, 1 };
///*const*/ unsigned char color_blue[] = { 1, 1, 255 };
///*const*/ unsigned char color_cyan[] = { 1, 255, 255 };

struct Tone
{
	std::vector<Circle> circles;
	int greyLevel;

	Tone() : circles( 0 ), greyLevel( 0 ) {}
	Tone( int _greyLevel ) : greyLevel( greyLevel ) {}
	Tone( int _greyLevel, std::vector<Circle> _circles ) : greyLevel( _greyLevel), circles( _circles ) {}
};

struct MipMap
{
	CImgSharedPtr image;
	int size;
	Tone redTone;
	Tone greenTone;
	Tone blueTone;

	MipMap() : image( nullptr ), size( 0 ), redTone(), greenTone(), blueTone() {}
	MipMap(const int _size, const CImgSharedPtr& _image, const Tone& _redTone, const Tone& _greenTone, const Tone& _blueTone ) : size(_size), image(_image), redTone(_redTone), greenTone(_greenTone), blueTone(_blueTone)
	{
	}
};

class Map
{
private:
	//MipMap m_mainImage;
	std::vector<MipMap> m_mipMaps;
	int m_size;

	Map() : m_mipMaps( 0 ) {};


	MipMap GenerateMipMap(const  int _size, const int _firstGreyLevel, const int _secondGreyLevel, const int _thirdGreyLevel, const MipMap* _preceding = nullptr );
	MipMap GenerateLowerMipMap( const int _size, const MipMap& _ref );
	void TilePoint( const CImgSharedPtr& _img, const Position& _pos, const int _offset, const int _radius, const unsigned char _color[] );
	//void GenerateTone();
	bool CheckTone( const CImgSharedPtr& _img, const int _posOffset, const int _greyLevel, const int _channel ) const;

public:
	Map( const int _size, const int _firstGreyLevel, const int _secondGreyLevel, const int _thirdGreyLevel );
	~Map() {};

	const MipMap getMainImage() const { return m_mipMaps.back(); };
	const MipMap getMipMap( const int _index ) const { return m_mipMaps[ _index ]; }
	const std::vector<MipMap> getMipMaps() const { return m_mipMaps; }
	void SaveMipMapToBMP( const MipMap& _mipMap ) const;

	/*static Map Create( const int _size, const int _firstGreyLevel, const int _secondGreyLevel, const int _thirdGreyLevel );*/
};

#endif // !DEF_MAP
