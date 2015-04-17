#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <ctime>

using namespace cv;
using namespace std;

#define WIDTH 24			// size of dst bmp
#define HEIGHT 24
#define SCALE 5				// cut number of (SCALE * SCALE) bmps
#define RAND_CYCLE 100		// random times

int main(void)
{
	srand(time(NULL));

	char srcName[256] = { 0 }, dstName[256] = { 0 }, stageDirName[256] = "F:\\not_face\\stage00", tmpStr[256] = { 0 }, sysStageDir[256] = { 0 }, dstNum[256] = { 0 };
	char sysMove[256] = { 0 }, sysRename[256] = { 0 };
	FILE *config, *config_imgBackup;
	int dstCount[20] = { 0 }, stageNum = 1, endFlag = 0, imgBackupNum = 0;
	DIR *dir = NULL, *tmpDir = NULL;
	dirent *entry = NULL;

	if (fopen_s(&config_imgBackup, "F:\\config_imgBackup.txt", "r+") != 0){
		fopen_s(&config_imgBackup, "F:\\config_imgBackup.txt", "w+");
		fclose(config_imgBackup);
		fopen_s(&config_imgBackup, "F:\\config_imgBackup.txt", "r+");
	}
	if (fscanf_s(config_imgBackup, "%d", &imgBackupNum) == EOF){
		fprintf_s(config_imgBackup, "0");
	}

	if (fopen_s(&config, "F:\\config.txt", "r+") != 0){
		fopen_s(&config, "F:\\config.txt", "w+");
		fclose(config);
		fopen_s(&config, "F:\\config.txt", "r+");
	}
	if (fscanf_s(config, "%d", &stageNum) == EOF){
		fprintf_s(config, "1\n0\n");
	}
	for (int i = 0; i < stageNum; i++)
		fscanf_s(config, "%d", &dstCount[i]);


	Size size = Size(WIDTH, HEIGHT), size2 = Size(WIDTH * SCALE, HEIGHT * SCALE);

	if ((dir = opendir("F:\\not_face")) == NULL)
		system("mkdir F:\\not_face");
	closedir(dir);
	if ((dir = opendir("F:\\image_backup")) == NULL)
		system("mkdir F:\\image_backup");
	closedir(dir);
	if ((dir = opendir("F:\\image")) == NULL)
		return 0;

	entry = readdir(dir);
	entry = readdir(dir);
	int index = 0;
	while (!endFlag){
		sprintf_s(tmpStr, "%02d", stageNum);
		stageDirName[strlen(stageDirName) - 2] = '\0';
		strcat_s(stageDirName, tmpStr);

		if ((tmpDir = opendir(stageDirName)) == NULL){
			sprintf_s(sysStageDir, "mkdir %s", stageDirName);
			system(sysStageDir);
		}
		closedir(tmpDir);

		sprintf_s(dstName, stageDirName);
		int len = strlen(dstName);
		while (1){
			Mat SrcImg, newSrcImg;

			if (!(entry = readdir(dir))){
				endFlag = 1;
				break;
			}
			if (strstr(entry->d_name, ".jpg") ||
				strstr(entry->d_name, ".JPG") ||
				strstr(entry->d_name, ".jpeg") ||
				strstr(entry->d_name, ".JPEG") ||
				strstr(entry->d_name, ".png") ||
				strstr(entry->d_name, ".PNG")){
				sprintf_s(srcName, "F:\\image\\%s", entry->d_name);
				SrcImg = imread(srcName);
				entry = NULL;
			}
			else{
				entry = NULL;
				continue;
			}

			sprintf_s(sysRename, "rename %s image_%07d.jpg", srcName, imgBackupNum);
			system(sysRename);
			sprintf_s(srcName, "F:\\image\\image_%07d.jpg", imgBackupNum++);
			sprintf_s(sysMove, "move %s %s", srcName, "F:\\image_backup");
			system(sysMove);

			// if src is strange, don't do anything
			if (SrcImg.rows == 0 || SrcImg.cols == 0){
				SrcImg.release();
				continue;
			}

			// directly resize to 24 * 24 bmp
			resize(SrcImg, newSrcImg, size, INTER_AREA);
			sprintf_s(dstNum, "\\not_face_No_%06d.bmp", dstCount[stageNum - 1]++);
			strcat_s(dstName, dstNum);
			imwrite(dstName, newSrcImg);
			dstName[len] = '\0';
			newSrcImg.release();

			// if src too small, don't cut and random
			if (SrcImg.rows <= size.height && SrcImg.cols <= size.width){
				SrcImg.release();
				continue;
			}

			// cut to 24 * 24 bmp for number of (SCALE * SCALE)
			if (!(SrcImg.cols < size2.width || SrcImg.rows < size2.height))
				resize(SrcImg, newSrcImg, size2, INTER_AREA);
			else
				newSrcImg = SrcImg.clone();
			for (int x = 0; x + WIDTH <= newSrcImg.cols; x += WIDTH){
				for (int y = 0; y + HEIGHT <= newSrcImg.rows; y += HEIGHT){
					Rect rect = Rect(x, y, WIDTH, HEIGHT);
					Mat DstImg = newSrcImg(rect);

					sprintf_s(dstNum, "\\not_face_No_%06d.bmp", dstCount[stageNum - 1]++);
					strcat_s(dstName, dstNum);
					imwrite(dstName, DstImg);
					dstName[len] = '\0';

					DstImg.release();
				}
			}
			newSrcImg.release();	

			// random to 24 * 24 bmp for number of 100
			for (int index = 0; index < RAND_CYCLE; index++){
				int sW, sH, lW, lH;

				do{
					sW = rand() % SrcImg.cols;
					sH = rand() % SrcImg.rows;
					lW = rand() % SrcImg.cols;
					lH = rand() % SrcImg.rows;
				} while (sW == lW || sH == lH);
				int tmp;

				if (sW > lW){
					tmp = sW;
					sW = lW;
					lW = tmp;
				}
				if (sH > lH){
					tmp = sH;
					sH = lH;
					lH = tmp;
				}
				int h = lH - sH, w = lW - sW;
				Size size3 = Size(h, w);
				Rect rect = Rect(sW, sH, w, h);
				Mat DstImg = SrcImg(rect), newDstImg;

				resize(DstImg, newDstImg, size, INTER_AREA);
				sprintf_s(dstNum, "\\not_face_No_%06d.bmp", dstCount[stageNum - 1]++);
				strcat_s(dstName, dstNum);
				imwrite(dstName, newDstImg);
				dstName[len] = '\0';

				DstImg.release();
				newDstImg.release();
			}
			SrcImg.release();

			cout << "image" << ++index << " complete." << endl;
			if (dstCount[stageNum - 1] >= 100000){
				stageNum++;
				break;
			}
		}
	}

	rewind(config);
	fprintf_s(config, "%d\n", stageNum);
	for (int i = 0; i < stageNum; i++)
		fprintf_s(config, "%d\n", dstCount[i]);
	fclose(config);

	rewind(config_imgBackup);
	fprintf_s(config_imgBackup, "%d\n", imgBackupNum);
	fclose(config_imgBackup);

	return 0;
}