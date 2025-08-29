#include <SFML/Graphics.hpp> //ignore this as when compiled using mingw32-make -f MakeFile it works just fine, its due to not being able to comiple using gcc/g++ directly
#include <iostream>
#include <vector>
#include <string>
#include <random>
using namespace std;

//here as well has in gamefunctions.cpp as couldnt seem to get working from another file - look into this
vector<vector<string>> makeminefield (int xsize, int ysize, string fillchar){
	//declares the mf (minefield)
	vector<vector<string>> minefield(ysize, vector<string>(xsize, fillchar));
    return (minefield);
}

vector<vector<string>> generatemines (int xsize, int ysize,int mines){
	vector<vector<string>> mf = makeminefield(xsize, ysize, "0");

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

unsigned int clickedelement(vector<vector<string>> knownmf,unsigned int gamexcoord,unsigned int gameycoord){
    unsigned int element;
    if (knownmf[gameycoord][gamexcoord] == "M"){
        element = 11; //as is 11th 'element' of the texture
    } else {
        element = stoi(knownmf[gameycoord][gamexcoord]); //converts string to int
    }
    return element;
}
 
void showminefield(vector<vector<string>> mf){
	for (auto i : mf){
		for (auto j : i){
			cout << j << " ";
		}
		cout << endl;
	}
}

vector<vector<sf::Sprite>> revealall(vector<vector<string>> knownmf,vector<vector<sf::Sprite>> allgrids,sf::Texture texture){
    for (int i = 0;i<32;i++){
        for (int j = 0;j<14;j++){
            if (knownmf[j][i] == "M"){
            //     //creates sprite with texture
            //     sf::Sprite sprite(texture);
            //     //makes sprite only use a certain part of the texture 
                allgrids[j][i].setTextureRect(sf::IntRect({331,0},{31,31})); //texture element of a bomb
            //     sprite.setPosition({(float)30*i, (float)30*j});
            //     allgrids[j][i] = (sprite);
            }
            else{
                sf::Sprite sprite(texture);
                int texturelocation;
                try
                {
                    texturelocation = stoi(knownmf[j][i])*30 + 1;
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
                //makes sprite only use a certain part of the texture 
                allgrids[j][i].setTextureRect(sf::IntRect({texturelocation,0},{31,31})); //texture element of a bomb
            }
        }
    }
    return allgrids;
}

/*texturepositions = {
    {{0,0},{31,31}},  //represents empty 
    {{31,0},{31,31}}, //represents 1
    {{61,0},{31,31}}, //represents 2
    {{91,0},{31,31}}, //represents 3
    {{121,0},{31,31}},//represents 4
    {{151,0},{31,31}},//represents 5
    {{181,0},{31,31}},//represents 6
    {{211,0},{31,31}},//represents 7
    {{241,0},{31,31}},//represents 8
    {{271,0},{31,31}},//represents unknown
    {{301,0},{31,31}},//represents flag
    {{331,0},{31,31}},//represents bomb
    };
*/

/*texutre "gameover.png"
width = 766px
height = 274px*/

int main()
{
    //size_t = type that is long long unsigned int 
    unsigned int cellsize {30};
    unsigned int windowx {32*cellsize};
    unsigned int windowy {14*cellsize};
    sf::RenderWindow window(sf::VideoMode({windowx, windowy}), "Title");

    // Load texture once (outside the loop)
    sf::Texture texture;
    if (!texture.loadFromFile("gametextures.png")) {
        std::cerr << "Failed to load texture" << std::endl;
        return -1;
    }
    //
    sf::Texture gameover;
    if (!gameover.loadFromFile("gameover.png")) {
        std::cerr << "Failed to load texture2" << std::endl;
        return -1;
    }
    sf::Sprite end(gameover);

    vector<vector<string>> knownmf = (generatenumbers(32,14,60));
    //for testing
    showminefield(knownmf);

    //creates playerview, where intially all the tiles are marked "U" for unknown
    vector<vector<string>> playerview = (makeminefield(32,14,"U"));


    // Debug: Check the texture's size
    std::cout << "Texture size: " << texture.getSize().x << "x" << texture.getSize().y << std::endl;
    std::cout << "Texture2 size: " << gameover.getSize().x << "x" << gameover.getSize().y << std::endl;

    sf::Sprite inital(texture);
    inital.setTextureRect(sf::IntRect({271,0},{31,31})); //sets base texture as unseen grids

    vector<vector<sf::Sprite>> allgrids(14, vector<sf::Sprite>(32,inital));
    //creating the inital grid, adding all tile sprites to a vector of sprites for easy lookup when user clicks 
    for (int i = 0;i<32;i++){
        for (int j = 0;j<14;j++){
            sf::Sprite sprite(texture);
            //makes sprite only use a certain part of the texture 
            sprite.setTextureRect(sf::IntRect({271,0},{31,31})); //texture element of a bomb
            sprite.setPosition({(float)30*i, (float)30*j});
            allgrids[j][i] = (sprite);
        }
    }
    //make the cell clicked event work out cell coordinated from window coordinates and then use that to work out the loctaion of the sprite in the vecotr and select that way
    //NEXT NEED TO GO THROUGH THE VECOTR UNTIL FIND THE SPRITE THAT CONTAINS THE MOUSCLICK AND THEN CAN EDIT, WILL NEED TO CHECK AND VALIDATE IT WONT END THE GAME FIRST ALSO
    while (window.isOpen())
    {
        window.clear(sf::Color(238,238,228));

        for (vector<sf::Sprite> gridline : allgrids)
            for (sf::Sprite i : gridline){
                window.draw(i);
            }

        window.display();
    

        while (std::optional event = window.pollEvent())
        {
            //if close button is clicked
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            
            if (event->is<sf::Event::MouseButtonPressed>()){
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
                    sf::Vector2i xycoords = sf::Mouse::getPosition(window); //gives xy coords of mouse click reletive to the window -> if getPosition() gives coords reletive to entire screen 
                    cout<<"leftclick registered at "<< xycoords.x << "," << xycoords.y <<endl;
                    unsigned int gamexcoord = xycoords.x/cellsize;
                    unsigned int gameycoord = xycoords.y/cellsize;

                    //to get whole number representing the mine clicked
                    floor(gamexcoord);
                    floor(gameycoord);

                    cout<<"mine: "<< gamexcoord << "," << gameycoord <<endl;
                    cout<<endl;

                    if (playerview[gameycoord][gamexcoord] == "U"){
                        unsigned int value = clickedelement(knownmf,gamexcoord,gameycoord);
                        int textureval = value*30 + 1;

                        allgrids[gameycoord][gamexcoord].setTextureRect(sf::IntRect({textureval,0},{31,31}));

                        playerview[gameycoord][gamexcoord] = "K";

                        if (value == 11){
                            allgrids = revealall(knownmf,allgrids,texture);
                            window.draw(end); //NEED TO FIND OUT WHY THIS ISNT WORKING AND WHAT CAN BE DONE TO FIX IT 
                        }
                        // if (this happens) do {this thing}
                    }

                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)){
                    sf::Vector2i xycoords = sf::Mouse::getPosition(window); //gives xy coords of mouse click reletive to the window -> if getPosition() gives coords reletive to entire screen 
                    cout<<"rightclick registered at "<< xycoords.x << "," << xycoords.y <<endl;
                    unsigned int gamexcoord = xycoords.x/cellsize;
                    unsigned int gameycoord = xycoords.y/cellsize;

                    //to get whole number representing the mine clicked
                    floor(gamexcoord);
                    floor(gameycoord);

                    cout<<"mine: "<< gamexcoord << "," << gameycoord <<endl;
                    cout<<endl;

                    //only works if not already marked or already clicked/'known', i.e marked as "U" = unknown
                    if (playerview[gameycoord][gamexcoord] == "U"){ 
                        allgrids[gameycoord][gamexcoord].setTextureRect(sf::IntRect({301,0},{31,31})); //texture location of marker

                        playerview[gameycoord][gamexcoord] = "F"; //F for flag
                    } else if (playerview[gameycoord][gamexcoord] == "F"){
                        allgrids[gameycoord][gamexcoord].setTextureRect(sf::IntRect({271,0},{31,31})); //texture location of unknown

                        playerview[gameycoord][gamexcoord] = "U"; //U for unknown
                    }
                }
            }
        }


    }
    return 0;
}


//experiments
        /*
        sf::RectangleShape cell(sf::Vector2f(cellsize - 1, cellsize - 1));
        cell.setPosition(sf::Vector2f(50,50));
        cell.setFillColor(sf::Color(250,250,250));
        cell.setOutlineColor(sf::Color::Black);
        cell.setOutlineThickness(1);
        window.draw(cell);
        sf::RectangleShape cell2(sf::Vector2f(cellsize - 1, cellsize - 1));
        cell2.setPosition(sf::Vector2f(100,100));
        cell2.setFillColor(sf::Color::White);
        cell2.setOutlineColor(sf::Color(250,250,250));
        cell2.setOutlineThickness(5.f);
        window.draw(cell2);
        */
// a sprite is just a textured cell
           /*
        //creates sprite with texture
        sf::Sprite sprite(texture);
        //makes sprite only use a certain part of the texture 
        sprite.setTextureRect(sf::IntRect({0,0},{31,31}));
        sprite.setPosition({50.f, 50.f});


        //creates sprite2 with other part of texture
        sf::Sprite sprite2(texture);
        //makes sprite only use a certain part of the texture 
        sprite2.setTextureRect(sf::IntRect({31,0},{31,31}));

        sprite2.setPosition({100.f, 150.f});
        */