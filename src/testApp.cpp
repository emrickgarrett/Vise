#include "testApp.h"
#include <Windows.h>
#include <mmsystem.h>

#include <set>
#include <iostream> //For debugging

//////////////////
/* Function declarations
 *
 * These are all functions that are defined and used in this file, so we don't 
 * need to put them in the .h file. You can ignore this: The comments about what
 * each function does is down in the body of the code.
 */
//Functions you might want to use, game logic
bool inVise(int x, int y);
void doVise();
void getNbrs(int x, int y, int& okayNbrs, int& badNbrs, std::vector<pair<int, int>>& neighborList);
bool canPlaceNewPiece(int x, int y);
bool isNeighboringSpace(int x, int y);
bool isJumpSpace(int x, int y);
bool isConnected();
bool canPlaceOldPiece(int x, int y);
int pieceAt(int x,int y);
void putPieceAt(int x, int y, int whichPiece);
std::vector<pair<int,int>> mergeVectors(std::vector<pair<int,int>> v1, std::vector<pair<int,int>> v2);
void getOpenNbrs(int x, int y, std::vector<pair<int,int>>& list);
std::vector<pair<int,int>> determineSmallest(int whoseTurn);
void returnPieces(std::vector<pair<int,int>> cluster);
bool checkBoardForWin(int& winner);
void resetGame();
void makeAIMove();

//Drawing functions
void drawHex(float x, float y, float sideLen);
void drawBoard();
void drawSpares();
void drawWinner();
void drawMenu();
void drawCredits();
void drawGUI();

//////////////////
/*
 * Model variables
 *
 * These are the variables that keep track of the game model:
 * the state of the board and the number of pieces
 */

//Width and heigh of the board, in hexes.
int boardW = 20;
int boardH = 20;

int pl1Wins = 0;
int pl2Wins = 0;

//TODO: Make any variables you need for representing your board here

//Number of spare playing pieces left, for each player
int pl1spares=4;
int pl2spares=4;

//Number of destroyed pieces
int destroyedPieces = 0;
int aiDestroyed = 0;

//If the player is playing the AI
bool playingAI = false;

//Valid values are 1 or 2, to indicate whose turn it is
int whoseTurn=1;

//Vector to hold all spots on the board, using flags above to indicate if spot is controlled (1 or 2)
std::vector<int> board = std::vector<int>(boardW*boardH, 0);

/*
 * What is the current player doing?
 *
 * 0: Doing nothing
 * 1: Holding a new piece to place
 * 2: Holding an existing piece to move
 */
int currentAction=0;

//The coordinates of the selected piece, if the player is moving an existing piece. This is in Board coordinates, not screen coordinates.
int selectedPieceX=0;
int selectedPieceY=0;

/////////////////////
/*
 * State variables
 *
 * These keep track of the current state of the program, but only the parts
 * that are NOT part of the game model
 */
int currentFrame = 0;

//Time at the start of the program
float startTime;

//Is there a winner?
bool isWinner = false;

//Is in the game?
bool inGame = true;

//Who is the winner?
int winner = 0;

/////////////////
/*
 * View variables
 *
 * Variables that are used for drawing, but are unrelated to the model and
 * program state
 */

//The upper-left corner of the board should be at this x,y position
float boardXOffset = 50;
float boardYOffset = 50;

//The side length of each hex, in pixels
float sideLen = 20.0;

//The width and height of a hex, in pixels. Notice that you shouldn't
// change these, just change sideLen
float hexW = sideLen*2.0*0.86602540378444;
float hexH = 1.5*sideLen;





//--------------------------------------------------------------
void testApp::setup(){

    //This is the *maximum* rate. Your program might go slower if your
    // updates or draws are too time conusming.
    ofSetFrameRate(60);

	srand(clock());
	resetGame();

    //TODO: Initialize your "board" data structure here
	inGame = false;
    startTime = ofGetElapsedTimef();
}

void resetGame(){
	pl1spares = 4;
	pl2spares = 4;
	board = std::vector<int>(boardW*boardH, 0);

	putPieceAt(9,9,1);
	putPieceAt(10,9,2);

	winner = 0;
	isWinner = false;
	currentAction = 0;
	whoseTurn = 1;
	destroyedPieces = 0;
	selectedPieceX = 0;
	selectedPieceY = 0;
	aiDestroyed = 0;
}

