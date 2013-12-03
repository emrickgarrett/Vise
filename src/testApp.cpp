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

//Drawing functions
void drawHex(float x, float y, float sideLen);
void drawBoard();
void drawSpares();

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

//TODO: Make any variables you need for representing your board here

//Number of spare playing pieces left, for each player
int pl1spares=4;
int pl2spares=4;

//Number of destroyed pieces
int destroyedPieces = 0;

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
    
    //TODO: Initialize your "board" data structure here

    //TODO: Put 1 piece for each player in the middle of hte board, side by side
    
    startTime = ofGetElapsedTimef();



	/************************************** TESTS ************************************/

	putPieceAt(4,4,1);
	putPieceAt(4,5,2);

	/***** Test to check if they are connected! *****

	putPieceAt(4,4,1);
	putPieceAt(4,5,2);
	putPieceAt(4,6,1);
	putPieceAt(4,3,2);
	putPieceAt(5,4,1);

	pl1spares = 2;
	pl2spares = 3;

	std::cout << "Is it connected? " << isConnected() << std::endl;

	*/

	/***** Test to see if jump spaces are working! *****
	selectedPieceX = 4;
	selectedPieceY = 3;

	putPieceAt(4, 3, 1);
	putPieceAt(4, 4, 2);
	putPieceAt(4, 5, 2);
	putPieceAt(4, 2, 2);
	putPieceAt(5, 3, 2);

	std::cout << "Space at (4,4): " << isJumpSpace(4, 4) << std::endl;
	std::cout << "Space at (4,5): " << isJumpSpace(4, 5) << std::endl;
	std::cout << "Space at (4,2): " << isJumpSpace(4, 2) << std::endl;
	std::cout << "Space at (5,3): " << isJumpSpace(5, 3) << std::endl;

	*/

	/**** Placing Pieces/ Checking for Neighbors test ****
	putPieceAt(4, 4, 1);
	putPieceAt(4, 3, 1);
	putPieceAt(4, 5, 1);
	putPieceAt(5, 4, 2);
	putPieceAt(5, 3, 2);
	putPieceAt(5, 5, 2);
	putPieceAt(3, 3, 2);
	putPieceAt(3, 4, 1);
	
	int okayNbrs = 0;
	int badNbrs = 0;

	std::vector<std::pair<int, int>> temp = std::vector<std::pair<int, int>>();
	
	getNbrs(4, 5, okayNbrs, badNbrs, temp);

	//Method to test if it successfully gets all the neighbor pairs
	int i = 0;
	for(std::vector<std::pair<int, int>>::iterator it = temp.begin(); it != temp.end(); it++, i++){
		std::cout << "Neighbor #" << i << " is: " << (*it).first << ", " << (*it).second << std::endl;
	}


	std::cout << "Okay Numbers is: " << okayNbrs << "\nBad Numbers is: " << badNbrs << std::endl;
	*/

	/**** Is a Neighbor test ****
	selectedPieceX = 1;
	selectedPieceY = 1;
	std::cout << "Is a Neighbor (1, 0)? " << isNeighboringSpace(1, 0) << std::endl;
	std::cout << "Is a Neighbor (2, 0)? " << isNeighboringSpace(2, 0) << std::endl;
	std::cout << "Is a Neighbor (2, 1)? " << isNeighboringSpace(2, 1) << std::endl;
	std::cout << "Is a Neighbor? (0, 1)" << isNeighboringSpace(0, 1) << std::endl;
	std::cout << "Is a Neighbor? (1, 2)" << isNeighboringSpace(1, 2) << std::endl;
	std::cout << "Is a Neighbor? (2, 2)" << isNeighboringSpace(2, 2) << std::endl;
	std::cout << "Is a Neighbor? (1, 3)" << isNeighboringSpace(1, 3) << std::endl;
	std::cout << "Is a Neighbor? (2, 3)" << isNeighboringSpace(2, 3) << std::endl;
	std::cout << "Is a Neighbor? (0, 3)" << isNeighboringSpace(0, 3) << std::endl;
	*/ 

}

//Return true iff there is a piece in board space (x,y), and that piece
// is caught in a vise. Note that x and y are in board coordinates,
// not screen coordinates
bool inVise(int x, int y){
    //TODO
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
    //TODO
}

//--------------------------------------------------------------
void testApp::update(){
    //This can be helpful if you are doing animations
    currentFrame = (ofGetElapsedTimef() - startTime)/(1.0/60);
    
    //Check for vised pieces on every update
    doVise();
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
void testApp::draw(){
    ofBackground(128,128,128); //gray
    drawBoard();
    drawSpares();
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
    if(x >= boardXOffset + (boardW)*hexW){
        //We are clicking the right-hand side of the screen, so we are
        // picking up or putting back a piece
        if(whoseTurn == 1 && pl1spares > 0 && currentAction == 0){
            currentAction = 1;
            pl1spares--;
        } else if(whoseTurn == 2 && pl2spares > 0 && currentAction == 0){
            currentAction = 1;
            pl2spares--;
        } else if (whoseTurn == 1 && currentAction == 1){
            currentAction = 0;
            pl1spares++;
        } else if (whoseTurn == 2 && currentAction == 1){
            currentAction = 0;
            pl2spares++;
        }
    } else if(x > boardXOffset && x <= boardXOffset +(boardW)*hexW ) {
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
                }
            }
			if(whoseTurn == 1){
				PlaySound("..//sounds//pl1_place.wav", NULL, SND_ASYNC);
			}else{
				PlaySound("..//sounds//pl2_place.wav", NULL, SND_ASYNC);
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
                }
            }
        }
    }
}

