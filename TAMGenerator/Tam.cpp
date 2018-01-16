#include "Tam.h"
#include <random>
#include <iostream>
#include <string>
#include "TamOptions.h"
#include <cmath>

using namespace std;

Point::Point() {}
Point::Point( Position _position ) : m_position( _position ) {}
Point::~Point() {}


Circle::Circle( unsigned int _radius ) : Point(), m_radius( _radius ) {}
Circle::Circle( Position _position, unsigned int _radius )
	: Point( _position ), m_radius( _radius ) {}


Map::Map() : m_size( 1 ), m_greyLvl( 0 ), m_isGenerated(false), m_img( make_shared<ci::CImg<char>>(1, 1, 1, 1, 255)) {}
Map::Map( int _size, int _greylvl ) :
	m_tileOffset( ( ( options::circleRadius * 2 ) + 1 ) * ( ( options::radiusVariation > 0 ) ?
	( options::circleRadius* ( options::radiusVariation / 100 ) + 1 ) : 1 ) ),
	m_size( _size ), m_greyLvl( _greylvl ), m_isGenerated(false),
	m_img( make_shared<ci::CImg<char>>(_size + m_tileOffset * 2, _size + m_tileOffset * 2, 1, 1, 255) )
{}

Map::~Map()
{}


void Map::TilePoint( const Position& _pos, const int _radius )
{
	const int lowerTile( m_tileOffset + _radius + 1 );
	const int upperTile( m_tileOffset + m_size - _radius - 1 );

	int xPos(_pos.x), yPos(_pos.y); //declare them as parameters ?

	if( xPos >= lowerTile && yPos >= lowerTile &&
		xPos <= upperTile && yPos <= lowerTile )
		return;

	Position tiledPos( 0.f, 0.f );

	if( xPos < lowerTile )
		m_img->draw_circle( xPos + m_size, yPos, _radius, color_black, 1.f );
	else if( xPos > upperTile )
		m_img->draw_circle( xPos - m_size, yPos, _radius, color_black, 1.f );

	if( yPos < lowerTile )
		m_img->draw_circle( xPos, yPos + m_size, _radius, color_black, 1.f );
	else if( yPos > upperTile )
		m_img->draw_circle( xPos, yPos - m_size, _radius, color_black, 1.f );

	if( xPos < lowerTile && yPos < lowerTile )
		m_img->draw_circle( xPos + m_size, yPos + m_size, _radius, color_black, 1.f );
	else if( xPos > upperTile && yPos < lowerTile )
		m_img->draw_circle( xPos - m_size, yPos + m_size, _radius, color_black, 1.f );
	else if( xPos < lowerTile && yPos > upperTile )
		m_img->draw_circle( xPos + m_size, yPos - m_size, _radius, color_black, 1.f );
	else if( xPos > upperTile && yPos > upperTile )
		m_img->draw_circle( xPos - m_size, yPos - m_size, _radius, color_black, 1.f );
}

int Map::CheckGreyLevel() const
{
	int average = 0;

	for( int x = m_tileOffset + 1; x < m_size + 1 + m_tileOffset; ++x )
		for( int y = m_tileOffset + 1; y < m_size + 1 + m_tileOffset; ++y )
			average += (int)m_img->atXY( x, y, 0, 0 );

	return static_cast<int>( nearbyint( average / ( m_size *  m_size ) ) );
}

