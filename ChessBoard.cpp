#include "ChessBoard.h"
#include "Windows.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <set>
#include <algorithm>
#include <tuple>
#include <random>

using namespace std;

ChessBoard::ChessBoard() : positions({}), blackKingPos(-1), whiteKingPos(-1), currentTurn('w') { }
ChessBoard::ChessBoard(vector<int> a) : positions(a), blackKingPos(getPosFromPiece(B_KING)), whiteKingPos(getPosFromPiece(W_KING)), currentTurn('w') { }
ChessBoard::ChessBoard(string fen) : currentTurn('w') { getPositionsFromFen(fen); }
ChessBoard::~ChessBoard() { }

template <typename T>
void printVector2(vector<T> vec) {
    cout << "[";
    for (const auto& item : vec) {
        if (item == vec.at(0)) {
            cout << item;
        }
        else {
            cout << ", " << item;
        }
    }
    cout << "]" << endl;
}

void printVector1(vector<tuple<int, int, int, int>> vec) {
    cout << "[";
    for (int i = 0; i < vec.size(); i++) {
        tuple<int, int, int, int> item = vec[i];
        if (i == 0) {
            cout << "(" << get<0>(item) << "," << get<1>(item) << "," << get<2>(item) << "," << get<3>(item) << ")";
        }
        else {
            cout << ", " << "(" << get<0>(item) << "," << get<1>(item) << "," << get<2>(item) << "," << get<3>(item) << ")";
        }
    }
    cout << "]" << endl;
}

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
            break;
        case B_PAWN:
            return "bP";
            break;
        case B_BISHOP:
            return "bB";
            break;
        case B_ROOK:
            return "bR";
            break;
        case B_QUEEN:
            return "bQ";
            break;
        case B_KING:
            return "bK";
            break;
        case B_KNIGHT:
            return "bN";
            break;
        case W_PAWN:
            return "wP";
            break;
        case W_BISHOP:
            return "wB";
            break;
        case W_ROOK:
            return "wR";
            break;
        case W_QUEEN:
            return "wQ";
            break;
        case W_KING:
            return "wK";
            break;
        case W_KNIGHT:
            return "wN";
            break;
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

        return validMoves;
    }
}

vector<int> ChessBoard::knightMovement(int pos) {
    int rPos = pos - 6; int rPos1 = pos + 10; int lPos = pos - 10; int lPos1 = pos + 6;
    int dPos = pos + 15; int dPos1 = pos + 17; int uPos = pos - 15; int uPos1 = pos - 17;

    int piece = getPiece(pos);
    string errorMessage = createMessage("Knight Movement @ {} - {}", pos, translatePiece(piece));
    checkForErrors(piece, errorMessage);

    char color = piece < 8 ? 'w' : 'b';
    vector<int> allAvailable;
    vector<int> possibleMoves;

    switch (pos % 8) {
    case 7:
        allAvailable = { lPos, lPos1, dPos, uPos1 };
        break;
    case 0:
        allAvailable = { rPos, rPos1, dPos1, uPos };
        break;
    case 1:
        allAvailable = { rPos, rPos1, uPos, uPos1, dPos, dPos1 };
        break;
    case 6:
        allAvailable = { lPos, lPos1, uPos, uPos1, dPos, dPos1 };
        break;
    default:
        allAvailable = { lPos, lPos1, rPos, rPos1, uPos, uPos1, dPos, dPos1 };
        break;
    }

    for (const int pos : allAvailable) {
        if (checkValidMove(color, pos)) {
            possibleMoves.push_back(pos);
        }
    }
    return possibleMoves;
}

vector<int> ChessBoard::kingMovement(int pos) {
    int tr = pos - 7; int tl = pos - 9; int t = pos - 8; int r = pos + 1;
    int l = pos - 1; int br = pos + 9; int bl = pos + 7; int b = pos + 8;

    int piece = getPiece(pos);
    string errorMessage = createMessage("King Movement @ {} - {}", pos, translatePiece(piece));
    checkForErrors(piece, errorMessage);

    char color = piece < 8 ? 'w' : 'b';
    vector<int> allAvailable;
    vector<int> possibleMoves;

    if (pos % 8 == 0) {
        allAvailable = { t, b, r, tr, br };
    }
    else if (pos % 8 == 7) {
        allAvailable = { t, b, l, tl, bl };
    }
    else {
        allAvailable = { tr, br, r, t, b, l, tl, bl };
    }

    for (const int pos : allAvailable) {
        if (checkValidMove(color, pos)) {
            possibleMoves.push_back(pos);
        }
    }

    return possibleMoves;
}

