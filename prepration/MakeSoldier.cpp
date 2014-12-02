#include <cstdlib>
#include <iostream>
#include "IntImg.h"
#include "soldier.h"

using namespace std;

class JudgeResult{
	public:
		bool isFace;
		int result;
};

int compare(const void *a,const void *b){
	return ((JudgeResult*)a)->result-((JudgeResult*)b)->result;
}

IntImg intImg[20000];
int main(int argc, char *argv[])
{
	FILE *sample=fopen("IntegralImage","rb"),*output=fopen("soldiers.txt","w");
	Soldier *soldier[180000];
	int sCount,x1,x2,y1,y2,sampleN,correctA,correctB,max,maxI,maxT,t;
	JudgeResult judgeResult[20000];
	char str[50];
	
	if(sample==NULL){
		printf("sample file open error\n");
		exit(1);
	}
	
	if(output==NULL){
		printf("output file open error\n");
		exit(1);
	}
	//開始窮舉產生分類器
	/*規則:
		x1<x2 y1<y2
		為了計算方便,實際範圍不包含x2那個col以及y2那個row 
	*/ 
	sCount=0;
	for(x1=0;x1<24;x1++)
		for(y1=0;y1<24;y1++)
			for(x2=x1+1;x2<=24;x2++)
				for(y2=y1+1;y2<=24;y2++){
					if((x2-x1)%2==0)
						soldier[sCount++]=new Soldier(x1,y1,x2,y2,0);
					if((y2-y1)%2==0)
						soldier[sCount++]=new Soldier(x1,y1,x2,y2,1);
					if((x2-x1)%3==0)
						soldier[sCount++]=new Soldier(x1,y1,x2,y2,2);
					if((y2-y1)%3==0)
						soldier[sCount++]=new Soldier(x1,y1,x2,y2,3);
					if((x2-x1)%2==0 && (y2-y1)%2==0)
						soldier[sCount++]=new Soldier(x1,y1,x2,y2,4);
				}
	
	fread(&sampleN,sizeof(int),1,sample);
	fread(&t,sizeof(int),1,sample);
	fread(&t,sizeof(int),1,sample);
	if(fread(&intImg,sizeof(IntImg),sampleN,sample)!=sampleN){ //從樣本檔案讀取樣本
		printf("sec file error\n");
		exit(0);
	} 
	fclose(sample);
	
	printf("Soldier: %d\tSample: %d\n",sCount,sampleN);
	fprintf(output,"%d\n",sCount);
	
	for(int i=0;i<sCount;i++){
		for(int j=0;j<sampleN;j++){ //讓弱分類器判斷每個樣本 
			judgeResult[j].result=soldier[i]->judge(&intImg[j]);
			judgeResult[j].isFace=intImg[j].isFace;
		}
		
		qsort(judgeResult,sampleN,sizeof(JudgeResult),compare); //對結果排序 
		
		/*for(int i=0;i<sampleN;i++)
			printf("%d\t",judgeResult[i].result);
		printf("\n");
		system("pause");*/
		
		correctA=correctB=0;
		
		for(int i=0;i<sampleN;i++)       //先將基準點設置在最小 
			if(judgeResult[i].isFace)
				correctB++;
			else
				correctA++;
				
		if(correctA>correctB){
			max=correctA;
			maxT=1;
			maxI=0;
		}else{
			max=correctB;
			maxT=-1;
			maxI=0;
		}
		
		for(int i=0;i<sampleN-1;i++){   //將基準點後移 
			if(judgeResult[i].isFace){
				correctA++;
				correctB--;
			}else{
				correctA--;
				correctB++;
			}
			
			if(correctA>max){
				max=correctA;
				maxT=1;
				maxI=i+1;
			}
			
			if(correctB>max){
				max=correctB;
				maxT=-1;
				maxI=i+1;
			}
		}
		
		soldier[i]->getData(str);
		fprintf(output,"%s %2d %6d \n",str,maxT,judgeResult[maxI].result);
		if(i%10000==0)
			printf("%d complete\n",i);
	}
	
	fclose(output);
    system("PAUSE");
    return EXIT_SUCCESS;
}
