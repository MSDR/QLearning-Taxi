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

class Taxi {
public:
	Taxi(const int &x = 1, const int &y = 1) : x_(x), y_(y) {}
	Taxi(const intMatrix &board, const int &x = 1, const int &y = 1);

	const int getQState() const;

	int x_, y_, destX_, destY_;
	bool pickUpMode_; //when true, looking to pick up a passenger. when false, looking to drop off a passenger
private:
	intMatrix board_;
	doubleMatrix qTable_;
		//states: N,S,E,W,C[urrent],destDistX,destDistY
		//actions: N,S,E,W,pickUp,dropOff

	void getNewDest();
};
#endif
