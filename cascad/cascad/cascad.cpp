#include <iostream>
#include <cstdlib>
#include "training.h"
using namespace std;
int judge(IntImg *ex, Soldier **h, double *alpha, double th, double ctrl, int sCount){
	double hSum = 0;

	for (int i = 0; i<sCount; i++){
		hSum += alpha[i] * h[i]->judge(ex);
	}
	return (int)hSum >= (th + ctrl);
}
int main(){
	double f; //maximum acceptable fase positive rate  per layer  -- value is selected by user  
	double d; //minimum acceptable detection rate per layer -- value is selected by user
	double F[17000],D[17000],Ftarget;
	int n[17001];
	int i;
	IntImg *P, *N;
	Soldier **h;
	char str[256];
	Soldier **strong =nullptr;
	F[0] = 1.0;
	D[0] = 1.0;
	i = 0;
    
	cin >> f >> d >> Ftarget;
	
	while (F[i] > Ftarget){
		i++;
		n[i] = 0; // so, n is start by 1;   the number of features , ni , of each stage;
		F[i] = F[i - 1];
		while (F[i] > f*F[i - 1]){
			n[i]++;
			strong = (Soldier**)malloc(sizeof(Soldier*)*n[i]);
			training(n[i], strong);
			strong[0]->getData(str);
			cout << str << endl;
		}

	}
	system("pause");
}
