#include "ChessBoard.h"
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

int getNumberFromUser() {
    string input;
    getline(cin, input);
    int number = ChessBoard::translateToPos(input);
    return number;
}

int main() {
    string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    ChessBoard board(fen);
    board.printBoard(-1, -1);
    bool gameEnd = false;

    while (!gameEnd) {
        int posToMove;
        cout << endl;
        board.printTurn();
        posToMove = getNumberFromUser();
        if (posToMove == -1) break;

        if (posToMove == -3) {
            cout << "Invalid board position (a1 <= pos <= h8)." << endl;
            continue;
        }

        tuple<bool, vector<int>> result = board.printPossibleMovesBoard(posToMove);
        bool validPos = get<0>(result); vector<int> moves = get<1>(result);
        if (validPos) cout << endl;

        while (validPos && true) {
            int nextMove;
            cout << "Select destination, or select a different piece (ex. a4): ";
            nextMove = getNumberFromUser();

            if (nextMove == -3) {
                cout << "Invalid board position (a1 <= pos <= h8)." << endl;
                continue;
            }
            
            if (nextMove == -1) { gameEnd = true; break; };
            if (find(moves.begin(), moves.end(), nextMove) != moves.end()) {
                cout << endl;
                Move moveObj = board.makeMoveObj(posToMove, nextMove);
                board.makeMove(moveObj);
                board.printBoard(posToMove, nextMove);
                if (board.isCheckmate()) {
                    cout << (board.getCurrentTurn() == 'w' ? "Black Wins!" : "White Wins!") << endl;
                    gameEnd = true;
                }
                else if (board.isStalemate()) {
                    cout << "It's a draw!" << endl;
                    gameEnd = true;
                }
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