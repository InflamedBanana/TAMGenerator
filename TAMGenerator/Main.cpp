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
	srand(time(0));

	options::SetOptions();

	vector<Tone> tones;

	for ( int indice = 0, greyValue = 255; indice <  options::nbOfGreys; ++indice, greyValue -= ( ( 255 - options::minGrey ) / (options::nbOfGreys - 1 ) ) )
	{
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

	//CImg<unsigned char>redimg(tone.maps()[0].img());

	for (int i = 0; i < tones.size(); ++i)
		cout << tones[i].maps()[0].img()->size() << endl;

	//int mipMapSize(0);

	//for (int i = 0; i < tones[0].maps().size() - 1; i++)
	//{
	//	if (tones[0].maps()[i].size() == 1)
	//		mipMapSize += 4;
	//	else
	//		mipMapSize += (int)pow(tones[0].maps()[i].size(), 2) * 3;

	//	cout << "map size " << tones[0].maps()[i].size() << " data size : " << tones[0].maps()[i].img().size() << "  nb of channel " << tones[0].maps()[i].img().spectrum() << endl;
	//}


	//for (int i = 0; i < tones[2].maps()[0].img().size(); ++i)
		//cout << (int)*(tones[2].maps()[0].img().data() + i) << endl;

#define SAVE_DDS

#ifdef SAVE_DDS

	//
	// HEADER
	//
	dx::DDS_HEADER header;
	header.size = sizeof(dx::DDS_HEADER);
	header.flags = 0x00001007 /*DDS_HEADER_FLAGS_TEXTURE*/ | 0x00000008 /*DDS_HEADER_HEADER_FLAGS_PITCH*/
					| 0x00020000 /*MIP MAP COUNT*/;
	header.height = options::maxMapSize;
	header.width = options::maxMapSize;
	header.pitchOrLinearSize = (int)(((options::maxMapSize) * 24/*(sizeof(unsigned char) * 3*)*/ + 7) / 8);
	//header.caps = DDS_SURFACE_FLAGS_TEXTURE;
	header.caps = DDS_SURFACE_FLAGS_TEXTURE | DDS_SURFACE_FLAGS_MIPMAP;
	header.mipMapCount = (int)log2(options::maxMapSize) + 1;
	header.caps2 = 0;
	header.ddspf = dx::DDSPF_R8G8B8;
	//
	// HEADER
	//

	for (int e = 0; e < tones.size(); e+=3)
	{
		ci::CImg<unsigned char>finalImage(options::maxMapSize, options::maxMapSize, 1, 3, 255);

		const CImgUniquePtr& blueImg = tones[e].maps().back().img();
		const CImgUniquePtr& greenImg = tones[e + 1].maps().back().img();
		const CImgUniquePtr& redImg = tones[e + 2].maps().back().img();

		for (int i = 0; i < finalImage.size(); ++i)
		{
			int channelIndice(i / 3);
			//finalImage.data()[i] = blueImg->data()[channelIndice];
			//finalImage.data()[++i /*+ 1*/] = greenImg->data()[channelIndice + 1];
			//finalImage.data()[++i /*+ 3*/] = redImg->data()[channelIndice + 2];
			finalImage.data()[i] = tones[e].maps().back().img()->data()[channelIndice];
			finalImage.data()[++i /*+ 1*/] = tones[e+1].maps().back().img()->data()[channelIndice + 1];
			finalImage.data()[++i /*+ 3*/] = tones[e+2].maps().back().img()->data()[channelIndice + 2];
		}

		int mipMapSize(0);

		for (int i = 0; i < tones[0].maps().size() - 1; i++)
		{
			if(tones[0].maps()[i].size() == 1)
				mipMapSize += 4;
			else
				mipMapSize += (int)pow(tones[0].maps()[i].size(), 2) * 3;

			//cout << "map size " << tones[0].maps()[i].size()  << " data size : " << tones[0].maps()[i].img().size() << endl;
		}

		cout << "mip map size " << mipMapSize << endl;

		unsigned char* mipMapsDatas = (unsigned char*)malloc(mipMapSize);

		int mipMapLevel( tones[e].maps().size() - 2);
		int mipMapPixelDataCount( 0 );

		for (int j = mipMapLevel; j >= 0; --j)
		{

	/*		const CImgUniquePtr& blueMipMap = tones[e].maps()[j].img();
			const CImgUniquePtr& greenMipMap = tones[e + 1].maps()[j].img();
			const CImgUniquePtr& redMipMap = tones[e + 2].maps()[j].img();*/
			cout << "1 : mipMapLevel = " << j << " | mipMapPixelDataCount = " << mipMapPixelDataCount << endl;

			for (int k = 0; k < tones[e].maps()[j].img()->size() * 3; ++k, ++mipMapPixelDataCount)
			{

				/*mipMapsDatas[mipMapPixelDataCount] = blueMipMap->data()[k];
				mipMapsDatas[++mipMapPixelDataCount] = greenMipMap->data()[++k];
				mipMapsDatas[++mipMapPixelDataCount] = redMipMap->data()[++k];*/
				mipMapsDatas[mipMapPixelDataCount] = tones[e].maps()[j].img()->data()[k];
				mipMapsDatas[++mipMapPixelDataCount] = tones[e + 1].maps()[j].img()->data()[++k];
				mipMapsDatas[++mipMapPixelDataCount] = tones[e + 2].maps()[j].img()->data()[++k];
				//
				// Error ici ya un truc qui se fait pas bien.
				//
			/*		mipMapsDatas[mipMapPixelDataCount] = tones[e].maps()[mipMapLevel].img().data()[k];
					mipMapsDatas[++mipMapPixelDataCount] = tones[e + 1].maps()[mipMapLevel].img().data()[++k];
					mipMapsDatas[++mipMapPixelDataCount] = tones[e + 2].maps()[mipMapLevel].img().data()[++k];*/
			}
			cout << "2 : mipMapLevel = " << j << " | mipMapPixelDataCount = " << mipMapPixelDataCount << " | img data size = " << tones[e].maps()[j].img()->size() << endl;

		}


		cout << " mipMapPixelDataCount = " << mipMapPixelDataCount << endl;

		//for (int j = 0; j < mipMapSize; ++j /*, ++mipMapPixelDataCount*/)
		//{
			/*if (mipMapPixelDataCount >= bTone.maps()[mipMapLevel].img().size())
			{
				mipMapPixelDataCount = 0;
				mipMapLevel--;
				if (mipMapLevel < 0)
				{
					cout << "MIP MAP LEVEL GOES UNDER 0" << endl;
					break;
				}
			}*/
			//cout << " j " << j << endl;
		/*	mipMapsDatas[j] = bTone.maps()[mipMapLevel].img().data()[mipMapPixelDataCount];
			mipMapsDatas[++j] = gTone.maps()[mipMapLevel].img().data()[++mipMapPixelDataCount];
			mipMapsDatas[++j] = rTone.maps()[mipMapLevel].img().data()[++mipMapPixelDataCount];*/
			/*mipMapsDatas[j] = (rand() % 230 )+ 15;
			mipMapsDatas[++j] = (rand() % 230) + 15;
			mipMapsDatas[++j] = (rand() % 230) + 15;*/
			//cout << j / mipMapSize << " %" << endl;
		//}
		cout << "lel " << endl;
		string fileName("TAM_size_");
		fileName.append(to_string(options::maxMapSize));
		fileName.append("_");
		fileName.append(to_string(e / 3));
		fileName.append(".dds");

		ofstream file(fileName, ios::binary);

		file.write((char*)&dx::DDS_MAGIC, sizeof(dx::DDS_MAGIC));
		file.write((char*)&header, sizeof(header));
		file << finalImage.data() << endl;
		file << mipMapsDatas << endl;
		file.close();

		cout << "File : " << fileName << " has been saved." << endl;

		//save img
		//repeat
	}
#endif // SAVE_DDS

	cout << "FINISHED" << endl;
	system("PAUSE");

	return 0;
}
