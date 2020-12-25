#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Global Constants
#define MAX_DEPTH 6  // max minimax depth
#define PLAYER_1_PIECE 'x'
#define PLAYER_2_PIECE 'o'

// Global Variables
char board[7][7];
int player;
char playerPiece;
char botPiece;
int pieceCount;
int turnLimit;
struct Move {
    int pieceI;
    int pieceJ;
    int targetI;
    int targetJ;
    struct Move *next;
};

// Function Declerations
void printBoard();
void configureBoardParameters();
void putPiecesToRandomCoordinates(char piece);
void putPiecesToUserDefinedCoordinates();
void initializePieces();
void initializeEmptyBoard();
void selectPlayer();
void flush();
void startGameLoop();
void botMove(int currentTurnCount);
void playerMove();
int isPlayerAbleToMakeMove(char piece);
int getValidMoveableSpaceCount(char piece);
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
struct Move *getPossibleMoves(char piece);
void pushMove(struct Move **move, int pieceI, int pieceJ, int targetI,
              int targetJ);
void applyMove(struct Move *move);
void undoMove(struct Move *move);
char *convertIndexesToCoordinate(int i, int j);
int minimax(int depth, int maximizingPlayer, int playerTurn, int alpha,
            int beta);
int staticEval();
void freeList(struct Move *head);

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
    while (currentTurnCount < turnLimit * 2) {
        // Check if game ended because current player has no other move
        if (gameEndsWithNoValidMoveRule(playerTurn)) {
            return;
        }

        if (playerTurn) {
            printf("TURN #%d\n", currentTurnCount / 2 + 1);
            playerMove();
            --playerTurn;
        } else {
            botMove(currentTurnCount);
            ++playerTurn;
        }
        ++currentTurnCount;
        printBoard();
    }

    gameEndsWithTurnLimitReached();
}

// After turn limit reached, check who won
void gameEndsWithTurnLimitReached() {
    int validMoveableSpaceCountForPlayer =
        getValidMoveableSpaceCount(playerPiece);
    int validMoveableSpaceCountForBot = getValidMoveableSpaceCount(botPiece);

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

// Return 1 if current turn player is not able to make a move therefore game is
// ended
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

// Loop through the board and find all valid spaces that given piece can go
int getValidMoveableSpaceCount(char piece) {
    int count = 0;

    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            if (board[i][j] == piece) {
                if (i - 1 != -1 && board[i - 1][j] == ' ') {
                    ++count;
                }
                if (i + 1 != 7 && board[i + 1][j] == ' ') {
                    ++count;
                }
                if (j - 1 != -1 && board[i][j - 1] == ' ') {
                    ++count;
                }
                if (j + 1 != 7 && board[i][j + 1] == ' ') {
                    ++count;
                }
            }
        }
    }

    return count;
}

// Return 1 if player is able to make a move in current game state
int isPlayerAbleToMakeMove(char piece) {
    if (getValidMoveableSpaceCount(piece) == 0) {
        return 0;
    } else {
        return 1;
    }
}

// Main player movement function
void playerMove() {
    char pieceCoordinate[3], targetCoordinate[3];
    int pieceRow, pieceColumn, targetRow, targetColumn;

    // Get a piece to move, from User, then assign it to pieceCoordinate var
    getPieceToMoveFromUser(pieceCoordinate, &pieceRow, &pieceColumn);
    // Get target coordinate, from User, then assign it to targetCoordinate var
    getTargetPieceLocationFromUserAndMakeMove(pieceCoordinate, targetCoordinate,
                                              pieceRow, pieceColumn, &targetRow,
                                              &targetColumn);

    printf("Player moves the piece at %s to %s", pieceCoordinate,
           targetCoordinate);
}

