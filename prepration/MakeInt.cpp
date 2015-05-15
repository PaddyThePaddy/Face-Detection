#include <cstdlib>
#include <iostream>
#include "IntImg.h"
#include "image.h"

using namespace std;

#include <cstdlib>
#include <iostream>

using namespace std;

class Soldier{
	private:
		int x1,x2,y1,y2;
		int type;
		int th;
		int p;
		double e;
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
		void setP(int in){
			p = in;
		};
		void setTh(int in){
			th = in;
		};
		void setE(double in){
			e = in;
		};
		void setth(int t){
			th= t;
		};

		int getth(){
			return th;
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
		
	/*	void getData(char *str){
			sprintf(str,"%02d %02d %02d %02d %d %2d %8d %.8lf",x1,y1,x2,y2,type,p,th,e);
		}*/
		void getData(){
			printf("%02d %02d %02d %02d %d\n",x1,y1,x2,y2,type);
		}
		void writeData(FILE *f){
			fwrite(&x1,sizeof(int),1,f);
			fwrite(&y1,sizeof(int),1,f);
			fwrite(&x2,sizeof(int),1,f);
			fwrite(&y2,sizeof(int),1,f);
			fwrite(&type,sizeof(int),1,f);
		}
};

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
	int num,i,j,sum=0,l,m,sCount;
	IntImg *intImg;
	LinkNode *root=new LinkNode(),*cur;
	Soldier *soldier[200000];
	
	out=fopen("IntegralImage","wb");
	if(out==NULL){
		printf("output dile open fail\n");
		exit(0);
	}

	printf("start read face image...\n");
	
	for(num=0,cur=root;num<5000;num++){                      //從face資料夾讀取人臉圖案 
		sprintf(fname,".\\face\\%05d.bmp",num); //產生檔名 
		if((f=fopen(fname,"r"))==NULL){ //檢查是否存在 
			break;
		}
		fclose(f);
		
		intImg=new IntImg();
		readBMPfile(fname,&img);  //讀取圖片 
		if(img->width!=24 && img->height!=24){
			printf("error at %05d face:size!=24\n",num);
			continue;
		}
		
		intImg->isFace=true;
		intImg->data[0][0]=sumPixel(img,0,0); //第一像素 
		
		for(j=1;j<24;j++){  //第一row的其他像素 
			intImg->data[0][j]=intImg->data[0][j-1]+sumPixel(img,j,0);
		}
		
		for(i=1;i<24;i++){
			intImg->data[i][0]=intImg->data[i-1][0]+sumPixel(img,0,i); //每一row第一col的像素 
			for(j=1;j<24;j++){
				intImg->data[i][j]=intImg->data[i-1][j]+intImg->data[i][j-1]-intImg->data[i-1][j-1]+sumPixel(img,j,i);
			}
		}
		cur->p=intImg;
		cur->next=new LinkNode();
		cur=cur->next;
		//fwrite((const void*)&intImg,sizeof(intImg),1,out);  //寫入結果 
		
	}
	m=num;
	sum+=num;	
	printf("face %05d complete\n",num);
	for(num=0;num<500000;num++){   //從nonface資料夾讀取非人臉圖片 
		sprintf(fname,".\\nonface\\not_face_No_%06d.bmp",num);
		if((f=fopen(fname,"r"))==NULL){
			break;
		}
		fclose(f);
		
		intImg=new IntImg();
		readBMPfile(fname,&img);  //讀取圖片 
		if(img->width!=24 && img->height!=24){
			printf("error at %05d face:size!=24\n",num);
			continue;
		}
		
		intImg->isFace=false;
		intImg->data[0][0]=sumPixel(img,0,0); //第一像素 
		
		for(j=1;j<24;j++){  //第一row的其他像素 
			intImg->data[0][j]=intImg->data[0][j-1]+sumPixel(img,j,0);
		}
		
		for(i=1;i<24;i++){
			intImg->data[i][0]=intImg->data[i-1][0]+sumPixel(img,0,i); //每一row第一col的像素 
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

	sCount = 0;
	for (int x1 = 0; x1 < 24; x1++)
		for (int y1 = 0; y1 < 24; y1++)
			for (int x2 = x1 + 1; x2 <= 24; x2++)
				for (int y2 = y1 + 1; y2 <= 24; y2++){
					if ((x2 - x1) % 2 == 0)
						soldier[sCount++] = new Soldier(x1, y1, x2, y2, 0, 0, 0);
					if ((y2 - y1) % 2 == 0)
						soldier[sCount++] = new Soldier(x1, y1, x2, y2, 1, 0, 0);
					if ((x2 - x1) % 3 == 0)
						soldier[sCount++] = new Soldier(x1, y1, x2, y2, 2, 0, 0);
					if ((y2 - y1) % 3 == 0)
						soldier[sCount++] = new Soldier(x1, y1, x2, y2, 3, 0, 0);
					if ((x2 - x1) % 2 == 0 && (y2 - y1) % 2 == 0)
						soldier[sCount++] = new Soldier(x1, y1, x2, y2, 4, 0, 0);
				}
	printf("create %d soldier\n",sCount);
    fwrite(&sCount,sizeof(int),1,out);
    
    for(int i=0;i<sCount;i++){
            soldier[i]->writeData(out);
            soldier[i]->getData();
            
            
    }

	printf("complete\n");
	fclose(out);
	system("pause");
    return EXIT_SUCCESS;
}
