#include <feos.h>
#include "image.h"

int main(int argc, char** argv)
{
	FeOS_DirectMode();
	videoSetMode(MODE_5_2D);
	vramSetPrimaryBanks (VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
	int sbitmap = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);

	/* Open the image and decode it */
	if(openImage("test.png")) {
		decodeImage(bgGetGfxPtr(sbitmap));
	}
	closeImage();

	for(;;) {
		FeOS_WaitForVBlank();
	}
	FeOS_ConsoleMode();
	return 0;
}
