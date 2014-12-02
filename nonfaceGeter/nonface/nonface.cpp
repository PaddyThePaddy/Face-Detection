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
	char srcName[256], dstName[256];
	FILE *config;
	int dstCount = 0, tmp;
	DIR *dir = NULL;
	dirent *entry = NULL;

	if (fopen_s(&config, "config.txt", "r+") != 0){
		fopen_s(&config, "config.txt", "w+");
		fclose(config);
		fopen_s(&config, "config.txt", "r+");
	}

	if (fscanf_s(config, "%d%d", &tmp, &dstCount) == EOF){
		fprintf_s(config, "0\n0\n");
	}

	CvSize size = cvSize(WIDTH, HEIGHT), size2 = cvSize(WIDTH * 10, HEIGHT * 10);

	if ((dir = opendir(".\\not_face")) == NULL)
		system("mkdir not_face");
	closedir(dir);
	if ((dir = opendir(".\\image")) == NULL)
		return 0;
	
	entry = readdir(dir);
	entry = readdir(dir);
	int i = 0;
	while (entry = readdir(dir)){
		if (strstr(entry->d_name, ".jpg") || 
			strstr(entry->d_name, ".JPG") || 
			strstr(entry->d_name, ".jpeg") || 
			strstr(entry->d_name, ".JPEG") || 
			strstr(entry->d_name, ".png") ||
			strstr(entry->d_name, ".PNG")){
			sprintf_s(srcName, ".\\image\\%s", entry->d_name);
			pSrcImg = cvLoadImage(srcName, 1);
		}
		pNewSrcImg = cvCreateImage(size2, pSrcImg->depth, pSrcImg->nChannels);
		cvResize(pSrcImg, pNewSrcImg, CV_INTER_LINEAR);
		cvReleaseImage(&pSrcImg);
		for (int x = 0; x + WIDTH <= pNewSrcImg->width; x += WIDTH){
			for (int y = 0; y + HEIGHT <= pNewSrcImg->height; y += HEIGHT){
				mat = cvCreateMat(HEIGHT, WIDTH, CV_8UC3);
				pDstImg = cvCreateImage(size, IPL_DEPTH_8U, 3);
				
				rect = cvRect(x, y, WIDTH, HEIGHT);
				cvGetSubRect(pNewSrcImg, mat, rect);
				cvGetImage(mat, pDstImg);
				
				sprintf_s(dstName, ".\\not_face\\not_face_No_%d.bmp", dstCount++);
				cvSaveImage(dstName, pDstImg);
			}
		}
		cvReleaseImage(&pNewSrcImg);	
		/*
		cvReleaseImage(&pDstImg);
		cvReleaseMat(&mat);
		*/
		cout << "image" << ++i << " complete." << endl;
	}

	rewind(config);
	fprintf_s(config, "%d\n%d\n", tmp, dstCount);
	fclose(config);

	return 0;
}



#if 0
int captur(Mat img, int &nWrites);
void face_dec(Mat img, CascadeClassifier face_cascade, vector<Rect>&, int min_face_height, int min_face_width);
int main(){
	CascadeClassifier face_cascade;
	VideoCapture cap;
	string classifier_name = "haarcascade_frontalface_default.xml";
	unsigned char *inp, *outp;
	face_cascade.load(classifier_name);
	int nWrites = 0;
	int nnWrites = 0;
	vector<Rect> faces;
	char x;
	char outname[256];
	string filename;
	Mat img, out;
	FILE *config;
	if (fopen_s(&config, "config.txt", "r+") != 0){
		fopen_s(&config, "config.txt", "w+");
		fclose(config);
		fopen_s(&config, "config.txt", "r+");
	}
	waitKey(10);
	//	cout<<fread(&nWrites,sizeof(int),1,config);
	if (fscanf_s(config, "%d%d", &nWrites, &nnWrites) == EOF){
		fprintf_s(config, "0\n0\n");
	}
	namedWindow("face", 0);
	//namedWindow("img", 0);
	for (;;){
		system("CLS");
		destroyWindow("face");
		//destroyWindow("img");
		cout << "face : " << nWrites << endl;
		cout << "Not face : " << nnWrites << endl;
		cout << "File (\"exit\" to end): ";
		cin >> filename;
		system("CLS");
		cout << "face : " << nWrites << endl;

		if (filename == "exit" || filename == "EXIT")break;
		img = imread(filename, -1);
		//imshow("img", img);
		waitKey(10);
		imwrite("test.bmp", img);
		out = img;
		face_dec(img, face_cascade, faces, 24, 24);

		for (vector<cv::Rect>::const_iterator i = faces.begin(); i != faces.end(); i++){
			resize(out, out, Size(i->width, i->height));
			for (int j = 0; j < i->height; j++){
				outp = out.ptr<unsigned char>(j);
				inp = img.ptr<unsigned char>(i->y + j);
				for (int k = 0; k < i->width * 3; k += 3){
					*(outp + k) = *(inp + i->x * 3 + k);
					*(outp + k + 1) = *(inp + i->x * 3 + k + 1);
					*(outp + k + 2) = *(inp + i->x * 3 + k + 2);
				}
			}
			imshow("face", out);
			waitKey(10);
			resize(out, out, Size(24, 24));
			cout << endl << "save ? (y/n) :";
			x = _getch();
			if (x == 'y' || x == 'Y'){
				system("cls");
				cout << "face : " << nWrites << endl;
				cout << "Not face : " << nnWrites << endl;
				sprintf_s(outname, ".\\face\\face_No_%d.bmp", nWrites);
				nWrites++;
				cout << endl << "No." << nWrites - 1 << " face has been saved ." << endl;
				if (!imwrite(outname, out)){
					system("mkdir face");
					imwrite(outname, out);
				}
			}
			if (x == 'n' || x == 'N'){
				system("cls");
				cout << "face : " << nWrites << endl;
				cout << "Not face : " << nnWrites << endl;
				sprintf_s(outname, ".\\not_face\\not_face_No_%d.bmp", nnWrites);
				nnWrites++;
				cout << endl << "No." << nnWrites - 1 << " not face has been saved ." << endl;
				if (!imwrite(outname, out)){
					system("mkdir not_face");
					imwrite(outname, out);
				}
			}
		}
	}
	rewind(config);
	fprintf_s(config, "%d\n%d\n", nWrites, nnWrites);
	fclose(config);
	return 0;
}
void face_dec(Mat frame, CascadeClassifier face_cascade, vector<Rect> &faces, int min_face_height, int min_face_width){
	Mat frame_gray, out, fa;
	out = Mat(Size(min_face_height, min_face_width), frame.type());
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0, Size(24, 24));
}
#endif