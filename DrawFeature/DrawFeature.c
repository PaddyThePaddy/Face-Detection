#include <stdio.h>
#include <stdlib.h>
#include "image.h"

int main(void)
{
	FILE *soldier;
	Pixel black = { 0, 0, 0 }, white = { 255, 255, 255 };
	char outname[30] = {0};
	int sCount;
	int i;

	soldier = fopen("outdata.txt", "r");
	fscanf(soldier, "%d", &sCount);
	system("mkdir feature");

	for (i = 0; i < sCount; i++){
		int x1, x2, y1, y2, t, p, th;
		int xi, yi;
		double e, correctR;
		ImageData *feature;

		fscanf(soldier, "%d%d%d%d%d%d%d%lf%lf", &x1, &y1, &x2, &y2, &t, &p, &th, &e, &correctR);
		readBMPfile("face.bmp", &feature);
		sprintf(outname, ".\\feature\\feature_%d.bmp", i);

		for (xi = 0; xi < feature->width; xi++){
			for (yi = 0; yi < feature->height; yi++){
				if (xi >= x1 && yi >= y1 && xi <= x2 && yi <= y2){
                    if (xi == x1 || xi == x2 || yi == y1 || yi == y2){
    					setPixel(feature, xi, yi, &black);
    					continue;
				    }
					switch (t){
						case 0:
							if (xi < (x1 + x2) / 2)
								setPixel(feature, xi, yi, &white);
							else
								setPixel(feature, xi, yi, &black);
							break;
						case 1:
							if (yi < (y1 + y2) / 2)
								setPixel(feature, xi, yi, &white);
							else
								setPixel(feature, xi, yi, &black);
							break;
						case 2:
							if ((xi < x1 + (x2 - x1) / 3) || (xi >= x1 + (x2 - x1) * 2 / 3))
								setPixel(feature, xi, yi, &white);
							else
								setPixel(feature, xi, yi, &black);
							break;
						case 3:
							if ((yi < y1 + (y2 - y1) / 3) || (yi >= y1 + (y2 - y1) * 2 / 3))
								setPixel(feature, xi, yi, &white);
							else
								setPixel(feature, xi, yi, &black);
							break;
						case 4:
							if (((xi < (x1 + x2) / 2) && (yi < (y1 + y2) / 2)) || ((xi >= (x1 + x2) / 2) && (yi >= (y1 + y2) / 2)))
								setPixel(feature, xi, yi, &white);
							else
								setPixel(feature, xi, yi, &black);
							break;
					}
				}
			}
		}

		writeBMPfile(outname, feature);
		disposeImage(feature);
	}

	return 0;
}
