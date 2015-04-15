#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <ctime>

using namespace cv;
using namespace std;

#define WIDTH 24
#define HEIGHT 24
#define RAND_CYCLE 10

int main(void)
{
	srand(time(NULL));

	IplImage *pSrcImg = NULL, *pNewSrcImg = NULL;
	IplImage *pDstImg = NULL;
	CvMat *mat;
	CvRect rect;
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
				entry = NULL;
			}
			else{
				entry = NULL;
				continue;
			}
			/*
			sprintf_s(sysRename, "rename %s image_%07d.jpg", srcName, imgBackupNum);
			system(sysRename);*/
			sprintf_s(srcName, "F:\\image\\image_%07d.jpg", imgBackupNum++);
			sprintf_s(sysMove, "move %s %s", srcName, "F:\\image_backup_test");
			system(sysMove);
			/*
			if (!(pSrcImg->width < size2.width || pSrcImg->height < size2.height)){
				pNewSrcImg = cvCreateImage(size2, pSrcImg->depth, pSrcImg->nChannels);
				cvResize(pSrcImg, pNewSrcImg, CV_INTER_LINEAR);
			}
			else
				pNewSrcImg = cvCloneImage(pSrcImg);
			*/
			for (int index = 0; index < RAND_CYCLE; index++){
				int sW = rand() % pSrcImg->width, sH = rand() % pSrcImg->height, 
					lW = rand() % pSrcImg->width, lH = rand() % pSrcImg->height;
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
				CvSize size3 = cvSize(h, w);
				mat = cvCreateMat(h, w, CV_8UC3);
				pDstImg = cvCreateImage(size3, IPL_DEPTH_8U, 3);
				IplImage *tmpDstImg = cvCloneImage(pDstImg), *pNewDstImg = cvCreateImage(size, IPL_DEPTH_8U, 3);

				rect = cvRect(sW, sH, w, h);
				cvGetSubRect(pSrcImg, mat, rect);
				cvGetImage(mat, pDstImg);
				cvResize(pDstImg, pNewDstImg, CV_INTER_LINEAR);

				sprintf_s(dstNum, "\\not_face_No_%06d.bmp", dstCount[stageNum - 1]++);
				strcat_s(dstName, dstNum);
				cvSaveImage(dstName, pNewDstImg);
				dstName[len] = '\0';

				pDstImg = cvCloneImage(tmpDstImg);
				cvReleaseImage(&pDstImg);
				cvReleaseImage(&tmpDstImg);
				cvReleaseImage(&pNewDstImg);
				cvReleaseMat(&mat);
			}
			/*
			for (int x = 0; x + WIDTH <= pNewSrcImg->width; x += WIDTH){
				for (int y = 0; y + HEIGHT <= pNewSrcImg->height; y += HEIGHT){
					mat = cvCreateMat(HEIGHT, WIDTH, CV_8UC3);
					pDstImg = cvCreateImage(size, IPL_DEPTH_8U, 3);
					IplImage *tmpDstImg = cvCloneImage(pDstImg);

					rect = cvRect(x, y, WIDTH, HEIGHT);
					cvGetSubRect(pNewSrcImg, mat, rect);
					cvGetImage(mat, pDstImg);

					sprintf_s(dstNum, "\\not_face_No_%06d.bmp", dstCount[stageNum - 1]++);
					strcat_s(dstName, dstNum);
					cvSaveImage(dstName, pDstImg);
					dstName[len] = '\0';

					pDstImg = cvCloneImage(tmpDstImg);
					cvReleaseImage(&pDstImg);
					cvReleaseImage(&tmpDstImg);
					cvReleaseMat(&mat);
				}
			}
			*/
			/*
			cvReleaseImage(&pNewSrcImg);
			pNewSrcImg = cvCreateImage(size, pSrcImg->depth, pSrcImg->nChannels);
			cvResize(pSrcImg, pNewSrcImg, CV_INTER_LINEAR);
			sprintf_s(dstNum, "\\not_face_No_%06d.bmp", dstCount[stageNum - 1]++);
			strcat_s(dstName, dstNum);
			cvSaveImage(dstName, pNewSrcImg);
			dstName[len] = '\0';
			*/
			//cvReleaseImage(&pNewSrcImg);
			cvReleaseImage(&pSrcImg);

			cout << "image" << ++index << " complete." << endl;
			if (index >= 5000){
				endFlag = 1;
				break;
			}
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
	/*
	rewind(config_imgBackup);
	fprintf_s(config_imgBackup, "%d\n", imgBackupNum);
	*/
	fclose(config_imgBackup);

	return 0;
}