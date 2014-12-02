#include <cstdlib>
#include <iostream>
#include "IntImg.h"
#include "image.h"

using namespace std;

class LinkNode{
	public:
		IntImg *p;
		LinkNode *next;
		
		LinkNode(){
			p=NULL;
			next=NULL;
		}
};

int sumPixel(ImageData *img,int x,int y){
	int sum=0;
	Pixel p;
	
	getPixel(img,x,y,&p);
	sum+=p.r*77;
	sum+=p.g*151;
	sum+=p.b*28;
	
	return sum/256;
}

int main(int argc, char *argv[])
{
	FILE *f,*out;
	char fname[128];
	ImageData *img;
	int num,i,j,sum=0,l,m;
	IntImg *intImg;
	LinkNode *root=new LinkNode(),*cur;
	
	out=fopen("IntegralImage","wb");
	if(out==NULL){
		printf("output dile open fail\n");
		exit(0);
	}
	printf("start read face image...\n");
	
	for(num=0,cur=root;num<5000;num++){                      //�qface��Ƨ�Ū���H�y�Ϯ� 
		sprintf(fname,".\\face\\%05d.bmp",num); //�����ɦW 
		if((f=fopen(fname,"r"))==NULL){ //�ˬd�O�_�s�b 
			break;
		}
		fclose(f);
		
		intImg=new IntImg();
		readBMPfile(fname,&img);  //Ū���Ϥ� 
		if(img->width!=24 && img->height!=24){
			printf("error at %05d face:size!=24\n",num);
			continue;
		}
		
		intImg->isFace=true;
		intImg->data[0][0]=sumPixel(img,0,0); //�Ĥ@���� 
		
		for(j=1;j<24;j++){  //�Ĥ@row����L���� 
			intImg->data[0][j]=intImg->data[0][j-1]+sumPixel(img,j,0);
		}
		
		for(i=1;i<24;i++){
			intImg->data[i][0]=intImg->data[i-1][0]+sumPixel(img,0,i); //�C�@row�Ĥ@col������ 
			for(j=1;j<24;j++){
				intImg->data[i][j]=intImg->data[i-1][j]+intImg->data[i][j-1]-intImg->data[i-1][j-1]+sumPixel(img,j,i);
			}
		}
		cur->p=intImg;
		cur->next=new LinkNode();
		cur=cur->next;
		//fwrite((const void*)&intImg,sizeof(intImg),1,out);  //�g�J���G 
		
	}
	m=num;
	sum+=num;	
	printf("face %05d complete\n",num);
	for(num=0;num<50000;num++){   //�qnonface��Ƨ�Ū���D�H�y�Ϥ� 
		sprintf(fname,".\\nonface\\%05d.bmp",num);
		if((f=fopen(fname,"r"))==NULL){
			break;
		}
		fclose(f);
		
		intImg=new IntImg();
		readBMPfile(fname,&img);  //Ū���Ϥ� 
		if(img->width!=24 && img->height!=24){
			printf("error at %05d face:size!=24\n",num);
			continue;
		}
		
		intImg->isFace=false;
		intImg->data[0][0]=sumPixel(img,0,0); //�Ĥ@���� 
		
		for(j=1;j<24;j++){  //�Ĥ@row����L���� 
			intImg->data[0][j]=intImg->data[0][j-1]+sumPixel(img,j,0);
		}
		
		for(i=1;i<24;i++){
			intImg->data[i][0]=intImg->data[i-1][0]+sumPixel(img,0,i); //�C�@row�Ĥ@col������ 
			for(j=1;j<24;j++){
				intImg->data[i][j]=intImg->data[i-1][j]+intImg->data[i][j-1]-intImg->data[i-1][j-1]+sumPixel(img,j,i);
			}
		}
		cur->p=intImg;
		cur->next=new LinkNode();
		cur=cur->next;
		//fwrite((const void*)&intImg,sizeof(intImg),1,out);
		
	}
	l=num;
	sum+=num;
	printf("nonface %05d complete\n",num);
	
	printf("sum: %d\nwriting file...\n",sum);
	
	fwrite(&sum,sizeof(int),1,out);
	fwrite(&m,sizeof(int),1,out);
	fwrite(&l,sizeof(int),1,out);
	for(cur=root;cur->p!=NULL;cur=cur->next)
		fwrite(cur->p,sizeof(IntImg),1,out);
	printf("complete\n");
	fclose(out);
	system("pause");
    return EXIT_SUCCESS;
}