//Return true iff there is a piece in board space (x,y), and that piece
// is caught in a vise. Note that x and y are in board coordinates,
// not screen coordinates
bool inVise(int x, int y){
    //TODO
	int color = 3- board[x + y*boardH];
	int offset = (y%2 == 0)? -1: 1;

	//I'm thinking same logic as jump space, except I'm looking at the midpoints! 
	//Also going to split into even/odd for now, because I'm burnt out on thinking atm and want to get this done
	//for the video tonight!

	//Split into 3 checks, bottom left, left, top left (if there's no opposing piece, it doesn't need to check opposite side!)
	if(offset == -1){ // Even
		if(x-1 >= 0 && y-1 >= 0 && board[(x-1)+(y-1)*boardH] == color ){
			if(y+1 < boardH && board[x+(y+1)*boardH] == color){
				board[x+(y)*boardH] = 0;
				destroyedPieces++;
				PlaySound("..//sounds//destroy.wav", NULL, SND_ASYNC);
				if(playingAI && color == 2) aiDestroyed++;
			}
		}
		if(x-1 >= 0 && board[(x-1)+y*boardH] == color){
			if(x+1 < boardW && board[(x+1)+y*boardH] == color){
				board[x+(y)*boardH] = 0;
				destroyedPieces++;
				PlaySound("..//sounds//destroy.wav", NULL, SND_ASYNC);
				if(playingAI && color == 2) aiDestroyed++;
			}
		}
		if(x-1 >= 0 && y+1 < boardH && board[(x-1)+(y+1)*boardH] == color){
			if(y-1 >= 0 && board[x + (y-1)*boardH] == color){
				board[x+(y)*boardH] = 0;
				destroyedPieces++;
				PlaySound("..//sounds//destroy.wav", NULL, SND_ASYNC);
				if(playingAI && color == 2) aiDestroyed++;
			}
		}
	}
	if(offset == 1){// Odd
		if(y-1 >= 0 && board[x+(y-1)*boardH] == color ){
			if(x+1 < boardW && y+1 < boardH && board[(x+1)+(y+1)*boardH] == color){
				board[x+(y)*boardH] = 0;
				destroyedPieces++;
				PlaySound("..//sounds//destroy.wav", NULL, SND_ASYNC);
				if(playingAI && color == 2) aiDestroyed++;
			}
		}
		if(x-1 >= 0 && board[(x-1)+y*boardH] == color){
			if(x+1 < boardW && board[(x+1)+y*boardH] == color){
				board[x+(y)*boardH] = 0;
				destroyedPieces++;
				PlaySound("..//sounds//destroy.wav", NULL, SND_ASYNC);
				if(playingAI && color == 2) aiDestroyed++;
			}
		}
		if( y+1 < boardH && board[x+(y+1)*boardH] == color){
			if(x+1 < boardW && y-1 >= 0 && board[(x+1) + (y-1)*boardH] == color){
				board[x+(y)*boardH] = 0;
				destroyedPieces++;
				PlaySound("..//sounds//destroy.wav", NULL, SND_ASYNC);
				if(playingAI && color == 2) aiDestroyed++;
			}
		}
	}

    return false;
}

/*
 * This is the method that updates the board if a player is caught in a vise at the end of the turn.
 * You may want to break this method up into several sub-methods.
 *
 * 1) FIRST, identify all pieces that are caught in a vise Note: If you have 0101, then
 *    both the middle 1 and middle 0 are caught in the vice. Use the "inVise" method defined above.
 * 2) NEXT, delete them all from the game (in previous example, you would get 0__1)
 * 3) LAST, find the largest connected component that contains
 *    at least 1 piece from each player. Place all other pieces back in the
 *    storehouse (that is, update pl1spares and pl2spares). If there is a tie, pick the one that has the most pieces from the player
 *    that just played.
 * 3b) If no such component exists, then select the largest
 *    connected component that contains a piece of the player who played
 *    most recently.
 * 3c) If no such component exists, then select the largest connected component.
 * 3d) Tie-breaking: If there is a tie under any of these rules, pick arbitrarily
 */
void doVise(){

	//Remove all pieces caught in a vise!
	for(int x = 0; x < boardW; x++){
		for(int y = 0; y < boardH; y++){
			if(board[x + y*boardH] != 0){
				inVise(x, y);
			}
		}
	}
	if(isConnected() || currentAction != 0) return;
	std::vector<pair<int,int>> smallest = determineSmallest(whoseTurn);

	returnPieces(smallest);

	int tWinner = 0;

	bool hasWon = checkBoardForWin(tWinner);

	if(hasWon){
		isWinner = true;
		winner = tWinner;
		inGame = false;

		if(winner == 0){
			std::cout << "Tie!" << std::endl;
			return;
		}

		(winner == 1)? pl1Wins++ : pl2Wins++;

		std::cout << "Winner is: " << tWinner << std::endl;
		PlaySound("..//sounds//victory.wav", NULL, SND_ASYNC);
	}

}

bool checkBoardForWin(int& winner){
	bool p1hasPiece = false;
	int p1total = 0;
	bool p2hasPiece = false;
	int p2total = 0;

	for(int x = 0; x < boardW; x++){
		for(int y = 0; y < boardH; y++){
			if(board[x+y*boardH] == 1){ p1hasPiece = true; p1total++;}
			if(board[x+y*boardH] == 2){ p2hasPiece = true; p2total++;}
		}
		
	}

	if(p1hasPiece && p2hasPiece){
		if(pl1spares == 0 && p1total == 1){
			winner = 2;
			return true;
		}else if(pl2spares == 0 && p2total == 1){
			winner = 1;
			return true;
		}else{
			return false;
		}
	}

	if(!p1hasPiece || !p2hasPiece){
		if(p1hasPiece){
			winner = 1;
			return true;
		}else if(p2hasPiece){
			winner = 2;
			return true;
		}else{
			winner = 0;
			return true;
		}
	}
		return false;
}

