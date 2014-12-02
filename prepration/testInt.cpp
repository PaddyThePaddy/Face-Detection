#include <cstdlib>
#include <iostream>

class IntImg{
	public:
		bool isFace;
		int data[24][24];
};


using namespace std;

int main(int argc, char *argv[])
{
	FILE *f=fopen("IntegralImage","rb");
	IntImg intImg;
	int m,l,sum;
	
	fread(&sum,sizeof(int),1,f);
	fread(&m,sizeof(int),1,f);
	fread(&l,sizeof(int),1,f);
	printf("get %d face, %d nonface, total %d img\n",m,l,sum);
	while(1){
		system("pause");
		if(fread(&intImg,sizeof(intImg),1,f)!=1)
			break;
		
		printf("is face:%d\n",intImg.isFace);
		
		for(int i=0;i<24;i++){
			for(int j=0;j<24;j++){
				printf("%d\t",intImg.data[i][j]);
			}
			printf("\n");
		}
	}
	printf("read complete\n");
	system("pause");
    return EXIT_SUCCESS;
}
