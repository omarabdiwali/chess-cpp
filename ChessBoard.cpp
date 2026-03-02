#include "ChessBoard.h"
#include "Windows.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <set>
#include <algorithm>
#include <tuple>
#include <random>
#include <cctype>

using namespace std;

ChessBoard::ChessBoard() : positions({}), blackKingPos(-1), whiteKingPos(-1), currentTurn('w') { }
ChessBoard::ChessBoard(vector<int> a) : positions(a), blackKingPos(getPosFromPiece(B_KING)), whiteKingPos(getPosFromPiece(W_KING)), currentTurn('w') { }
ChessBoard::ChessBoard(string fen) : currentTurn('w') { getPositionsFromFen(fen); }
ChessBoard::~ChessBoard() { }

string createMessage(const string& format) {
    return format;
}

bool verifySameColor(int piece, char color) {
    if (color == 'w') return piece < 8;
    return piece > 8;
}

string translatePiece(int piece) {
    switch (piece)
    {
        case EMPTY:
            return "  ";
        case B_PAWN:
            return "bP";
        case B_BISHOP:
            return "bB";
        case B_ROOK:
            return "bR";
        case B_QUEEN:
            return "bQ";
        case B_KING:
            return "bK";
        case B_KNIGHT:
            return "bN";
        case W_PAWN:
            return "wP";
        case W_BISHOP:
            return "wB";
        case W_ROOK:
            return "wR";
        case W_QUEEN:
            return "wQ";
        case W_KING:
            return "wK";
        case W_KNIGHT:
            return "wN";
        default:
            return "";
    }
}

char getPieceColor(int piece) {
    return piece < 8 ? 'w' : 'b';
}

bool isEmpty(int piece) {
    return piece == EMPTY;
}

bool isCorrectDistance(int pos1, int pos2, int maxStep) {
    int modDiff = abs(pos1 % 8 - pos2 % 8);
    return modDiff <= maxStep;
}

bool isAdjacent(int a, int b) {
    int dx = abs((a & 7) - (b & 7));
    int dy = abs((a >> 3) - (b >> 3));
    return (dx <= 1 && dy <= 1);
}

template<typename T, typename... Args>
string createMessage(const string& format, T value, Args... args) {
    ostringstream oss;
    for (size_t i = 0; i < format.size(); ++i) {
        if (format[i] == '{' && i + 1 < format.size() && format[i + 1] == '}') {
            oss << value;
            return oss.str() + createMessage(format.substr(i + 2), args...);
        }
        oss << format[i];
    }
    return oss.str();
}

void checkForErrors(int piece, string errorMessage) {
    if (piece == -1) throw runtime_error(errorMessage);
}

bool isError(int piece) {
    return piece == -1;
}

vector<int> removeDuplicates(const vector<int>& a, const vector<int>& b) {
    set<int> uniqueElements(a.begin(), a.end());
    uniqueElements.insert(b.begin(), b.end());
    return vector<int>(uniqueElements.begin(), uniqueElements.end());
}

void ChessBoard::updatePositions(vector<int> newPositions) {
    positions = newPositions;
};

bool ChessBoard::checkValidMove(char color, int pos) {
    int piece = getPiece(pos);
    return piece != -1 && (isEmpty(piece) || !verifySameColor(piece, color));
}

int ChessBoard::getPiece(int pos) {
    if (pos >= 0 && pos < positions.size()) {
        return positions[pos];
    }
    return -1;
}

vector<int> ChessBoard::generateMoves(int pos) {
    int piece = getPiece(pos);
    vector<int> totalMoves;
    vector<int> validMoves;
    
    if (piece != EMPTY) {
        if (piece == W_PAWN || piece == B_PAWN) { totalMoves = pawnMovement(pos); }
        else if (piece == W_ROOK || piece == B_ROOK) { totalMoves = rookMovement(pos); }
        else if (piece == W_KNIGHT || piece == B_KNIGHT) { totalMoves = knightMovement(pos); }
        else if (piece == W_BISHOP || piece == B_BISHOP) { totalMoves = bishopMovement(pos); }
        else if (piece == W_QUEEN || piece == B_QUEEN) { totalMoves = queenMovement(pos); }
        else if (piece == W_KING || piece == B_KING) { totalMoves = kingMovement(pos); }

        for (auto& to : totalMoves) {
            int toPiece = getPiece(to);
            if (toPiece != -1) {
                tuple<int, int, int, int> move = make_tuple(piece, toPiece, pos, to);
                if (checkValidMove(move)) validMoves.push_back(to);
            }
        }
    }
    
    return validMoves;
}

