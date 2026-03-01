#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>

enum Piece {
    EMPTY = 0,
    W_PAWN = 1, W_KNIGHT = 2, W_BISHOP = 3, W_ROOK = 4, W_QUEEN = 5, W_KING = 6,
    B_PAWN = 9, B_KNIGHT = 10, B_BISHOP = 11, B_ROOK = 12, B_QUEEN = 13, B_KING = 14
};

const int pieceValues[] = {
    0, 100, 300, 320, 500, 900, 0, 0,
    0, 100, 300, 320, 500, 900, 0
};

const int pawnScores[] = {
    90, 90, 90, 90, 90, 90, 90, 90,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0
};

const int knightScores[] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

const int bishopScores[] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
};

const int rookScores[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, 10, 10, 10, 10, 5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 5, 5, 0, 0, 0
};

const int queenScores[] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 5, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20
};

const int kingScores[] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, 0, 0, 0, 0, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20
};

class ChessBoard {
public:
    ChessBoard();
    ChessBoard(std::vector<int> pos);
    ChessBoard(std::string fen);
    ~ChessBoard();

    void updatePositions(std::vector<int> positions);
    void getPositionsFromFen(std::string& fen);
    
    std::vector<int> generateMoves(int pos);
    std::vector<std::tuple<int, int, int, int>> getColorMoves(char color);
    std::string getMoveData(std::tuple<int, int, int, int> move);

    std::tuple<int, int, int, int> makeMoveObj(int from, int to);
    void makeRandomMove(std::string turn);
    void makeMove(int from, int to, bool printMove=false);
    void makeMove(std::tuple<int, int, int, int> move, bool printMove = false);
    void unmakeMove(int from, int to, int movedPiece, int capturedPiece);
    void unmakeMove(std::tuple<int, int, int, int> move);
    void printMoveData(std::tuple<int, int, int, int> move);
    
    void printPositions();
    void printBoard();
    void printTurn();
    void printBoard(int from, int to);
    std::tuple<bool, std::vector<int>> printPossibleMovesBoard(int pos);
    
    bool checkValidMove(char color, int pos);
    int getPiece(int pos);
    int getPosFromPiece(int piece);
    double evaluateBoard(char color);
    bool isInCheck(char color);
    bool isCheckmate();
    bool checkPawn(int targetPos, int kingPos, int enemyColorBit);
    bool checkValidMove(std::tuple<int, int, int, int> move);
    void checkEnPassant(std::tuple<int, int, int, int> move);
    void checkCastling(std::tuple<int, int, int, int> move);

    std::vector<int> getPositions();
    std::map<int, int> getLastPositions();
    bool getCurrentTurn();
    std::vector<std::tuple<int, int, int, int>> getGenerationOnlyCaptures(char color);
    std::vector<std::tuple<int, int, int, int>> getColorMoves(std::vector<int> positions, char color);
    
    std::vector<int> knightMovement(int pos);
    std::vector<int> pawnMovement(int pos);
    std::vector<int> kingMovement(int pos);
    std::vector<int> queenMovement(int pos);
    std::vector<int> rookMovement(int pos);
    std::vector<int> bishopMovement(int pos);

private:
    std::vector<int> positions;
    std::map<int, int> lastPositions;
    char currentTurn;
    int whiteKingPos;
    int blackKingPos;
    std::tuple<bool, bool> whiteCastle = std::make_tuple(true, true);
    std::tuple<bool, bool> blackCastle = std::make_tuple(true, true);
};

template<typename T>
T getRandomMove(std::vector<T> values);