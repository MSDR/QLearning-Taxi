#include "taxi.h"
Taxi::Taxi(const intMatrix &board, const int &x, const int &y) :
	x_(x), y_(y), board_(board), pickUpMode_(false) {
	//check board is large enough for the game to be played
	if (board_.size() < 4 || board_[0].size() < 4) {
		throw "Board size too small.";
		return;
	}

	//initialize qTable to 0's
	numTileStates = 2;
	numStates = std::pow(numTileStates, 5) * board_.size()-2 * board_[0].size()-2; //board_.size()-2 is because the walls take up 2 spaces
	numActions = 6;
	for (int state = 0; state < numStates; ++state) {
		qTable_.push_back(std::vector<double>());
		for (int action = 0; action < numActions; ++action) {
			qTable_[state].push_back(0.0);
		}
	}

	getNewDest();
}

const int Taxi::getQState() const {
	int N =									 std::pow(numTileStates, 0)+board_[y_-1][x_];
	int S =									 std::pow(numTileStates, 1)+board_[y_+1][x_];
	int E =                           std::pow(numTileStates, 2)+board_[y_][x_-1];
	int W =                           std::pow(numTileStates, 3)+board_[y_][x_+1];
	int C =                           std::pow(numTileStates, 4)+board_[y_][x_];
	int distDestX =                   std::pow(numTileStates, 5)+(destX_ - x_);
	int distDestY = (board_.size()-3)*std::pow(numTileStates, 5)+(destY_ - y_);

	return N+S+E+W+C+distDestX+distDestY;
}

void Taxi::executeAction(const int &action) {
	switch (action) {
	case MOVE_N:
		y_--;
		break;
	case MOVE_S:
		y_++;
		break;
	case MOVE_W:
		x_--;
		break;
	case MOVE_E:
		x_++;
		break;
	case PICK_UP:
		getNewDest();
		break;
	case DROP_OFF:
		getNewDest();
		break;
	default:
		throw "Unknown action.";
		break;
	}
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