void Map::Generate( const Map* _precedingMap, const Map* _precedingToneMap )
{
	if( _precedingMap != nullptr )
		m_circles.insert( _precedingMap->circles().begin(), _precedingMap->circles().end() );

	if( _precedingToneMap != nullptr )
		m_circles.insert( _precedingToneMap->circles().begin(), _precedingToneMap->circles().end() );

	for( const auto& point : m_circles )
	{
		Position pos( point->position() );
		pos.ToSize( m_size, m_tileOffset );
		m_img->draw_circle( pos.x, pos.y, point->radius(), color_black, 1.f );
		TilePoint( pos, point->radius() );
	}

	random_device rd;
	mt19937 rng( rd() );
	uniform_int_distribution<int> uni( 0, m_size - 1 );

	while( CheckGreyLevel() > m_greyLvl )
	{
		Position pointPos;
		do
		{
			pointPos.x = uni( rng );
			pointPos.y = uni( rng );
		} while( m_img->atXY( pointPos.x + m_tileOffset, pointPos.y + m_tileOffset, 0, 0 ) < 240 );

		short int sign( 1 );
		const short int variation( rand() % ( static_cast<int>(nearbyint( options::circleRadius * ( options::radiusVariation / 100.f ) + 1 ) )) );

		if( options::radiusVariation > 0 )
			sign = ( rand() % 2 + 1 ) < 1 ? -1 : 1;

		short int radius( options::circleRadius + variation * sign );

		m_img->draw_circle( pointPos.xInt() + m_tileOffset, pointPos.yInt() + m_tileOffset, radius, color_black, 1.f );

		Position tilePos( pointPos.x + m_tileOffset, pointPos.y + m_tileOffset );
		TilePoint( tilePos, radius );

		pointPos.Normalize( m_size );

		m_circles.insert( new Circle( pointPos, radius ) );
	}

	//const int center = m_size / 2;
	m_img->crop( m_tileOffset + 1, m_tileOffset + 1, m_tileOffset + m_size, m_tileOffset + m_size );

	SaveMap();

	m_isGenerated = true;
}

void Map::Resize( const float _resizeValue, const Map& _higherMap )
{
	int newMapSize( static_cast<int>(m_size * _resizeValue ));

	CImgSharedPtr lowerImg(make_shared<ci::CImg<char>>());
	lowerImg->assign( newMapSize, newMapSize, 1, 1 );

	ci::CImg<char> higherImg( _higherMap.img()->get_resize( _resizeValue * -100, _resizeValue * -100, -100, -100, 3 ) , false );

	for( int x = 0; x < newMapSize; ++x )
		for( int y = 0; y < newMapSize; ++y )
			( *lowerImg )( x, y, 0, 0 ) = higherImg( x, y, 0, 0 );
	
	std::cout << "1 : Map size : " << m_size << " | lower img size : " << lowerImg->width() << " | lower img data size : " << lowerImg->size() << endl;

	m_img = /*move(*/lowerImg/*)*/;
	m_size *= _resizeValue;
	std::cout << "2 : Map size : " << m_size << " | lower img size : " << lowerImg->width() << " | lower img data size : " << lowerImg->size() << endl;
	std::cout << "2 : Map size : " << m_size << " | img size : " << lowerImg->width() << " | img data size : " << lowerImg->size() << endl;
}

void Map::SaveMap()
{
	string file( "" );
	file.append( to_string( m_greyLvl ) );
	file.append( "_" );
	file.append( to_string( m_size ) );
	file.append( ".bmp" );

	m_img->save( file.c_str() );

	//cout << "Map " << m_size << " tone " << m_greyLvl << " saved !" << endl;
}

Tone::Tone( int _greyLvl ) : m_greylvl( _greyLvl ) {};

void Tone::Generate( const int _maxMapSize, const Tone* _precedingTone )
{
	for( int size = 1; size < _maxMapSize * 2; size *= 2 )
	{
		Map newMap( size, m_greylvl );
		m_maps.push_back( newMap );
	}

	int i( 0 );
	for( vector<Map>::iterator it = m_maps.begin() + 4; it != m_maps.end(); ++it, ++i )
	{
		if( i == 0 )
			it->Generate( nullptr, ( _precedingTone == nullptr ) ? nullptr : &_precedingTone->maps()[ i ] );
		else
			it->Generate( &( *( it - 1 ) ), ( _precedingTone == nullptr ) ? nullptr : &_precedingTone->maps()[ i ] );
	}

	ComputeLowerMipMaps();

	//cout << "tone " << m_greylvl << " generated with " << m_maps.size() << " maps." << endl;
}

void Tone::Save()
{
	for( auto& map : m_maps )
		map.SaveMap();
}

void Tone::ComputeLowerMipMaps()
{
	for( int i = 3; i >= 0; i-- )
	{
		Map lower( m_maps[ 4 ].size(), m_greylvl );

		lower.Resize( 1 / pow( 2, 4 - i ), m_maps[ 4 ] );

		std::cout << "3 : Map size : " << m_maps[ i ].size() << " | img data size : " << m_maps[ i ].img()->size() << endl;

		lower.SaveMap();
	}
}