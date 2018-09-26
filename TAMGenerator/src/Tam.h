#pragma once

#ifndef DEF_TAM
#define DEF_TAM

#include <unordered_set>
#include <vector>
#include "Position.h"
#include <CImg/CImg.h>
#include <memory>
#include "Shapes.h"

namespace ci = cimg_library;

const unsigned char color_black[] = { 1, 1, 1 };
const unsigned char color_white[] = { 255, 255, 255 };
const unsigned char color_red[] = { 255, 1, 1 };
const unsigned char color_green[] = { 1, 255, 1 };
const unsigned char color_blue[] = { 1, 1, 255 };
const unsigned char color_cyan[] = { 1, 255, 255 };

typedef std::shared_ptr<ci::CImg<unsigned char>> CImgSharedPtr;

class Map
{
private:
	Map();

	std::unordered_set<Circle*> m_circles;
	//ci::CImg<unsigned char>* m_img;

	CImgSharedPtr m_img;
	int m_greyLvl;
	int m_size;
	bool m_isGenerated;
	int m_tileOffset;

	void TilePoint( const Position& _pos, const int _radius );
	int CheckGreyLevel() const;

public:
	~Map();
	Map( int _size, int _greylvl );
	//Map(const Map& _map);

	const std::unordered_set<Circle*>& circles() const { return m_circles; }
	const CImgSharedPtr& img() const { return m_img; }
	const ci::CImg<unsigned char>* imgPtr() const { return m_img.get(); }
	//void img(const ci::CImg<unsigned char>& _img) { m_img = _img; }

	int greyLvl() const { return m_greyLvl; }
	int size() const { return m_size; }
	bool isGenerated() const { return m_isGenerated; }

	void Resize( const float _resizeValue, const Map* _higherMap );
	void Generate( const Map* _precedingMap, const Map* _precedingToneMap );
	void SaveMap();
};

class Tone
{
private:
	std::vector<Map*> m_maps;
	int m_greylvl;

	void ComputeLowerMipMaps();

public:
	Tone( int _greyLvl );
	const std::vector<Map*> maps() const { return m_maps; }
	int greylvl() const { return m_greylvl; }

	void Generate( const int _maxMapSize, const Tone* _precedingTone = nullptr );

	void Save();
};

#endif // !DEF_TAM