void getPieceToMoveFromUser(char *pieceCoordinate, int *pieceRow,
                            int *pieceColumn) {
    int invalidInput = 1;

    // Error handling using infinite loop that breaks when user input is correct
    while (invalidInput) {
        flush();
        printf("Choose piece to move: ");
        scanf("%s", pieceCoordinate);

        // Transform given piece coordinate to array indexes
        (*pieceRow) = pieceCoordinate[0] - 'a';
        (*pieceColumn) = pieceCoordinate[1] - '0' - 1;

        // Check if user has a piece at given coordinate
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

    // Error handling using infinite loop that breaks when user input is correct
    while (invalidInput) {
        flush();
        printf("Choose the new position for %s: ", pieceCoordinate);
        scanf("%s", targetCoordinate);

        // Transform given piece coordinate to array indexes
        (*targetRow) = targetCoordinate[0] - 'a';
        (*targetColumn) = targetCoordinate[1] - '0' - 1;

        // If given coordinates indicates a valid move then move the piece
        if (isValidMove(pieceRow, pieceColumn, (*targetRow), (*targetColumn))) {
            board[pieceRow][pieceColumn] = ' ';
            board[(*targetRow)][(*targetColumn)] = playerPiece;
            invalidInput = 0;
        } else {
            printf(
                "That is not a valid square! Try vertical and horizontal "
                "neighbours with no pieces!\n");
        }
    }
}

// Checks if target coordinate is one square away and empty
int isValidMove(int pieceRow, int pieceColumn, int targetRow,
                int targetColumn) {
    return (((pieceRow + 1 == targetRow || pieceRow - 1 == targetRow) &&
             (pieceColumn == targetColumn)) ||
            ((pieceColumn + 1 == targetColumn ||
              pieceColumn - 1 == targetColumn) &&
             (pieceRow == targetRow))) &&
           board[targetRow][targetColumn] == ' ';
}

// Main AI movement function
void botMove(int currentTurnCount) {
    // Get all possible moves that bot can apply
    // Returns a linked list that contains all possible moves
    struct Move *possibleMove = getPossibleMoves(botPiece);

    // Linked list node that hold the best move information
    struct Move *bestMove;

    // Local varible to store the possible moves score that is gathered from
    // minimax algorithm
    int score;

    // X is maximizing player, O is minimizing player
    int maximizingPlayer = (player == 1) ? 0 : 1;

    // Best score is negative infinity if player is maximizing and vise versa
    int bestScore = maximizingPlayer ? INT_MIN : INT_MAX;

    // Adjust minimax depth according to how many turns left and maximum depth.
    // If remaining turns is less then the maximum depth then minimax will go
    // less deep
    int depth = (turnLimit - currentTurnCount / 2 < MAX_DEPTH)
                    ? (turnLimit - currentTurnCount / 2)
                    : MAX_DEPTH;

    // Loop through all possible moves
    while (possibleMove != NULL) {
        // Apply the current move
        applyMove(possibleMove);
        // Run minimax on that move
        // The next will be the players so give opposite of current
        // maximizingPlayer variable. Alpha is negative infinity and beta is
        // positive infinity at the beginning.
        score = minimax(depth, !maximizingPlayer, 1, INT_MIN, INT_MAX);
        // Undo the applied move. Now board is back to initial position
        undoMove(possibleMove);

        // Update best score and best move if requirements satisfied
        if ((maximizingPlayer && (score > bestScore)) ||
            (!maximizingPlayer && (score < bestScore))) {
            bestScore = score;
            bestMove = possibleMove;
        }

        // Go to the next move
        possibleMove = possibleMove->next;
    }

    // Apply the best move
    applyMove(bestMove);

    // Prompt user
    printf("Bot moves the piece at %s to %s",
           convertIndexesToCoordinate(bestMove->pieceI, bestMove->pieceJ),
           convertIndexesToCoordinate(bestMove->targetI, bestMove->targetJ));

    // Free the current linked list containing possible moves to save allocated
    // memory space
    freeList(possibleMove);
}

// Minimax algorithm
int minimax(int depth, int maximizingPlayer, int playerTurn, int alpha,
            int beta) {
    // If depth is 0 or game is over return static evaluation
    if (depth == 0 || gameEndsWithNoValidMoveRule(playerTurn)) {
        return staticEval();
    }

    char piece = playerTurn ? playerPiece : botPiece;
    // Get all possible moves that bot can apply
    // Returns a linked list that contains all possible moves
    struct Move *possibleMove = getPossibleMoves(piece);

    // Maximizing Player
    if (maximizingPlayer) {
        int maxEval = INT_MIN;  // Max eval is negative infinity

        // Loop through the possible moves linked list
        while (possibleMove != NULL) {
            // Apply the current move
            applyMove(possibleMove);
            // Run minimax on that move with minimizing player and decreased
            // depth. Pass current alpha, beta values
            int eval = minimax(depth - 1, 0, !playerTurn, alpha, beta);
            // Undo the applied move
            undoMove(possibleMove);

            if (eval > maxEval) maxEval = eval;  // Max evaluation update
            if (eval > alpha) alpha = eval;      // Alpha update
            if (beta <= alpha) break;            // Pruning successfull

            possibleMove = possibleMove->next;  // Go to next move
        }

        // Free the current linked list containing possible moves to save
        // allocated memory space
        freeList(possibleMove);

        // Returns the biggest score amongst all possible moves
        return maxEval;
    } else {                    // Minimizing player
        int minEval = INT_MAX;  // Max eval is positive infinity

        // Loop through the possible moves linked list
        while (possibleMove != NULL) {
            // Apply the current move
            applyMove(possibleMove);
            // Run minimax on that move with maximizing player and decreased
            // depth. Pass current alpha, beta values
            int eval = minimax(depth - 1, 1, !playerTurn, alpha, beta);
            // Undo the applied move
            undoMove(possibleMove);

            if (eval < minEval) minEval = eval;  // Min evaluation update
            if (eval < beta) beta = eval;        // Beta update
            if (beta <= alpha) break;            // Pruning successfull

            possibleMove = possibleMove->next;  // Go to next move
        }

        // Free the current linked list containing possible moves to save
        // allocated memory space
        freeList(possibleMove);
        // Returns the smallest score amongst all possible moves
        return minEval;
    }
}

// Static Evaluation function that returns valid moveable space count of x - o
int staticEval() {
    return getValidMoveableSpaceCount(PLAYER_1_PIECE) -
           getValidMoveableSpaceCount(PLAYER_2_PIECE);
}

// Returns the head od linked list containing all possible moves of given piece
struct Move *getPossibleMoves(char piece) {
    struct Move *head = NULL;

    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            if (board[i][j] == piece) {
                if (i - 1 != -1 && board[i - 1][j] == ' ') {
                    pushMove(&head, i, j, i - 1, j);
                }
                if (i + 1 != 7 && board[i + 1][j] == ' ') {
                    pushMove(&head, i, j, i + 1, j);
                }
                if (j - 1 != -1 && board[i][j - 1] == ' ') {
                    pushMove(&head, i, j, i, j - 1);
                }
                if (j + 1 != 7 && board[i][j + 1] == ' ') {
                    pushMove(&head, i, j, i, j + 1);
                }
            }
        }
    }
    return head;
}