//Called to get the largest cluster on the board, containing a piece of the last player to go :)
std::vector<pair<int,int>> determineSmallest(int whoseTurn){
	std::vector<pair<int,int>> largest = std::vector<pair<int,int>>();
	std::vector<pair<int,int>> ignore = std::vector<pair<int,int>>();

	//TODO
	int tempX = 0;
	int tempY = 0;
	int numCount = 0;


	for(int x = 0; x < boardW; x++){
		for(int y = 0; y < boardH; y++){
			bool notInIgnore = true;
			for(std::vector<pair<int,int>>::iterator ignIt = ignore.begin(); ignIt != ignore.end() && notInIgnore; ignIt++){
				if(x == (*ignIt).first && y == (*ignIt).second) notInIgnore = false;
			}
			if(board[x + y*boardH] != 0 && notInIgnore){
				 numCount++;

				 std::vector<pair<int, int>> nodes = std::vector<pair<int,int>>();
				 nodes.push_back(pair<int, int>(x, y));

				 int goodNbrs = 0;
				 int badNbrs = 0;
				 

				 std::vector<pair<int,int>> neighbors = std::vector<pair<int, int>>();
				 getNbrs(x, y, goodNbrs, badNbrs, neighbors);

				 std::vector<pair<int,int>> temp = std::vector<pair<int,int>>();

				 neighbors.push_back(pair<int,int>(x,y));

				 for(std::vector<pair<int,int>>::iterator it = neighbors.begin(); it != neighbors.end(); it++){
					 bool flag = false;
					for(std::vector<pair<int,int>>::iterator nIt = nodes.begin(); nIt != nodes.end(); nIt++){
						if((*it) == (*nIt))flag = true;
					}
					 if(!flag){
							numCount++;
							temp.clear();
							nodes.push_back((*it));

							getNbrs((*it).first, (*it).second, goodNbrs, badNbrs, temp);
							neighbors = mergeVectors(neighbors, temp);
							it = neighbors.begin();

						}
					
				 }
				 if((numCount < (2 + (4 - pl1spares) + (4-pl2spares) - destroyedPieces)/2)){
					  return neighbors;
				 }
				 if(largest.size() == neighbors.size()){
					 int largestSize = 0;
					 int smallestSize = 0;

					 for(std::vector<pair<int,int>>::iterator it = largest.begin(); it != largest.end(); it++){
						 if(board[(*it).first + (*it).second*boardH] == whoseTurn) largestSize++;
					 }
					 for(std::vector<pair<int,int>>::iterator it = neighbors.begin(); it != neighbors.end(); it++){
						 if(board[(*it).first + (*it).second*boardH] == whoseTurn) smallestSize++;
					 }

					 if(largestSize >= smallestSize){
						 //std::cout << "large" << std::endl;
						 return largest;
					 }else{
						 //std::cout << "small" << std::endl;
						 return neighbors;
					 }

				 }else if(largest.size() != 0 && largest.size() >= neighbors.size()){
					 //neighbors.push_back(pair<int,int>(x,y));
					 //std::cout << "Large Size: " << largest.size() << "\nSmall Size: " << neighbors.size() << std::endl;
					 //std::cout << "small2" << std::endl;
					 return neighbors;
				 }else if(largest.size() != 0 && largest.size() < neighbors.size()){
					// std::cout << "large2" << std::endl;
					 return largest;
				 }
				 //std::cout << "This should run twice" << std::endl;
				 largest = neighbors;
				 ignore = mergeVectors(ignore, neighbors);
				 

			}

			


		}
	}
	std::cout << "oops" << std::endl;
	return largest;
}


//Called to return the pieces of a cluster!
void returnPieces(std::vector<pair<int,int>> cluster){
	for(std::vector<pair<int,int>>::iterator it = cluster.begin(); it != cluster.end(); it++){
		int color = 3 - board[(*it).first + (*it).second*boardH];
		if(color == 1){
			board[(*it).first + (*it).second*boardH] = 0;
			pl2spares++;
		}else{
			board[(*it).first + (*it).second*boardH] = 0;
			pl1spares++;
		}
	}
}


//--------------------------------------------------------------
void testApp::update(){
    //This can be helpful if you are doing animations
    currentFrame = (ofGetElapsedTimef() - startTime)/(1.0/60);
    
    //Check for vised pieces on every update
	if(inGame){
		doVise();

		if(playingAI && whoseTurn == 2){
			makeAIMove();
		}
	}
	
}

//Draw a single hexagon centered at (x,y).
// These are screen coordinates, not board coordinates.
void drawHex(float x, float y, float sideLen){
    //point 1 is sideLen up from the center
    ofPoint pTop(x,y-sideLen);
    ofPoint pBot(x,y+sideLen);
    ofPoint pRtTop(x+0.86602540378444*sideLen,y-0.5*sideLen);
    ofPoint pRtBot(x+0.86602540378444*sideLen,y+0.5*sideLen);
    ofPoint pLtTop(x-0.86602540378444*sideLen,y-0.5*sideLen);
    ofPoint pLtBot(x-0.86602540378444*sideLen,y+0.5*sideLen);
    
    ofSetLineWidth(1.0);
    ofLine(pTop,pRtTop);
    ofLine(pRtTop,pRtBot);
    ofLine(pRtBot, pBot);
    ofLine(pBot,pLtBot);
    ofLine(pLtBot,pLtTop);
    ofLine(pLtTop,pTop);
}


/*
 * x and y are the input parameters, which are in board coordinates
 * For a given space (x,y), check to see how many of the neighboring spaces
 * contain a piece of the current player (that is okayNbrs) and how many
 * contain a piece of the opposing player (that is badNbrs). This is useful
 * in determining if the current player can play a new piece in the hex
 * under consideration.
 */