vector<int> ChessBoard::knightMovement(int pos) {
    const int movement[] = { -17, -15, -10, -6, 6, 10, 15, 17 };
    int piece = getPiece(pos);
    string errorMessage = createMessage("Knight Movement @ {} - {}", pos, translatePiece(piece));
    checkForErrors(piece, errorMessage);

    char color = piece < 8 ? 'w' : 'b';
    vector<int> moves;

    for (const int diff : movement) {
        int nextPos = pos + diff;
        if (isCorrectDistance(pos, nextPos, 2) && checkValidMove(color, nextPos)) {
            moves.push_back(nextPos);
        }
    }
    
    return moves;
}

vector<int> ChessBoard::kingMovement(int pos) {
    const int movement[] = { -9, -8, -7, -1, 1, 7, 8, 9 };
    int piece = getPiece(pos);
    string errorMessage = createMessage("King Movement @ {} - {}", pos, translatePiece(piece));
    checkForErrors(piece, errorMessage);

    char color = piece < 8 ? 'w' : 'b';
    vector<int> moves;

    for (const int diff : movement) {
        int nextPos = pos + diff;
        if (isCorrectDistance(pos, nextPos, 1) && checkValidMove(color, nextPos)) {
            moves.push_back(nextPos);
        }
    }

    bool leftCastle = color == 'w' ? get<0>(whiteCastle) : get<0>(blackCastle);
    bool rightCastle = color == 'w' ? get<1>(whiteCastle) : get<1>(blackCastle);

    if (leftCastle) {
        bool canCastle = true;
        for (int i = 1; i < 4; i++) {
            int skipOver = pos - i;
            int skipPiece = getPiece(skipOver);
            if (skipPiece != EMPTY) { canCastle = false; break; }
            tuple<int, int, int, int> move = makeMoveObj(pos, skipOver);
            if (!checkValidMove(move)) { canCastle = false; break; }
        }

        if (canCastle) moves.push_back(pos - 2);
    }

    if (rightCastle) {
        bool canCastle = true;
        for (int i = 1; i < 3; i++) {
            int skipOver = pos + i;
            int skipPiece = getPiece(skipOver);
            if (skipPiece != EMPTY) { canCastle = false; break; }
            tuple<int, int, int, int> move = makeMoveObj(pos, skipOver);
            if (!checkValidMove(move)) { canCastle = false; break; }
        }

        if (canCastle) moves.push_back(pos + 2);
    }

    return moves;
}

vector<int> ChessBoard::bishopMovement(int pos) {
    const int movement[] = { -9, -7, 7, 9 };
    int piece = getPiece(pos);
    string errorMessage = createMessage("Bishop Movement @ {} - {}", pos, translatePiece(piece));
    checkForErrors(piece, errorMessage);

    char opColor = piece < 8 ? 'b' : 'w';
    vector<int> moves;

    for (auto& diff : movement) {
        int curPos = pos;
        while (true) {
            curPos += diff;
            if (curPos < 0 || curPos > 63 || !isAdjacent(curPos - diff, curPos)) break;
            int curPiece = getPiece(curPos);
            if (curPiece != EMPTY) {
                if (curPiece == -1) break;
                if (getPieceColor(curPiece) == opColor) moves.push_back(curPos);
                break;
            }
            moves.push_back(curPos);
        }
    }

    return moves;
}

