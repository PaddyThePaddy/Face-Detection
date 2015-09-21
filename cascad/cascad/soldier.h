using namespace std;
class Soldier{
	private:
		
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
		int x1, x2, y1, y2;
		int type;
		double th;
		int p;
		double sigma;
		double e;
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
		void setSigma(double in){
			sigma = in;
		};
		
		void setTh(double in){
			th = in;
		};
		void setE(double in){
			e = in;
		};
		void setth(double t){
			th= t;
		};
		double getE(){
			return e;
		};
		double getth(){
			return th;
		};

		int judge(IntImg *intImg){
			double sum= (double)comput(intImg)/this->sigma;
			//cout << "sum : "<<sum<<" , sigma : "<<this->sigma<<" , th : "<<th<<" , p : "<< p<<endl;
			if(p==1)
				return th>=sum;
			else
				return th<=sum;
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
			sprintf_s(str,200,"%02d %02d %02d %02d %d %2d %E %E",x1,y1,x2,y2,type,p,th,e);
			//sprintf_s(str, 200, "%02d %02d %02d %02d %d %2d", x1, y1, x2, y2, type,p);
		};
};