void getNbrs(int x, int y, int& okayNbrs, int& badNbrs, std::vector<std::pair<int, int>>& neighborList){
	int offSetCalc = y % 2; // Need this because of how board is drawn (To make this easier on me)
	int offset = (offSetCalc == 0)? -1 : 1;


	//Spots on same x axis are easy to check

	if(x > 0){
		if(board[x-1 + y*boardH] >0){
			(board[x-1 + y*boardH] == whoseTurn)? okayNbrs++ : badNbrs++;
			neighborList.push_back(std::pair<int, int>(x-1, y));
		}
	}
	if(x < boardW-1){
		if(board[x+1 + y*boardH] > 0){
			(board[x+1+y*boardH] == whoseTurn)? okayNbrs++ : badNbrs++;
			neighborList.push_back(std::pair<int, int>(x+1, y));
		}
	}
	

	//Use offset to account for odd/even rows

		if( y > 0){
				if(board[x + (y-1)*boardH] > 0){
					(board[x + (y-1)*boardH] == whoseTurn)? okayNbrs++ : badNbrs++;
					neighborList.push_back(std::pair<int, int>(x, y-1));
				}

				if(x < boardW-1){
					if(board[x+offset + (y-1)*boardH] > 0){
						(board[x+offset + (y-1)*boardH] == whoseTurn)? okayNbrs++ : badNbrs++;
						neighborList.push_back(std::pair<int, int>(x+offset, y-1));
					}
				}
			}
			if( y < boardH-1){
				if(board[x + (y+1)*boardH] > 0){
					(board[x + (y+1)*boardH] == whoseTurn)? okayNbrs++ : badNbrs++;
					neighborList.push_back(std::pair<int, int>(x, y+1));
				}

				if(x < boardW-1){
					if(board[x+offset + (y+1)*boardH] > 0){
						(board[x+offset + (y+1)*boardH] == whoseTurn)? okayNbrs++ : badNbrs++;
						neighborList.push_back(std::pair<int, int>(x+offset, y+1));
					}
				}
			}

			return;
}

void getOpenNbrs(int x, int y, std::vector<pair<int,int>>& list){
	int offSetCalc = y % 2; // Need this because of how board is drawn (To make this easier on me)
	int offset = (offSetCalc == 0)? -1 : 1;


	//Spots on same x axis are easy to check

	if(x > 0){
		if(board[x-1 + y*boardH] == 0){
			list.push_back(std::pair<int, int>(x-1, y));
		}
	}
	if(x < boardW-1){
		if(board[x+1 + y*boardH] == 0){
			list.push_back(std::pair<int, int>(x+1, y));
		}
	}
	

	//Use offset to account for odd/even rows

		if( y > 0){
				if(board[x + (y-1)*boardH] == 0){
					list.push_back(std::pair<int, int>(x, y-1));
				}

				if(x < boardW-1){
					if(board[x+offset + (y-1)*boardH] == 0){
						list.push_back(std::pair<int, int>(x+offset, y-1));
					}
				}
			}
			if( y < boardH-1){
				if(board[x + (y+1)*boardH] == 0){
					list.push_back(std::pair<int, int>(x, y+1));
				}

				if(x < boardW-1){
					if(board[x+offset + (y+1)*boardH] == 0){
						list.push_back(std::pair<int, int>(x+offset, y+1));
					}
				}
			}
	return;
}

/*
 * Return true iff the current player can place a new piece
 * in row y, column x, without violating the rules. That is,
 * at least 1 neighboring hex must contain one of the player's
 * pieces, and none of the neighboring hex can contain an
 * opposing player's piece
 */
bool canPlaceNewPiece(int x, int y){
	if(board[x + y*boardW] != 0) return false;
    int okayNbrs=0;
    int badNbrs=0;
	std::vector<std::pair<int, int>> garbage = std::vector<std::pair<int, int>>();
    getNbrs(x,y,okayNbrs,badNbrs, garbage);
    return(okayNbrs > 0 && badNbrs == 0);
}

//Return true iff (x,y) is neighboring to (selectedPieceX,selectedPieceY)
//These inputs are in board coordinates, not screen coordinates
//May have bad logic!
bool isNeighboringSpace(int x, int y){
    // Simple, just see if the absolute values of the difference of x/y are both less or equal to one.
	// Need an alternative method that gets all 'open' neighboring spaces!
	std::vector<pair<int,int>> neighbors = std::vector<pair<int,int>>();
	getOpenNbrs(selectedPieceX, selectedPieceY, neighbors);

	pair<int,int> temp = pair<int,int>(x,y);

	for(std::vector<pair<int,int>>::iterator it = neighbors.begin(); it != neighbors.end(); it++){
		if((*it).first == temp.first && (*it).second == temp.second){
			return true;
		}
	}

    return false;
}

