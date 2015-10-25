class Face_Window{
public:
	int x1, x2, y1, y2, width;
	double move, scale;

	Face_Window(){};
	Face_Window(int x1, int y1, int x2, int y2, double move, double scale){
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;
		this->width = x2 - x1;
		this->move = move;
		this->scale = scale;
	}
};
