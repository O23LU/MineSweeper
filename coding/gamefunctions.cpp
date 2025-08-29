#include <iostream>
#include <vector>
#include <string>
#include <random>
using namespace std;

vector<vector<string>> makeminefield (int xsize, int ysize){
	//declares the mf (minefield)
	vector<vector<string>> minefield(ysize, vector<string>(xsize, "0"));
    return (minefield);
}

vector<vector<string>> generatemines (int xsize, int ysize,int mines){
	vector<vector<string>> mf = makeminefield(xsize, ysize);

	// Get a random seed
	random_device rd;  
	// Initialize Mersenne Twister PRNG
    mt19937 gen(rd()); 

	uniform_int_distribution<int> xrandom(0, xsize-1);
	uniform_int_distribution<int> yrandom(0, ysize-1);
	//loops through based on amount of mines and generates random x,y locations to place
	for (int i = 0; i<mines; i++){
		// Generate random number in range [0, size of minefield]
		//creates random x and y values for mines to be placed
		int x = xrandom(gen);
		int y = yrandom(gen); 
		if (mf[y][x] != "M"){
			//if a mine not placed already in that x,y location then place on there
			mf[y][x] = "M";
		}
		else{
			//if mine already placed there deincrement to same amount of mines placed is kept the same 
			i--;
		}
	}
	return (mf);
}

vector<vector<string>> generatenumbers (int xsize, int ysize, int mines){
	vector<vector<string>> mf = generatemines(xsize,ysize,mines);
	for (int x = 0; x<xsize;x++){
		for (int y = 0; y<ysize;y++){
			if (mf[y][x] != "M"){
				int count = 0;
				for (int dx = -1;dx<2;dx++){
					for (int dy = -1;dy<2;dy++){
						if(dx == 0 && dy == 0){
							continue;
						} 
						int boundedx = x+dx;
						int boundedy = y+dy;
						if (boundedx >= 0 && boundedx < xsize && boundedy >= 0 && boundedy < ysize){
							if (mf[boundedy][boundedx] == "M"){
								count++;
							}
						}
					}
				}
				mf[y][x] = to_string(count);
			}
		}
	}
	return (mf);
}

void showminefield(vector<vector<string>> mf){
	for (auto i : mf){
		for (auto j : i){
			cout << j << " ";
		}
		cout << endl;
	}
}