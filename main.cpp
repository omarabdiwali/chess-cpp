#include "ChessBoard.h"
#include <iostream>
#include <string>

using namespace std;

template <typename T>
void printVector(vector<T> vec) {
    cout << "[";
    for (const auto& item: vec) {
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
    string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    ChessBoard board(fen);
    board.printBoard();
    
    while (true) {
        int posToMove;
        cout << endl;
        board.printTurn();
        posToMove = getNumberFromUser();
        if (posToMove == -1) break;

        cout << endl;
        tuple<bool, vector<int>> result = board.printPossibleMovesBoard(posToMove);
        bool validPos = get<0>(result); vector<int> moves = get<1>(result);

        if (validPos) {
            int nextMove;
            cout << endl << "Select destination, or -2 to select a different piece: ";
            nextMove = getNumberFromUser();

            if (nextMove == -2) continue;
            if (nextMove == -1) break;

            while ((find(moves.begin(), moves.end(), nextMove) == moves.end()) && nextMove != -2 && nextMove != -1) {
                cout << "Invalid move! ";
                cout << "Select destination, or -2 to select a different piece: ";
                nextMove = getNumberFromUser();
            }

            cout << endl;
            if (nextMove == -2) continue;
            if (nextMove == -1) break;

            tuple<int, int, int, int> moveObj = board.makeMoveObj(posToMove, nextMove);
            board.makeMove(moveObj);
            board.printBoard(posToMove, nextMove);
            cout << endl;

            if (board.isCheckmate()) break;
        }
    }
    
    return 0;
}