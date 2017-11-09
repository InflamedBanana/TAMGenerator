#include <CImg/CImg.h>

using namespace cimg_library;

struct tamOptions
{
	unsigned int mapSize;
	unsigned int maxMapSize;
	unsigned int minMapSize;
	unsigned char greyLvl;
};

int main() {
	/*CImg<unsigned char> image(500,400,1,3,50), visu(500, 400, 1, 3, 0);

	const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };

	image.blur(2.5);
	CImgDisplay main_disp(image, "Click a point"), draw_disp(visu, "Intensity profile");

	while (!main_disp.is_closed() && !draw_disp.is_closed()) {
		main_disp.wait();
		if (main_disp.button() && main_disp.mouse_y() >= 0) {
			const int y = main_disp.mouse_y();
			visu.fill(0).draw_graph(image.get_crop(0, y, 0, 0, image.width() - 1, y, 0, 0), red, 1, 1, 0, 255, 0);
			visu.draw_graph(image.get_crop(0, y, 0, 1, image.width() - 1, y, 0, 1), green, 1, 1, 0, 255, 0);
			visu.draw_graph(image.get_crop(0, y, 0, 2, image.width() - 1, y, 0, 2), blue, 1, 1, 0, 255, 0).display(draw_disp);
		}
	}*/

	CImg<unsigned char> rsltImage(32, 32, 1, 1, 255);
	const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };

	CImgDisplay rslt_display(512, 512, "Rslt"), opt_display(500, 250, "Options");

	rsltImage.draw_line(5, 15, 32, 15, blue).display(rslt_display);
	tamOptions opt;

	while (!rslt_display.is_closed() && !opt_display.is_closed() ) 
	{

	}

	return 0;
}
