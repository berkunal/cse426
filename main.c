#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Global Constants
const char PLAYER_1_PIECE = 'x';
const char PLAYER_2_PIECE = 'o';

// Global Variables
char board[7][7];
int player;
char playerPiece;
char botPiece;
int pieceCount;
int turnCount;

// Function Definitions
void printBoard();
void configureBoardParameters();
void putPiecesToRandomCoordinates(char piece);
void putPiecesToUserDefinedCoordinates();
void initializePieces();
void initializeEmptyBoard();
void selectPlayer();
void flush();
void startGameLoop();
void botMove();
void playerMove();
int isPlayerAbleToMakeMove(char piece);
int validMoveableSpaceCount(char piece);
int gameEndsWithNoValidMoveRule(int turn);
void gameEndsWithTurnLimitReached();
void getTargetPieceLocationFromUserAndMakeMove(char *pieceCoordinate,
                                               char *targetCoordinate,
                                               int pieceRow, int pieceColumn,
                                               int *targetRow,
                                               int *targetColumn);
void getPieceToMoveFromUser(char *pieceCoordinate, int *pieceRow,
                            int *pieceColumn);
int isValidMove(int pieceRow, int pieceColumn, int targetRow, int targetColumn);
int isAlreadyCounted(int validMoveableSpace[50], int i, int j);

int main() {
    initializeEmptyBoard();
    srand(time(NULL));
    configureBoardParameters();
    initializePieces();
    selectPlayer();
    startGameLoop();

    return 0;
}

// Main game loop
void startGameLoop() {
    int currentTurnCount = 0;
    int playerTurn = player % 2;
    while (currentTurnCount < turnCount * 2) {
        if (gameEndsWithNoValidMoveRule(playerTurn)) {
            return;
        }

        if (playerTurn) {
            printf("TURN #%d\n", currentTurnCount / 2 + 1);
            playerMove();
            --playerTurn;
        } else {
            botMove();
            ++playerTurn;
        }
        ++currentTurnCount;
        printBoard();
    }

    gameEndsWithTurnLimitReached();
}

void gameEndsWithTurnLimitReached() {
    int validMoveableSpaceCountForPlayer = validMoveableSpaceCount(playerPiece);
    int validMoveableSpaceCountForBot = validMoveableSpaceCount(botPiece);

    if (validMoveableSpaceCountForPlayer > validMoveableSpaceCountForBot) {
        printf("You won!\n");
        return;
    } else if (validMoveableSpaceCountForPlayer ==
               validMoveableSpaceCountForBot) {
        printf("Draw!\n");
        return;
    } else {
        printf("You lost!\n");
        return;
    }
}

int gameEndsWithNoValidMoveRule(int playerTurn) {
    if (playerTurn && !isPlayerAbleToMakeMove(playerPiece)) {
        printf("You lost!\n");
        return 1;
    } else if (!playerTurn && !isPlayerAbleToMakeMove(botPiece)) {
        printf("You won!\n");
        return 1;
    } else {
        return 0;
    }
}

int validMoveableSpaceCount(char piece) {
    int validMoveableSpace[50] = {[0 ... 49] = 99};
    int next = 0;

    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            if (board[i][j] == piece) {
                if (i - 1 != -1 && board[i - 1][j] == ' ' &&
                    !isAlreadyCounted(validMoveableSpace, i - 1, j)) {
                    validMoveableSpace[next] = (i - 1) * 10 + j;
                    ++next;
                }
                if (i + 1 != 7 && board[i + 1][j] == ' ' &&
                    !isAlreadyCounted(validMoveableSpace, i + 1, j)) {
                    validMoveableSpace[next] = (i + 1) * 10 + j;
                    ++next;
                }
                if (j - 1 != -1 && board[i][j - 1] == ' ' &&
                    !isAlreadyCounted(validMoveableSpace, i, j - 1)) {
                    validMoveableSpace[next] = i * 10 + (j - 1);
                    ++next;
                }
                if (j + 1 != 7 && board[i][j + 1] == ' ' &&
                    !isAlreadyCounted(validMoveableSpace, i, j + 1)) {
                    validMoveableSpace[next] = i * 10 + (j + 1);
                    ++next;
                }
            }
        }
    }

    int count = 0;
    for (int i = 0; i < next; ++i) {
        if (validMoveableSpace[i] != -1) {
            ++count;
        }
    }
    return count;
}

int isAlreadyCounted(int validMoveableSpace[50], int i, int j) {
    for (int k = 0; k < 50; ++k) {
        if (validMoveableSpace[k] == i * 10 + j) {
            return 1;
        }
    }
    return 0;
}

int isPlayerAbleToMakeMove(char piece) {
    if (validMoveableSpaceCount(piece) == 0) {
        return 0;
    } else {
        return 1;
    }
}

void playerMove() {
    char pieceCoordinate[3], targetCoordinate[3];
    int pieceRow, pieceColumn, targetRow, targetColumn;

    getPieceToMoveFromUser(pieceCoordinate, &pieceRow, &pieceColumn);
    getTargetPieceLocationFromUserAndMakeMove(pieceCoordinate, targetCoordinate,
                                              pieceRow, pieceColumn, &targetRow,
                                              &targetColumn);

    printf("Player moves the piece at %s to %s", pieceCoordinate,
           targetCoordinate);
}

