#include <vector>
#include <array>
#include <iostream>
#include <ctime>

#include "dds.h"
#include <fstream>
#include <iostream>
#include <string>

#include "Position.h"
#include "Tam.h"
//#include "Map.h"
#include "TamOptions.h"

using namespace std;

namespace dx = DirectX;

int main()
{
	srand( time( 0 ) );

	options::SetOptions();

	vector<Tone> tones;
	
	for( int indice = 0, greyValue = options::maxGrey; indice < options::nbOfGreys; ++indice, greyValue -= ( ( options::maxGrey - options::minGrey ) / ( options::nbOfGreys - 1 ) ) )
	{
		Tone tone( greyValue );
		tones.push_back( tone );
	}
	cout << "Start ! " << endl;

	for ( int i = 0; i < tones.size(); ++i )
	{
		if (i == 0)
			tones[i].Generate( options::maxMapSize );
		else
			tones[i].Generate( options::maxMapSize, &tones[i - 1] );
	}

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
	
	header.caps = DDS_SURFACE_FLAGS_TEXTURE | DDS_SURFACE_FLAGS_MIPMAP;
	header.mipMapCount = (int)log2( options::maxMapSize ) + 1;
	header.caps2 = 0;

	//
	// HEADER
	//

	int lastMapIndex = tones[0].maps().size() - 1;

	for ( int i = 0; i < tones.size(); i += 3 )
	{
		ci::CImg<unsigned char> finalImage( options::maxMapSize, options::maxMapSize, 1, 3 );

		finalImage.draw_image( 0, 0, 0, 0, *tones[i + 2].maps()[lastMapIndex]->img() );
		finalImage.draw_image( 0, 0, 0, 1, *tones[i + 1].maps()[lastMapIndex]->img() );
		finalImage.draw_image( 0, 0, 0, 2, *tones[i].maps()[lastMapIndex]->img() );

		ofstream file( "TAM_Size_" +
			to_string( options::maxMapSize ) +
			"_Tone_" +
			to_string( i )
			+ ".dds", ios::binary );

		file.write((char*)&dx::DDS_MAGIC, sizeof(dx::DDS_MAGIC));
		file.write((char*)&header, sizeof(header));
		file.write((char*)finalImage.get_permute_axes( "cxyz" ).data(), finalImage.size());

		for ( int e = lastMapIndex - 1; e >= 0; --e )
		{
			ci::CImg<unsigned char> finalMipMaps( tones[i].maps()[e]->size(), tones[i].maps()[e]->size(), 1, 3 );

			finalMipMaps.draw_image(0, 0, 0, 0, *tones[ i + 2 ].maps()[e]->img() );
			finalMipMaps.draw_image(0, 0, 0, 1, *tones[ i + 1 ].maps()[e]->img() );
			finalMipMaps.draw_image(0, 0, 0, 2, *tones[ i ].maps()[e]->img() );

			file.write( (char*)finalMipMaps.get_permute_axes( "cxyz" ).data(), finalMipMaps.size() );
		}

		file.close();
	}

#endif // SAVE_DDS

	cout << "FINISHED" << endl;
	system( "PAUSE" );

	return 0;
}
