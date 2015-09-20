#include <iostream>
#include <cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include "IntImg_d.h"
#include "Soldier_d.h"
#include <conio.h>

using namespace cv;
using namespace std;

#define FACTOR 1.25
#define SHIFT 1.5

Mat SrcImg, SrcImg_gray;
int stageNum;
vector<double> th_stage;
vector<vector<Soldier>> s; 
vector<int> s_num;
IntImg img, img_square;
FILE *output;
vector<Point> pt;
Mat OutImg_click;
Mat click_subWindow;
int scan_flag;

FILE *click_pass, *click_nopass;

Mat drawFeature(Mat click_tmp, Soldier feature){
	int x1 = feature.tori_x1, x2 = feature.tori_x2, y1 = feature.tori_y1, y2 = feature.tori_y2;

	for (int i = 0; i < click_tmp.rows; i++){
		for (int j = 0; j < click_tmp.cols; j++){
			if (i >= y1 && i <= y2 && j >= x1 && j <= x2){
				if (i == y1 || i == y2 || j == x1 || j == x2){
					click_tmp.at<Vec3b>(i, j)[0] = 255;
					click_tmp.at<Vec3b>(i, j)[1] = 0;
					click_tmp.at<Vec3b>(i, j)[2] = 0;
				}
				else{
					switch (feature.type){
					case 0:
						if (j < (x1 + x2) / 2){
							click_tmp.at<Vec3b>(i, j)[0] = 255;
							click_tmp.at<Vec3b>(i, j)[1] = 255;
							click_tmp.at<Vec3b>(i, j)[2] = 255;
						}
						else{
							click_tmp.at<Vec3b>(i, j)[0] = 0;
							click_tmp.at<Vec3b>(i, j)[1] = 0;
							click_tmp.at<Vec3b>(i, j)[2] = 0;
						}
						break;
					case 1:
						if (i < (y1 + y2) / 2){
							click_tmp.at<Vec3b>(i, j)[0] = 255;
							click_tmp.at<Vec3b>(i, j)[1] = 255;
							click_tmp.at<Vec3b>(i, j)[2] = 255;
						}
						else{
							click_tmp.at<Vec3b>(i, j)[0] = 0;
							click_tmp.at<Vec3b>(i, j)[1] = 0;
							click_tmp.at<Vec3b>(i, j)[2] = 0;
						}
						break;
					case 2:
						if ((j < x1 + (x2 - x1) / 3) || (j >= x1 + (double)(x2 - x1) * 2 / 3)){
							click_tmp.at<Vec3b>(i, j)[0] = 255;
							click_tmp.at<Vec3b>(i, j)[1] = 255;
							click_tmp.at<Vec3b>(i, j)[2] = 255;
						}
						else{
							click_tmp.at<Vec3b>(i, j)[0] = 0;
							click_tmp.at<Vec3b>(i, j)[1] = 0;
							click_tmp.at<Vec3b>(i, j)[2] = 0;
						}
						break;
					case 3:
						if ((i < y1 + (y2 - y1) / 3) || (i >= y1 + (double)(y2 - y1) * 2 / 3)){
							click_tmp.at<Vec3b>(i, j)[0] = 255;
							click_tmp.at<Vec3b>(i, j)[1] = 255;
							click_tmp.at<Vec3b>(i, j)[2] = 255;
						}
						else{
							click_tmp.at<Vec3b>(i, j)[0] = 0;
							click_tmp.at<Vec3b>(i, j)[1] = 0;
							click_tmp.at<Vec3b>(i, j)[2] = 0;
						}
						break;
					case 4:
						if (((j < (x1 + x2) / 2) && (i < (y1 + y2) / 2)) || ((j >= (x1 + x2) / 2) && (i >= (y1 + y2) / 2))){
							click_tmp.at<Vec3b>(i, j)[0] = 255;
							click_tmp.at<Vec3b>(i, j)[1] = 255;
							click_tmp.at<Vec3b>(i, j)[2] = 255;
						}
						else{
							click_tmp.at<Vec3b>(i, j)[0] = 0;
							click_tmp.at<Vec3b>(i, j)[1] = 0;
							click_tmp.at<Vec3b>(i, j)[2] = 0;
						}
						break;

					}
				}
			}
		}
	}

	return click_tmp;
}