vector<int> ChessBoard::pawnMovement(int pos) {
    int piece = getPiece(pos);
    string errorMessage = createMessage("Pawn Movement @ {} - {}", pos, translatePiece(piece));
    checkForErrors(piece, errorMessage);

    vector<int> moves;
    char color = getPieceColor(piece);
    bool isWhite = color == 'w';
    int dir = isWhite ? -8 : 8;
    int row = floor(pos / 8);

    int one = pos + dir;
    int startRank = isWhite ? 6 : 1;
    int two = pos + 2 * dir;

    int onePiece = getPiece(one);
    int twoPiece = getPiece(two);
    bool isOneEmpty = isEmpty(onePiece);
    bool isTwoEmpty = isEmpty(twoPiece);

    int leftCap = pos + dir - 1;
    int rightCap = pos + dir + 1;
    bool onLeftFile = pos % 8 == 0;
    bool onRightFile = pos % 8 == 7;

    int leftCapPiece = getPiece(leftCap);
    int rightCapPiece = getPiece(rightCap);

    if (isOneEmpty) {
        moves.push_back(one);
    }
    if (row == startRank && isOneEmpty && isTwoEmpty) {
        moves.push_back(two);
    }
    if (!onLeftFile && !isError(leftCapPiece) && leftCapPiece != EMPTY && getPieceColor(leftCapPiece) != color) {
        moves.push_back(leftCap);
    }
    if (!onRightFile && !isError(rightCapPiece) && rightCapPiece != EMPTY && getPieceColor(rightCapPiece) != color) {
        moves.push_back(rightCap);
    }

    auto prev = lastPositions.find(pos);
    if (prev != lastPositions.end()) {
        int prevPosition = prev->second;
        int prevRow = floor(prevPosition / 8);
        if (prevRow == startRank) {
            int rightPiece = getPiece(pos + 1);
            int leftPiece = getPiece(pos - 1);
            if (rightPiece != -1 && rightPiece != EMPTY && rightCapPiece == EMPTY 
                && !onRightFile && getPieceColor(rightPiece) != color) moves.push_back(rightCap);
            if (leftPiece != -1 && leftPiece != EMPTY && leftCapPiece == EMPTY 
                && !onLeftFile && getPieceColor(leftPiece) != color) moves.push_back(leftCap);
        }
    }

    return moves;
}

map<int, int> ChessBoard::getLastPositions() {
    return lastPositions;
}

vector<int> ChessBoard::rookMovement(int pos) {
    const int movement[] = { -8, -1, 8, 1 };
    int piece = getPiece(pos);
    vector<int> moves;
    string errorMessage = createMessage("Rook Movement @ {} - {}", pos, translatePiece(piece));
    checkForErrors(piece, errorMessage);

    for (auto& diff : movement) {
        int curPos = pos;
        while (true) {
            curPos += diff;
            if (curPos < 0 || curPos > 63 || !isAdjacent(curPos - diff, curPos)) break;
            int curPiece = getPiece(curPos);
            if (curPiece != EMPTY) {
                if (curPiece == -1) break;
                if (getPieceColor(curPiece) != getPieceColor(piece)) moves.push_back(curPos);
                break;
            }
            moves.push_back(curPos);
        }
    }

    return moves;
}

vector<int> ChessBoard::queenMovement(int pos) {
    vector<int> a = rookMovement(pos);
    vector<int> b = bishopMovement(pos);
    return removeDuplicates(a, b);
}

void ChessBoard::getPositionsFromFen(string& fen) {
    positions = {};

    for (int i = 0; i < fen.length(); i++) {
        int piece = EMPTY;
        char letter = fen.at(i);

        if (isdigit(static_cast<unsigned char>(letter))) {
            int numValue = letter - '0';
            for (int j = 0; j < numValue; j++) {
                positions.push_back(EMPTY);
            }
        }
        else if (islower(static_cast<unsigned char>(letter))) {
            if (letter == 'p') piece = B_PAWN;
            else if (letter == 'r') piece = B_ROOK;
            else if (letter == 'n') piece = B_KNIGHT;
            else if (letter == 'b') piece = B_BISHOP;
            else if (letter == 'q') piece = B_QUEEN;
            else if (letter == 'k') piece = B_KING;
            if (piece == B_KING) blackKingPos = positions.size();
            positions.push_back(piece);
        }
        else if (isupper(static_cast<unsigned char>(letter))) {
            if (letter == 'P') piece = W_PAWN;
            else if (letter == 'R') piece = W_ROOK;
            else if (letter == 'N') piece = W_KNIGHT;
            else if (letter == 'B') piece = W_BISHOP;
            else if (letter == 'Q') piece = W_QUEEN;
            else if (letter == 'K') piece = W_KING;
            if (piece == W_KING) whiteKingPos = positions.size();
            positions.push_back(piece);
        }
    }
}

