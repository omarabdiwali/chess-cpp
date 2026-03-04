#include "ChessBoard.h"
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

template <typename T>
void printVector(vector<T> vec) {
    cout << "[";
    for (const auto& item : vec) {
        if (item == vec.at(0)) {
            cout << item;
        }
        else {
            cout << ", " << item;
        }
    }
    cout << "]";
}

int getNumberFromUser() {
    string input;
    int number;
    bool isValid = false;
    getline(cin, input);

    try {
        number = stoi(input);
        isValid = true;
    }
    catch (const exception& e) {
        isValid = false;
    }

    return isValid ? number : -3;
}

int main() {
    string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk";
    ChessBoard board(fen);
    board.printBoard();
    bool gameEnd = false;

    while (!gameEnd) {
        int posToMove;
        cout << endl;
        board.printTurn();
        posToMove = getNumberFromUser();
        if (posToMove == -1) break;

        cout << endl;

        tuple<bool, vector<int>> result = board.printPossibleMovesBoard(posToMove);
        bool validPos = get<0>(result); vector<int> moves = get<1>(result);
        if (validPos) cout << endl;

        while (validPos && true) {
            int nextMove;
            cout << "Select destination, or select a different piece: ";
            nextMove = getNumberFromUser();

            if (nextMove == -1) { gameEnd = true; break; };
            if (find(moves.begin(), moves.end(), nextMove) != moves.end()) {
                cout << endl;
                tuple<int, int, int, int> moveObj = board.makeMoveObj(posToMove, nextMove);
                board.makeMove(moveObj);
                board.printBoard(posToMove, nextMove);
                cout << endl;
                if (board.isCheckmate()) { gameEnd = true; }
                break;
            }

            tuple<bool, vector<int>> testResult = board.printPossibleMovesBoard(nextMove);
            if (get<0>(testResult)) {
                validPos = get<0>(testResult); moves = get<1>(testResult);
                posToMove = nextMove;
                cout << endl;
            }
        }
    }
    return 0;
}