//Return true iff (x,y) is one jump to (selectedPieceX,selectedPieceY)
//These inputs are in board coordinates, not screen coordinates
//The logic for this isn't right! NEED TO FIX
bool isJumpSpace(int x, int y){

	if(board[x+y*boardH] != 0) return false; //Space isn't empty ;)
	//TODO
	int offset = ((y % 2 == 0))? -1 : 1;

	//Simplify the branch to make this part easier, I don't have enough time to really think it through :/
	if(offset == -1){ //Even Row
		if(selectedPieceY -2 >= 0){
			if(selectedPieceX + 1 == x && selectedPieceY -2 == y && selectedPieceX +1 < boardW){
				if(board[selectedPieceX + (selectedPieceY-1)*boardH] != 0) return true;
			}
			if(selectedPieceX-1 == x && selectedPieceY - 2 == y && selectedPieceX -1 >= 0){
				if(board[(selectedPieceX-1) + (selectedPieceY-1)*boardH] != 0) return true;
			}
		}
		if(selectedPieceX-2 == x && selectedPieceY == y && selectedPieceX-2 >= 0){
			if(board[(selectedPieceX-1) + (selectedPieceY)*boardH] != 0) return true;
		}
		if(selectedPieceX+2 == x && selectedPieceY == y && selectedPieceX+2 < boardW){
			if(board[(selectedPieceX+1) + (selectedPieceY)*boardH] != 0) return true;
		}
		if(selectedPieceY +2 < boardH){
			if(selectedPieceX -1 == x && selectedPieceY +2 == y && selectedPieceX -1 >= 0){
				if(board[(selectedPieceX-1) + (selectedPieceY+1)*boardH] != 0) return true;
			}
			if(selectedPieceX +1 == x && selectedPieceY +2 == y && selectedPieceX +1 < boardW){
				if(board[(selectedPieceX) + (selectedPieceY+1)*boardH] != 0) return true;
			}
		}
	}else{//Odd Row
		if(selectedPieceY -2 >= 0){
			if(selectedPieceX + 1 == x && selectedPieceY -2 == y && selectedPieceX +1 < boardW){
				if(board[(selectedPieceX+1) + (selectedPieceY-1)*boardH] != 0) return true;
			}
			if(selectedPieceX-1 == x && selectedPieceY - 2 == y && selectedPieceX -1 >= 0){
				if(board[(selectedPieceX) + (selectedPieceY-1)*boardH] != 0) return true;
			}
		}
		if(selectedPieceX-2 == x && selectedPieceY == y && selectedPieceX -2 >= 0){
			if(board[(selectedPieceX-1) + (selectedPieceY)*boardH] != 0) return true;
		}
		if(selectedPieceX+2 == x && selectedPieceY == y && selectedPieceX + 2 < boardW){
			if(board[(selectedPieceX+1) + (selectedPieceY)*boardH] != 0) return true;
		}
		if(selectedPieceX -1 == x && selectedPieceY +2 == y && selectedPieceX -1 >= 0 && selectedPieceY +2 < boardH){
			if(board[(selectedPieceX) + (selectedPieceY+1)*boardH] != 0) return true;
		}
		if(selectedPieceX +1 == x && selectedPieceY +2 == y && selectedPieceX +1 < boardW && selectedPieceY +2 < boardH){
			if(board[(selectedPieceX) + (selectedPieceY+1)*boardH] != 0) return true;
		}
	}


	return false;

	
}

//Return true if and only if the board currently contains
// only 1 connected component
//Hint: In my solution I first counted the total number of pieces on the board.
// Then I looped to find any piece of the board. From there I did a depth
// first search, counting how many pieces I found. If the number found
// equals the total number on the board, then return true. Otherwise,
// return false
bool isConnected(){
    //TODO
	int tempX = 0;
	int tempY = 0;
	int numCount = 0;


	for(int x = 0; x < boardW; x++){
		for(int y = 0; y < boardH; y++){
			if(board[x + y*boardH] != 0){
				 numCount++;

				 std::vector<pair<int, int>> nodes = std::vector<pair<int,int>>();
				 nodes.push_back(pair<int, int>(x, y));

				 int goodNbrs = 0;
				 int badNbrs = 0;
				 

				 std::vector<pair<int,int>> neighbors = std::vector<pair<int, int>>();
				 getNbrs(x, y, goodNbrs, badNbrs, neighbors);

				 std::vector<pair<int,int>> temp = std::vector<pair<int,int>>();

				 for(std::vector<pair<int,int>>::iterator it = neighbors.begin(); it != neighbors.end(); it++){
					 bool flag = false;
					for(std::vector<pair<int,int>>::iterator nIt = nodes.begin(); nIt != nodes.end(); nIt++){
						if((*it) == (*nIt))flag = true;
					}
					 if(!flag){
							numCount++;
							temp.clear();
							nodes.push_back((*it));

							getNbrs((*it).first, (*it).second, goodNbrs, badNbrs, temp);
							neighbors = mergeVectors(neighbors, temp);
							it = neighbors.begin();

						}
					
				 }
				 return (numCount == 2 + (4 - pl1spares) + (4-pl2spares) - destroyedPieces);

			}

			


		}
		
	}
	//TODO return:
	return (numCount == 2 + (4-pl1spares)+ (4-pl2spares) - destroyedPieces);
}

std::vector<pair<int,int>> mergeVectors(std::vector<pair<int,int>> v1, std::vector<pair<int,int>> v2){
	std::vector<pair<int,int>> temp = std::vector<pair<int,int>>();

	for(std::vector<pair<int,int>>::iterator it = v1.begin(); it != v1.end(); it++){
		temp.push_back((*it));
	}

	for(std::vector<pair<int,int>>::iterator it = v2.begin(); it != v2.end(); it++){
		bool flag = false;
		for(std::vector<pair<int,int>>::iterator tempIt = temp.begin(); tempIt != temp.end(); tempIt++){
			if((*it) == (*tempIt)) flag = true;
		}
		if(!flag){
				temp.push_back((*it));
		}
	}

	return temp;
}