void ChessBoard::printPositions() {
    for (int i = 0; i < positions.size(); i++) {
        cout << i << ": " << translatePiece(positions[i]) << endl;
    }
}

void printRow(char ch, int width, bool noDividers=false, bool endWithDivider=false) {
    cout << "   ";
    for (int i = 0; i < width; i++) {
        if (!noDividers && i % 4 == 0) {
            cout << "|";
        }
        cout << ch;
    }
    if (endWithDivider) cout << "|";
    cout << endl;
}

void printAxis(int width) {
    cout << "   ";
    int count = 0;
    int countPos = 2;
    for (int i = 0; i < width; i++) {
        if (i == countPos) {
            cout << count;
            count += 1;
            countPos += 5;
        }
        else cout << " ";
    }

    cout << endl;
}

template<typename T>
T getRandomMove(vector<T> values) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, values.size() - 1);
    return values[dis(gen)];
}

bool ChessBoard::getCurrentTurn() {
    return currentTurn;
}

tuple<bool, vector<int>> ChessBoard::printPossibleMovesBoard(int pos) {
    int fromPiece = getPiece(pos);
    vector<int> moves;

    if (fromPiece == -1 || fromPiece == EMPTY) {
        string reason = fromPiece == -1 ? "Invalid board position (0 <= pos <= 63)." : "Selected position is empty.";
        cout << reason << endl;
        return make_tuple(false, moves);
    }
    
    char pieceColor = getPieceColor(fromPiece);
    if (pieceColor != currentTurn) {
        string whosTurn = currentTurn == 'w' ? "White's turn." : "Black's turn.";
        cout << "Invalid piece to move - It is currently " << whosTurn << endl;
        return make_tuple(false, moves);
    }

    moves = generateMoves(pos);
    if (moves.size() == 0) {
        string reason = isInCheck(pieceColor) ? "Currently in check - Invalid piece." : "There are no destinations.";
        cout << reason << endl;
        return make_tuple(false, moves);
    }

    int dashesWidth = 8 * 5 + 1;
    int spacesWidth = 8 * 4 + 1;
    printAxis(dashesWidth);
    printRow('-', dashesWidth, true);

    int count = 0;
    for (int i = 0; i < 64; i++) {
        int piece = getPiece(i);
        string translated = translatePiece(piece);
        bool contains = find(moves.begin(), moves.end(), i) != moves.end();

        if (i % 8 == 0) {
            if (count > 10) cout << count << " ";
            else cout << " " << count << " ";
            count += 8;
        }
        cout << "|";
        if (i == pos || contains) {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, contains ? BACKGROUND_BLUE : BACKGROUND_GREEN);
        }
        
        string pieceString = contains ? i < 10 ? createMessage(" {}", i) : createMessage("{}", i) : translated;
        cout << " " << pieceString << " ";

        if (i % 8 == 7) {
            if (i == pos || contains) {
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, 0);
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            cout << "|" << endl;
            printRow('-', dashesWidth - 9, false, true);
        }
        else if (i == pos || contains) {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, 0);
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    }

    return make_tuple(true, moves);
}

void ChessBoard::printBoard(int from, int to) {
    int dashesWidth = 8 * 5 + 1;
    int spacesWidth = 8 * 4 + 1;
    printAxis(dashesWidth);
    printRow('-', dashesWidth, true);

    int count = 0;
    for (int i = 0; i < 64; i++) {  
        int piece = getPiece(i);
        string translated = translatePiece(piece);

        if (i % 8 == 0) {
            if (count > 10) cout << count << " ";
            else cout << " " << count << " ";
            count += 8;
        }
        cout << "|";
        if (i == from || i == to) {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, i == from ? BACKGROUND_RED : BACKGROUND_GREEN);
        }
        cout << " " << translated << " ";
        
        if (i % 8 == 7) {
            if (i == from || i == to) {
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, 0);
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            cout << "|" << endl;
            printRow('-', dashesWidth - 9, false, true);
        }
        else if (i == from || i == to) {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, 0);
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    }
}

