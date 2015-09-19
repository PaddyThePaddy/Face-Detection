class Soldier{
	private:
		
		long long int getAreaSum(IntImg *intImg,int x1,int y1,int x2,int y2){
			long long int sum=0;
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
		int x1, x2, y1, y2, ori_x1, ori_x2, ori_y1, ori_y2, tori_x1, tori_x2, tori_y1, tori_y2;			// ori: original
		int type;
		long double th, ori_th;
		int p;
		double e;
		double sigma;
		long double sum;
		Soldier(){};
		Soldier(int x1,int y1,int x2,int y2,int type,int p,long double th,double e){	
			this->x1=x1;
			this->x2=x2;
			this->y1=y1;
			this->y2=y2;
			this->ori_x1 = x1;
			this->ori_x2 = x2;
			this->ori_y1 = y1;
			this->ori_y2 = y2;
			this->tori_x1 = x1;
			this->tori_x2 = x2;
			this->tori_y1 = y1;
			this->tori_y2 = y2;
			this->type=type;
			this->p=p;
			this->th=th;
			this->ori_th = th;
			this->e=e;
		};
		// sw: subWindow
		void setPosition(int swx1, int swy1, double sigma){
			x1 = tori_x1 + swx1;
			x2 = tori_x2 + swx1;
			y1 = tori_y1 + swy1;
			y2 = tori_y2 + swy1;
			this->sigma = sigma;
		};
		void setScale(double scale){
			tori_x1 = ori_x1 * scale;
			tori_x2 = ori_x2 * scale;
			tori_y1 = ori_y1 * scale;
			tori_y2 = ori_y2 * scale;


			int tmpx1 = tori_x1, tmpx2 = tori_x2, tmpy1 = tori_y1, tmpy2 = tori_y2;
			double rate = 1;
			switch (type){
			case 0:
				if ((tori_x2 - tori_x1) % 2 != 0){
					tori_x2--;
					rate *= (double)(tori_x2 - tori_x1) / (tmpx2 - tmpx1);
				}
				break;
			case 1:
				if ((tori_y2 - tori_y1) % 2 != 0){
					tori_y2--;
					rate *= (double)(tori_y2 - tori_y1) / (tmpy2 - tmpy1);
				}
				break;
			case 2:
				if ((tori_x2 - tori_x1) % 3 != 0){
					tori_x2 -= (tori_x2 - tori_x1) % 3;
					rate *= (double)(tori_x2 - tori_x1) / (tmpx2 - tmpx1);
				}
				break;
			case 3:
				if ((tori_y2 - tori_y1) % 3 != 0){
					tori_y2 -= (tori_y2 - tori_y1) % 3;
					rate *= (double)(tori_y2 - tori_y1) / (tmpy2 - tmpy1);
				}
				break;
			case 4:
				if ((tori_x2 - tori_x1) % 2 != 0){
					tori_x2--;
					rate *= (double)(tori_x2 - tori_x1) / (tmpx2 - tmpx1);
				}
				if ((tori_y2 - tori_y1) % 2 != 0){
					tori_y2--;
					rate *= (double)(tori_y2 - tori_y1) / (tmpy2 - tmpy1);
				}
				break;
			}
			th = ori_th * scale * scale * rate;
		}
		int getX1(){
			return x1;
		};
		int getX2(){
			return x2;
		};
		int getY1(){
			return y1;
		};
		int getY2(){
			return y2;
		};
		void setP(int in){
			p = in;
		};
		void setTh(int in){
			th = in;
			ori_th = in;
		};
		void setE(double in){
			e = in;
		};
		void setth(int t){
			th= t;
		};
		double getE(){
			return e;
		};
		int getth(){
			return th;
		};

		int judge(IntImg *intImg){
			this->sum = comput(intImg) * sigma;

			if(p==1)
				return th>=sum;
			else
				return th<=sum;
		};
		
		long long int comput(IntImg *intImg){
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
			sprintf_s(str,200,"%02d %02d %02d %02d %d %2d %8d %E",x1,y1,x2,y2,type,p,th,e);
			//sprintf_s(str, 200, "%02d %02d %02d %02d %d %2d", x1, y1, x2, y2, type,p);
		};
};
