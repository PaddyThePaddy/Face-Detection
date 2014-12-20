#include <iostream>
#include <cstdlib>
#include "training.h"
using namespace std;

int main(){
	double f; //maximum acceptable fase positive rate  per layer  -- value is selected by user  
	double d; //minimum acceptable detection rate per layer -- value is selected by user
	double F[17000],D[17000],Ftarget;
	int n[17001];
	int i;
	IntImg *P, *N;
	F[0] = 1.0;
	D[0] = 1.0;
	i = 0;
    fopen_s(&example, "SortedIntegralImage", "rb");

	fread(&eCount, sizeof(int), 1, example);//從檔案讀取樣本
	fread(&m, sizeof(int), 1, example);
	fread(&l, sizeof(int), 1, example);
	P = (IntImg*)malloc(sizeof(IntImg)*m);
	N = (IntImg*)malloc(sizeof(IntImg)*l);
	ex = (IntImg*)malloc(sizeof(IntImg)*eCount);
	fread(ex, sizeof(IntImg), eCount, example);
	cout << "maximum acceptable fase positive rate : ";
	cin >> f;
	cout << "\nminimum acceptable detection rate : ";
	cin >> d;
	cout << "\nfase positive rate : ";
	cin >> Ftarget;
	

	
	while (F[i] > Ftarget){
		i++;
		n[i] = 0; // so, n is start by 1;   the number of features , ni , of each stage;
		F[i] = F[i - 1];
		while (F[i] > f*F[i - 1]){
			n[i]++;
			training(n[i]);
		}

	}
	system("pause");
}