void ChessBoard::printBoard() {
    int dashesWidth = 8 * 5 + 1;
    int spacesWidth = 8 * 4 + 1;
    printAxis(dashesWidth);
    printRow('-', dashesWidth, true);
    
    int count = 0;
    for (int i = 0; i < 64; i++) {
        int piece = getPiece(i);
        string translated = translatePiece(piece);

        if (i % 8 == 0) {
            if (count > 10) cout << count << " ";
            else cout << " " << count << " ";
            count += 8;
        }
        cout << "| " << translated << " ";
        if (i % 8 == 7) {
            cout << "|" << endl;
            printRow('-', dashesWidth - 9, false, true);
        }
    }
}

vector<tuple<int, int, int, int>> ChessBoard::getColorMoves(vector<int> positions, char color) {
    vector<int> colorPositions;
    vector<tuple<int, int, int, int>> moves;
    
    for (int i = 0; i < positions.size(); i++) {
        int piece = positions[i];
        if (piece != EMPTY && verifySameColor(piece, color)) colorPositions.push_back(i);
    }

    for (auto& start : colorPositions) {
        int fromPiece = getPiece(start);
        vector<int> destinations = generateMoves(start);
        for (auto& end : destinations) {
            moves.push_back(make_tuple(fromPiece, getPiece(end), start, end));
        }
    }

    return moves;
}

vector<tuple<int, int, int, int>> ChessBoard::getColorMoves(char color) {
    vector<int> colorPositions;
    vector<tuple<int, int, int, int>> moves;

    for (int i = 0; i < positions.size(); i++) {
        int piece = positions[i];
        if (piece != EMPTY && verifySameColor(piece, color)) colorPositions.push_back(i);
    }

    for (auto& start : colorPositions) {
        int fromPiece = positions[start];
        vector<int> destinations = generateMoves(start);
        for (auto& end : destinations) {
            moves.push_back(make_tuple(fromPiece, positions[end], start, end));
        }
    }

    return moves;
}

void ChessBoard::checkEnPassant(tuple<int, int, int, int> move) {
    int fromPiece = get<0>(move); int toPiece = get<1>(move);
    int from = get<2>(move); int to = get<3>(move);

    if (fromPiece == W_PAWN || fromPiece == B_PAWN) {
        int diff = abs(from - to);
        if (diff == 7 || diff == 9) {
            if (toPiece == EMPTY) {
                char color = getPieceColor(fromPiece);
                int enPassPos = color == 'w' ? to + 8 : to - 8;
                positions[enPassPos] = EMPTY;
            }
        }
    }
}

void ChessBoard::checkCastling(tuple<int, int, int, int> move) {
    int fromPiece = get<0>(move); int toPiece = get<1>(move);
    int from = get<2>(move); int to = get<3>(move);
    int diff = abs(from - to);
    char color = getPieceColor(fromPiece);
    
    bool leftCastle = color == 'w' ? get<0>(whiteCastle) : get<0>(blackCastle);
    bool rightCastle = color == 'w' ? get<1>(whiteCastle) : get<1>(blackCastle);
    int targetRook = color == 'w' ? W_ROOK : B_ROOK;

    if (!leftCastle && !rightCastle) return;
    
    if (fromPiece == W_KING || fromPiece == B_KING) {
        color == 'w' ? whiteCastle = make_tuple(false, false) : blackCastle = make_tuple(false, false);
        if (from != 60 || from != 4) return;

        if (diff == 2) {
            int dir = from - to;
            if (dir > 0 && leftCastle) {
                int rookPiece = getPiece(to - 2);
                if (rookPiece == targetRook) {
                    positions[to + 1] = targetRook;
                    positions[to - 2] = EMPTY;
                }
            }
            else if (dir < 0 && rightCastle) {
                int rookPiece = getPiece(to + 1);
                if (rookPiece == targetRook) {
                    positions[to - 1] = targetRook;
                    positions[to + 1] = EMPTY;
                }
            }
        }
    }
    else if (fromPiece == W_ROOK || fromPiece == B_ROOK) {
        if (from == 0 || from == 7) {
            blackCastle = from == 0 ? make_tuple(false, rightCastle) : make_tuple(leftCastle, false);
        }
        else if (from == 56 || from == 63) {
            whiteCastle = from == 56 ? make_tuple(false, rightCastle) : make_tuple(leftCastle, false);
        }
    }
}

