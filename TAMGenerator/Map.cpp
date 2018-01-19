#include "Map.h"
#include <math.h>
#include "TamOptions.h"
#include <random>
#include <string>

#include <iostream>

using namespace std;

/*const*/ unsigned char color_black[] = { 1, 1, 1 };
/*const*/ unsigned char color_white[] = { 255, 255, 255 };
/*const*/ unsigned char color_red[] = { 255, 1, 1 };
/*const*/ unsigned char color_green[] = { 1, 255, 1 };
/*const*/ unsigned char color_blue[] = { 1, 1, 255 };
/*const*/ unsigned char color_cyan[] = { 1, 255, 255 };

Map::Map( const int _size, const int _firstGreyLevel, const int _secondGreyLevel, const int _thirdGreyLevel ) :
	/*m_mipMaps( log2( m_size ) + 1 ),*/ m_size( _size )
{
	int nbOfMipMaps( log2( m_size ) + 1 );
	MipMap* last = nullptr;

	for( int i = 4; i < nbOfMipMaps; ++i )
	{
		m_mipMaps.push_back( GenerateMipMap( pow( 2, i ), _firstGreyLevel, _secondGreyLevel, _thirdGreyLevel, last ) );

		last = &m_mipMaps[ i ];
	}

	/*for( int i = 3; i >= 0; --i )
	{
		m_mipMaps[ i ] = GenerateLowerMipMap( pow( 2, i ), m_mipMaps[ 4 ] );
	}*/
}

MipMap Map::GenerateMipMap( const int _size, const int _firstGreyLevel, const int _secondGreyLevel, const int _thirdGreyLevel, const MipMap* _preceding )
{

	int offset = ( 2 * options::circleRadius ) + 1 *
		( options::radiusVariation > 0 ? ( options::circleRadius* ( options::radiusVariation / 100 ) + 1 ) : 1 );

	CImgSharedPtr newImage( CImgMakeShared( _size + offset * 2, _size + offset * 2, 1, 3, 255 ) );

	vector<Circle> redCircles( 0 );
	vector<Circle> greenCircles( 0 );
	vector<Circle> blueCircles( 0 );

	//put preceding circles in vectors & draw them

	random_device rd;
	mt19937 rng( rd() );
	uniform_int_distribution<int> uni( 0, m_size - 1 );

	Position pointPos( 0.f, 0.f );

	/*while( !CheckTone( newImage, offset, _firstGreyLevel, 0 ) )
	{
		do
		{
			pointPos.x = uni( rng );
			pointPos.y = uni( rng );
		} while( newImage->atXY( pointPos.xInt + offset, pointPos.yInt + offset, 0, 0 ) < 240 );

		short int sign( 1 );
		const short int variation( rand() % ( static_cast<int>( nearbyint( options::circleRadius * ( options::radiusVariation / 100.f ) + 1 ) ) ) );

		if( options::radiusVariation > 0 )
			sign = ( rand() % 2 + 1 ) < 1 ? -1 : 1;

		short int radius( options::circleRadius + variation * sign );

		newImage->draw_circle( pointPos.xInt() + offset, pointPos.yInt() + offset, radius, color_black, 1.f );

		Position tilePos( pointPos.x + offset, pointPos.y + offset );
		TilePoint( newImage, pointPos, offset, radius, color_black );

		pointPos.Normalize( m_size );

		Circle circle( pointPos, radius );

		redCircles.push_back( circle );
		greenCircles.push_back( circle );
		blueCircles.push_back( circle );
	}

	while( !CheckTone( newImage, offset, _secondGreyLevel, 1 ) )
	{
		do
		{
			pointPos.x = uni( rng );
			pointPos.y = uni( rng );
		} while( newImage->atXY( pointPos.xInt + offset, pointPos.yInt + offset, 0, 1 ) < 240 );

		short int sign( 1 );
		const short int variation( rand() % ( static_cast<int>( nearbyint( options::circleRadius * ( options::radiusVariation / 100.f ) + 1 ) ) ) );

		if( options::radiusVariation > 0 )
			sign = ( rand() % 2 + 1 ) < 1 ? -1 : 1;

		short int radius( options::circleRadius + variation * sign );

		newImage->draw_circle( pointPos.xInt() + offset, pointPos.yInt() + offset, radius, color_cyan, 1.f );

		Position tilePos( pointPos.x + offset, pointPos.y + offset );
		TilePoint( newImage, pointPos, offset, radius, color_cyan );

		pointPos.Normalize( m_size );

		Circle circle( pointPos, radius );

		greenCircles.push_back( circle );
		blueCircles.push_back( circle );
	}

	while( !CheckTone( newImage, offset, _thirdGreyLevel, 2 ) )
	{
		do
		{
			pointPos.x = uni( rng );
			pointPos.y = uni( rng );
		} while( newImage->atXY( pointPos.xInt + offset, pointPos.yInt + offset, 0, 2 ) < 240 );

		short int sign( 1 );
		const short int variation( rand() % ( static_cast<int>( nearbyint( options::circleRadius * ( options::radiusVariation / 100.f ) + 1 ) ) ) );

		if( options::radiusVariation > 0 )
			sign = ( rand() % 2 + 1 ) < 1 ? -1 : 1;

		short int radius( options::circleRadius + variation * sign );

		newImage->draw_circle( pointPos.xInt() + offset, pointPos.yInt() + offset, radius, color_blue, 1.f );

		Position tilePos( pointPos.x + offset, pointPos.y + offset );
		TilePoint( newImage, pointPos, offset, radius, color_blue );

		pointPos.Normalize( m_size );

		Circle circle( pointPos, radius );

		blueCircles.push_back( circle );
	}*/

	unsigned char* color( color_black );
	int channel (0);

	while( !CheckTone( newImage, offset, _thirdGreyLevel, 2 ) )
	{
		do
		{
			pointPos.x = uni( rng );
			pointPos.y = uni( rng );
		} while( newImage->atXY( pointPos.xInt() + offset, pointPos.yInt() + offset, 0, channel ) < 240 );

		short int sign( 1 );
		const short int variation( rand() % ( static_cast<int>( nearbyint( options::circleRadius * ( options::radiusVariation / 100.f ) + 1 ) ) ) );

		if( options::radiusVariation > 0 )
			sign = ( rand() % 2 + 1 ) < 1 ? -1 : 1;

		short int radius( options::circleRadius + variation * sign );

		newImage->draw_circle( pointPos.xInt() + offset, pointPos.yInt() + offset, radius, color, 1.f );

		Position tilePos( pointPos.x + offset, pointPos.y + offset );
		TilePoint( newImage, pointPos, offset, radius, color );

		pointPos.Normalize( m_size );

		Circle circle( pointPos, radius );

		if( !CheckTone( newImage, offset, _firstGreyLevel, 0 ) )
			redCircles.push_back( circle );
		else
		{
			color = color_cyan;
			channel = 1;
		}

		if( !CheckTone( newImage, offset, _secondGreyLevel, 1 ) )
			greenCircles.push_back( circle );
		else
		{
			color = color_blue;
			channel = 2;
		}

		blueCircles.push_back( circle );
	}

	Tone redTone( _firstGreyLevel, redCircles );
	Tone greenTone( _secondGreyLevel, greenCircles );
	Tone blueTone( _thirdGreyLevel, blueCircles );

	MipMap map( _size, newImage, redTone, greenTone, blueTone );
	SaveMipMapToBMP( map );
	return map;
}

