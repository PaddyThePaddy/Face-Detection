#include <cstdlib>
#include <iostream>
#include <ctime>
#include <thread>
#include <conio.h>
#include "IntImg.h"
#include "soldier.h"
using namespace std;
class Fv{
public:
	int fValue;
	int eNum;
};

int sCount, eCount;
Soldier *soldier[180000];
IntImg *ex;
int *sThread, *pThread;
double *w, *eThread;
double Tp, Tn;
long long int seekDegree, seekDegree_2;
FILE *example;
int *exCanUse;
void mthread(int start, int end){   //以多執行緒執行的區段

	int i, j, k;
	int mkey = 0;
	double Sp, Sn, e, eeMin;
	int eflag, ssTemp, eflagTemp,key;
	int *fss;
	FILE *example_2;
	int *check = new int[eCount];
	fopen_s(&example_2, "SortedIntegralImage", "rb");
	fss = (int*)malloc(sizeof(int)*eCount);
	if (end > sCount)
		end = sCount;
	if (start < 0 || end<0 || start >= end || start>sCount)
		printf("error: %d %d\n", start, end);
	for (k = start; k < end; k++){
		
		_fseeki64(example_2, seekDegree, SEEK_SET);
		_fseeki64(example_2, (long long int)seekDegree_2*k + sizeof(int) * 5, SEEK_CUR);
		fread(fss, sizeof(int), eCount, example_2);

		for (i = 0; i < eCount; i++)
			check[i] = 0;
		for (i = 0; i < eCount; i++)
			check[fss[i]]++;
		for (i = 0; i < eCount; i++)
			if (check[i] != 1)
				printf("help");

		for (Sp = 0, Sn = 0, i = 0; i < eCount; i++){
			if (exCanUse[fss[i]] == false){
				continue;
			}
		
			if (fss[i] >= eCount || fss[i]<0){
				printf("hello");
			}

			if (ex[fss[i]].isFace == true) // compute  S+ and S-
				Sp += w[fss[i]];
			else
				Sn += w[fss[i]];

			if (Sp + (Tn - Sn) < Sn + (Tp - Sp)){// compute e
				e = Sp + (Tn - Sn);
				eflag = 0;
			}
			else{
				e = Sn + (Tp - Sp);
				eflag = 1;
			}
			if (e < 0){
				Sn = Sn;
			}
			if (mkey == 0){
				eeMin = e;
				ssTemp = fss[i];
				eflagTemp = eflag;
				mkey = 1;
			}
			if (eeMin < 0){
				eeMin = eeMin;
			}
			if (eeMin > e){
				eeMin = e;
				ssTemp = fss[i];
				eflagTemp = eflag;
			}
		}

		*(eThread + k) = eeMin;
		*(sThread + k) = ssTemp;
		*(pThread + k) = eflagTemp;	//e = min(Sp+(Tn-Sn),Sn+(Tp-Sp)) 左邊比較小時  p = 0
		//p=0: 小於threshold的要判斷為非人臉; p=1: 大於threshold的要判斷為非人臉 

	}
	free(fss);
	fclose(example_2);
	return;
}

int c, tn,m,l;
int xx1, xx2, yy1, yy2, type;
FILE *outdata;
char str[200];
thread ** mt;
int tCount;
double *E, *correct, *ET, *correctT, cor;