int getUserPromotion(char color, string details) {
    vector<string> possibilites = { "rook", "bishop", "knight", "queen", "r", "b", "n", "q" };
    string input;
    
    while (true) {
        cout << details;
        getline(cin, input);
        transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return tolower(c); });
        
        bool isValid = find(possibilites.begin(), possibilites.end(), input) != possibilites.end();

        if (!isValid) {
            cout << "Invalid input!" << endl;
            continue;
        }

        if (input == "rook" || input == "r") { return color == 'w' ? W_ROOK : B_ROOK; }
        if (input == "bishop" || input == "b") { return color == 'w' ? W_BISHOP : B_BISHOP; }
        if (input == "knight" || input == "n") { return color == 'w' ? W_KNIGHT : B_KNIGHT; }
        if (input == "queen" || input == "q") { return color == 'w' ? W_QUEEN : B_QUEEN; }
    }
}

bool ChessBoard::checkPromotion(tuple<int, int, int, int> move) {
    int fromPiece = get<0>(move); int toPiece = get<1>(move);
    int from = get<2>(move); int to = get<3>(move);
    int toRank = floor(to / 8);

    if (fromPiece == W_PAWN || fromPiece == B_PAWN) {
        if (fromPiece == W_PAWN && toRank == 0) return true;
        if (fromPiece == B_PAWN && toRank == 7) return true;
    }

    return false;
}

int ChessBoard::getPromotion(tuple<int, int, int, int> move) {
    int fromPiece = get<0>(move); int toPiece = get<1>(move);
    int from = get<2>(move); int to = get<3>(move);
    int toRank = floor(to / 8);

    string details = "Select piece to promote to [rook ('r'), bishop ('b'), knight ('n'), queen ('q')]: ";
    
    if (fromPiece == W_PAWN || fromPiece == B_PAWN) {
        if (fromPiece == W_PAWN && toRank == 0) {
            return getUserPromotion('w', details);
        }
        else if (fromPiece == B_PAWN && toRank == 7) {
            return getUserPromotion('b', details);
        }
    }

    return fromPiece;
}

void ChessBoard::makeMove(int from, int to, bool simulated) {
    tuple<int, int, int, int> move = makeMoveObj(from, to);
    makeMove(move, simulated);
}

void ChessBoard::makeMove(tuple<int, int, int, int> move, bool simulated) {
    int fromPiece = get<0>(move); int toPiece = get<1>(move);
    int from = get<2>(move); int to = get<3>(move);
    char nextMove = currentTurn == 'w' ? 'b' : 'w';
    if (fromPiece == -1) return;

    //if (printMove) {
    //    string capturing = toPiece == EMPTY ? "." : createMessage(", capturing {}.", translatePiece(toPiece));
    //    string message = createMessage("Moving {} from {} to {}{}", translatePiece(fromPiece), from, to, capturing);
    //    cout << message << endl;
    //}

    fromPiece = checkPromotion(move) ? simulated ? (currentTurn == 'w' ? W_QUEEN : B_QUEEN) : getPromotion(move) : fromPiece;
    checkCastling(move);
    checkEnPassant(move);
    positions[to] = fromPiece;
    positions[from] = EMPTY;
    lastPositions[to] = from;
    
    currentTurn = nextMove;
    if (fromPiece == W_KING) whiteKingPos = to;
    else if (fromPiece == B_KING) blackKingPos = to;
    if (toPiece == W_KING) whiteKingPos = -1;
    else if (toPiece == B_KING) blackKingPos = -1;
}

void ChessBoard::unmakeMove(int from, int to, int movedPiece, int capturedPiece) {
    tuple<int, int, int, int> move = make_tuple(movedPiece, capturedPiece, from, to);
    unmakeMove(move);
}

void ChessBoard::unmakeMove(tuple<int, int, int, int> move) {
    int fromPiece = get<0>(move); int toPiece = get<1>(move);
    int from = get<2>(move); int to = get<3>(move);
    char prevTurn = currentTurn == 'w' ? 'b' : 'w';

    positions[from] = fromPiece;
    positions[to] = toPiece;
    currentTurn = prevTurn;
    
    if (fromPiece == W_KING) whiteKingPos = from;
    else if (fromPiece == B_KING) blackKingPos = from;
    if (toPiece == W_KING) whiteKingPos = to;
    else if (toPiece == B_KING) blackKingPos = to;
}