// Creates a node with given configuration
// and pushes in to the beginning of the given linked list
void pushMove(struct Move **head, int pieceI, int pieceJ, int targetI,
              int targetJ) {
    struct Move *move = (struct Move *)calloc(1, sizeof(struct Move));

    move->pieceI = pieceI;
    move->pieceJ = pieceJ;
    move->targetI = targetI;
    move->targetJ = targetJ;
    move->next = *head;
    *head = move;
}

// Applies the given move on to the board
void applyMove(struct Move *move) {
    board[move->targetI][move->targetJ] = board[move->pieceI][move->pieceJ];
    board[move->pieceI][move->pieceJ] = ' ';
}

// Undoes the given move on to the board
void undoMove(struct Move *move) {
    board[move->pieceI][move->pieceJ] = board[move->targetI][move->targetJ];
    board[move->targetI][move->targetJ] = ' ';
}

// Convert array indexes to human readable coordinate representation
char *convertIndexesToCoordinate(int i, int j) {
    char *coordinate = malloc(sizeof(char) * 3);
    coordinate[0] = i + 'a';
    coordinate[1] = j + '0' + 1;
    coordinate[2] = '\0';
    return coordinate;
}

// Frees the allocated space in memory of the given linked list
void freeList(struct Move *head) {
    struct Move *tmp;

    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
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
    scanf("%d", &turnLimit);
}

// Put pieces in to the board according to user's choice
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

// Randomly put given player's all pieces to board matrix
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
               (currentPieceCount % pieceCount) + 1, playerName);
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
