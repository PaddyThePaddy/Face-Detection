/*
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <thread>
#include <conio.h>
#include "IntImg.h"
#include "soldier.h"

using namespace std;

int  sCount, eCount,*check;
IntImg *ex;
Soldier **h, **strong;
double *w, *E, *correct;

void mthread(int start,int end){   //以多執行緒執行的區段
	
	int i, j,count,res;
	if (end > sCount)
		end = sCount;
	if (start < 0 || end<0 || start >= end || start>sCount)
		printf("error: %d %d\n",start,end);
	for (i = start; i < end;i++){
		count = 0;
		//printf("%d\t",i);
		if (h[i] == NULL){
			continue;
		}
		for (j = 0; j<eCount; j++){   //判斷每個樣本
			res = abs((*(h+i))->judge(ex+j) - (int)((ex+j)->isFace));
			if (res == 0)
				count++;
			else
				*(E+i) += *(w+j);
		}
		correct[i] = (double)count / eCount;
		check[i]++;
	}
	return;
}

int main(int argc, char *argv[])
{

	FILE *example, *soldier, *output;
	IntImg intImg;
	int x1, x2, y1, y2, p, th, t, i, j, m = 0, l = 0, minI, t0, t1,c,tn,tCount;
	double wSum, eSum, min,*ET,*correctT;
	
	thread **mt;
	char str[200];

	fopen_s(&soldier,"soldiers.txt", "r");
	fopen_s(&example,"IntegralImage", "rb");

	fscanf_s(soldier, "%d", &sCount);
	h = (Soldier**)malloc(sizeof(Soldier*)*sCount);
	
	i = 0;
	while (fscanf_s(soldier, "%d%d%d%d%d%d%d", &x1, &y1, &x2, &y2, &t, &p, &th) != EOF)  //從檔案讀取弱分類器
		//if (t!=1)
			h[i++] = new Soldier(x1, y1, x2, y2, t, p, th);
	//sCount = i;
	fclose(soldier);
	printf("%d soldiers\n", sCount);
	
	fread(&eCount, sizeof(int), 1, example);//從檔案讀取樣本
	fread(&m, sizeof(int), 1, example);
	fread(&l, sizeof(int), 1, example);
	ex = (IntImg*)malloc(sizeof(IntImg)*eCount);
	fread(ex,sizeof(IntImg),eCount,example);
	fclose(example);

	printf("%d examples: %d face, %d nonface\n", eCount,m,l);

	printf("intput round count: ");
	scanf_s("%d",&tCount);
	printf("input thread count: ");
	scanf_s("%d",&tn);

	w = (double*)malloc(sizeof(double)*eCount);  //初始化樣本權重
	for (i = 0; i<eCount; i++)
		w[i] = ex[i].isFace ? (double)1 / (2 * m) : (double)1 / (2 * l);

	strong = (Soldier**)malloc(sizeof(Soldier*)*tCount);  //選拔完成的分類器儲存區
	ET = (double*)malloc(sizeof(double)*tCount);
	correctT = (double*)malloc(sizeof(double)*tCount);
	E = (double*)malloc(sizeof(double)*sCount); 
	mt = (thread**)malloc(sizeof(thread*) * tn);
	correct = (double*)malloc(sizeof(double)*sCount);

	t1 = t0 = time(NULL);

	check = (int*)malloc(sizeof(int)*sCount);
	for (t = 0; t < tCount; t++){
		wSum = 0;

		for (i = 0; i < eCount; i++)      //設定權重
			wSum += w[i];
		for (i = 0; i < eCount; i++)
			w[i] = w[i] / wSum;

		/*for (i = 0, wSum = 0; i < eCount; i++)
			wSum += w[i];

			printf("Weight sum: %lf\n", wSum);*//*

		for (i = 0; i < sCount; i++)
			check[i] = E[i] = 0;

		c = sCount / tn + 1;                         //讓每個弱分類器評判每個樣本  將工作切割成數段以多執行緒完成
		for (i = 0, j = 0; i < tn; i++){
			mt[i] = new thread(mthread, j, j + c);
			j += c;
		}
		for (i = 0; i < tn; i++){
			mt[i]->join();
			delete mt[i];
		}

		for (i = 0; i < sCount; i++)
			if (check[i] != 1 && h[i] != NULL){
			printf("%d check error: %d\n", i, check[i]);
			system("pause");
			exit(0);
			}

		for (i = 0; true; i++)           //找到最小值
			if (h[i] != NULL){
			min = E[i];
			minI = i;
			break;
			}

		for (; i < sCount; i++){
			if (h[i] == NULL){
				continue;
			}

			if (E[i] < min){
				min = E[i];
				minI = i;
			}

		}

		/*fopen_s(&output, "record.txt", "w");
		fprintf(output,"%lf %d\n",min,minI);
		for (i = 0; i < sCount; i++){
		h[i]->getData(str);
		fprintf(output, "%d %s %lf %lf\n",i, str, E[i], correct[i]);
		}
		exit(0);*//*
		strong[t] = h[minI];   //將最小值放到儲存區
		h[minI] = NULL;
		ET[t] = E[minI];

		correctT[t] = correct[minI];

		for (i = 0; i < eCount; i++){        //設定樣本權重
			if ((strong[t]->judge(ex + i) - (int)(ex[i].isFace)) == 0)
				w[i] = w[i] * min / (1 - min);
			/*else
			w[i] = w[i] * (1 - min) / min;*//*
		}
		strong[t]->getData(str);
		printf("%-4d: %s E: %E  Correct rate: %lf\ntime: %d\n", t, str, ET[t], correctT[t], time(NULL) - t1);
		t1 = time(NULL);

		if (_kbhit()){
			char ch=0;
			while (_kbhit() && ch != 'p' && ch != 'P')
				ch = _getch();
			if (ch == 'p' || ch=='P'){
				int tmp;
				printf("\nnow using %d threads\nwant chang? (0 to exit and save result)\n", tn);
				scanf_s("%d", &tmp);
				if (tmp == 0){
					t++;
					break;
				}
				else{
					tn = tmp;
					free(mt);
					mt = (thread**)malloc(sizeof(thread*) * tn);
				}
			}
		}
		
	}
	
	fopen_s(&output,"output.txt", "w");

	fprintf(output, "%d\n", t);

	for (i = 0; i<t; i++){
		strong[i]->getData(str);
		fprintf(output, "%s %E %lf\n", str,ET[i],correctT[i]);
	}
	fclose(output);
	printf("complete!\nused time: %d\n", time(0) - t0);

	system("pause");
	return EXIT_SUCCESS;
}
*/
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <thread>
#include <conio.h>
#include "IntImg.h"
#include "soldier.h"
class Fv{
	public:
		int fValue;
		int eNum;
};
int compare(const void * a, const void * b)
{
	Fv* ta = *(Fv**)a;
	Fv* tb = *(Fv**)b;
	if (ta->fValue <  tb->fValue) return -1;
	if (ta->fValue == tb->fValue) return 0;
	if (ta->fValue >  tb->fValue) return 1;
}
void mthread(int start, int end){   //以多執行緒執行的區段
	/*
	int i, j, count, res;
	if (end > sCount)
		end = sCount;
	if (start < 0 || end<0 || start >= end || start>sCount)
		printf("error: %d %d\n", start, end);
	for (i = start; i < end; i++){
		count = 0;
		//printf("%d\t",i);
		if (h[i] == NULL){
			continue;
		}
		for (j = 0; j<eCount; j++){   //判斷每個樣本
			res = abs((*(h + i))->judge(ex + j) - (int)((ex + j)->isFace));
			if (res == 0)
				count++;
			else
				*(E + i) += *(w + j);
		}
		correct[i] = (double)count / eCount;
		check[i]++;
	}
	return;*/
}
using namespace std;
int tn;
int main(){
	int t0, t1;
	int sCount,eCount,tCount,sTemp,ssTemp,eflagTemp,kflag,pTemp;
	int x1, x2, y1, y2,m,l,i,t,j,k;
	int *check,eflag;
	int **fs;
	thread ** mt;
	char str[200];
	double *w, *E, *correct, *ET, *correctT, Tp, Tn, Sp, Sn, e,eMin,eeMin,cor;
	IntImg *ex;
	Soldier *soldier[180000],**strong;
	Fv *fv;
	FILE *example;
	fread(&eCount, sizeof(int), 1, example);//從檔案讀取樣本
	fread(&m, sizeof(int), 1, example);
	fread(&l, sizeof(int), 1, example);
	ex = (IntImg*)malloc(sizeof(IntImg)*eCount);
	fread(ex, sizeof(IntImg), eCount, example);
	fclose(example);

	printf("%d examples: %d face, %d nonface\n", eCount, m, l);

	printf("intput round count: ");
	scanf_s("%d", &tCount);
	printf("input thread count: ");
	scanf_s("%d", &tn);

	w = (double*)malloc(sizeof(double)*eCount);  //初始化樣本權重
	for (i = 0; i<eCount; i++)
		w[i] = ex[i].isFace ? (double)1 / (2 * m) : (double)1 / (2 * l);

	strong = (Soldier**)malloc(sizeof(Soldier*)*tCount);  //選拔完成的分類器儲存區
	ET = (double*)malloc(sizeof(double)*tCount);
	correctT = (double*)malloc(sizeof(double)*tCount);
	E = (double*)malloc(sizeof(double)*sCount);
	mt = (thread**)malloc(sizeof(thread*) * tn);
	correct = (double*)malloc(sizeof(double)*sCount);
	fv = (Fv*)malloc(sizeof(Fv)*eCount);
	fs = (int **)malloc(sizeof(int)*eCount*sCount);
	//開始窮舉產生分類器
	/*規則:
	x1<x2 y1<y2
	為了計算方便,實際範圍不包含x2那個col以及y2那個row
	*/
	sCount = 0;
	for (x1 = 0; x1 < 24; x1++)
		for (y1 = 0; y1 < 24; y1++)
			for (x2 = x1 + 1; x2 <= 24; x2++)
				for (y2 = y1 + 1; y2 <= 24; y2++){
		if ((x2 - x1) % 2 == 0)
			soldier[sCount++] = new Soldier(x1, y1, x2, y2, 0, 0, 0);
		if ((y2 - y1) % 2 == 0)
			soldier[sCount++] = new Soldier(x1, y1, x2, y2, 1, 0, 0);
		if ((x2 - x1) % 3 == 0)
			soldier[sCount++] = new Soldier(x1, y1, x2, y2, 2, 0, 0);
		if ((y2 - y1) % 3 == 0)
			soldier[sCount++] = new Soldier(x1, y1, x2, y2, 3, 0, 0);
		if ((x2 - x1) % 2 == 0 && (y2 - y1) % 2 == 0)
			soldier[sCount++] = new Soldier(x1, y1, x2, y2, 4, 0, 0);
				}
	//分類器產生結束
	//將sample以feature value去排序
	for (i = 0; i < sCount; i++){
		
		for (j = 0; j < eCount; j++){
			fv[j].fValue=soldier[i]->comput(&ex[j]);
			fv[j].eNum = j;
		}

		qsort(fv,eCount,sizeof(int),compare);

		for (j = 0; j < eCount; j++)
			fs[i][j]=fv[j].eNum;

	}
	//排序完畢 結果在fs

	t1 = t0 = time(NULL);

	check = (int*)malloc(sizeof(int)*sCount);

	for (t = 0;t<tCount;t++){
		
		for (Tp = 0, Tn = 0, i = 0; i < eCount; i++){
			if (ex[i].isFace == true)       // compute  T+ and T-
				Tp += w[i];
			else
				Tn += w[i];
		}
		for (k = 0; k < sCount; k++){

			for (Sp = 0, Sn = 0, i = 0; i < eCount; i++){
			
					if (ex[fs[k][i]].isFace == true) // compute  S+ and S-
						Sp += w[fs[k][i]];
					else
						Sn += w[fs[k][i]];
				
					if (Sp + (Tn - Sn) < Sn + (Tp - Sp)){// compute e
						e = Sp + (Tn - Sn);
						eflag = 0;
					}
					else{
						e = Sn + (Tp - Sp);
						eflag = 1;
					}
				
					if (i == 0){
						eeMin = e;
						ssTemp = fs[k][i];
						eflagTemp = eflag;
					}
					if (eeMin > e){
						eeMin = e;
						ssTemp = fs[k][i];
						eflagTemp = eflag;
					}
			}

			if (k == 0){
				eMin = eeMin;
				sTemp = ssTemp;
				kflag = 0;
				pTemp = eflagTemp;
			}

			
			if (eMin > e){
				eMin = eeMin;
				sTemp = ssTemp;
				kflag = k;
				pTemp = eflagTemp; //e = min(Sp+(Tn-Sn),Sn+(Tp-Sp)) 左邊比較小時  p = 0
								   //p=0: 小於threshold的要判斷為非人臉; p=1: 大於threshold的要判斷為非人臉 
			}

			

		}
		soldier[kflag]->setP(pTemp);// 已找出當圈最佳小兵  
		soldier[kflag]->setTh(sTemp);
		soldier[kflag]->setE(eMin);
		soldier[kflag]->getData(str);
		
		//cout << "Soldier " << t << " " << str << endl;
		//cout << "Time : " << t1 - t0<<" sec"<<endl;
		strong[t] = soldier[kflag];

		int ctmp=0;
		
		for (i = 0; i < eCount; i++){        //設定樣本權重
			if ((strong[t]->judge(ex + i) - (int)(ex[i].isFace)) == 0){
				w[i] = w[i] * eMin / (1 - eMin);
				ctmp++;
			}
			/*else
			w[i] = w[i] * (1 - min) / min;*/
			}
		cor = (double)ctmp / eCount;
		printf("%-4d: %s E: %E \ntime: %d\n", t, str, eMin, cor, time(NULL) - t1);
	}





	return 0;
}