void ChessBoard::makeRandomMove(string turn) {
    vector<tuple<int, int, int, int>> moves = getColorMoves(turn.at(0));
    tuple<int, int, int, int> randMove = getRandomMove(moves);
    int from = get<2>(randMove); int to = get<3>(randMove);
    int toPiece = get<1>(randMove);
    
    //string capturing = toPiece == EMPTY ? "." : createMessage(", capturing {}.", translatePiece(toPiece));
    //string message = createMessage("Moving {} from {} to {}{}", translatePiece(getPiece(from)), from, to, capturing);
    //cout << message << endl << endl;

    makeMove(from, to);
    printBoard();
    cout << endl;
}

vector<int> ChessBoard::getPositions() {
    return positions;
}

int ChessBoard::getPosFromPiece(int piece) {
    for (int pos = 0; pos < positions.size(); pos++) {
        if (positions[pos] == piece) return pos;
    }
    return -1;
}

void ChessBoard::printMoveData(tuple<int, int, int, int> move) {
    int fromPiece = get<0>(move); int toPiece = get<1>(move);
    int from = get<2>(move); int to = get<3>(move);
    string capturing = toPiece == EMPTY ? "." : createMessage(", capturing {}.", translatePiece(toPiece));
    string message = createMessage("Moving {} from {} to {}{}", translatePiece(fromPiece), from, to, capturing);
    cout << message << endl << endl;
}

string ChessBoard::getMoveData(tuple<int, int, int, int> move) {
    int fromPiece = get<0>(move); int toPiece = get<1>(move);
    int from = get<2>(move); int to = get<3>(move);
    string capturing = toPiece == EMPTY ? "." : createMessage(", capturing {}.", translatePiece(toPiece));
    string message = createMessage("Moving {} from {} to {}{}", translatePiece(fromPiece), from, to, capturing);
    return message;
}

bool ChessBoard::checkPawn(int targetPos, int kingPos, int enemyColorBit) {
    if (targetPos < 0 || targetPos >= 64) return false;
    if (abs((targetPos & 7) - (kingPos & 7)) != 1) return false;

    int piece = positions[targetPos];
    return (piece != EMPTY &&
        (piece & 8) == enemyColorBit &&
        (piece & 7) == W_PAWN);
}

int getMoveScore(const tuple<int, int, int, int>& move) {
    int from = get<0>(move);
    int to = get<1>(move);
    int attacker = get<2>(move);
    int victim = get<3>(move);

    if (victim == 0) return 0;
    return (pieceValues[victim] * 10) - pieceValues[attacker];
}

void sortCaptures(vector<tuple<int, int, int, int>>& moves) {
    sort(moves.begin(), moves.end(), [&](const auto& a, const auto& b) {
        return getMoveScore(a) > getMoveScore(b);
    });
}

vector<tuple<int, int, int, int>> ChessBoard::getGenerationOnlyCaptures(char color) {
    vector<tuple<int, int, int, int>> captures;
    vector<tuple<int, int, int, int>> moves = getColorMoves(color);
    for (auto& move : moves) {
        int toPiece = get<1>(move);
        if (toPiece == EMPTY) continue;
        captures.push_back(move);
    }

    sortCaptures(captures);
    return captures;
}

int getTargetedPieceType(string piece, char color) {
    if (piece == "pawn") return color == 'w' ? W_PAWN : B_PAWN;
    if (piece == "rook") return color == 'w' ? W_ROOK : B_ROOK;
    if (piece == "bishop") return color == 'w' ? W_BISHOP : B_BISHOP;
    if (piece == "knight") return color == 'w' ? W_KNIGHT : B_KNIGHT;
    if (piece == "queen") return color == 'w' ? W_QUEEN : B_QUEEN;
    if (piece == "king") return color == 'w' ? W_KING : B_KING;
    return EMPTY;
}

