#include <cstdlib>
#include <iostream>
#include <ctime>
#include <thread>
#include "IntImg.h"
#include "soldier.h"
#define T 10

using namespace std;

class LinkNode{
	public:
		IntImg* data;
		LinkNode *next;
		
		LinkNode(){
			next=NULL;
			data=NULL;
		}
};

int count,sCount,eCount;
LinkNode *ex;
Soldier **h,**strong;
double *w,*E;

void mthread(){
	LinkNode *cur;
	int i,j;
	while(count!=sCount){
		i=count;
		if(h[i]==NULL){
			continue;
		}
		for(j=0,cur=ex;j<eCount;j++){
			E[i]+=w[j]*abs(h[i]->judge(cur->data)-(int)(cur->data->isFace));
			cur=cur->next;
		}
		
	}
}

int main(int argc, char *argv[])
{
    
    FILE *example,*soldier,*output;
    IntImg intImg;
    int x1,x2,y1,y2,p,th,t,i,j,m=0,l=0,minI,v,t0,t1;
    double wSum,eSum,min;
    LinkNode *cur;
    thread **mt;    
    char str[200];
    
    soldier=fopen("soldiers.txt","r");
    example=fopen("IntegralImage","rb");
    
    fscanf(soldier,"%d",&sCount);
    h=(Soldier**)malloc(sizeof(Soldier*)*sCount);
    printf("%d\n",sCount);
    
    i=0;
    while(fscanf(soldier,"%d%d%d%d%d%d%d",&x1,&y1,&x2,&y2,&t,&p,&th)!=EOF)
    	h[i++]=new Soldier(x1,y1,x2,y2,t,p,th);
    fclose(soldier);    
    
    fread(&intImg,sizeof(IntImg),1,example);
    if(intImg.isFace)
    	m++;
	else
    	l++;
    ex=new LinkNode();
    ex->data=new IntImg();
    *(ex->data)=intImg;
    cur=ex;
    eCount=1;
    while(fread(&intImg,sizeof(IntImg),1,example)==1){
	    if(intImg.isFace)
	    	m++;
		else
	    	l++;
		cur->next=new LinkNode();
		cur=cur->next;
		cur->data=new IntImg();
		cur->data->isFace=intImg.isFace;
		for(i=0;i<24;i++)
			for(j=0;j<24;j++)
				cur->data->data[i][j]=intImg.data[i][j];
		
		eCount++;
	}
	fclose(example);
	w=(double*)malloc(sizeof(double)*eCount);
	printf("%d\n",eCount);
    
    for(i=0,cur=ex;i<eCount;i++)
    	w[i]=ex->data->isFace?1/(2*m):1/(2*l);
    
    strong=(Soldier**)malloc(sizeof(Soldier*)*T);
    E=(double*)malloc(sizeof(double)*sCount);
    mt=(thread*)malloc(sizeof(thread*)*8);
    t1=t0=time(NULL);
	for(t=0;t<T;t++){
		wSum=0;
		
		for(i=0;i<eCount;i++)
			wSum+=w[i];
		for(i=0;i<eCount;i++)
			w[i]=w[i]/wSum;
		
		for(i=0;i<8;i++)
			mt[i]=new thread(mthread);
		for(i=0;i<8;i++){
			mt[i]->join();
			delete mt[i];
		}
		/*for(i=0;i<sCount;i++){
			
			if(h[i]==NULL){
				continue;
			}
			
			for(j=0,cur=ex;j<eCount;j++){
				
				E[i]+=w[j]*abs(h[i]->judge(cur->data)-(int)(cur->data->isFace));
				cur=cur->next;
			}
		}*/
				
		for(i=0;;i++)
			if(h[i]!=NULL){
				min=E[i];
				minI=i;
				break;
			}
		
		for(;i<sCount;i++){
			if(h[i]==NULL){
				continue;
			}
			
			if(E[i]<min){
				min=E[i];
				minI=i;
			}
		}
		
		strong[t]=h[minI];
		h[minI]=NULL;
		
		for(i=0,cur=ex;i<eCount;i++){
			if((strong[t]->judge(cur->data)-(int)(cur->data->isFace))!=0)
				w[i]=w[i]*min/(1-min);
		}
		strong[t]->getData(str);
		printf("%d: %s\ntime: %d\n",t,str,time(NULL)-t1);
		t1=time(NULL);
	}
	
	output=fopen("output.txt","w");
	
	fprintf(output,"%d\n",T);
	
	for(i=0;i<T;i++){
		strong[i]->getData(str);
		fprintf(output,"%s\n",str);
	}
	fclose(output);
    printf("used time: %d\n",time(0)-t0);
    system("pause");
    return EXIT_SUCCESS;
}