bool Map::CheckTone( const CImgSharedPtr& _img, const int _posOffset, const int _greyLevel, const int _channel ) const
{
	int average = 0;

	for( int x = _posOffset + 1; x < m_size + 1 + _posOffset; ++x )
		for( int y = _posOffset + 1; y < m_size + 1 + _posOffset; ++y )
		{
			average += _img->atXY( x, y, 0, _channel );
		}
	cout << "channel " << _channel << "  greyLevel " << static_cast<int>( nearbyint( average / ( m_size *  m_size ) ) ) << endl;
	return static_cast<int>( nearbyint( average / ( m_size *  m_size ) ) ) <= _greyLevel;
}

void Map::TilePoint( const CImgSharedPtr& _img, const Position& _pos, const int _offset, const int _radius, const unsigned char _color[] )
{
	const int lowerTile( _offset + _radius + 1 );
	const int upperTile( _offset + m_size - _radius - 1 );

	if( _pos.x >= lowerTile && _pos.y >= lowerTile &&
		_pos.x <= upperTile && _pos.y <= lowerTile )
		return;

	if( _pos.x < lowerTile )
		_img->draw_circle( _pos.x + m_size, _pos.y, _radius, _color, 1.f );
	else if( _pos.x > upperTile )
		_img->draw_circle( _pos.x - m_size, _pos.y, _radius, _color, 1.f );

	if( _pos.y < lowerTile )
		_img->draw_circle( _pos.x, _pos.y + m_size, _radius, _color, 1.f );
	else if( _pos.y > upperTile )
		_img->draw_circle( _pos.x, _pos.y - m_size, _radius, _color, 1.f );

	if( _pos.x < lowerTile && _pos.y < lowerTile )
		_img->draw_circle( _pos.x + m_size, _pos.y + m_size, _radius, _color, 1.f );
	else if( _pos.x > upperTile && _pos.y < lowerTile )
		_img->draw_circle( _pos.x - m_size, _pos.y + m_size, _radius, _color, 1.f );
	else if( _pos.x < lowerTile && _pos.y > upperTile )
		_img->draw_circle( _pos.x + m_size, _pos.y - m_size, _radius, _color, 1.f );
	else if( _pos.x > upperTile && _pos.y > upperTile )
		_img->draw_circle( _pos.x - m_size, _pos.y - m_size, _radius, _color, 1.f );
}

MipMap Map::GenerateLowerMipMap( const int _size, const MipMap& _ref )
{
	MipMap temp;
	return temp;
}

void Map::SaveMipMapToBMP(const MipMap& _mipMap) const
{
	
		string file( "" );
		file.append( to_string( _mipMap.greenTone.greyLevel ) );
		file.append( "_" );
		file.append( to_string( _mipMap.size ) );
		file.append( ".bmp" );

		_mipMap.image->save( file.c_str() );
	
}