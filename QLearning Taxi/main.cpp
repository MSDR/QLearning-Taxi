#include <fstream>
#include <iostream>
#include <string>
#include <vector>
typedef std::vector<std::vector<int>> intMatrix;

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

void printBoard(const intMatrix &board) {
	for (int i = 0; i < board.size(); ++i) {
		std::cout << "|";
		for(int k = 0; k < board[i].size(); ++k) {
			std::cout << board[i][k] << (k == board[i].size() - 1 ? "" : " ");
		}
		std::cout << "|" << (i == board.size()-1 ? "" : "\n");
	}
}

int main(int argc, char* argv[]) {
	intMatrix board;
	std::string boardFilename = argc > 1 ? argv[1] : "board.txt";
	std::cout << "File: " << boardFilename << "\n";
	std::pair<int, int> taxiStart;

	try {
		initBoard(boardFilename, board, taxiStart);
	} catch (const char* errorMsg) {
		std::cerr << errorMsg;
		return(1);
	}
	printBoard(board);
	
}