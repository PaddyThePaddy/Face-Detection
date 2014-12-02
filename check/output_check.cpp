#include <cstdlib>
#include <iostream>
#define TH 0.05

using namespace std;

class IntImg{
	public:
		bool isFace;
		int data[24][24];
};

class Soldier{
	private:
		int x1,x2,y1,y2;
		int type;
		int th;
		int p;
		
		int getAreaSum(IntImg *intImg,int x1,int y1,int x2,int y2){
			int sum=0;
			sum+=intImg->data[y2-1][x2-1];
			if(y1>0)
				sum-=intImg->data[y1-1][x2-1];
			if(x1>0)
				sum-=intImg->data[y2-1][x1-1];
			if(y1>0 && x1>0)
				sum+=intImg->data[y1-1][x1-1];
			return sum;
		};
	public:
		
		Soldier(int x1,int y1,int x2,int y2,int type,int p,int th){	
			this->x1=x1;
			this->x2=x2;
			this->y1=y1;
			this->y2=y2;
			this->type=type;
			this->p=p;
			this->th=th;
		};
		
		int judge(IntImg *intImg){
			int sum=comput(intImg);
			
			if(p==1)
				return (int)th>=sum;
			else
				return (int)th<=sum;
		};
		
		int comput(IntImg *intImg){
			switch(type){
				case 0:
					return getAreaSum(intImg,x1,y1,(x2+x1)/2,y2)-getAreaSum(intImg,(x2+x1)/2,y1,x2,y2);
					break;
				case 1:
					return getAreaSum(intImg,x1,y1,x2,(y1+y2)/2)-getAreaSum(intImg,x1,(y1+y2)/2,x2,y2);
					break;
				case 2:
					return getAreaSum(intImg,x1,y1,x1+(x2-x1)/3,y2)-getAreaSum(intImg,x1+(x2-x1)/3,y1,x1+(x2-x1)*2/3,y2)+getAreaSum(intImg,x1+(x2-x1)*2/3,y1,x2,y2);
					break;
				case 3:
					return getAreaSum(intImg,x1,y1,x2,y1+(y2-y1)/3)-getAreaSum(intImg,x1,y1+(y2-y1)/3,x2,y1+(y2-y1)*2/3)+getAreaSum(intImg,x1,y1+(y2-y1)*2/3,x2,y2);
					break;
				case 4:
					return getAreaSum(intImg,x1,y1,(x2+x1)/2,(y1+y2)/2)+getAreaSum(intImg,(x2+x1)/2,(y1+y2)/2,x2,y2)-getAreaSum(intImg,(x2+x1)/2,y1,x2,(y1+y2)/2)-getAreaSum(intImg,x1,(y1+y2)/2,(x2+x1)/2,y2);
					break;
			}
		};
		
		void getData(char *str){
			sprintf(str,"%02d %02d %02d %02d %d %2d %8d",x1,y1,x2,y2,type,p,th);
		}
};

int main(int argc,char *argv[]){
	FILE *soldier=fopen("output.txt","r"),*example=fopen("IntegralImage","rb"),*out;
	IntImg *ex;
	Soldier **h;
	int sCount,eCount,m,l,*correct,x1,x2,y1,y2,t,p,th;
	double *correctR,*record;
	char str[2000];
	
	fscanf(soldier,"%d",&sCount);
	fread(&eCount,sizeof(int),1,example);
	fread(&m,sizeof(int),1,example);
	fread(&l,sizeof(int),1,example);
	printf("%d soldiers, %d examples, %d faces,%d nonfaces\n",sCount,eCount,m,l);
	
	ex=(IntImg*)malloc(sizeof(IntImg)*eCount);
	h=(Soldier**)malloc(sizeof(Soldier*)*sCount);
	correct=(int*)malloc(sizeof(int)*sCount);
	correctR=(double*)malloc(sizeof(double)*sCount);
	record=(double*)malloc(sizeof(double)*sCount);
	
	fread(ex,sizeof(IntImg),eCount,example);
	
	for(int i=0;i<sCount;i++){
		fscanf(soldier, "%d%d%d%d%d%d%d", &x1, &y1, &x2, &y2, &t, &p, &th);
		fscanf(soldier,"%lf",&record[i]);
		fscanf(soldier,"%lf",&record[i]);
		h[i]=new Soldier(x1, y1, x2, y2, t, p, th);
		h[i]->getData(str);
		//printf("%d: %s %lf\n",i,str,record[i]);
		//system("pause");
	}
	fclose(soldier);
	fclose(example);
	printf("start checking...\n");
	
	for(int i=0;i<sCount;i++){
		correct[i]=0;
		for(int j=0;j<eCount;j++){
			//printf("hi\n");
			if(h[i]->judge(ex+j)==(ex+j)->isFace)
				correct[i]++;
		}
		correctR[i]=(double)correct[i]/eCount;
		
		if(correctR[i]-record[i]>TH || record[i]-correctR[i]>TH){
			printf("something strange at %d ......\n",i);
		}
		
		//printf("%d\n",correct[i]);
	}
	
	printf("check finish,writing record...\n");
	out=fopen("check_record.txt","w");
	for(int i=0;i<sCount;i++){
		h[i]->getData(str);
		fprintf(out,"%-4d: %s %lf %lf\n",i,str,correctR[i],record[i]);
	}
	
	printf("all finish\n");
	system("pause");
	return 0;
}
