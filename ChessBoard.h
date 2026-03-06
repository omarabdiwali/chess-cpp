#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>

using Move = std::tuple<int, int, int, int>;

enum Piece {
    EMPTY = 0,
    W_PAWN = 1, W_KNIGHT = 2, W_BISHOP = 3, W_ROOK = 4, W_QUEEN = 5, W_KING = 6,
    B_PAWN = 9, B_KNIGHT = 10, B_BISHOP = 11, B_ROOK = 12, B_QUEEN = 13, B_KING = 14
};

class ChessBoard {
public:
    ChessBoard(std::string fen);
    ~ChessBoard();

    void getPositionsFromFen(std::string& fen);
    std::string getFenFromPositions();
    int getPromotion(Move move);

    std::vector<int> generateMoves(int pos);
    std::string translateToSquareNames(int pos, bool isEnPassant = false);
    static int translateToPos(std::string squareNames);

    Move makeMoveObj(int from, int to);
    void makeMove(Move move, bool simulated = false);

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
    bool checkValidMove(Move move);
    void checkEnPassant(Move move);
    void checkCastling(Move move);
    bool checkPromotion(Move move);

    std::vector<int> knightMovement(int pos);
    std::vector<int> pawnMovement(int pos);
    std::vector<int> kingMovement(int pos);
    std::vector<int> diagonalAndOrthogonalMovement(std::string piece, int pos);

private:
    std::vector<int> positions;
    std::map<int, int> enPassantCtx;
    std::tuple<bool, bool> whiteCastle;
    std::tuple<bool, bool> blackCastle;
    char currentTurn;
    int whiteKingPos;
    int blackKingPos;
    int halfMoveClock = 0;
    int fullMoveNumber = 1;
};