/* This is used when the player is moving one of her pieces that is
 * already on the board to a new space.
 *
 * Return true iff the current player can place the piece in board
 * position (x,y).
 *
 * We can move the old piece here if
 * 0) Target space is empty,
 * AND
 * 1a) Moving to an unoccupied neighboring space, OR
 * 1b) Moving to an unoccupied space that is one jump over a neighbor
 * AND
 * 2) The move does not result in disconnected board
 * AND
 * 3) Target space is still adjacent to an existing piece (could be our own
 *    piece or an enemy piece, doesn't matter)
 *
 * Hint: you may want to use getNbrs, isNeighboringSpace,
 *       isJumpSpace, and isConnected as subroutines here.
 */
bool canPlaceOldPiece(int x, int y){
    //TODO
	int okayNbrs = 0;
	int badNbrs = 0;
	std::vector<pair<int,int>> neighbors = std::vector<pair<int,int>>();

	if(board[x + y*boardH] != 0) return false;

	getNbrs(selectedPieceX, selectedPieceY, okayNbrs, badNbrs, neighbors);
	if(okayNbrs == 0 && badNbrs == 0) return false;
	if(!isJumpSpace(x, y) && !isNeighboringSpace(x, y)) return false;

	std::vector<int> tempBoard = board;
	if((abs(selectedPieceX-x) >= 2 || abs(selectedPieceY-y) >= 2) && !isJumpSpace(x, y)) return false;
	putPieceAt(x, y, whoseTurn);
	putPieceAt(selectedPieceX, selectedPieceY, 0);

	if(isConnected()){
		board = tempBoard;
		return true;
	}else{
		board = tempBoard;
		return false;
	}


    return true;
}
/*
 * Which type of piece is at board position (x,y)?
 * If no piece, return 0. Otherwise, return the player number of the piece
 * (1 or 2)
 */
int pieceAt(int x,int y){
    return board[x+y*boardH];
}

void drawBoard(){
    //NOTE: This will need to be adapted to your board
    // data structure!
    //For each board hex...
    for(int y=0;y<boardH;y++){
        for(int x=0;x<boardW;x++){
            //Calculate the center, and draw the border
            float offset = (hexW/2) * (y%2);
            ofSetColor(0, 0, 0);
            drawHex(boardXOffset+x*hexW+offset,boardYOffset+y*hexH,sideLen);
            
            if(pieceAt(x,y) != 0){
                //If there is a playing piece in the current hex,
                // draw it
                if(pieceAt(x,y) == 1){
                    ofSetColor(255,255,255);
                } else {
                    ofSetColor(0,0,0);
                }
                ofCircle(boardXOffset+x*hexW+offset,boardYOffset+y*hexH,sideLen/2);
            } else {
                //This is an unoccupied space, but we might need to draw some
                // highlights...
                if(currentAction == 1){
                    //If the user is trying to place a new piece,
                    // higlight the space if it is valid to place the piece here
                    ofSetColor(64,192,64); //green highlight
                    
                    if(canPlaceNewPiece(x, y)){
                        ofCircle(boardXOffset+x*hexW+offset,boardYOffset+y*hexH,sideLen/2);
                    }
                } else if(currentAction == 2){
                    //If the user is trying to move an old piece,
                    // higlight the space if it is valid to place the piece here
                    ofSetColor(64,192,64); //green
                    
                    if(canPlaceOldPiece(x, y)){
                        ofCircle(boardXOffset+x*hexW+offset,boardYOffset+y*hexH,sideLen/2);
                    }
                }
            }
        }
    }
    
    if(currentAction == 1){
        //If placing a new piece, draw the piece that the user is placing
        // at the mouse location
        if(whoseTurn == 1){
            ofSetColor(255,255,255);
        } else {
            ofSetColor(0,0,0);
        }
        
        ofCircle(ofGetMouseX(),ofGetMouseY(),sideLen/2);
    } else if(currentAction == 2){
        //If moving an old piece...
        
        //...show where it is being moved FROM
        ofSetColor(64,64,192); //blue
        float offset = (hexW/2) * (selectedPieceY%2);
        ofCircle(boardXOffset+selectedPieceX*hexW+offset,boardYOffset+selectedPieceY*hexH,sideLen/2);
        
        //...and also show the piece in the player's "hand" being moved
        if(whoseTurn == 1){
            ofSetColor(255,255,255);
        } else {
            ofSetColor(0,0,0);
        }
        ofCircle(ofGetMouseX(),ofGetMouseY(),sideLen/2);
    }
    
}

/*
 * Draw the store of remaining pieces on the right side of the screen
 */
void drawSpares(){
    float xOffset = boardXOffset + (1+boardW)*hexW;
    
    ofSetColor(255, 255, 255);
    for(int i=0;i<pl1spares;i++){
        ofCircle(xOffset + i*2*sideLen,2*sideLen,sideLen/2);
    }
    
    ofSetColor(0, 0, 0);
    for(int i=0;i<pl2spares;i++){
        ofCircle(xOffset + i*2*sideLen,3.5*sideLen,sideLen/2);
    }
}

