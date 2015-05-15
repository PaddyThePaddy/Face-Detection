#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include "IntImg_d.h"
#include "Soldier_d.h"


using namespace cv;
using namespace std;

#define FACTOR 1.25
#define SHIFT 1.5

int main(void)
{
	FILE *soldier_cascade, *output;
	int stageNum;
	vector<vector<Soldier>> s;
	vector<int> s_num;
	vector<double> th_stage;

	fopen_s(&output, "detection_output.txt", "w");
	fopen_s(&soldier_cascade, "soldier_cascade.txt", "r");
	if (soldier_cascade == NULL){
		cout << "ERROR: can't find soldier_cascade.txt" << endl;
		return 0;
	}
	fscanf_s(soldier_cascade, "%d", &stageNum);
	s.resize(stageNum);
	s_num.resize(stageNum);
	th_stage.resize(stageNum);
	for (int i = 0; i < stageNum; i++){
		int x1, y1, x2, y2, type, p, th;
		double e;

		fscanf_s(soldier_cascade, "%d", &s_num[i]);
		s[i].resize(s_num[i]);
		for (int j = 0; j < s_num[i]; j++){
			fscanf_s(soldier_cascade, "%02d %02d %02d %02d %d %2d %8d %lf", &x1, &y1, &x2, &y2, &type, &p, &th, &e);
			s[i][j] = Soldier(x1, y1, x2, y2, type, p, th, e);
		}
		fscanf_s(soldier_cascade, "%lf", &th_stage[i]);
	}
	fclose(soldier_cascade);
	/*
	cout << stageNum << endl;
	for (int i = 0; i < stageNum; i++){
		cout << s_num[i] << endl;
		for (int j = 0; j < s_num[i]; j++){
			char str[200];
			s[i][j].getData(str);
			cout << str << endl;
		}
		cout << th_stage[i] << endl;
	}
	system("PAUSE");
	*/

	Mat SrcImg, SrcImg_gray;
	unsigned char *p;

	SrcImg = imread("test.jpg");
	imshow("SrcImg", SrcImg);
	cvtColor(SrcImg, SrcImg_gray, CV_RGB2GRAY);

	IntImg img = IntImg(SrcImg_gray.rows, SrcImg_gray.cols);

	for (int i = 0; i < SrcImg_gray.rows; i++){
		p = SrcImg_gray.ptr<unsigned char>(i);
		for (int j = 0; j < SrcImg_gray.cols; j++){
			if (i == 0 && j == 0)
				img.data[i][j] = p[j];
			else if (i == 0)
				img.data[i][j] = p[j] + img.data[i][j - 1];
			else if (j == 0)
				img.data[i][j] = p[j] + img.data[i - 1][j];
			else
				img.data[i][j] = p[j] + img.data[i][j - 1] + img.data[i - 1][j] - img.data[i - 1][j - 1];
			/*
			if (i < 3 && j < 3)
				printf("%3lld, ", p[j]);
			if (i < 3 && j == 3)
				printf("\n");
			*/
		}
	}
	/*
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			printf("%3lld, ", img.data[i][j]);
		}
		printf("\n");
	}
	printf("%lld\n", img.data[SrcImg_gray.rows - 1][SrcImg_gray.cols - 1]);
	system("PAUSE");
	*/

	double scale = 1, move = SHIFT;
	int subWindow_x1 = 0, subWindow_y1 = 0, subWindow_x2 = 23, subWindow_y2 = 23;
	bool isFace = FALSE;
	for (int c = 0; c < 10; c++, scale *= FACTOR, move *= FACTOR){
		subWindow_x2 *= FACTOR;
		subWindow_y2 *= FACTOR;

		//printf("%lf, %lf\n", scale, move);
		//printf("P:\n%d, %d, %d, %d\n", subWindow_x1, subWindow_y1, subWindow_x2, subWindow_y2);

		/*
		Mat tmp = SrcImg.clone();
		Rect rect = Rect(subWindow_x1, subWindow_y1, subWindow_x2 - subWindow_x1 + 1, subWindow_y2 - subWindow_y1 + 1);

		rectangle(tmp, rect, Scalar(255, 0, 0));
		imshow("Test", tmp);
		waitKey(1);
		*/
		int tmpy2 = subWindow_y2, tmpx2 = subWindow_x2;
		int FrameSum = 0, P = 0;
		vector<int> FrameStage, Ps;
		FrameStage.resize(stageNum);
		Ps.resize(stageNum);
		while (subWindow_y2 < SrcImg_gray.rows){
			while (subWindow_x2 < SrcImg_gray.cols){
				for (int i = 0; i < stageNum; i++){
					double sum = 0;
					
					for (int j = 0; j < s_num[i]; j++){
						double alpha = log((1 - s[i][j].getE()) / s[i][j].getE());

						s[i][j].setPosition(subWindow_x1, subWindow_y1, scale);
						sum += alpha * s[i][j].judge(&img);

						/*if (c == 1){
							printf("%d/%d: %lf\n", j, s_num[i], alpha);
							printf("%d, %d, %d, %d\n", s[i][j].getX1(), s[i][j].getY1(), s[i][j].getX2(), s[i][j].getY2());
							system("PAUSE");
						}*/

					}
					FrameStage[i]++;
					if (sum >= th_stage[i]){
						isFace = TRUE;
						Ps[i]++;
					}
					else{
						isFace = FALSE;
						break;
					}
					
					/*if (c == 1){
						printf("%lf vs %lf\n", sum, th_stage[i]);
						system("PAUSE");
					}*/
					
				}
				if (isFace){
					Rect rect = Rect(subWindow_x1, subWindow_y1, subWindow_x2 - subWindow_x1 + 1, subWindow_y2 - subWindow_y1 + 1);
					rectangle(SrcImg, rect, Scalar(255, 0, 0));
					isFace = FALSE;
					P++;
					//imshow("Test", SrcImg);
					//waitKey(1);
				}
				FrameSum++;

				/*if (c >= 2){
					Mat tmp = SrcImg.clone();
					Rect rect = Rect(subWindow_x1, subWindow_y1, subWindow_x2 - subWindow_x1 + 1, subWindow_y2 - subWindow_y1 + 1);

					rectangle(tmp, rect, Scalar(255, 0, 0));
					imshow("Test", tmp);
					waitKey(1);
				}*/

				subWindow_x1 += move;
				subWindow_x2 += move;
			}
			subWindow_x1 = 0;
			subWindow_x2 = tmpx2;
			subWindow_y1 += move;
			subWindow_y2 += move;
		}
		subWindow_x1 = 0;
		subWindow_x2 = tmpx2;
		subWindow_y1 = 0;
		subWindow_y2 = tmpy2;

		printf("Scale: %lf, FrameSum: %d, P: %d, N: %d, PFrate: %lf\n", scale, FrameSum, P, FrameSum - P, (double)P / FrameSum);
		fprintf_s(output, "Scale: %lf, FrameSum: %d, P: %d, N: %d, PFrate: %lf\n", scale, FrameSum, P, FrameSum - P, (double)P / FrameSum);
		for (int i = 0; i < stageNum; i++){
			printf("Stage %d => Total: %d, Pass: %d, noPass: %d, PassRate: %lf\n", i + 1, FrameStage[i], Ps[i], FrameStage[i] - Ps[i], (double)Ps[i] / FrameStage[i]);
			fprintf_s(output, "Stage %d => Total: %d, Pass: %d, noPass: %d, PassRate: %lf\n", i + 1, FrameStage[i], Ps[i], FrameStage[i] - Ps[i], (double)Ps[i] / FrameStage[i]);
		}
		printf("\n");
		fprintf_s(output, "\n");
		/*
		imshow("Test", SrcImg);
		waitKey(1);
		*/
	}
	imshow("Test", SrcImg);
	fclose(output);
	waitKey(0);

	return 0;
}