void getPieceToMoveFromUser(char *pieceCoordinate, int *pieceRow,
                            int *pieceColumn) {
    int invalidInput = 1;
    while (invalidInput) {
        flush();
        printf("Choose piece to move: ");
        scanf("%s", pieceCoordinate);

        (*pieceRow) = pieceCoordinate[0] - 'a';
        (*pieceColumn) = pieceCoordinate[1] - '0' - 1;

        if (board[(*pieceRow)][(*pieceColumn)] != playerPiece) {
            printf(
                "You don't have a piece at that location! Please try again!\n");
        } else {
            invalidInput = 0;
        }
    }
}

void getTargetPieceLocationFromUserAndMakeMove(char *pieceCoordinate,
                                               char *targetCoordinate,
                                               int pieceRow, int pieceColumn,
                                               int *targetRow,
                                               int *targetColumn) {
    int invalidInput = 1;
    while (invalidInput) {
        flush();
        printf("Choose the new position for %s: ", pieceCoordinate);
        scanf("%s", targetCoordinate);

        (*targetRow) = targetCoordinate[0] - 'a';
        (*targetColumn) = targetCoordinate[1] - '0' - 1;

        if (isValidMove(pieceRow, pieceColumn, (*targetRow), (*targetColumn))) {
            board[pieceRow][pieceColumn] = ' ';
            board[(*targetRow)][(*targetColumn)] = playerPiece;
            invalidInput = 0;
        } else {
            printf(
                "That is not a valid square! Try vertical and horizontal "
                "neighbours "
                "with no pieces!\n");
        }
    }
}

int isValidMove(int pieceRow, int pieceColumn, int targetRow,
                int targetColumn) {
    return (((pieceRow + 1 == targetRow || pieceRow - 1 == targetRow) &&
             (pieceColumn == targetColumn)) ||
            ((pieceColumn + 1 == targetColumn ||
              pieceColumn - 1 == targetColumn) &&
             (pieceRow == targetRow))) &&
           board[targetRow][targetColumn] == ' ';
}

// TODO: implement
void botMove() {
    char pieceCoordinate[3] = "a1",
         targetCoordinate[3] = "a1";  // TODO: dont overlook
    printf("Bot moves the piece at %s to %s", pieceCoordinate,
           targetCoordinate);
}

// Board Printer
void printBoard() {
    printf("\n   1 2 3 4 5 6 7 \n");
    for (int i = 0; i < 7; ++i) {
        char rowNameList[7] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};
        printf("%c |", rowNameList[i]);

        for (int j = 0; j < 7; ++j) {
            printf("%c|", board[i][j]);
        }
        printf("\n");
    }
}

// Get initial game configuration from the user
void configureBoardParameters() {
    printf("Please enter how many piece each player will get(min 1, max 24): ");
    scanf("%d", &pieceCount);

    printf("Please enter turn limit for the game(min 1): ");
    scanf("%d", &turnCount);
}

void initializePieces() {
    char isRandomPlacement;
    flush();
    printf("Do you want the locations of the pieces randomized? (y/n): ");
    scanf("%c", &isRandomPlacement);

    if (isRandomPlacement == 'y') {
        putPiecesToRandomCoordinates(PLAYER_1_PIECE);
        putPiecesToRandomCoordinates(PLAYER_2_PIECE);
    } else if (isRandomPlacement == 'n') {
        putPiecesToUserDefinedCoordinates();
    }
}

// Put given player's all pieces to board matrix
void putPiecesToRandomCoordinates(char piece) {
    int currentPieceCount = 0;

    while (currentPieceCount < pieceCount) {
        int i = rand() % 7;
        int j = rand() % 7;

        if (board[i][j] == ' ') {
            board[i][j] = piece;
            currentPieceCount++;
        }
    }
}

// Interact with user and fill the board with pieces
void putPiecesToUserDefinedCoordinates() {
    int currentPieceCount = 0;
    char piece = PLAYER_1_PIECE;
    char playerName[] = "Player1";

    while (currentPieceCount < pieceCount * 2) {
        if (currentPieceCount == pieceCount) {
            piece = PLAYER_2_PIECE;
            strcpy(playerName, "Player2");
        }

        char pieceCoordinate[3];
        flush();
        printf("Please enter coordinate of piece #%d for %s: ",
               (currentPieceCount % 5) + 1, playerName);
        scanf("%s", pieceCoordinate);

        int i = pieceCoordinate[0] - 'a';
        int j = pieceCoordinate[1] - '0' - 1;

        if (board[i][j] == ' ') {
            board[i][j] = piece;
            currentPieceCount++;
        } else {
            printf(
                "This coordinate is occupied by another piece or does not "
                "exist!\n");
        }
    }
}

void selectPlayer() {
    printf("This is how the board looks like: ");
    printBoard();
    printf("Do you want to play as Player1(%c) or Player2(%c)? (1/2): ",
           PLAYER_1_PIECE, PLAYER_2_PIECE);
    scanf("%d", &player);
    playerPiece = (player == 1) ? PLAYER_1_PIECE : PLAYER_2_PIECE;
    botPiece = (player == 1) ? PLAYER_2_PIECE : PLAYER_1_PIECE;
}

void initializeEmptyBoard() {
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            board[i][j] = ' ';
        }
    }
}

// Function that empties the buffer of stdin to avoid bugs during scanf reads
void flush() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}
