#include <iostream>
#include <cstdlib>
#include "training.h"
#include <String>
using namespace std;
int judge(IntImg *ex, Soldier **h, double *alpha, double th, int sCount){
	double hSum = 0;

	for (int i = 0; i<sCount; i++){
		hSum += alpha[i] * h[i]->judge(ex);
	}
	
	return (int)hSum >= (th);
}
int fileNo=0;


void getNextFileName(){
	string filename="IntegralImage_";
	char s[10];
	int i;
	_itoa_s(fileNo,s,10);
	filename += s;
	fileNo++;
	for ( i = 0; i < filename.length(); i++)
		curFileName[i] = filename[i];
	/*---------debug use---------
	string filename_debug = "IntegralImage_test";
	for (i = 0; i < filename_debug.length(); i++)
		curFileName[i] = filename_debug[i];
	//-------------------------------*/
}
int main(){
	double f; //maximum acceptable fase positive rate  per layer  -- value is selected by user  
	double d; //minimum acceptable detection rate per layer -- value is selected by user
	double F[17000], D[17000], Ftarget, e, *alpha, th, thvalue;
	int n[17001], fdc, correctCount, countk, countj, counte, correctflage,thCount;
	int i,count,vCount,vm,vl,ecuCount;
	int conf_non_auto_num,conf_sCount[100],non_auto_key,conf_counter; //config file: non auto num , how many solider at current layer and the minima acceptable detection rate at current layer
	double cascade_alpha[17000],cascade_th[17000];
	double conf_dRate[100];
	IntImg *P, *N,*V;
	Soldier **h;
	char str[256];
	Soldier **strong =nullptr;
	Soldier **cascadedStrong = nullptr;
	FILE *vSet,*out,*conf; // conf : configration file
	
	fopen_s(&vSet, "vSetIntegralImage", "rb");
	fopen_s(&conf,"config.txt","r");
	fscanf_s(conf,"%d",&conf_non_auto_num);
	cout << "conf non auto num:" << conf_non_auto_num << endl;
	for (i = 0; i < conf_non_auto_num; i++){
		fscanf_s(conf, "%d", &conf_sCount[i]);
		fscanf_s(conf, "%lf", &conf_dRate[i]);
		cout << i + 1 << " , conf_sCount: " << conf_sCount[i] << " , conf_dRate: " << conf_dRate[i] << endl;
	}
	fclose(conf);
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
	getNextFileName();
	preset(curFileName);
	
	cascadedStrong = (Soldier**)malloc(sizeof(Soldier*)*17000);
	alpha = (double*)malloc(sizeof(double) * 17000);
	ecuCount = 0;// example can use, means how many nonfase sample were been delete at the last stage
	//while (F[i] > Ftarget){
	while (i<38){
		cout << "-----------------  stage  " << i <<"  -----------------"<<endl;
		if (i == 2)
			return 0;
//		fprintf_s(out, "%d\n", i);
		i++;
		n[i] = 0; // so, n is start by 1;   the number of features , ni , of each stage;
		//F[i] = F[i - 1];
		F[i] = 1;
		strong = (Soldier**)malloc(sizeof(Soldier*) * 17000);
		l-=ecuCount;
		for (count = 0; count<eCount; count++){
			if (exCanUse[count] == false){
				continue;
			}
			w[count] = ex[count].isFace ? (double)1 / (2 * m) : (double)1 / (2 * l);
		}
		th = 0;
		
		//while (F[i] > f*F[i - 1]){
		while (F[i]>f||non_auto_key==1){
		//while (n[i]<2){
			non_auto_key = 0;
			if (conf_non_auto_num != 0){
				conf_non_auto_num--;
				non_auto_key = 1;
			}
			
			if (non_auto_key)
				n[i] = conf_sCount[i-1];
			else
				n[i]++;
			
			cout << n[i] << ". face : " << m << " nonface : " << l << "ecuCount : " << ecuCount << endl;

			if (non_auto_key){
				for (conf_counter = 1; conf_counter <= conf_sCount[i - 1]; conf_counter++){
					strong[conf_counter - 1] = (Soldier*)malloc(sizeof(Soldier));
					training(conf_counter, strong);
				}
			}
			else{
				strong[n[i] - 1] = (Soldier*)malloc(sizeof(Soldier));
				training(n[i], strong);
			}

			if (non_auto_key){
				for (conf_counter = 0; conf_counter < conf_sCount[i - 1]; conf_counter++){
					e = strong[conf_counter ]->getE();
					alpha[conf_counter] = log((1 - e) / e);
					th += alpha[conf_counter] / 2;
				}
			}
			else{
				//strong[n[i]-1]->getData(str);
				//cout << str << endl;
				e = strong[n[i] - 1]->getE();
				alpha[n[i] - 1] = log((1 - e) / e);
				for (th = 0, count = 0; count < n[i]; count++)
					th += alpha[count] / 2;
				//th += alpha[n[i] - 1] / 2;
			}
			thvalue = th / 100000;
			thCount = 0;
			while (1){
				for (count = 0, fdc = 0, correctCount = 0; count < vCount; count++){

					if (judge(V + count, strong, alpha, th, n[i]) == 1 && V[count].isFace)
						correctCount++;
					if (!V[count].isFace && (judge(V + count, strong, alpha, th, n[i]) == 1))
						fdc++;
				}
				D[i] = (double)correctCount / vm;
				F[i] = (double)fdc / vl;
				if (non_auto_key){
					if (D[i] < conf_dRate[i - 1]){
						if (th <= 0)break;
						th -= thvalue;
						thCount++;
						if (thCount >= 99999)break;
						
					}
					else
						break;
				}
				else{
					//if (D[i] < d*D[i - 1]){
					if (D[i]<0.9975){
						if (th <= 0)break;
						th -= thvalue;
						thCount++;
						if (thCount >= 99999)break;
					}
					else
						break;
				}
			}
			cout << "F[i] > f * F[i-1] : " << F[i] << " > " << f << " * " << F[i - 1] << " , " << f*F[i - 1] << endl;
			cout << "F[i] : " << F[i] << " , D[i] : " << D[i] <<" , th : "<<th<< endl;
			if (non_auto_key)
				break;

		}
		freeall();
		getNextFileName();
		preset(curFileName);
		cascade_th[i - 1] = th;
		cascadedStrong[i - 1] = (Soldier*)malloc(sizeof(Soldier)*n[i]);
		for (count = 0; count < n[i]; count++)//storage strong into cascaded strong
			cascadedStrong[i - 1][count] = *strong[count];
		fopen_s(&out, "out.txt", "w");
		for (int ccount = 1; ccount <=i; ccount++){
			fprintf_s(out, "---------- %d ----------\n%d\n", ccount,n[ccount]);
			for (count = 0; count < n[ccount]; count++){
				cascadedStrong[ccount - 1][count].getData(str);
				cout << str << endl;
				fprintf_s(out, "%s\n", str);
			}
			fprintf_s(out, "F : %lf , D : %lf , th : %lf\n", F[ccount], D[ccount], cascade_th[ccount - 1]);
		}
		
		fclose(out);
		cout << "F : " << F[i] << " D : " << D[i] <<" th : "<<cascade_th[i-1]<< endl;
		
		if (F[i]>Ftarget){
			for ( counte = 0 , ecuCount = 0 ; counte < eCount; counte++){
				if (!exCanUse[counte])
					continue;
				if (ex[counte].isFace)
					continue;
				
				
					/*for (countj = 0; countj < n[i]; countj++){
						e = cascadedStrong[i-1][countj].getE();
						alpha[countj] = log((1 - e) / e);
					}
					for (countk = 0, th = 0; countk < n[i]; countk++)
						th += alpha[countk] / 2;*/
					correctflage = 0;
					if (judge(ex + counte, strong, alpha, cascade_th[i-1], n[i]) - (int)ex[counte].isFace == 0)
						correctflage = 1;
					else
						correctflage = 0;
				
				if (correctflage){
					exCanUse[counte] = 0;
					ecuCount++;
				}
			}
		}
		free(strong);
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
	
	system("pause");
}