void click_judge()
{
	int subWindow_x1 = pt[0].x, subWindow_y1 = pt[0].y, subWindow_x2 = pt[1].x, subWindow_y2 = pt[1].y;
	int area = (subWindow_x2 - subWindow_x1) * (subWindow_y2 - subWindow_y1);
	long double sigma_square, Lx, Lm;
	double sigma;
	bool isFace = FALSE;

	Lx = (long double)img_square.data[subWindow_y2-1][subWindow_x2-1];
	Lm = (long double)img.data[subWindow_y2-1][subWindow_x2-1];
	if (subWindow_x1 > 0){
		Lx -= img_square.data[subWindow_y2-1][subWindow_x1 - 1];
		Lm -= img.data[subWindow_y2-1][subWindow_x1 - 1];
	}
	if (subWindow_y1 > 0){
		Lx -= img_square.data[subWindow_y1 - 1][subWindow_x2-1];
		Lm -= img.data[subWindow_y1 - 1][subWindow_x2-1];
	}
	if (subWindow_x1 > 0 && subWindow_y1 > 0){
		Lx += img_square.data[subWindow_y1 - 1][subWindow_x1 - 1];
		Lm += img.data[subWindow_y1 - 1][subWindow_x1 - 1];
	}
	Lm = Lm / area;
	sigma_square = Lx / area - Lm * Lm;
	
	/*檢測sigma有無算錯
	long double lx=0, lm=0;
	int a = 0;
	unsigned char *p;
	for (int i = subWindow_y1; i < subWindow_y2; i++){
		p = SrcImg_gray.ptr<unsigned char>(i);
		for (int j = subWindow_x1; j < subWindow_x2; j++){
			lm += p[j];
			lx += p[j] * p[j];
			a++;
			//printf("%lf %lf\n", lm, lx);
			//system("PAUSE");
		}
	}
	lm = lm / area;
	printf("%lf %lf %d\n%lf %lf %d\n", Lm, Lx, area, lm, lx, a);
	system("PAUSE");
	*/
	
	
	
	click_subWindow = Mat::zeros((subWindow_x2 - subWindow_x1), (subWindow_y2 - subWindow_y1), OutImg_click.type());
	//printf("%d %d\n", subWindow_x1, subWindow_x2);
	for (int i = 0, io = subWindow_x1; io < subWindow_x2; i++, io++)
		for (int j = 0, jo = subWindow_y1; jo < subWindow_y2; j++, jo++){
		click_subWindow.at<Vec3b>(j, i)[0] = SrcImg.at<Vec3b>(jo, io)[0];
		click_subWindow.at<Vec3b>(j, i)[1] = SrcImg.at<Vec3b>(jo, io)[1];
		click_subWindow.at<Vec3b>(j, i)[2] = SrcImg.at<Vec3b>(jo, io)[2];
		}
	imshow("click_subWindow", click_subWindow);

	int cp = 0, cn = 0;

	if (sigma_square < 0)
		sigma_square *= -1;
	sigma = sqrt(sigma_square);
	if (sigma < 1e-15 && sigma > -1e-15){
		printf("(%d, %d) to (%d, %d)'s sigma == 0.\n", subWindow_x1, subWindow_y1, subWindow_x2, subWindow_y2);
	}
	else{
		fopen_s(&click_pass, "click_pass.txt", "w");
		system("rmdir click_pass /s /q");
		system("mkdir click_pass");
		fopen_s(&click_nopass, "click_nopass.txt", "w");
		system("rmdir click_nopass /s /q");
		system("mkdir click_nopass");

		double scale = (double)(subWindow_x2 - subWindow_x1) / 24;
		for (int i = 0; i < stageNum; i++)
			for (int j = 0; j < s_num[i]; j++)
				s[i][j].setScale(scale);

		for (int i = 0; i < stageNum; i++){
			double sum = 0;

			for (int j = 0; j < s_num[i]; j++){
				double alpha = log((1 - s[i][j].getE()) / s[i][j].getE());

				s[i][j].setPosition(subWindow_x1, subWindow_y1, sigma);
				int result = s[i][j].judge(&img);
				sum += alpha * result;
				printf("th = %lf  sum = %lf\n", s[i][j].th, s[i][j].sum);

				int x1 = s[i][j].getX1(), x2 = s[i][j].getX2(), y1 = s[i][j].getY1(), y2 = s[i][j].getY2();
				char imgName[100] = {0};
				sprintf_s(imgName, "[%03d](%3d,%3d)to(%3d,%3d)", j, x1, y1, x2, y2);
				if (result == 1){
					fprintf_s(click_pass, "%s: alpha=%lf p=%d th=%lf sum=%lf\n", imgName, alpha, s[i][j].p, s[i][j].th, s[i][j].sum);
					Mat click_tmp = click_subWindow.clone();
					char save[200] = { 0 };
					sprintf_s(save, "click_pass\\%s.jpg", imgName);
					click_tmp = drawFeature(click_tmp, s[i][j]);
					imwrite(save, click_tmp);
					cp++;
				}
				else{
					fprintf_s(click_nopass, "%s: alpha=%lf p=%d th=%lf sum=%lf\n", imgName, alpha, s[i][j].p, s[i][j].th, s[i][j].sum);
					Mat click_tmp = click_subWindow.clone();
					char save[200] = { 0 };
					sprintf_s(save, "click_nopass\\%s.jpg", imgName);
					click_tmp = drawFeature(click_tmp, s[i][j]);
					imwrite(save, click_tmp);
					cn++;
				}
			}
			if (sum >= th_stage[i]){
				isFace = TRUE;
			}
			else{
				isFace = FALSE;
				break;
			}
		}
		if (isFace){
			Rect rect = Rect(subWindow_x1, subWindow_y1, subWindow_x2 - subWindow_x1, subWindow_y2 - subWindow_y1);
			rectangle(OutImg_click, rect, Scalar(255, 0, 0));
			imshow("Output_Click", OutImg_click);
			waitKey(1);
		}
	}

	fclose(click_pass);
	fclose(click_nopass);
	printf("\npass=%d nopass=%d\n", cp, cn);
	printf("\n(%d, %d) to (%d, %d)\narea = %d * %d = %d\nsigma = %lf\nisFace = %d\n", subWindow_x1, subWindow_y1, subWindow_x2, subWindow_y2, (subWindow_x2 - subWindow_x1 + 1), (subWindow_y2 - subWindow_y1 + 1), (subWindow_x2 - subWindow_x1 + 1) * (subWindow_y2 - subWindow_y1 + 1), sigma, isFace);

	return;
}

