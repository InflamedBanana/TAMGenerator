#include <vector>
#include <iostream>
#include <ctime>

#include "dds.h"
#include <fstream>
#include <iostream>

#include "Position.h"
#include "Tam.h"
#include "TamOptions.h"

using namespace std;

namespace dx = DirectX;

int main()
{
	srand((unsigned int)time(0));

	options::SetOptions();

	std::vector<Tone> tones;

	for ( int indice = 0, greyValue = options::maxGrey; indice < (int)options::nbOfGreys; ++indice, greyValue -= ( (options::maxGrey - options::minGrey ) / (options::nbOfGreys - 1 ) ) )
	{
		std::cout << "tone grey value " << greyValue << std::endl;
		Tone tone(greyValue);
		tones.push_back(tone);
	}

	for (std::vector<Tone>::iterator toneIt = tones.begin(); toneIt != tones.end(); ++toneIt)
	{
		if ( toneIt == tones.begin() )
			toneIt->Generate(options::maxMapSize );
		else
			toneIt->Generate(options::maxMapSize, &(*(toneIt - 1)));
	}

	//for(const auto& tone : tones)
	//	for (const auto& map : tone.maps())
	//	{
	//		std::string mapName(std::to_string(map.greyLvl()));
	//		mapName.append("  ");
	//		mapName.append(std::to_string(map.size()));
	//		CImgDisplay display( 256, 256, mapName.c_str(), 0 );
	//		displays.push_back( display );
	//		map.img().display( displays.back() );
	//	}

	//CImg<unsigned char>redimg(tone.maps()[0].img());

	ci::CImg<unsigned char>img(16, 16, 1, 3, 255);

	/*for (int i = 0; i < img.size(); i += 3)
	{
		img.data()[i] = redimg.data()[i / 3];
	}*/

	//std::ofstream file("wesh.dds", std::ios::binary);

	dx::DDS_HEADER header;
	header.size = sizeof(dx::DDS_HEADER);
	header.flags = 0x00001007 /*DDS_HEADER_FLAGS_TEXTURE*/ | 0x00000008 /*DDS_HEADER_HEADER_FLAGS_PITCH*/;
	header.height = img.height();
	header.width = img.width();
	header.pitchOrLinearSize = (img.width() * (sizeof(unsigned char) * 3) + 7) / 8;
	header.caps = DDS_SURFACE_FLAGS_TEXTURE;
	header.caps2 = 0;
	header.ddspf = dx::DDSPF_R8G8B8;

	//file.write((char*)&dx::DDS_MAGIC, sizeof(dx::DDS_MAGIC));
	//file.write((char*)&header, sizeof(header));
	//file << img.data() << std::endl;
	//file.close();

	std::cout << "FINISHED" << std::endl;
	system("PAUSE");

	return 0;
}
