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

class ChessBoard {
public:
    ChessBoard();
    ChessBoard(std::vector<int> pos);
    ChessBoard(std::string fen);
    ~ChessBoard();

    void getPositionsFromFen(std::string& fen);

    std::vector<int> generateMoves(int pos);

    std::tuple<int, int, int, int> makeMoveObj(int from, int to);
    void makeMove(std::tuple<int, int, int, int> move, bool simulated = false);

    void printBoard();
    void printTurn();
    void printBoard(int from, int to);
    std::tuple<bool, std::vector<int>> printPossibleMovesBoard(int pos);

    bool checkValidMove(char color, int pos);
    int getPiece(int pos);
    int getPosFromPiece(int piece);
    bool isInCheck(char color);
    bool isCheckmate();
    bool checkPawn(int targetPos, int kingPos, int enemyColorBit);
    bool checkValidMove(std::tuple<int, int, int, int> move);
    void checkEnPassant(std::tuple<int, int, int, int> move);
    void checkCastling(std::tuple<int, int, int, int> move);
    bool checkPromotion(std::tuple<int, int, int, int> move);

    int getPromotion(std::tuple<int, int, int, int> move);

    std::vector<int> knightMovement(int pos);
    std::vector<int> pawnMovement(int pos);
    std::vector<int> kingMovement(int pos);
    std::vector<int> diagonalAndOrthogonalMovement(std::string piece, int pos);

private:
    std::vector<int> positions;
    std::map<int, int> enPassantCtx;
    char currentTurn;
    int whiteKingPos;
    int blackKingPos;
    std::tuple<bool, bool> whiteCastle;
    std::tuple<bool, bool> blackCastle;
};
