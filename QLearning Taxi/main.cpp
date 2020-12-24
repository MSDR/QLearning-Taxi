#include <algorithm>
#include <cmath>
#include <fstream>
#include "taxi.h"

void initBoard(const std::string &filename, intMatrix &board, std::pair<int, int> &taxiStart) {
	std::ifstream reader;
	reader.open(filename);
	if (!reader.good())
		throw "Invalid filename.\n";

	bool taxiStartRead = false;
	std::string line = "";
	int row = -1;
	while (std::getline(reader, line)) {
		if(row == -1 && line.length() == 3) { //Read taxi start coordinates
			if (line.at(0) < 48 || line.at(0) > 57 || line.at(2) < 48 || line.at(2) > 57)
				throw "Taxi start coordinates not given or are invalid.1";
			taxiStartRead = true;
			taxiStart = std::make_pair(line.at(0)-48, line.at(2)-48);
		} else { //Read board setup
			board.push_back(std::vector<int>()); 
			for (std::string::iterator lineItr = line.begin(); lineItr != line.end(); lineItr++) {
				board[row].push_back(int((*lineItr) - 48));
			}
		}
		row++;
	}
	reader.close();
	if(!taxiStartRead)
		throw "Taxi start coordinates not given or are invalid.";
}

void printBoard(const intMatrix &board, const Taxi &taxi, const bool &printQValues) {
	std::cout << std::endl;
	for (int i = 0; i < board.size(); ++i) {
		std::cout << "|";
		for(int j = 0; j < board[i].size(); ++j) {
			char outChar = ' ';
			if (taxi.destX_ == j && taxi.destY_ == i) {
				outChar = taxi.pickUpMode_ ? 'p' : 'd';
				if (taxi.x_ == j && taxi.y_ == i) outChar -= 32;

			}	else if (taxi.x_ == j && taxi.y_ == i)
				outChar = 'T';
			else if (board[i][j] == BUILDING)
				outChar = '#';
				
			std::cout << outChar << (j == board[i].size() - 1 ? "" : " ");
		}
		std::cout << "|" << (i == board.size()-1 ? "" : "\n");
	}
	std::cout << std::endl;

	if (printQValues) taxi.printQValues();
}

void timeStep(const intMatrix &board, Taxi &taxi, const double &randomActionChance, const double &learnRate, const double &discountFactor, const bool &printInfo = false) {
	if (randomActionChance < 0.0 || randomActionChance > 1.0) throw "randomActionChance out of range. make sure 0 <= randomActionChance <= 1.\n";
	if (learnRate < 0.0 || learnRate > 1.0) throw "learnRate out of range. make sure 0 <= learnRate <= 1.\n";
	if (discountFactor < 0.0 || discountFactor > 1.0) throw "discountFactor out of range. make sure 0 <= discountFactor <= 1.\n";

	//get qState of taxi
	int qState = taxi.getQState();

	//select action in qState -- either random or max(qValue at qState) -- and move the taxi accordingly
	std::vector<double>::iterator maxQValueAction = std::max_element(taxi.qTable_[qState].begin(), taxi.qTable_[qState].end());
	int action; 
	if((rand() % 100) / 100.0 < randomActionChance) {
		action = rand()%taxi.numActions; //chooses random action
		std::cout << "Chose action randomly.";
	} else {
		action = std::distance(taxi.qTable_[qState].begin(), maxQValueAction); //chooses action with max qValue
		std::cout << "Chose action selectively.";
	}
	std::cout << " Action chosen: " << Actions(action) << std::endl << std::endl; 

	if (printInfo) {
		std::cout << "Current state qValues:\n";
		taxi.printQValues();
	}

	bool crashed = false; //used for nextMaxQValue calculation

	//calculate reward
	int reward = MOVE; //the default
	if ((action == MOVE_N && board[taxi.y_ - 1][taxi.x_] == BUILDING) ||
		 (action == MOVE_S && board[taxi.y_ + 1][taxi.x_] == BUILDING) ||
		 (action == MOVE_E && board[taxi.y_][taxi.x_ + 1] == BUILDING) ||
		 (action == MOVE_W && board[taxi.y_][taxi.x_ - 1] == BUILDING)) {
		reward += CRASH;
		crashed = true;
		if (printInfo) std::cout << "Crashed! Reward: " << CRASH << std::endl;
	}
	if (taxi.x_ == taxi.destX_ && taxi.y_ == taxi.destY_ && action != (taxi.pickUpMode_ ? PICK_UP : DROP_OFF)) {
		reward += MISSED_PASSENGER_ACTION;
		if (printInfo) std::cout << "Missed a passenger action. Reward: " << MISSED_PASSENGER_ACTION << std::endl;
	} 
	if ((taxi.x_ != taxi.destX_ && taxi.y_ != taxi.destY_ && (action == PICK_UP || action == DROP_OFF)) || //pickup/drop off at wrong location
		 (taxi.x_ == taxi.destX_ && taxi.y_ == taxi.destY_ && action == (taxi.pickUpMode_ ? DROP_OFF : PICK_UP))) { //executed wrong passenger action, right location
		reward += INCORRECT_PASSENGER_ACTION;
		if (printInfo) std::cout << "Incorrect passenger action. Reward: " << INCORRECT_PASSENGER_ACTION << std::endl;
	}
	if (taxi.x_ == taxi.destX_ && taxi.y_ == taxi.destY_ && action == (taxi.pickUpMode_ ? PICK_UP : DROP_OFF)) {
		reward = CORRECT_PASSENGER_ACTION; //note that this overrides any other mistakes on that turn, maybe reconsider if more features added later
		if (printInfo) std::cout << "Correct passenger action! Reward: " << CORRECT_PASSENGER_ACTION << std::endl;
	}
	if (printInfo) std::cout << "Total reward: " << reward << std::endl << std::endl;

	//update qValue of previous state using qValue of current state
	double nextMaxQValue = 0.0;
	if (crashed) //then taxi's next state is identical
		nextMaxQValue = *maxQValueAction;
	else { //taxi's next state is changed, so we have to recalculate
		taxi.executeAction(action);
		int nextQState = taxi.getQState();
		nextMaxQValue = *std::max_element(taxi.qTable_[nextQState].begin(), taxi.qTable_[nextQState].end());
	}
	//Q(state, action) = (1-learnRate)+learnRate(reward+discountFactor*max(Q(next state, all actions))) 
	taxi.qTable_[qState][action] = ((1.0-learnRate)*taxi.qTable_[qState][action] + (learnRate*((double)reward + discountFactor*nextMaxQValue)));
	if (printInfo) {
		std::cout << "Updated qValues:\n";
		taxi.printQValues(qState);
		std::cout << "________________________________\n";
	}
}

int main(int argc, char* argv[]) {
	Taxi taxi;
	intMatrix board;
	std::string boardFilename = argc > 1 ? argv[1] : "board.txt";
	std::cout << "File: " << boardFilename << "\n";
	std::pair<int, int> taxiStart;

	srand(time(NULL));

	try {
		initBoard(boardFilename, board, taxiStart);
		taxi = Taxi(board);
	} catch (const char* errorMsg) {
		std::cerr << errorMsg << std::endl;
		return(1);
	}
	printBoard(board, taxi, true);
	
	for (int epoch = 0; epoch < 1000000; ++epoch) {
		try {
			printBoard(board, taxi, false); 
			timeStep(board, taxi, 0.1, 0.5, 0.5, false);
		} catch (const char* errorMsg) {
			std::cerr << errorMsg << std::endl;
		}
	}

}