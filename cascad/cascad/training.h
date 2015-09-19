#include <cstdlib>
#include <iostream>
#include <ctime>
#include <thread>
#include <conio.h>
#include "IntImg.h"
#include "soldier.h"
#include <cmath>
using namespace std;

class Fv{
public:
	double fValue;
	int eNum;
};
char curFileName[40];
int c, tn, m, l;
int sCount, eCount;
Soldier *soldier[180000];
IntImg *ex;
int *sThread, *pThread;
double *w, *eThread;
double Tp, Tn;
long long int seekDegree, seekDegree_2;
FILE *example;
int *exCanUse;
double Lz, Lm, Lx, Lxtmp, *zigma;// Light Nomoralize 
int compare(const void * a, const void * b)
{
	double t;
	t = (((Fv*)a)->fValue - ((Fv*)b)->fValue);
	if (t < 0)
		return -1;
	else if (t == 0)
		return 0;
	else return 1;
}
int compares(const void *  a , const void * b)
{
	return  (double*)a - (double*)b;

}
void mthread(int start, int end){   //以多執行緒執行的區段
	
	int i, j, k;
	int mkey = 0;
	double Sp, Sn, e, eeMin;
	double Tpp, Tnn, Cpp, Cnn;
	int eflag, ssTemp, eflagTemp,key;
	Fv *fss;
	FILE *example_2;
	int *check = new int[eCount];
	
	fopen_s(&example_2, curFileName, "rb");
	//fss = (int*)malloc(sizeof(int)*eCount);
	fss = (Fv*)malloc(sizeof(Fv)*(m+l));
	Cpp = 0;
	Cnn = 0;
	if (end > sCount)
		end = sCount;
	if (start < 0 || end<0 || start >= end || start>sCount)
		printf("error: %d %d\n", start, end);
	
	for (k = start; k < end; k++){

		/*_fseeki64(example_2, seekDegree, SEEK_SET);
		_fseeki64(example_2, (long long int)seekDegree_2*k + sizeof(int) * 5, SEEK_CUR);
		fread(fss, sizeof(int), eCount, example_2);*/
		
		for (i = 0,j=0; i < (m+l); j++){
			if (exCanUse[j] == true){
				if (zigma[j] == 0)
					cout << "1";
				(fss + i)->fValue = soldier[k]->comput(ex + j)/zigma[j];
				//cout << "f " << soldier[k]->comput(ex + j) <<" z "<<zigma[j]<< " fVaule : " << (fss + i)->fValue << endl;
				(fss + i)->eNum = j;
				i++;
			}

	}
		qsort(fss, m + l, sizeof(Fv), compare);
		for (i = 0; i < m + l; i++){
			check[i] = 0;
			//cout << "fVaule : " << (fss + i)->fValue << endl;
			
		}
		for (Tpp = 0, Tnn = 0, i = 0; i < m+l; i++){
			if (!exCanUse[fss[i].eNum])
				continue;
			if (ex[fss[i].eNum].isFace == true)       // compute  T+ and T-
				Tpp += w[fss[i].eNum];
			else
				Tnn += w[fss[i].eNum];
		}
	
		
		if (Tnn == 1)
			cout << "here" << endl;
		for (Sp = 0, Sn = 0, i = 0; i < m + l; i++){

			

			if (exCanUse[fss[i].eNum] == false){
				continue;
			}
		
			if (fss[i].eNum >= eCount || fss[i].eNum<0){
				printf("hello");
			}

			if (ex[fss[i].eNum].isFace == true) // compute  S+ and S-
				Sp += w[fss[i].eNum];
			else
				Sn += w[fss[i].eNum];

			if (Sp + (Tnn - Sn) < Sn + (Tpp - Sp)){// compute e
				e = Sp + (Tnn - Sn);
				eflag = 0;
			}
			else{
				e = Sn + (Tpp - Sp);
				eflag = 1;
			}
			if (e < 0){
				Sn = Sn;
			}
			if (mkey == 0){
				eeMin = e;
				ssTemp = fss[i].eNum;
				eflagTemp = eflag;
				mkey = 1;
			}
			if (e==0){
				eeMin = eeMin;
			}

			if (eeMin > e){
				eeMin = e;
				ssTemp = fss[i].eNum;
				eflagTemp = eflag;
			}
		}
		if (Sp != Tpp || Sn != Tnn){
			//cout << "here" << endl;

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


int xx1, xx2, yy1, yy2, type;
FILE *outdata;
char str[200];
thread ** mt;
int tCount;
double *E, *correct, *ET, *correctT, cor;


void preset(char  *FileName){

//fopen_s(&example, "SortedIntegralImage", "rb");
	fopen_s(&example, FileName, "rb");
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
	//_fseeki64(example, sizeof(int)*eCount, SEEK_CUR);
	//fread(fs,sizeof(int),eCount,example);
	if (i == 0){
		soldier[i]->getData(str);
		printf("%d : %s \n", i, str);
	}
}
	w = (double*)malloc(sizeof(double)*eCount);  //初始化樣本權重
	for (int i = 0; i < eCount; i++){
		w[i] = ex[i].isFace ? (double)1 / (2 * m) : (double)1 / (2 * l);
		//cout << ex[i].isFace << endl;
	}

zigma = (double*)malloc(sizeof(double)*eCount);  // Light Nomoralize 
E = (double*)malloc(sizeof(double)*sCount);
mt = (thread**)malloc(sizeof(thread*) * tn);
correct = (double*)malloc(sizeof(double)*sCount);
eThread = (double*)malloc(sizeof(double) * sCount);
sThread = (int*)malloc(sizeof(int) * sCount);
pThread = (int*)malloc(sizeof(int) * sCount);
exCanUse = (int*)malloc(sizeof(int)*eCount);
for (int i = 0; i < eCount; i++){
	*(exCanUse + i) = 1;
	Lm = 0, Lx = 0;
	for (int j = 0; j < 24; j++)
		for (int k = 0; k < 24; k++){
			Lm += ex[i].data[j][k];
			Lx += ex[i].data[j][k] * ex[i].data[j][k];
		}
	Lx /= 576;
	//cout << "M :" << Lm<<endl;
	Lm /= 576;
	//cout << "m :" << Lm << endl;
	Lm *= Lm;
	Lz = Lm - Lx;
	if (Lz < 0)
		Lz *= -1;
	zigma[i] = sqrt(Lz);
	if (zigma[i] <= 1e-15){
		if (ex[i].isFace == true)
			m--;
		else
			l--;
		*(exCanUse + i) = 0;
	}
	cout << i << "  zig: " << zigma[i]<<" ,x: "<<Lx<<" ,m: "<<Lm<<" ,z: "<<Lz << endl;
}
/*double x = 1, y = 0.0000000000000000000000001;
int ace=0;
for (int i = 0 ; i < 1000000000; i++){
	for (int j = 0; j < 1000000000;j++){
		if (j % 28 == 0){
			cout << ace << " % ,x : " <<x<< endl;
			ace++;
		}
		for (int k = 0; k < 1000000000; k++){
			x += y;
		}
	}
}
cout << "jjef" << endl;*/
//strong = (Soldier**)malloc(sizeof(Soldier*)*tCount);  //選拔完成的分類器儲存區


}
void freeall(){
	fclose(example);
	free(ex);
	for (int i = 0; i < sCount; i++){
		delete(soldier[i]);
	}
	free(w);
	free(E);
	free(zigma);
	free(correct);
	free(eThread);
	free(sThread);
	free(pThread);
	free(exCanUse);
}
int training(int tC, Soldier **strong){
	int t0, t1,aCount;
	
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

		printf("Weight sum: %lf\n", wSum);

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
		cout << "Tp : " << Tp << " , Tn : " << Tn << endl;
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

		
		strong[tC - 1]->x1 = soldier[iMin]->x1;
		strong[tC - 1]->x2 = soldier[iMin]->x2;
		strong[tC - 1]->y1 = soldier[iMin]->y1;
		strong[tC - 1]->y2 = soldier[iMin]->y2;
		strong[tC - 1]->th = soldier[iMin]->th;
		strong[tC - 1]->e = soldier[iMin]->e;
		strong[tC - 1]->p = soldier[iMin]->p;
		strong[tC - 1]->type = soldier[iMin]->type;
		for (i = 0; i < tC - 1; i++){
			if (strong[i]->x1 == strong[tC - 1]->x1&&strong[i]->x2 == strong[tC - 1]->x2&&strong[i]->y1 == strong[tC - 1]->y1&&strong[i]->y2 == strong[tC - 1]->y2)
				cout << "here" << endl;
		}
		double wtmp = 0;
		int jugT, jugF;
		for (i = 0, jugT = 0, jugF = 0,aCount=0; i < eCount; i++){        //設定樣本權重
			if (!exCanUse[i])
				continue;
			aCount++;
			
			if ((strong[tC - 1]->judge(ex + i) - (int)(ex[i].isFace)) == 0){
				wtmp = w[i];
				w[i] = w[i] * eMin / (1 - eMin);
				if (eMin == 0)
					w[i] = 0;
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
		printf("%-4d: %s  ,canUsenum :%d , false isFace: %d , nonface: %d\n", t, str,aCount, jugT, jugF);
		fprintf_s(outdata, "%s %lf\n", str, cor);
	}



	fclose(example);
	fclose(outdata);
	return 0;
}