#include <vector>
#include <iostream>
#include <ctime>

#include "dds.h"
#include <fstream>
#include <iostream>
#include <string>

#include "Position.h"
#include "Tam.h"
#include "TamOptions.h"

using namespace std;

namespace dx = DirectX;

int main()
{
	srand( time( 0 ) );

	options::SetOptions();

	vector<Tone> tones;

	for( int indice = 0, greyValue = 255; indice < options::nbOfGreys; ++indice, greyValue -= ( ( 255 - options::minGrey ) / ( options::nbOfGreys - 1 ) ) )
	{
		Tone tone( greyValue );
		tones.push_back( tone );
	}

	for( vector<Tone>::iterator toneIt = tones.begin(); toneIt != tones.end(); ++toneIt )
	{
		if( toneIt == tones.begin() )
			toneIt->Generate( options::maxMapSize );
		else
			toneIt->Generate( options::maxMapSize, &( *( toneIt - 1 ) ) );
	}


	for( int i = 0; i < tones.size(); ++i )
		cout << tones[ i ].maps()[ 0 ].img()->size() << endl;

#define SAVE_DDS

#ifdef SAVE_DDS

	//
	// HEADER
	//
	dx::DDS_HEADER header;
	header.size = sizeof( dx::DDS_HEADER );
	header.flags = 0x00001007 /*DDS_HEADER_FLAGS_TEXTURE*/ | 0x00000008 /*DDS_HEADER_HEADER_FLAGS_PITCH*/
		| 0x00020000 /*MIP MAP COUNT*/;
	header.height = options::maxMapSize;
	header.width = options::maxMapSize;
	header.ddspf = dx::DDSPF_R8G8B8;
	header.pitchOrLinearSize = (int)( ( ( options::maxMapSize ) * header.ddspf.RGBBitCount + 7 ) / 8 );
	//header.caps = DDS_SURFACE_FLAGS_TEXTURE;
	header.caps = DDS_SURFACE_FLAGS_TEXTURE | DDS_SURFACE_FLAGS_MIPMAP;
	header.mipMapCount = (int)log2( options::maxMapSize ) + 1;
	header.caps2 = 0;

	//
	// HEADER
	//

	//ci::CImg<unsigned char> test

	/*for( int e = 0; e < tones.size(); e += 3 )
	{
		cout << 1 << endl;
		char* imgMainDatas = (char*)malloc( options::maxMapSize * options::maxMapSize * 3 );

		cout << 2 << endl;

		for( int i = 0; i < options::maxMapSize * options::maxMapSize * 3; i += 3 )
		{
			imgMainDatas[ i + 2 ] = tones[ e ].maps().back().img()->data()[ i / 3 ];
			imgMainDatas[ i + 1 ] = tones[ e + 1 ].maps().back().img()->data()[ ( i ) / 3 ];
			imgMainDatas[ i ] = tones[ e + 2 ].maps().back().img()->data()[ ( i ) / 3 ];
			cout << 3 << " " << i << endl;
		}

		int mipMapSize( 0 );

		cout << 4 << endl;

		for( int i = tones[ e ].maps().size() - 2; i >= 0; i-- )
		{
			mipMapSize += (int)pow( tones[ 0 ].maps()[ i ].size(), 2 ) * 3;
		}

		cout << "mip map size " << mipMapSize << endl;

		char* mipMapsDatas = (char*)malloc( mipMapSize );

		int currentMapIndex = tones[ e ].maps().size() - 2;

		int size = tones[ e ].maps().back().img()->size() / 4;

		for( int i = 0, imgIndex = 0; i < mipMapSize; i+=3, imgIndex++ )
		{
			if( imgIndex >= size )
			{
				--currentMapIndex;
				imgIndex = 0;
				size /= 4;
			}

			mipMapsDatas[ i + 2] = tones[ e ].maps()[ currentMapIndex ].img()->data()[ imgIndex ];
			mipMapsDatas[ i + 1 ] = tones[ e + 1 ].maps()[ currentMapIndex ].img()->data()[ imgIndex ];
			mipMapsDatas[ i ] = tones[ e + 2 ].maps()[ currentMapIndex ].img()->data()[ imgIndex ];
		}*/

	for( int e = 0; e < tones.size(); e++ )
	{
		string fileName( "TAM_Size_" );
		fileName.append( to_string( options::maxMapSize ) );
		fileName.append( "_Tone_" );
		fileName.append( to_string( e ) );
		fileName.append( ".dds" );

		ofstream file( fileName, ios::binary );

		file.write( (char*)&dx::DDS_MAGIC, sizeof( dx::DDS_MAGIC ) );
		file.write( (char*)&header, sizeof( header ) );
		file.write( (char*)tones[ e ].maps().back().img()->get_permute_axes("cxyz").data(), tones[ e ].maps().back().img()->size() );


		for( int i = tones[ e ].maps().size() - 2; i >= 0; --i )
		{
			ci::CImg<unsigned char> temp = tones[ e ].maps()[ i ].img()->get_permute_axes( "cxyz" );
			file.write( (char*)temp.data(), tones[ e ].maps()[ i ].img()->size() );
		}

		file.close();

		cout << "File : " << fileName << " has been saved." << endl;
	}


	//free( mipMapsDatas );

	//save img
	//repeat
//}
#endif // SAVE_DDS

	cout << "FINISHED" << endl;
	system( "PAUSE" );

	return 0;
}