//--------------------------------------------------------------
void drawMenu(){
	//draw my menu here :)
	ofSetColor(255,255,255);

	ofDrawBitmapString("Play Vs. Human", boardXOffset, boardYOffset, 0);
	ofDrawBitmapString("Play Vs. Computer", boardXOffset, boardYOffset+20, 0);
	ofDrawBitmapString("Resume", boardXOffset, boardYOffset+40, 0);
	ofDrawBitmapString("Exit", boardXOffset, boardYOffset+60, 0);

}

//--------------------------------------------------------------
void drawWinner(){
	//draw my winner here :)
	ofSetColor(255,255,255);
	ofDrawBitmapString(("Winner was player " + static_cast<ostringstream*>( &(ostringstream() << winner) )->str() + "!"), boardXOffset+(boardW/2*hexW), boardYOffset+(boardH/2*hexH), 0);
	
	(winner == 1)? ofSetColor(255,255,255) : ofSetColor(0,0,0);
	
	ofCircle(boardXOffset+(boardW/2*hexW)+180,  boardYOffset+(boardH/2*hexH)- sideLen/2/2, sideLen/2);

}

void drawCredits(){
	//Draw credits Here :)
	ofSetColor(255, 255, 255);
	ofDrawBitmapString("Creator: Garrett Emrick", boardXOffset, boardYOffset+ 600, 0);
	ofDrawBitmapString("Instructor: Dr. Brinkman", boardXOffset, boardYOffset+620, 0);
	ofDrawBitmapString("CSE 274 VISE", boardXOffset, boardYOffset+640, 0);
}

void drawGUI(){
	//Draw game gui here!
	 float xOffset = boardXOffset + (1+boardW)*hexW;

	 //Draw the Current Turn, plus the players color :)
	 int color = (whoseTurn == 1)? 255 : 0;
	 
	 ofSetColor(255, 255, 255);
	 ofDrawBitmapString("It is your turn : ", xOffset-sideLen/2, boardYOffset+70, 0);

	 ofSetColor(color, color, color);
	 ofCircle(xOffset+160, boardYOffset+70-sideLen/2/2, sideLen/2);


	 //Draw Player Records Here :)
	 ofSetColor(255, 255, 255); //Player 1
	 ofDrawBitmapString("Total Wins For Player : ", xOffset-sideLen/2, boardYOffset+110,0);
	 ofCircle(xOffset+200, boardYOffset+110-sideLen/2/2, sideLen/2);
	 ofDrawBitmapString((" - " + static_cast<ostringstream*>( &(ostringstream() << pl1Wins) )->str()), xOffset+210, boardYOffset+110, 0);

	 ofSetColor(0, 0, 0);       //Player 2
	 ofDrawBitmapString("Total Wins For Player : ", xOffset-sideLen/2, boardYOffset+130,0);
	 ofCircle(xOffset+200, boardYOffset+130-sideLen/2/2, sideLen/2);
	 ofDrawBitmapString((" - " + static_cast<ostringstream*>( &(ostringstream() << pl2Wins) )->str()), xOffset+210, boardYOffset+130, 0);
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(128,128,128); //gray
	if(inGame){
		drawBoard();
		drawSpares();
		drawGUI();
	}else if(!inGame){
		drawMenu();
		drawCredits();
	}

	if(isWinner){
		drawWinner();
	}
}

/*
 * Put a piece on the board at position (x,y). 
 * If whichPieces is 0, then it clears that board position.
 */
