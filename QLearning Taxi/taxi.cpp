#include "taxi.h"
Taxi::Taxi(const intMatrix &board, const int &x, const int &y) :
	x_(x), y_(y), board_(board), pickUpMode_(false) {
	//check board is large enough for the game to be played
	if (board_.size() < 4 || board_[0].size() < 4) {
		throw "Board size too small.";
		return;
	}

	//initialize qTable to 0's
	int numStates = 2*2*2*2*2 * board_.size()-3 * board_[0].size()-3; //board_.size()-3 is because the walls take up 2 spaces, distance of 0 being possible takes up 1
	int numActions = 6;
	for (int state = 0; state < numStates; ++state) {
		qTable_.push_back(std::vector<double>());
		for (int action = 0; action < numActions; ++action) {
			qTable_[state].push_back(0.0);
		}
	}

	getNewDest();
}

const int Taxi::getQState() const {
	int N =                                     board_[y_-1][x_];
	int S =                                   2*board_[y_+1][x_];
	int E =                                 2*2*board_[y_][x_-1];
	int W =                               2*2*2*board_[y_][x_+1];
	int C =                             2*2*2*2*board_[y_][x_];
	int distDestX =                   2*2*2*2*2*(destX_ - x_);
	int distDestY = 2*2*2*2*2*(board_.size()-3)*(destY_ - y_);

	return N+S+E+W+C+distDestX+distDestY;
}

void Taxi::getNewDest() {
	pickUpMode_ = !pickUpMode_;
	//establish first pickup
	srand(time(NULL));
	bool validSpot = false;
	do {
		destX_ = rand()%(board_.size()-3)+1;
		destY_ = rand()%(board_.size()-3)+1;
		if (board_[destY_][destX_] != EMPTY)
			validSpot = false;
		else if (board_[destY_-1][destX_] == BUILDING || //spot must be adjacent to a building
			board_[destY_+1][destX_] == BUILDING ||
			board_[destY_][destX_-1] == BUILDING ||
			board_[destY_][destX_+1] == BUILDING)
			validSpot = true;
	} while (!validSpot);
}