vector<int> ChessBoard::bishopMovement(int pos) {
    int rPos = pos - 7; int lPos = pos - 9; int drPos = pos + 9; int dlPos = pos + 7;
    bool activeUpRight = true; bool activeUpLeft = true; bool activeDownRight = true;
    bool activeDownLeft = true; bool run = true;

    int piece = getPiece(pos);
    string errorMessage = createMessage("Bishop Movement @ {} - {}", pos, translatePiece(piece));
    checkForErrors(piece, errorMessage);

    char opColor = piece < 8 ? 'b' : 'w';
    vector<int> valid;

    while (run) {
        int ru = getPiece(rPos); int rd = getPiece(drPos);
        int lu = getPiece(lPos); int ld = getPiece(dlPos);
        bool end = true;

        if (!isError(ru) && (isEmpty(ru) || getPieceColor(ru) == opColor) && activeUpRight && pos % 8 != 7) {
            valid.push_back(rPos);
            activeUpRight = !(!isEmpty(ru) || rPos % 8 == 7);
            rPos -= 7;
            end = false;
        }

        if (!isError(rd) && (isEmpty(rd) || getPieceColor(rd) == opColor) && activeDownRight && pos % 8 != 7) {
            valid.push_back(drPos);
            activeDownRight = !(!isEmpty(rd) || drPos % 8 == 7);
            drPos += 9;
            end = false;
        }

        if (!isError(lu) && (isEmpty(lu) || getPieceColor(lu) == opColor) && activeUpLeft && pos % 8 != 0) {
            valid.push_back(lPos);
            activeUpLeft = !(!isEmpty(lu) || lPos % 8 == 0);
            lPos -= 9;
            end = false;
        }

        if (!isError(ld) && (isEmpty(ld) || getPieceColor(ld) == opColor) && activeDownLeft && pos % 8 != 0) {
            valid.push_back(dlPos);
            activeDownLeft = !(!isEmpty(ld) || dlPos % 8 == 0);
            dlPos += 7;
            end = false;
        }

        run = !end;
    }

    return valid;
}

vector<int> ChessBoard::pawnMovement(int pos) {
    int piece = getPiece(pos);
    string errorMessage = createMessage("Pawn Movement @ {} - {}", pos, translatePiece(piece));
    checkForErrors(piece, errorMessage);

    vector<int> moves;
    bool isWhite = getPieceColor(piece) == 'w';
    int dir = isWhite ? -8 : 8;
    int row = floor(pos / 8);

    int one = pos + dir;
    int startRank = isWhite ? 6 : 1;
    int two = pos + 2 * dir;

    int onePiece = getPiece(one);
    int twoPiece = getPiece(two);
    bool isOneEmpty = isEmpty(onePiece);

    if (!isError(onePiece) && isEmpty(onePiece)) {
        moves.push_back(one);
    }
    if (row == startRank && (!isError(onePiece) && isEmpty(onePiece)) && (!isError(twoPiece) && isEmpty(twoPiece))) {
        moves.push_back(two);
    }

    int leftCap = pos + dir - 1;
    int rightCap = pos + dir + 1;
    bool onLeftFile = pos % 8 == 0;
    bool onRightFile = pos % 8 == 7;

    int leftCapPiece = getPiece(leftCap);
    int rightCapPiece = getPiece(rightCap);

    if (!onLeftFile && !isError(leftCapPiece) && leftCapPiece != EMPTY && getPieceColor(leftCapPiece) != getPieceColor(piece)) {
        moves.push_back(leftCap);
    }
    if (!onRightFile && !isError(rightCapPiece) && rightCapPiece != EMPTY && getPieceColor(rightCapPiece) != getPieceColor(piece)) {
        moves.push_back(rightCap);
    }

    return moves;
}

