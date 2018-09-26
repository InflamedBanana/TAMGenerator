#include "Tam.h"
#include <random>
#include <iostream>
#include <string>
#include "TamOptions.h"
#include <cmath>

using namespace std;

Map::Map() : m_size( 1 ), m_greyLvl( 0 ), m_isGenerated( false ), m_img( make_shared<ci::CImg<unsigned char>>( 1, 1, 1, 3, 255 ) ) {}

Map::Map( int _size, int _greylvl ) :
	m_tileOffset( ( ( options::circleRadius * 2 ) + 1 ) * ( ( options::radiusVariation > 0 ) ? ( options::circleRadius* ( options::radiusVariation / 100 ) + 1 ) : 1 ) ),
	m_size( _size ), m_greyLvl( _greylvl ), m_isGenerated( false ),
	m_img( nullptr )
{
	m_img = make_shared<ci::CImg<unsigned char>>( _size + m_tileOffset * 2, _size + m_tileOffset * 2, 1, 3, 255 );
}

Map::~Map()
{}


void Map::TilePoint( const Position& _pos, const int _radius )
{
	const int lowerTile( m_tileOffset + _radius + 1 );
	const int upperTile( m_tileOffset + m_size - _radius - 1 );

	if( _pos.x >= lowerTile && _pos.y >= lowerTile &&
		_pos.x <= upperTile && _pos.y <= lowerTile )
		return;

	if( _pos.x < lowerTile )
		m_img->draw_circle( _pos.x + m_size, _pos.y, _radius, color_black, 1.f );
	else if( _pos.x > upperTile )
		m_img->draw_circle( _pos.x - m_size, _pos.y, _radius, color_black, 1.f );

	if( _pos.y < lowerTile )
		m_img->draw_circle( _pos.x, _pos.y + m_size, _radius, color_black, 1.f );
	else if( _pos.y > upperTile )
		m_img->draw_circle( _pos.x, _pos.y - m_size, _radius, color_black, 1.f );

	if( _pos.x < lowerTile && _pos.y < lowerTile )
		m_img->draw_circle( _pos.x + m_size, _pos.y + m_size, _radius, color_black, 1.f );
	else if( _pos.x > upperTile && _pos.y < lowerTile )
		m_img->draw_circle( _pos.x - m_size, _pos.y + m_size, _radius, color_black, 1.f );
	else if( _pos.x < lowerTile && _pos.y > upperTile )
		m_img->draw_circle( _pos.x + m_size, _pos.y - m_size, _radius, color_black, 1.f );
	else if( _pos.x > upperTile && _pos.y > upperTile )
		m_img->draw_circle( _pos.x - m_size, _pos.y - m_size, _radius, color_black, 1.f );
}

int Map::CheckGreyLevel() const
{
	int average = 0;

	for( int x = m_tileOffset + 1; x < m_size + 1 + m_tileOffset; ++x )
		for( int y = m_tileOffset + 1; y < m_size + 1 + m_tileOffset; ++y )
		{
			average += m_img->atXY( x, y, 0, 0 );
		}
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

	Position pPosition;

	while( CheckGreyLevel() > m_greyLvl )
	{
		do
		{
			pPosition.x = uni( rng );
			pPosition.y = uni( rng );
		} while( m_img->atXY( pPosition.x + m_tileOffset, pPosition.y + m_tileOffset, 0, 0 ) < 240 );

		short int sign( 1 );
		const short int variation( rand() % ( static_cast<int>( nearbyint( options::circleRadius * ( options::radiusVariation / 100.f ) + 1 ) ) ) );

		if( options::radiusVariation > 0 )
			sign = ( rand() % 2 + 1 ) < 1 ? -1 : 1;

		short int radius( max( options::circleRadius + variation * sign, 1 ) );

		m_img->draw_circle( pPosition.xInt() + m_tileOffset, pPosition.yInt() + m_tileOffset, radius, color_black, 1.f );

		Position tilePos( pPosition.x + m_tileOffset, pPosition.y + m_tileOffset );
		TilePoint( tilePos, radius );

		pPosition.Normalize( m_size );

		m_circles.insert( new Circle( pPosition, radius ) );
	}

	m_img->crop( m_tileOffset + 1, m_tileOffset + 1, m_tileOffset + m_size, m_tileOffset + m_size );

	cout << "Img tone : " << m_greyLvl << ", size : " << m_size << " -> Generated !" << endl;

	m_isGenerated = true;
}

void Map::Resize( const float _resizeValue, const Map* _higherMap )
{
	int newMapSize( static_cast<int>( m_size * _resizeValue ) );

	CImgSharedPtr lowerImg( make_shared<ci::CImg<unsigned char>>() );
	lowerImg->assign( newMapSize, newMapSize, 1, 3 );

	ci::CImg<unsigned char> higherImg( _higherMap->img()->get_resize( _resizeValue * -100, _resizeValue * -100, -100, -100, 3 ), false );

	for(int c = 0; c < 3; c++ )
		for( int x = 0; x < newMapSize; ++x )
			for( int y = 0; y < newMapSize; ++y )
				( *lowerImg )( x, y, 0, c ) = higherImg( x, y, 0, c );

	m_img = lowerImg;
	m_size *= _resizeValue;
}

void Map::SaveMap()
{
	string file( "" );
	file.append( to_string( m_greyLvl ) );
	file.append( "_" );
	file.append( to_string( m_size ) );
	file.append( ".bmp" );

	m_img->save( file.c_str() );
}

Tone::Tone( int _greyLvl ) : m_greylvl( _greyLvl ), m_maps(0) {};

void Tone::Generate( const int _maxMapSize, const Tone* _precedingTone )
{
	for( int size = 1; size < _maxMapSize * 2; size *= 2 )
	{
		Map* newMap = new Map( size, m_greylvl );
		m_maps.push_back( newMap );
	}

	int i( 4 );


	for( vector<Map*>::iterator it = m_maps.begin() + 4; it != m_maps.end(); ++it, ++i )
	{
		Map* precedingTone = (_precedingTone == nullptr) ? nullptr : _precedingTone->maps()[i];

		if( i == 4 )
			(*it)->Generate( nullptr, precedingTone);
		else
			(*it)->Generate( /*&(*/ *( it - 1 ) /*)*/, precedingTone);
	}

	ComputeLowerMipMaps();
}

void Tone::Save()
{
	for( auto& map : m_maps )
		map->SaveMap();
}

void Tone::ComputeLowerMipMaps()
{
	for( int i = 3; i >= 0; i-- )
	{
		//Map lower( m_maps[ 4 ]->size(), m_greylvl );
		Map* lower = new Map( m_maps[4]->size(), m_greylvl );
		lower->Resize( 1 / pow( 2, 4 - i ), m_maps[ 4 ] );

		m_maps[ i ] = lower;

		cout << "Img tone : " << m_maps[ i ]->greyLvl() << ", size : " << m_maps[i]->size() << " -> Generated !" << endl;


		//lower.SaveMap();
	}
}