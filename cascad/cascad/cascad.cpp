#include <iostream>
#include <cstdlib>
#include "training.h"
using namespace std;
int judge(IntImg *ex, Soldier **h, double *alpha, double th, int sCount){
	double hSum = 0;

	for (int i = 0; i<sCount; i++){
		hSum += alpha[i] * h[i]->judge(ex);
	}
	return (int)hSum >= (th);
}

int main(){
	double f; //maximum acceptable fase positive rate  per layer  -- value is selected by user  
	double d; //minimum acceptable detection rate per layer -- value is selected by user
	double F[17000],D[17000],Ftarget,e,*alpha;
	int n[17001], fdc, correctCount, countk, countj, counte, correctflage;
	int i,count,vCount,vm,vl,th,ecuCount;
	IntImg *P, *N,*V;
	Soldier **h;
	char str[256];
	Soldier **strong =nullptr;
	Soldier **cascadedStrong = nullptr;
	FILE *vSet,*out;
	fopen_s(&out, "out.txt", "w");
	fopen_s(&vSet, "vSetIntegralImage", "rb");
	if (!vSet)cout << "vSet error" << endl;
	fread(&vCount, sizeof(int), 1, vSet);//±qÀÉ®×Åª¨ú´ú¸Õ¥Î¼Ë¥»
	fread(&vm, sizeof(int), 1, vSet);
	fread(&vl, sizeof(int), 1, vSet);
	V = (IntImg*)malloc(sizeof(IntImg)*vCount);
	fread(V, sizeof(IntImg), vCount, vSet);
	cout << "face : " << vm << "\tnonface : " << vl << "\tvCount : "<<vCount<<endl;
	F[0] = 1.0;
	D[0] = 1.0;
	i = 0;
	cout << "input f , d , Ftarget : " << endl;
	cin >> f >> d >> Ftarget;
	preset();
	cascadedStrong = (Soldier**)malloc(sizeof(Soldier*)*17000);
	alpha = (double*)malloc(sizeof(double) * 17000);
	ecuCount = 0;
	while (F[i] > Ftarget){
		cout << "-----------------  stage  " << i <<"  -----------------"<<endl;
		fprintf_s(out, "%d\n", i);
		i++;
		n[i] = 0; // so, n is start by 1;   the number of features , ni , of each stage;
		F[i] = F[i - 1];
		strong = (Soldier**)malloc(sizeof(Soldier*) * 17000);
		l-=ecuCount;
		for (count = 0; count<eCount; count++){
			if (exCanUse[count] == false){
				continue;
			}
			w[count] = ex[count].isFace ? (double)1 / (2 * m) : (double)1 / (2 * l);
		}
		while (F[i] > f*F[i - 1]){
		//while (n[i]<2){
			n[i]++;
			cout << "face : " << m << " nonface : " << l << "ecuCount : " << ecuCount<<endl; 
			training(n[i], strong);
			strong[n[i]-1]->getData(str);
			//cout << str << endl;
			e = strong[n[i] - 1]->getE();
			alpha[n[i]-1] = log((1 - e) / e);
			for (th = 0, count = 0; count < n[i]; count++)
				th += alpha[count] / 2;
			do{
				for (count = 0, fdc = 0, correctCount = 0; count < vCount; count++){
					if (judge(V + count, strong, alpha, th, n[i]) - (int)V[count].isFace == 0)
						correctCount++;
					else if (!V[count].isFace)
						fdc++;
				}
				D[i] = (double)correctCount / vCount;
				F[i] = (double)fdc / vCount;
				th -= 0.000001;
			} while (D[i] >= d*D[i - 1]);
			cout << "F[i] > f * F[i-1] : " << F[i] << " > " << f << " * " << F[i - 1] << " , " << f*F[i - 1] << endl;
		}

		cascadedStrong[i - 1] = (Soldier*)malloc(sizeof(Soldier)*n[i]);
		for (count = 0; count < n[i]; count++){//storage strong into cascaded strong
			cascadedStrong[i - 1][count] = *strong[count];
			cascadedStrong[i - 1][count].getData(str);
			cout << str << endl;
			fprintf_s(out, "%s\n", str);
		}
		cout << "F : " << F[i] << " D : " << D[i] << endl;
		free(strong);
		if (F[i]>Ftarget){
			for ( counte = 0 , ecuCount = 0 ; counte < eCount; counte++){
				if (ex[counte].isFace)
					continue;
				
				for (count = 0; count < i; count++){
					for (countj = 0; countj < n[count + 1]; countj++){
						e = cascadedStrong[count][countj].getE();
						alpha[countj] = log((1 - e) / e);
					}
					for (countk = 0, th = 0; countk < n[count + 1]; countk++)
						th += alpha[countk] / 2;
					correctflage = 0;
					if (judge(ex + counte, &cascadedStrong[count], alpha, th, n[count + 1]) - (int)ex[counte].isFace == 0)
						correctflage = 1;
					else{
						correctflage = 0;
						break;
					}
				}
				if (correctflage){
					exCanUse[counte] = 0;
					ecuCount++;
				}
			}
		}
	}
	system("CLS");
	cout << "stage num : " << i << endl<<endl;
	for (count = 0; count < i; count++){
		cout << "----------  stage  " << count << "  ----------" << endl;
		for (countj = 0; countj < n[count + 1]; countj++){
			cascadedStrong[count][countj].getData(str);
			cout << countj << " : " << str << endl;
		}
		
	}
	fclose(out);
	system("pause");
}
