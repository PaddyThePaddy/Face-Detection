#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <dirent.h>

using namespace cv;
using namespace std;

#define WIDTH 24
#define HEIGHT 24

int main(void)
{
	IplImage *pSrcImg = NULL, *pNewSrcImg = NULL;
	IplImage *pDstImg = NULL;
	CvMat *mat;
	CvRect rect;
	char srcName[256], dstName[256], stageDirName[100] = "F:\\not_face\\stage00", tmpStr[20] = { 0 }, sysStageDir[200] = { 0 }, dstNum[50] = { 0 };
	FILE *config;
	int dstCount[20] = { 0 }, stageNum = 1, endFlag = 0;
	DIR *dir = NULL, *tmpDir = NULL;
	dirent *entry = NULL;

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


	CvSize size = cvSize(WIDTH, HEIGHT), size2 = cvSize(WIDTH * 5, HEIGHT * 5);

	if ((dir = opendir("F:\\not_face")) == NULL)
		system("mkdir F:\\not_face");
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
				pSrcImg = cvLoadImage(srcName, 1);
			}
			else
				continue;
			if (!(pSrcImg->width < size2.width || pSrcImg->height < size2.height)){
				pNewSrcImg = cvCreateImage(size2, pSrcImg->depth, pSrcImg->nChannels);
				cvResize(pSrcImg, pNewSrcImg, CV_INTER_LINEAR);
			}
			else
				pNewSrcImg = pSrcImg;

			for (int x = 0; x + WIDTH <= pNewSrcImg->width; x += WIDTH){
				for (int y = 0; y + HEIGHT <= pNewSrcImg->height; y += HEIGHT){
					mat = cvCreateMat(HEIGHT, WIDTH, CV_8UC3);
					pDstImg = cvCreateImage(size, IPL_DEPTH_8U, 3);

					rect = cvRect(x, y, WIDTH, HEIGHT);
					cvGetSubRect(pNewSrcImg, mat, rect);
					cvGetImage(mat, pDstImg);

					sprintf_s(dstNum, "\\not_face_No_%d.bmp", dstCount[stageNum - 1]++);
					strcat_s(dstName, dstNum);
					cvSaveImage(dstName, pDstImg);
					dstName[len] = '\0';
				}
			}
			cvReleaseImage(&pNewSrcImg);
			cvReleaseImage(&pSrcImg);
			/*
			cvReleaseImage(&pDstImg);
			cvReleaseMat(&mat);
			*/
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

	return 0;
}