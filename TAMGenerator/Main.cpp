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
	srand((unsigned int)time(0));

	options::SetOptions();

	vector<Tone> tones;

	for ( int indice = 0, greyValue = 255; indice < (int)options::nbOfGreys; ++indice, greyValue -= ( ( 255 - options::minGrey ) / (options::nbOfGreys - 1 ) ) )
	{
		cout << "tone grey value " << greyValue << endl;
		Tone tone(greyValue);
		tones.push_back(tone);
	}

	for (vector<Tone>::iterator toneIt = tones.begin(); toneIt != tones.end(); ++toneIt)
	{
		if ( toneIt == tones.begin() )
			toneIt->Generate(options::maxMapSize );
		else
			toneIt->Generate(options::maxMapSize, &(*(toneIt - 1)));
	}

	//for(const auto& tone : tones)
	//	for (const auto& map : tone.maps())
	//	{
	//		string mapName(to_string(map.greyLvl()));
	//		mapName.append("  ");
	//		mapName.append(to_string(map.size()));
	//		CImgDisplay display( 256, 256, mapName.c_str(), 0 );
	//		displays.push_back( display );
	//		map.img().display( displays.back() );
	//	}

	//CImg<unsigned char>redimg(tone.maps()[0].img());

	//
	// HEADER
	//
	dx::DDS_HEADER header;
	header.size = sizeof(dx::DDS_HEADER);
	header.flags = 0x00001007 /*DDS_HEADER_FLAGS_TEXTURE*/ | 0x00000008 /*DDS_HEADER_HEADER_FLAGS_PITCH*/
					| 0x00020000 /*MIP MAP COUNT*/;
	header.height = options::maxMapSize;
	header.width = options::maxMapSize;
	header.pitchOrLinearSize = ((options::maxMapSize) * (sizeof(unsigned char) * 3) + 7) / 8;
	//header.caps = DDS_SURFACE_FLAGS_TEXTURE;
	header.caps = DDS_SURFACE_FLAGS_TEXTURE | DDS_SURFACE_FLAGS_MIPMAP;
	header.mipMapCount = (int)log2(options::maxMapSize - 1);
	header.caps2 = 0;
	header.ddspf = dx::DDSPF_R8G8B8;
	//
	// HEADER
	//

	for (int e = 0; e < tones.size(); e+=3)
	{
		ci::CImg<unsigned char>finalImage(options::maxMapSize, options::maxMapSize, 1, 3, 255);

		ci::CImg<unsigned char>bImage(tones[e].maps().back().img());
		ci::CImg<unsigned char>gImage(tones[e + 1].maps().back().img());
		ci::CImg<unsigned char>rImage(tones[e + 2].maps().back().img());

		Tone bTone(tones[e]);
		Tone gTone(tones[e + 1]);
		Tone rTone(tones[e + 2]);

		for (int i = 0; i < finalImage.size(); ++i/*+=3*/)
		{
			int channelIndice(i / 3);
			finalImage.data()[i] = bTone.maps().back().img().data()[channelIndice];
			finalImage.data()[++i /*+ 1*/] = gTone.maps().back().img().data()[channelIndice + 1];
			finalImage.data()[++i /*+ 3*/] = rTone.maps().back().img().data()[channelIndice + 2];
		}

		int mipMapSize(3);

		for (int i = 0; i < tones[0].maps().size() - 1; i++)
		{
			mipMapSize += tones[0].maps()[i].img().size() * tones.size();
		}

		/*for (int size = 1; size < options::maxMapSize / 2; size *= 2)
			mipMapSize += (int)(pow(2, size) * pow(2, size) * 3);*/

		cout << "mip map size " << mipMapSize << endl;

		unsigned char* mipMapsDatas = (unsigned char*)malloc(mipMapSize);

		int mipMapLevel( bTone.maps().size() - 2);
		int mipMapPixelDataCount( 0 );

		for (int j = 0; j < mipMapSize; ++j , ++mipMapPixelDataCount)
		{
			if (mipMapPixelDataCount >= bTone.maps()[mipMapLevel].img().size())
			{
				mipMapPixelDataCount = 0;
				mipMapLevel--;
				if (mipMapLevel < 0)
				{
					cout << "MIP MAP LEVEL GOES UNDER 0" << endl;
					break;
				}
			}
			
			mipMapsDatas[j] = bTone.maps()[mipMapLevel].img().data()[mipMapPixelDataCount];
			mipMapsDatas[++j] = gTone.maps()[mipMapLevel].img().data()[++mipMapPixelDataCount];
			mipMapsDatas[++j] = rTone.maps()[mipMapLevel].img().data()[++mipMapPixelDataCount];
			//cout << j / mipMapSize << " %" << endl;
		}

		string fileName("TAM_size_");
		fileName.append(to_string(options::maxMapSize));
		fileName.append("_");
		fileName.append(to_string(e / 3));
		fileName.append(".dds");

		ofstream file(fileName, ios::binary);

		file.write((char*)&dx::DDS_MAGIC, sizeof(dx::DDS_MAGIC));
		file.write((char*)&header, sizeof(header));
		file << finalImage.data() /*<< endl*/;
		file << mipMapsDatas << endl;
		file.close();

		cout << "File : " << fileName << " has been saved." << endl;

		//save img
		//repeat
	}

	/*for (const auto& tone : tones)
	{*/
		//ci::CImg<unsigned char>channelImage(tone.maps().back().img());

	//}

	//ci::CImg<unsigned char>img( 16, 16, 1, 3, 255 );

	/*for (int i = 0; i < img.size(); i += 3)
	{
		img.data()[i] = redimg.data()[i / 3];
	}*/

	//ofstream file("wesh.dds", ios::binary);

	//dx::DDS_HEADER header;
	//header.size = sizeof(dx::DDS_HEADER);
	//header.flags = 0x00001007 /*DDS_HEADER_FLAGS_TEXTURE*/ | 0x00000008 /*DDS_HEADER_HEADER_FLAGS_PITCH*/;
	//header.height = img.height();
	//header.width = img.width();
	//header.pitchOrLinearSize = (img.width() * (sizeof(unsigned char) * 3) + 7) / 8;
	//header.caps = DDS_SURFACE_FLAGS_TEXTURE;
	//header.caps2 = 0;
	//header.ddspf = dx::DDSPF_R8G8B8;

	//file.write((char*)&dx::DDS_MAGIC, sizeof(dx::DDS_MAGIC));
	//file.write((char*)&header, sizeof(header));
	//file << img.data() << endl;
	//file.close();

	cout << "FINISHED" << endl;
	system("PAUSE");

	return 0;
}
