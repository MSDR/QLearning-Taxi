#ifndef TAXI_H_
#define TAXI_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <time.h>
#include <vector>

typedef std::vector<std::vector<int>> intMatrix;
typedef std::vector<std::vector<double>> doubleMatrix;

enum TileStates {
	EMPTY=1, BUILDING
};

enum Actions {
	MOVE_N, MOVE_S, MOVE_E, MOVE_W, PICK_UP, DROP_OFF
};
std::ostream& operator<<(std::ostream& out, Actions a);

enum Rewards {
	MOVE = -1, CRASH = -10, CORRECT_PASSENGER_ACTION = 20, INCORRECT_PASSENGER_ACTION = -20, MISSED_PASSENGER_ACTION = -5
};

class Taxi {
public:
	Taxi(const int &x = 1, const int &y = 1) : x_(x), y_(y) {} //don't ever use this, it's really just here to satisfy the compiler.
	Taxi(const intMatrix &board, const int &x = 1, const int &y = 1);

	int getQState() const;
	void printQValues(int qState = -1) const;

	void executeAction(const int &action);

	int x_, y_, destX_, destY_;
	bool pickUpMode_; //when true, looking to pick up a passenger. when false, looking to drop off a passenger
	int numStates, numActions, numTileStates; //actions is 0-based, states are not
	doubleMatrix qTable_;
	//states: N,S,E,W,C[urrent],destDistX,destDistY
	//actions: N,S,E,W,pickUp,dropOff
private:
	intMatrix board_;

	void getNewDest();
};
#endif