void putPieceAt(int x, int y, int whichPiece){
	board[x+y*boardH] = whichPiece;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if(inGame){
		if(x >= boardXOffset + (boardW)*hexW){
			//We are clicking the right-hand side of the screen, so we are
			// picking up or putting back a piece
			if(whoseTurn == 1 && pl1spares > 0 && currentAction == 0){
				currentAction = 1;
				pl1spares--;
				PlaySound("..//sounds//pickup.wav", NULL, SND_ASYNC);
			} else if(whoseTurn == 2 && pl2spares > 0 && currentAction == 0 && !playingAI){
				currentAction = 1;
				pl2spares--;
				PlaySound("..//sounds//pickup.wav", NULL, SND_ASYNC);
			} else if (whoseTurn == 1 && currentAction == 1){
				currentAction = 0;
				pl1spares++;
				PlaySound("..//sounds//pickup.wav", NULL, SND_ASYNC);
			} else if (whoseTurn == 2 && currentAction == 1 && !playingAI){
				currentAction = 0;
				pl2spares++;
				PlaySound("..//sounds//pickup.wav", NULL, SND_ASYNC);
			}else{
				PlaySound("..//sounds//error.wav", NULL, SND_ASYNC);
			}

		

		} else if(x > boardXOffset && x <= boardXOffset +(boardW)*hexW && (!playingAI || whoseTurn == 1)) {
			//We are clicking on the board...
			if(currentAction == 1){
				//...placing a new piece
				int whichRow = (y-boardYOffset+hexH/2)/hexH;
				int whichCol = (x-(boardXOffset+(whichRow%2)*(hexW/2))+hexW/2)/hexW;
				if(whichRow >= 0 && whichRow < boardH && whichCol >= 0 && whichCol < boardW){
					if(canPlaceNewPiece(whichCol,whichRow)){
						currentAction = 0;
						putPieceAt(whichCol,whichRow,whoseTurn);
						whoseTurn = 3 - whoseTurn;

						if(whoseTurn == 1){
						PlaySound("..//sounds//pl1_place.wav", NULL, SND_ASYNC);
						}else if(!playingAI){
						PlaySound("..//sounds//pl2_place.wav", NULL, SND_ASYNC);
						}

					}else{
						PlaySound("..//sounds//error.wav", NULL, SND_ASYNC);
					}
				}
			

			} else if(currentAction == 0){
				//...picking up and old piece
				int whichRow = (y-boardYOffset+hexH/2)/hexH;
				int whichCol = (x-(boardXOffset+(whichRow%2)*(hexW/2))+hexW/2)/hexW;
            
				if(pieceAt(whichCol,whichRow) == whoseTurn){
					selectedPieceX = whichCol;
					selectedPieceY  = whichRow;
					currentAction = 2;
					putPieceAt(whichCol,whichRow,0);
				}
			} else if(currentAction == 2){
				//...placing an old piece back on the board
				int whichRow = (y-boardYOffset+hexH/2)/hexH;
				int whichCol = (x-(boardXOffset+(whichRow%2)*(hexW/2))+hexW/2)/hexW;
				if(whichRow == selectedPieceY && whichCol == selectedPieceX){
					currentAction = 0;
					putPieceAt(whichCol,whichRow,whoseTurn);
				} else if(whichRow >= 0 && whichRow < boardH && whichCol >= 0 && whichCol < boardW){
					if(canPlaceOldPiece(whichCol, whichRow)){
						currentAction = 0;
						putPieceAt(whichCol,whichRow,whoseTurn);
						whoseTurn = 3 - whoseTurn;

						if(whoseTurn == 1){
						PlaySound("..//sounds//pl1_place.wav", NULL, SND_ASYNC);
						}else{
						PlaySound("..//sounds//pl2_place.wav", NULL, SND_ASYNC);
						}

					}else{
						PlaySound("..//sounds//error.wav", NULL, SND_ASYNC);
					}
				}
			}
		}
	}else if(!inGame){

		//Play vs. Human
		if(x >= boardXOffset && x <= boardXOffset + 115 && y >= boardYOffset-10 && y < boardYOffset){
			resetGame();
			inGame = true;
		
		//Play vs. Computer
		}else if(x >= boardXOffset && x <= boardXOffset + 140 && y >= boardYOffset+10 && y < boardYOffset+20){
			resetGame();
			playingAI = true;
			inGame = true;
			aiDestroyed = 0;

		//Resume
		}else if(x >= boardXOffset && x <= boardXOffset + 60 && y >= boardYOffset +30 && y < boardYOffset+40){
			if(!isWinner){
				inGame = true;
			}else{
				resetGame();
				inGame = true;
			}

		//Quit
		}else if(x >= boardXOffset && x <= boardXOffset + 50 && y >= boardYOffset + 50 && y < boardYOffset+60){
			std::exit(0);
		}

	}
}

void testApp::keyReleased(int key){
	//ESC == 27 p == 112 r == 114
	if(key == 27 || key == 112){
		inGame = false;
	}
	if(inGame && key == 114){
		resetGame();
	}
}





//HERE IS MY AI CODE

void makeAIMove(){
	//AI will always be player 2 :)

	int num = rand()%2;

	if(pl2spares + aiDestroyed > 2 || (num == 0 && pl2spares > 0)){ // Play a new piece
		std::vector<pair<int,int>> moves = std::vector<pair<int,int>>();
		for(int x = 0; x < boardW; x++){
			for(int y = 0; y < boardH; y++){
				if(canPlaceNewPiece(x, y)) moves.push_back(pair<int, int>(x, y));
			}
		}

		int r = rand()%moves.size();

		int x = moves[r].first;
		int y = moves[r].second;

		if(pl2spares > 0){//In case something weird happened -.-
			board[x + y*boardH] = 2;
			pl2spares--;
			whoseTurn = 1;
			currentAction = 0;
			//PlaySound("..//sounds//pl2_place.wav", NULL, SND_ASYNC);

			return;
		}

	}

	//Place an old piece!

	std::vector<pair<pair<int,int>, pair<int,int>>> moves = std::vector<pair<pair<int,int>,pair<int,int>>>();

	std::vector<pair<int,int>> oldPieces = std::vector<pair<int,int>>();
	for(int x = 0; x < boardW; x++){
		for(int y = 0; y < boardH; y++){
			if(board[x+y*boardH] == 2) oldPieces.push_back(pair<int,int>(x,y));
		}
	}

	for(std::vector<pair<int,int>>::iterator it = oldPieces.begin(); it != oldPieces.end(); it++){
		selectedPieceX = (*it).first;
		selectedPieceY = (*it).second;
		for(int x = 0; x < boardW; x++){
			for(int y = 0; y < boardH; y++){
				if(canPlaceOldPiece(x,y)){
					moves.push_back(pair<pair<int,int>,pair<int,int>>(pair<int,int>(selectedPieceX,selectedPieceY), pair<int,int>(x, y)));
				}
			}
		}

	}

	int r = rand()%moves.size();

	int selectedX = moves[r].first.first;
	int selectedY = moves[r].first.second;

	int x = moves[r].second.first;
	int y = moves[r].second.second;

	board[x + y*boardH] = 2;
	board[selectedX + selectedY*boardH] = 0;
	currentAction = 0;
	whoseTurn = 1;
	//PlaySound("..//sounds//pl2_place.wav", NULL, SND_ASYNC);
}