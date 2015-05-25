class IntImg{
	public:
		std::vector<std::vector<long long int>> data;
		IntImg(){};
		IntImg(int height, int width){
			data.resize(height);
			for (int i = 0; i < height; i++)
				data[i].resize(width);
		};
};