bool ChessBoard::isInCheck(char color) {
    int kingPos = color == 'w' ? whiteKingPos : blackKingPos;
    char enemyColor = color == 'w' ? 'b' : 'w';
    int enemyColorBit = (color == 'w') ? 8 : 0;

    static const int knightOffsets[] = { -17, -15, -10, -6, 6, 10, 15, 17 };
    for (int offset : knightOffsets) {
        int target = kingPos + offset;
        if (target >= 0 && target < 64 && isCorrectDistance(kingPos, target, 2)) {
            int piece = positions[target];
            if (piece != EMPTY && piece == getTargetedPieceType("knight", enemyColor)) return true;
        }
    }

    static const int directions[] = { -8, 8, -1, 1, -9, -7, 7, 9 };
    for (int i = 0; i < 8; i++) {
        int target = kingPos;
        while (true) {
            target += directions[i];
            if (target < 0 || target >= 64 || !isAdjacent(target - directions[i], target)) break;

            int piece = positions[target];
            if (piece != EMPTY) {
                if (i < 4 && (piece == getTargetedPieceType("bishop", enemyColor) || piece == getTargetedPieceType("queen", enemyColor))) return true;
                if (i >= 4 && (piece == getTargetedPieceType("bishop", enemyColor) || piece == getTargetedPieceType("queen", enemyColor))) return true;
                break;
            }
        }
    }

    int pawnDir = (color == 'w') ? -1 : 1;
    int pawnLeft = kingPos + (pawnDir * 8) - 1;
    int pawnRight = kingPos + (pawnDir * 8) + 1;
    if (color == 'w') {
        if (checkPawn(kingPos - 7, kingPos, enemyColorBit) ||
            checkPawn(kingPos - 9, kingPos, enemyColorBit)) return true;
    }
    else {
        if (checkPawn(kingPos + 7, kingPos, enemyColorBit) ||
            checkPawn(kingPos + 9, kingPos, enemyColorBit)) return true;
    }

    vector<int> oppKingMoves = kingMovement(color == 'w' ? blackKingPos : whiteKingPos);
    if (find(oppKingMoves.begin(), oppKingMoves.end(), kingPos) != oppKingMoves.end()) return true;

    return false;
}

double getPosValue(int piece, int pos, char color) {
    double score = 0;
    
    if (piece != EMPTY) {
        if (piece == W_PAWN || piece == B_PAWN) score = pawnScores[pos];
        else if (piece == W_ROOK || piece == B_ROOK) score = rookScores[pos];
        else if (piece == W_KNIGHT || piece == B_KNIGHT) score = knightScores[pos];
        else if (piece == W_BISHOP || piece == B_BISHOP) score = bishopScores[pos];
        else if (piece == W_QUEEN || piece == B_QUEEN) score = queenScores[pos];
        else if (piece == W_KING || piece == B_KING) score = kingScores[pos];
    }

    return score;
}

double ChessBoard::evaluateBoard(char color) {
    double score = 0;
    for (int pos = 0; pos < 64; pos++) {
        int piece = positions[pos];
        char pieceColor = getPieceColor(piece);
        if (piece == -1 || piece == EMPTY || pieceColor != color) continue;
        score += pieceValues[piece];
        score += getPosValue(piece, pos, color);
    }
    return score;
}

bool ChessBoard::checkValidMove(tuple<int, int, int, int> move) {
    int fromPiece = get<0>(move);
    char pieceColor = getPieceColor(fromPiece);

    vector<int> positionsCopy = getPositions();
    map<int, int> lastPositionsCopy = getLastPositions();
    tuple<bool, bool> whiteCastleCopy = whiteCastle;
    tuple<bool, bool> blackCastleCopy = blackCastle;

    makeMove(move, true);
    
    bool isInvalid = isInCheck(pieceColor);
    positions = positionsCopy;
    lastPositions = lastPositionsCopy;
    whiteCastle = whiteCastleCopy;
    blackCastle = blackCastleCopy;
    
    unmakeMove(move);
    return !isInvalid;
}

void ChessBoard::printTurn() {
    cout << (currentTurn == 'w' ? "White Turn" : "Black Turn") << " - Position to Move, or -1 to end match: ";
}

tuple<int, int, int, int> ChessBoard::makeMoveObj(int from, int to) {
    int fromPiece = getPiece(from);
    int toPiece = getPiece(to);
    return make_tuple(fromPiece, toPiece, from, to);
}

bool ChessBoard::isCheckmate() {
    for (int pos = 0; pos < 64; pos++) {
        int piece = getPiece(pos);
        if (piece == -1 || getPieceColor(piece) != currentTurn) continue;
        
        vector<int> moves = generateMoves(pos);
        if (moves.size() > 0) return false;
    }
    
    cout << (currentTurn == 'w' ? "Black Wins!" : "White Wins!") << endl;
    return true;
}