void on_mouse(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		switch (pt.size())
		{
		case 0:
			pt.push_back(Point(x, y));
			circle(OutImg_click, pt[pt.size() - 1], 5, CV_RGB(255, 0, 0), -1);
			imshow("Output_Click", OutImg_click);
			break;
		case 1:
			if (x <= pt[0].x || y <= pt[0].y)
				break;
			if (x - pt[0].x < y - pt[0].y)
				y = pt[0].y + x - pt[0].x;
			else
				x = pt[0].x + y - pt[0].y;
			pt.push_back(Point(x, y));		
			circle(OutImg_click, pt[pt.size() - 1], 5, CV_RGB(255, 0, 0), -1);
			imshow("Output_Click", OutImg_click);
			waitKey(1);
			click_judge();
			break;
		case 2:
			pt.clear();
			OutImg_click = SrcImg.clone();
			imshow("Output_Click", OutImg_click);
			break;
		}
	}
	return;
}

void click()
{
	system("CLS");
	cout << "請用滑鼠點選subWindow之左上及右下點座標" << endl;
	pt.clear();
	OutImg_click = SrcImg.clone();
	imshow("Output_Click", OutImg_click);
	setMouseCallback("Output_Click", on_mouse, 0);
	waitKey(0);
	return;
}

void scan()
{
	system("CLS");

	Mat OutImg = SrcImg.clone();

	double scale = 1, move = SHIFT;
	double sigma;
	int subWindow_x1 = 0, subWindow_y1 = 0, ori_subWindow_x2 = 24, ori_subWindow_y2 = 24, subWindow_x2 = 24, subWindow_y2 = 24;
	bool isFace = FALSE;

	for (int c = 0;; c++, scale *= FACTOR, move *= FACTOR){
		subWindow_x2 = ori_subWindow_x2 * scale;
		subWindow_y2 = ori_subWindow_y2 * scale;
		/*
		if (c != 0){
			subWindow_x2 *= FACTOR;
			subWindow_y2 *= FACTOR;
		}
		*/

		if (subWindow_x2 > SrcImg_gray.cols || subWindow_y2 > SrcImg_gray.rows)
			break;

		for (int i = 0; i < stageNum; i++)
			for (int j = 0; j < s_num[i]; j++)
				s[i][j].setScale(scale);

		int tmpy2 = subWindow_y2, tmpx2 = subWindow_x2;
		int FrameSum = 0, P = 0;
		vector<int> FrameStage, Ps;
		FrameStage.resize(stageNum);
		Ps.resize(stageNum);
		while (subWindow_y2 <= SrcImg_gray.rows){
			while (subWindow_x2 <= SrcImg_gray.cols){
				int area = (subWindow_x2 - subWindow_x1) * (subWindow_y2 - subWindow_y1);
				long double sigma_square, Lx, Lm;

				Lx = (long double)img_square.data[subWindow_y2-1][subWindow_x2-1];
				Lm = (long double)img.data[subWindow_y2-1][subWindow_x2-1];
				if (subWindow_x1 > 0){
					Lx -= img_square.data[subWindow_y2-1][subWindow_x1 - 1];
					Lm -= img.data[subWindow_y2-1][subWindow_x1 - 1];
				}
				if (subWindow_y1 > 0){
					Lx -= img_square.data[subWindow_y1 - 1][subWindow_x2-1];
					Lm -= img.data[subWindow_y1 - 1][subWindow_x2-1];
				}
				if (subWindow_x1 > 0 && subWindow_y1 > 0){
					Lx += img_square.data[subWindow_y1 - 1][subWindow_x1 - 1];
					Lm += img.data[subWindow_y1 - 1][subWindow_x1 - 1];
				}
				Lm = Lm / area;
				sigma_square = Lx / area - Lm * Lm;

				if (sigma_square < 0)
					sigma_square *= -1;
				sigma = sqrt(sigma_square);
				if (sigma < 1e-15 && sigma > -1e-15){
					//printf("(%d, %d) to (%d, %d)'s sigma == 0.\n", subWindow_x1, subWindow_y1, subWindow_x2, subWindow_y2);
				}
				else{
					for (int i = 0; i < stageNum; i++){
						double sum = 0;

						for (int j = 0; j < s_num[i]; j++){
							double alpha = log((1 - s[i][j].getE()) / s[i][j].getE());

							s[i][j].setPosition(subWindow_x1, subWindow_y1, sigma);
							sum += alpha * s[i][j].judge(&img);
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
					}
					if (isFace){
						Rect rect = Rect(subWindow_x1, subWindow_y1, subWindow_x2 - subWindow_x1, subWindow_y2 - subWindow_y1);
						rectangle(OutImg, rect, Scalar(255, 0, 0));
						isFace = FALSE;
						P++;
					}
				}
				FrameSum++;

				if (scan_flag){
					Mat tmp = OutImg.clone();
					Rect rect = Rect(subWindow_x1, subWindow_y1, subWindow_x2 - subWindow_x1, subWindow_y2 - subWindow_y1);

					rectangle(tmp, rect, Scalar(255, 0, 0));
					imshow("Output_Scan", tmp);
					waitKey(1);
				}

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
	}
	imshow("Output_Scan", OutImg);
	fclose(output);
	waitKey(0);
	return;
}

int main(void)
{
	system("CLS");

	FILE *soldier_cascade;
	int s_total = 0;

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
		int x1, y1, x2, y2, type, p;
		double e;
		long double th;

		fscanf_s(soldier_cascade, "%d", &s_num[i]);
		s_total += s_num[i];
		s[i].resize(s_num[i]);
		for (int j = 0; j < s_num[i]; j++){
			fscanf_s(soldier_cascade, "%d %d %d %d %d %d %lf %lf", &x1, &y1, &x2, &y2, &type, &p, &th, &e);
			//printf("%d %d %d %d %d %d %E %E\n", x1, y1, x2, y2, type, p, th, e);
			//system("PAUSE");
			s[i][j] = Soldier(x1, y1, x2, y2, type, p, th, e);
		}
		fscanf_s(soldier_cascade, "%lf", &th_stage[i]);
	}
	fclose(soldier_cascade);

	unsigned char *p;

	SrcImg = imread("test16.jpg");
	imshow("SrcImg", SrcImg);
	waitKey(1);
	cvtColor(SrcImg, SrcImg_gray, CV_RGB2GRAY);

	img = IntImg(SrcImg_gray.rows, SrcImg_gray.cols);
	img_square = IntImg(SrcImg_gray.rows, SrcImg_gray.cols);
	double sigma;

	for (int i = 0; i < SrcImg_gray.rows; i++){
		p = SrcImg_gray.ptr<unsigned char>(i);
		for (int j = 0; j < SrcImg_gray.cols; j++){
			if (i == 0 && j == 0){
				img.data[i][j] = p[j];
				img_square.data[i][j] = p[j] * p[j];
			}
			else if (i == 0){
				img.data[i][j] = p[j] + img.data[i][j - 1];
				img_square.data[i][j] = p[j] * p[j] + img_square.data[i][j - 1];
			}
			else if (j == 0){
				img.data[i][j] = p[j] + img.data[i - 1][j];
				img_square.data[i][j] = p[j] * p[j] + img_square.data[i - 1][j];
			}
			else{
				img.data[i][j] = p[j] + img.data[i][j - 1] + img.data[i - 1][j] - img.data[i - 1][j - 1];
				img_square.data[i][j] = p[j] * p[j] + img_square.data[i][j - 1] + img_square.data[i - 1][j] - img_square.data[i - 1][j - 1];
			}		
		}
	}
	
	sigma = (long double)img_square.data[SrcImg_gray.rows - 1][SrcImg_gray.cols - 1] / (SrcImg_gray.rows * SrcImg_gray.cols) - ((long double)img.data[SrcImg_gray.rows - 1][SrcImg_gray.cols - 1] / (SrcImg_gray.rows * SrcImg_gray.cols)) * ((long double)img.data[SrcImg_gray.rows - 1][SrcImg_gray.cols - 1] / (SrcImg_gray.rows * SrcImg_gray.cols));
	if (sigma < 0)
		sigma *= -1;
	sigma = sqrt(sigma);
	if (sigma < 1e-15 && sigma > -1e-15){
		cout << "This image's sigma == 0." << endl;
		system("PAUSE");
		return 0;
	}

	while (1){
		scan_flag = 0;
		system("CLS");
		cout << "Stage number: " << stageNum << endl << "Total feature number: " << s_total << endl;
		cout << endl << "Menu: (please input 1 or 2 to run, and 3 to exit)" << endl;
		cout << "1. Scan mode" << endl;
		cout << "2. Click mode" << endl;
		int c;
		cin >> c;

		if (c == 3)
			break;
		else if (c == 1){
			cout << "是否印出每個subWindow? (Y/N)\n";
			char ch;
			cin >> ch;
			if (ch == 'Y' || ch == 'y')
				scan_flag = 1;
			scan();
		}
		else if (c == 2){
			click();
		}
	}
	
	return 0;
}