void preset(){

fopen_s(&example, "SortedIntegralImage", "rb");
if (!example)cout << "example error" << endl;

fread(&eCount, sizeof(int), 1, example);//從檔案讀取樣本
fread(&m, sizeof(int), 1, example);
fread(&l, sizeof(int), 1, example);
ex = (IntImg*)malloc(sizeof(IntImg)*eCount);
fread(ex, sizeof(IntImg), eCount, example);
fread(&sCount, sizeof(int), 1, example);//讀取sCount;
seekDegree = sizeof(int) * 4 + sizeof(IntImg)*eCount;
seekDegree_2 = sizeof(int) * 5 + sizeof(int)*eCount;
cout << sCount << endl;

for (int i = 0; i < sCount; i++){
	fread(&xx1, sizeof(int), 1, example);
	fread(&yy1, sizeof(int), 1, example);
	fread(&xx2, sizeof(int), 1, example);
	fread(&yy2, sizeof(int), 1, example);
	fread(&type, sizeof(int), 1, example);
	soldier[i] = new Soldier(xx1, yy1, xx2, yy2, type, 0, 0);
	_fseeki64(example, sizeof(int)*eCount, SEEK_CUR);
	//fread(fs,sizeof(int),eCount,example);
	if (i == 0){
		soldier[i]->getData(str);
		printf("%d : %s \n", i, str);
	}
}
	w = (double*)malloc(sizeof(double)*eCount);  //初始化樣本權重
	for (int i = 0; i<eCount; i++)
		w[i] = ex[i].isFace ? (double)1 / (2 * m) : (double)1 / (2 * l);

	
E = (double*)malloc(sizeof(double)*sCount);
mt = (thread**)malloc(sizeof(thread*) * tn);
correct = (double*)malloc(sizeof(double)*sCount);
eThread = (double*)malloc(sizeof(double) * sCount);
sThread = (int*)malloc(sizeof(int) * sCount);
pThread = (int*)malloc(sizeof(int) * sCount);
exCanUse = (int*)malloc(sizeof(int)*eCount);
for (int i = 0; i < eCount; i++)
	*(exCanUse + i) = 1;
//strong = (Soldier**)malloc(sizeof(Soldier*)*tCount);  //選拔完成的分類器儲存區


}
int training(int tC, Soldier **strong){
	int t0, t1;
	
	double wSum;
	int i, t, j, k;
	int *check;

	

	//Soldier **strong;
	

	
	fopen_s(&outdata, "outdata.txt", "w");
		

	//system("PAUSE");
	//printf("%d examples: %d face, %d nonface, sCount: %d \n", eCount, m, l, sCount);
	/*for (i = 0; i < eCount; i += 1000){
		soldier[i]->getData(str);
		printf("%d : %s\n", i, str);
	}*/

	//printf("intput round count: ");
	//scanf_s("%d", &tCount);
	tCount = tC;
	fprintf_s(outdata, "%d\n", tCount);
	
	tn = 1;

	
	

	//strong = (Soldier**)malloc(sizeof(Soldier*)*tCount);  //選拔完成的分類器儲存區
	

	ET = (double*)malloc(sizeof(double)*tCount);
	correctT = (double*)malloc(sizeof(double)*tCount);
	






	check = (int*)malloc(sizeof(int)*sCount);

	for (t = 0; t<1; t++){
		t0 = time(NULL);
		wSum = 0;

		for (i = 0; i < eCount; i++)   //設定權重 (正規化)
			if (exCanUse[i])
				wSum += w[i];
		
		for (i = 0; i < eCount; i++)
			if (exCanUse[i])
				w[i] = w[i] / wSum;

		for (i = 0, wSum = 0; i < eCount; i++)
			if (exCanUse[i])
				wSum += w[i];

		//printf("Weight sum: %lf\n", wSum);

		for (i = 0; i < sCount; i++)
			check[i] = E[i] = 0;

		for (Tp = 0, Tn = 0, i = 0; i < eCount; i++){
			if (!exCanUse[i]) 
				continue;
			if (ex[i].isFace == true)       // compute  T+ and T-
				Tp += w[i];
			else
				Tn += w[i];
		}

		c = sCount / tn + 1;                         //讓每個弱分類器評判每個樣本  將工作切割成數段以多執行緒完成
		for (i = 0, j = 0; i < tn; i++){
			mt[i] = new thread(mthread, j, j + c);
			j += c;
		}
		for (i = 0; i < tn; i++){
			mt[i]->join();
			delete mt[i];
		}

		double eMin;
		int iMin, ctmp = 0;

		for (i = 1, eMin = *eThread, iMin = 0; i < sCount; i++){
			if (eMin > *(eThread + i)){
				eMin = *(eThread + i);
				iMin = i;
			}
		}
		soldier[iMin]->setP(*(pThread + iMin));// 已找出當圈最佳小兵  
		soldier[iMin]->setTh(soldier[iMin]->comput(&ex[*(sThread + iMin)]));
		soldier[iMin]->setE(*(eThread + iMin));
		soldier[iMin]->getData(str);

	
		strong[tC-1] = soldier[iMin];

		double wtmp = 0;
		int jugT, jugF;
		for (i = 0, jugT = 0, jugF = 0; i < eCount; i++){        //設定樣本權重
			if (!exCanUse[i])
				continue;
			if ((strong[t]->judge(ex + i) - (int)(ex[i].isFace)) == 0){
				wtmp = w[i];
				w[i] = w[i] * eMin / (1 - eMin);
				ctmp++;

				
			}
			else{
				
				if (ex[i].isFace)
					jugT++;
				else
					jugF++;
			}
		}
		cor = (double)ctmp / eCount;
		t1 = time(NULL);
		//printf("%-4d: %s correct rate :%lf , false isFace: %d , nonface: %d\ntime: %d\n", t, str, cor, jugT, jugF, t1 - t0);
		fprintf_s(outdata, "%s %lf\n", str, cor);
	}



	fclose(example);
	fclose(outdata);
	return 0;
}