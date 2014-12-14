#include <cstdlib>
#include <iostream>
#include <cmath>


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
			sprintf_s(str,sizeof(str),"%02d %02d %02d %02d %d %2d %8d",x1,y1,x2,y2,type,p,th);
		}
};

int judge(IntImg *ex,Soldier **h,double *alpha,double th,double ctrl,int sCount){
	double hSum=0;
	
	for(int i=0;i<sCount;i++){
		hSum+=alpha[i]*h[i]->judge(ex);
	}
	return (int)hSum>=(th+ctrl);
}

int main(int argc, char *argv[])
{
	FILE *example, *soldier;
	Soldier **h;
	IntImg *ex;
	int sCount,eCount,m,l,correct,t;
	double *alpha, th, scale, ctrl;
	
	fopen_s(&example, "IntegralImage", "rb");
	fopen_s(&soldier, "output.txt", "r");
	if(example==NULL || soldier==NULL){
		printf("file open error\n");
		system("pause");
		exit(0);
	}
	
	fscanf_s(soldier,"%d",&sCount);
	fread(&eCount,sizeof(int),1,example);
	fread(&m,sizeof(int),1,example);
	fread(&l,sizeof(int),1,example);
	h=(Soldier**)malloc(sizeof(Soldier*)*sCount);
	ex=(IntImg*)malloc(sizeof(IntImg)*eCount);
	alpha=(double*)malloc(sizeof(double)*sCount);
	fread(ex,sizeof(IntImg),eCount,example);
	
	for(int i=0;i<sCount;i++){
		int x1,x2,y1,y2,t,p,th;
		double e,correctR;
		char str[200];
		
		fscanf_s(soldier,"%d%d%d%d%d%d%d%E%lf",&x1,&y1,&x2,&y2,&t,&p,&th,&e,&correctR);
		h[i]=new Soldier(x1,y1,x2,y2,t,p,th);
		alpha[i]=log((1-e)/e);
	}
	
	th=0;
	for(int i=0;i<sCount;i++)
		th+=alpha[i]/2;
	
	scanf_s("%d%lf",&t,&scale);
	
	ctrl=-th*scale*t/2;
	
	for(int c=0;c<t;c++){
		correct=m=l=0;
		for(int i=0;i<eCount;i++){
			if(judge(ex+i,h,alpha,th,ctrl,sCount)-(int)ex[i].isFace==0)
				correct++;
			else
				if(ex[i].isFace)
					m++;
				else
					l++;
		}
		
		ctrl+=th*scale;
	
		printf("Correct rate: %lf\nerror at face: %d, nonface: %d\n",(double)correct/eCount,m,l);
	
	}
    system("PAUSE");
    return EXIT_SUCCESS;
}
