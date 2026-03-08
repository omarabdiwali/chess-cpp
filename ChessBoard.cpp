#include "ChessBoard.h"
#include "Windows.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <set>
#include <algorithm>
#include <tuple>
#include <cctype>
#include <cmath>

using namespace std;

ChessBoard::ChessBoard(string fen) : currentTurn('w') { getPositionsFromFen(fen); }
ChessBoard::~ChessBoard() { }

int getTargetedPieceType(string piece, char color) {
    if (piece == "pawn" || piece == "p") return color == 'w' ? W_PAWN : B_PAWN;
    if (piece == "rook" || piece == "r") return color == 'w' ? W_ROOK : B_ROOK;
    if (piece == "bishop" || piece == "b") return color == 'w' ? W_BISHOP : B_BISHOP;
    if (piece == "knight" || piece == "n") return color == 'w' ? W_KNIGHT : B_KNIGHT;
    if (piece == "queen" || piece == "q") return color == 'w' ? W_QUEEN : B_QUEEN;
    if (piece == "king" || piece == "k") return color == 'w' ? W_KING : B_KING;
    return EMPTY;
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

        return getTargetedPieceType(input, color);
    }
}

int ChessBoard::getPosFromPiece(int targetPiece) {
    for (int pos = 0; pos < positions.size(); pos++) {
        int piece = getPiece(pos);
        if (piece == targetPiece) return pos;
    }
    return -1;
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

void printRow(char ch, int width, bool noDividers = false, bool endWithDivider = false) {
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

void printFiles(int width) {
    cout << "   ";
    const char files[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    int count = 0;
    int countPos = 2;
    for (int i = 0; i < width; i++) {
        if (i == countPos) {
            cout << files[count];
            count += 1;
            countPos += 5;
        }
        else cout << " ";
    }

    cout << endl;
}

void ChessBoard::getPositionsFromFen(string& fen) {
    int spacePassed = 0;
    int enPassantPos = -1;
    string halfMove = "";
    string fullMove = "";
    
    positions.clear();
    enPassantCtx.clear();
    whiteCastle = make_tuple(false, false);
    blackCastle = make_tuple(false, false);

    for (int i = 0; i < fen.length(); i++) {
        int piece = EMPTY;
        char letter = fen.at(i);

        if (letter == ' ') spacePassed += 1;
        else if (spacePassed > 0) {
            if (spacePassed == 1) currentTurn = letter;
            else if (spacePassed == 2) {
                if (letter == '-') {
                    whiteCastle = make_tuple(false, false);
                    blackCastle = make_tuple(false, false);
                }
                else if (islower(static_cast<unsigned char>(letter))) {
                    bool leftCastle = get<0>(blackCastle); bool rightCastle = get<1>(blackCastle);
                    if (letter == 'q') blackCastle = make_tuple(true, rightCastle);
                    else if (letter == 'k') blackCastle = make_tuple(leftCastle, true);
                }
                else if (isupper(static_cast<unsigned char>(letter))) {
                    bool leftCastle = get<0>(whiteCastle); bool rightCastle = get<1>(whiteCastle);
                    if (letter == 'Q') { whiteCastle = make_tuple(true, rightCastle); }
                    else if (letter == 'K') { whiteCastle = make_tuple(leftCastle, true); }
                }
            }
            else if (spacePassed == 3) {
                if (letter == '-') {
                    enPassantPos = -1;
                }
                else if (islower(static_cast<unsigned char>(letter))) {
                    if (enPassantPos == -1) enPassantPos = 0;
                    enPassantPos += (letter - 'a');
                }
                else if (isdigit(static_cast<unsigned char>(letter))) {
                    if (enPassantPos == -1) enPassantPos = 0;
                    enPassantPos += (64 - ((letter - '0') * 8));
                }
            }
            else if (spacePassed == 4) {
                halfMove += letter;
            }
            else if (spacePassed == 5) {
                fullMove += letter;
            }
        }
        else if (isdigit(static_cast<unsigned char>(letter))) {
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

    if (enPassantPos != -1) {
        enPassantPos += (currentTurn == 'w' ? 8 : -8);
        enPassantCtx[enPassantPos] = (currentTurn == 'w' ? enPassantPos - 16 : enPassantPos + 16);
    }

    try {
        halfMoveClock = stoi(halfMove);
        fullMoveNumber = stoi(fullMove);
    }
    catch (...) {
        halfMoveClock = 0;
        fullMoveNumber = 1;
    }
}

string ChessBoard::getFen() {
    string fen = "";
    int skips = 0;

    for (int pos = 0; pos < 64; pos++) {
        int piece = getPiece(pos);

        if (pos % 8 == 0 && pos != 0) {
            if (skips > 0) {
                fen += to_string(skips);
                skips = 0;
            }
            fen += "/";
        }

        if (piece == EMPTY) {
            skips += 1;
        }
        else {
            if (piece == -1) continue;
            if (skips > 0) {
                fen += to_string(skips);
                skips = 0;
            }

            if (piece < 8) {
                if (piece == W_PAWN) fen += "P";
                else if (piece == W_ROOK) fen += "R";
                else if (piece == W_BISHOP) fen += "B";
                else if (piece == W_KNIGHT) fen += "N";
                else if (piece == W_QUEEN) fen += "Q";
                else if (piece == W_KING) fen += "K";
            }
            else {
                if (piece == B_PAWN) fen += "p";
                else if (piece == B_ROOK) fen += "r";
                else if (piece == B_BISHOP) fen += "b";
                else if (piece == B_KNIGHT) fen += "n";
                else if (piece == B_QUEEN) fen += "q";
                else if (piece == B_KING) fen += "k";
            }
        }
    }

    if (skips > 0) {
        fen += to_string(skips);
    }

    string turnMessage = createMessage(" {} ", currentTurn);
    fen += turnMessage;

    if (get<0>(whiteCastle) || get<1>(whiteCastle) || get<0>(blackCastle) || get<1>(blackCastle)) {
        if (get<0>(whiteCastle)) fen += "Q";
        if (get<1>(whiteCastle)) fen += "K";
        if (get<0>(blackCastle)) fen += "q";
        if (get<1>(blackCastle)) fen += "k";
    }
    else {
        fen += "-";
    }

    if (enPassantCtx.size() > 0) {
        fen += " ";
        int currentPos = enPassantCtx.begin()->first;
        fen += translateToSquareNames(currentPos, true);
    }
    else {
        fen += " -";
    }

    fen += createMessage(" {} {}", halfMoveClock, fullMoveNumber);
    return fen;
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
        else if (piece == W_ROOK || piece == B_ROOK) { totalMoves = diagonalAndOrthogonalMovement("rook", pos); }
        else if (piece == W_KNIGHT || piece == B_KNIGHT) { totalMoves = knightMovement(pos); }
        else if (piece == W_BISHOP || piece == B_BISHOP) { totalMoves = diagonalAndOrthogonalMovement("bishop", pos); }
        else if (piece == W_QUEEN || piece == B_QUEEN) { totalMoves = diagonalAndOrthogonalMovement("queen", pos); }
        else if (piece == W_KING || piece == B_KING) { totalMoves = kingMovement(pos); }

        for (auto& to : totalMoves) {
            int toPiece = getPiece(to);
            if (toPiece != -1) {
                Move move = make_tuple(piece, toPiece, pos, to);
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

vector<int> ChessBoard::diagonalAndOrthogonalMovement(string pieceType, int pos) {
    int piece = getPiece(pos);
    int startIndex = pieceType == "bishop" ? 4 : 0;
    int endIndex = pieceType == "rook" ? 4 : 8;
    vector<int> moves;
    static const int directions[] = { -8, 8, -1, 1, -9, -7, 7, 9 };

    if (piece == -1) return moves;
    
    for (int i = startIndex; i < endIndex; i++) {
        int target = pos;
        while (true) {
            target += directions[i];
            int toPiece = getPiece(target);
            
            if (toPiece == -1 || !isAdjacent(target - directions[i], target)) break;
            if (toPiece != EMPTY) {
                if (getPieceColor(toPiece) != getPieceColor(piece)) moves.push_back(target);
                break;
            }
            moves.push_back(target);
        }
    }
    
    return moves;
}

vector<int> ChessBoard::kingMovement(int pos) {
    const int movement[] = { -9, -8, -7, -1, 1, 7, 8, 9 };
    int piece = getPiece(pos);
    vector<int> moves;

    if (piece == -1) { return moves; }
    string errorMessage = createMessage("King Movement @ {} - {}", pos, piece);
    checkForErrors(piece, errorMessage);

    char color = piece < 8 ? 'w' : 'b';

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
            Move move = makeMoveObj(pos, skipOver);
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
            Move move = makeMoveObj(pos, skipOver);
            if (!checkValidMove(move)) { canCastle = false; break; }
        }

        if (canCastle) moves.push_back(pos + 2);
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

    const int leftAndRight[] = { pos - 1, pos + 1 };
    for (auto neighbor : leftAndRight) {
        int piece = getPiece(neighbor);
        int opStartRank = startRank == 6 ? 1 : 6;
        if (piece != getTargetedPieceType("pawn", isWhite ? 'b' : 'w')) continue;
        if (!isCorrectDistance(neighbor, pos, 1)) continue;

        auto prev = enPassantCtx.find(neighbor);
        if (prev != enPassantCtx.end()) {
            int prevPosition = prev->second;
            int prevRank = floor(prevPosition / 8);
            if (prevRank == opStartRank) {
                int posCapture = neighbor > pos ? rightCap : leftCap;
                moves.push_back(posCapture);
            }
        }
    }

    return moves;
}

bool ChessBoard::checkValidMove(char color, int pos) {
    int piece = getPiece(pos);
    return piece != -1 && (isEmpty(piece) || !verifySameColor(piece, color));
}

bool ChessBoard::checkValidMove(Move move) {
    int fromPiece = get<0>(move);
    char pieceColor = getPieceColor(fromPiece);
    string currentFen = getFen();

    makeMove(move, true);
    bool isInvalid = isInCheck(pieceColor);
    getPositionsFromFen(currentFen);
    return !isInvalid;
}

tuple<bool, vector<int>> ChessBoard::printPossibleMovesBoard(int pos) {
    int fromPiece = getPiece(pos);
    vector<int> moves;

    if (fromPiece == -1 || fromPiece == EMPTY) {
        string reason = fromPiece == -1 ? "Invalid board position (a1 <= pos <= h8) or (0 <= pos <= 63)." : "Selected position is empty.";
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
    cout << endl;

    printFiles(dashesWidth);
    printRow('-', dashesWidth, true);

    int count = 8;
    for (int i = 0; i < 64; i++) {
        int piece = getPiece(i);
        string translated = translatePiece(piece);
        bool contains = find(moves.begin(), moves.end(), i) != moves.end();

        if (i % 8 == 0) {
            cout << " " << count << " ";
            count -= 1;
        }
        cout << "|";
        if (i == pos || contains) {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, contains ? BACKGROUND_BLUE : BACKGROUND_GREEN);
        }

        string pieceString = contains ? translateToSquareNames(i) : translated;
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
    printFiles(dashesWidth);
    printRow('-', dashesWidth, true);

    int count = 8;
    for (int i = 0; i < 64; i++) {
        int piece = getPiece(i);
        string translated = translatePiece(piece);

        if (i % 8 == 0) {
            cout << " " << count << " ";
            count -= 1;
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

void ChessBoard::printTurn() {
    cout << (currentTurn == 'w' ? "White Turn" : "Black Turn") << " - Position to Move (ex. a2), or -1 to end match: ";
}

Move ChessBoard::makeMoveObj(int from, int to) {
    int fromPiece = getPiece(from);
    int toPiece = getPiece(to);
    return make_tuple(fromPiece, toPiece, from, to);
}

void ChessBoard::makeMove(Move move, bool simulated) {
    int fromPiece = get<0>(move); int toPiece = get<1>(move);
    int from = get<2>(move); int to = get<3>(move);
    char nextMove = currentTurn == 'w' ? 'b' : 'w';
    if (fromPiece == -1) return;

    if (!simulated) {
        if (fromPiece == W_PAWN || fromPiece == B_PAWN || toPiece != EMPTY) {
            halfMoveClock = 0;
        }
        else { halfMoveClock += 1; }

        if (getPieceColor(fromPiece) == 'b') {
            fullMoveNumber += 1;
        }
    }

    enPassantCtx.clear();
    fromPiece = checkPromotion(move) ? simulated ? (currentTurn == 'w' ? W_QUEEN : B_QUEEN) : getPromotion(move) : fromPiece;
    checkCastling(move);
    checkEnPassant(move);
    positions[to] = fromPiece;
    positions[from] = EMPTY;
    
    if (fromPiece == W_PAWN || fromPiece == B_PAWN) {
        int diff = abs(to - from);
        if (diff == 16) {
            enPassantCtx[to] = from;
        }
    }

    currentTurn = nextMove;
    if (fromPiece == W_KING) whiteKingPos = to;
    else if (fromPiece == B_KING) blackKingPos = to;
    if (toPiece == W_KING) whiteKingPos = -1;
    else if (toPiece == B_KING) blackKingPos = -1;
}

bool ChessBoard::isCheckmate() {
    if (isInCheck(currentTurn)) {
        for (int pos = 0; pos < 64; pos++) {
            int piece = getPiece(pos);
            if (piece == -1 || getPieceColor(piece) != currentTurn) continue;

            vector<int> moves = generateMoves(pos);
            if (moves.size() > 0) return false;
        }
        
        return true;
    }

    return false;
}

bool ChessBoard::isStalemate() {
    if (halfMoveClock >= 50) {
        return true;
    }

    for (int pos = 0; pos < 64; pos++) {
        int piece = getPiece(pos);
        if (piece == -1 || getPieceColor(piece) != currentTurn) continue;

        vector<int> moves = generateMoves(pos);
        if (moves.size() > 0) return false;
    }

    return true;
}

char ChessBoard::getCurrentTurn() {
    return currentTurn;
}

bool ChessBoard::checkPromotion(Move move) {
    int fromPiece = get<0>(move); int toPiece = get<1>(move);
    int from = get<2>(move); int to = get<3>(move);
    int toRank = floor(to / 8);

    if (fromPiece == W_PAWN || fromPiece == B_PAWN) {
        if (fromPiece == W_PAWN && toRank == 0) return true;
        if (fromPiece == B_PAWN && toRank == 7) return true;
    }

    return false;
}

int ChessBoard::getPromotion(Move move) {
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

void ChessBoard::checkCastling(Move move) {
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

        return;
    }

    if (fromPiece == W_ROOK || fromPiece == B_ROOK) {
        if (from == 0 || from == 7) {
            blackCastle = from == 0 ? make_tuple(false, rightCastle) : make_tuple(leftCastle, false);
        }
        else if (from == 56 || from == 63) {
            whiteCastle = from == 56 ? make_tuple(false, rightCastle) : make_tuple(leftCastle, false);
        }
    }

    if (to == 0 || to == 7 || to == 56 || to == 63) {
        if (to == 0 || to == 7) {
            blackCastle = to == 0 ? make_tuple(false, rightCastle) : make_tuple(leftCastle, false);
        }
        else {
            whiteCastle = to == 0 ? make_tuple(false, rightCastle) : make_tuple(leftCastle, false);
        }
    }
}

void ChessBoard::checkEnPassant(Move move) {
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
                if (i < 4 && (piece == getTargetedPieceType("rook", enemyColor) || piece == getTargetedPieceType("queen", enemyColor))) return true;
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

bool ChessBoard::checkPawn(int targetPos, int kingPos, int enemyColorBit) {
    if (targetPos < 0 || targetPos >= 64) return false;
    if (abs((targetPos & 7) - (kingPos & 7)) != 1) return false;

    int piece = getPiece(targetPos);
    return (piece != EMPTY && piece != -1 &&
        (piece & 8) == enemyColorBit &&
        (piece & 7) == W_PAWN);
}

string ChessBoard::translateToSquareNames(int pos, bool isEnPassant) {
    int rank = 8 - floor(pos / 8);
    if (isEnPassant) {
        char pieceColor = getPieceColor(getPiece(pos));
        rank += (pieceColor == 'w' ? -1 : 1);
    }
    int file = abs(pos - floor(pos / 8) * 8);
    char strFile = static_cast<char>(file + 'a');
    string squareName = createMessage("{}{}", strFile, rank);
    return squareName;
}

int ChessBoard::translateToPos(string squareNames) {
    set<char> validFiles = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    set<int> validRanks = { '1', '2', '3', '4', '5', '6', '7', '8' };

    if (squareNames == "-1") return -1;
    if (squareNames.size() != 2) return -3;

    char file = squareNames.at(0);
    char rank = squareNames.at(1);
    if (validFiles.find(file) == validFiles.end()) return -3;
    if (validRanks.find(rank) == validRanks.end()) return -3;

    int fileNum = file - 'a';
    int rankNum = '8' - rank;
    return rankNum * 8 + fileNum;
}