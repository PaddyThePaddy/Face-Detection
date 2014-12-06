#include <cstdlib>
#include <iostream>

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
		
		void getData(char *str){
			sprintf(str,"%02d %02d %02d %02d %d %2d %8d %.8lf",x1,y1,x2,y2,type,p,th,e);
		}
		
		void writeData(FILE *f){
			fwrite(&x1,sizeof(int),1,f);
			fwrite(&y1,sizeof(int),1,f);
			fwrite(&x2,sizeof(int),1,f);
			fwrite(&y2,sizeof(int),1,f);
			fwrite(&type,sizeof(int),1,f);
		}
};

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}


int main(int argc, char *argv[])
{
	FILE *in=fopen("IntegralImage","rb"),*out=fopen("SortedIntegralImage","wb");
	int eCount,m,l,sCount,*tmp,t;
	IntImg *ex;
	Soldier *soldier[200000];
	
	fread(&eCount,sizeof(int),1,in);
	fread(&m,sizeof(int),1,in);
	fread(&l,sizeof(int),1,in);
	fwrite(&eCount,sizeof(int),1,out);
	fwrite(&m,sizeof(int),1,out);
	fwrite(&l,sizeof(int),1,out);
	
	printf("get %d example,%d face %d nonface\n",eCount,m,l);
	
	ex=(IntImg*)malloc(sizeof(IntImg)*eCount);
	fread(ex,sizeof(IntImg),eCount,in);
	fwrite(ex,sizeof(IntImg),eCount,out);
	fclose(in);
	
	tmp=(int*)malloc(sizeof(int)*eCount*2);
	
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
	system("pause");
	t=time(NULL);
	int *check=new int[eCount];
	for(int i=0;i<sCount;i++){
		for(int j=0;j<eCount;j++){
			*(tmp+j*2+1)=j;
			*(tmp+j*2)=soldier[i]->comput(ex+j);
		}
		
		qsort(tmp,eCount,sizeof(int)*2,compare);
		
		soldier[i]->writeData(out);
		
		for(int j=0;j<eCount;j++){
			fwrite(tmp+j*2+1,sizeof(int),1,out);
		//	cout<<(tmp+j*2+1)<<endl;
        }
		printf("sort %d complete\n",i);

		for (int j = 0; j < eCount; j++)
			check[j] = 0;
		for (int j = 0; j < eCount; j++)
			check[*(tmp+j*2+1)]++;
		for (int j = 0; j < eCount; j++)
			if (check[j] != 1){
 				printf("help");
 				system("pause");
			}
	}
	fclose(out);
	printf("all complete\nuse %d sec\n",time(NULL)-t);
	
    system("PAUSE");
    return EXIT_SUCCESS;
}