vector<int> ChessBoard::rookMovement(int pos) {
    int piece = getPiece(pos);
    vector<int> moves;
    string errorMessage = createMessage("Rook Movement @ {} - {}", pos, translatePiece(piece));
    checkForErrors(piece, errorMessage);

    bool run = true;
    char opColor = piece < 8 ? 'b' : 'w';
    int rMove = 1; int lMove = 1; int nxtPos = pos + 8; int prevPos = pos - 8;
    bool moveRight = true; bool moveLeft = true; bool moveUp = true; bool moveDown = true;

    while (run) {
        bool end = true;
        int rNxt = pos + rMove; int lNxt = pos - lMove;
        int piece = getPiece(nxtPos); int prevPiece = getPiece(prevPos);
        int rPiece = getPiece(rNxt); int lPiece = getPiece(lNxt);

        if (!isError(piece) && (isEmpty(piece) || getPieceColor(piece) == opColor) && moveDown) {
            moves.push_back(nxtPos);
            moveDown = isEmpty(piece);
            nxtPos += 8;
            end = false;
        }

        if (!isError(prevPiece) && (isEmpty(prevPiece) || getPieceColor(prevPiece) == opColor) && moveUp) {
            moves.push_back(prevPos);
            moveUp = isEmpty(prevPiece);
            prevPos -= 8;
            end = false;
        }

        if (!isError(rPiece) && (isEmpty(rPiece) || getPieceColor(rPiece) == opColor) && moveRight && pos % 8 != 7) {
            moves.push_back(rNxt);
            moveRight = isEmpty(rPiece) && rNxt % 8 != 7;
            rMove += 1;
            end = false;
        }

        if (!isError(lPiece) && (isEmpty(lPiece) || getPieceColor(lPiece) == opColor) && moveLeft && pos % 8 != 0) {
            moves.push_back(lNxt);
            moveLeft = isEmpty(lPiece) && lNxt % 8 != 0;
            lMove += 1;
            end = false;
        }

        run = !end;
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
    int drawPos = 1;

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

tuple<bool, vector<int>> ChessBoard::printPossibleMovesBoard(int pos) {
    int fromPiece = getPiece(pos);
    vector<int> moves;

    if (fromPiece == -1 || fromPiece == EMPTY) {
        cout << (fromPiece == -1 ? "Invalid board position (0 <= pos <= 63)" : "Selected position is empty") << "." << endl;
        return make_tuple(false, moves);
    }
    
    char pieceColor = getPieceColor(fromPiece);
    if (pieceColor != currentTurn) {
        cout << "Invalid piece to move - It is currently " << (currentTurn == 'w' ? "White's turn" : "Black's turn") << "." << endl;
        return make_tuple(false, moves);
    }

    moves = generateMoves(pos);
    if (moves.size() == 0) {
        cout << "There are no destinations." << endl;
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

void ChessBoard::makeMove(int from, int to, bool printMove) {
    int piece = positions[from];
    int toPiece = positions[to];
    char nextTurn = currentTurn == 'w' ? 'b' : 'w';
    
    if (printMove) {
        string capturing = positions[to] == EMPTY ? "." : createMessage(", capturing {}.", translatePiece(positions[to]));
        string message = createMessage("Moving {} from {} to {}{}", translatePiece(piece), from, to, capturing);
        cout << message << endl << endl;
    }

    positions[to] = piece;
    positions[from] = EMPTY;
    currentTurn = nextTurn;
    if (piece == W_KING) whiteKingPos = to;
    else if (piece == B_KING) blackKingPos = to;
    if (toPiece == W_KING) whiteKingPos = -1;
    else if (toPiece == B_KING) blackKingPos = -1;
}

void ChessBoard::makeMove(tuple<int, int, int, int> move, bool printMove) {
    int fromPiece = get<0>(move); int toPiece = get<1>(move);
    int from = get<2>(move); int to = get<3>(move);
    char nextMove = currentTurn == 'w' ? 'b' : 'w';

    if (fromPiece == -1) return;

    if (printMove) {
        string capturing = toPiece == EMPTY ? "." : createMessage(", capturing {}.", translatePiece(toPiece));
        string message = createMessage("Moving {} from {} to {}{}", translatePiece(fromPiece), from, to, capturing);
        cout << message << endl;
    }

    positions[to] = fromPiece;
    positions[from] = EMPTY;
    currentTurn = nextMove;
    
    if (fromPiece == W_KING) whiteKingPos = to;
    else if (fromPiece == B_KING) blackKingPos = to;
    if (toPiece == W_KING) whiteKingPos = -1;
    else if (toPiece == B_KING) blackKingPos = -1;
}

void ChessBoard::unmakeMove(int from, int to, int movedPiece, int capturedPiece) {
    char prevTurn = currentTurn == 'w' ? 'b' : 'w';
    positions[from] = movedPiece;
    positions[to] = capturedPiece;
    currentTurn = prevTurn;
    
    if (movedPiece == W_KING) whiteKingPos = from;
    else if (movedPiece == B_KING) blackKingPos = from;
    if (capturedPiece == W_KING) whiteKingPos = to;
    else if (capturedPiece == B_KING) blackKingPos = to;
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

    makeMove(from, to, true);
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

bool isAdjacent(int a, int b) {
    int dx = abs((a & 7) - (b & 7));
    int dy = abs((a >> 3) - (b >> 3));
    return (dx <= 1 && dy <= 1);
}

bool isCorrectDistance(int pos1, int pos2, int maxStep) {
    int modDiff = abs(pos1 % 8 - pos2 % 8);
    return modDiff <= maxStep;
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

    makeMove(move);
    bool isInvalid = isInCheck(pieceColor);
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