#include <stdio.h>

#define WIN 1000
#define LOSS -1000

_Bool GAME_STATUS = 1; // 1 or 0 (playing or win/draw)
_Bool PLAYER = 0;      // 1 or 0 (player or computer)

void legalMovesGenerator(char board[8][8][2], int startingPosition[2], int legalMoves[][2], int *legalMoveIndex, char char_currentPlayer);

void allLegalPlayerMoves(char board[8][8][2], int moves[][2], int *lastIndexInMoves, char char_player);

void checkingPositionsGenerator(char board[8][8][2], int positionsArray[][2], int *arrayLengh, int kingsRow, int kingsCol, char char_currentPlayer, char char_opositePlayer);

// Render board with coordinates
void renderBoard(char board[8][8][2])
{
    char collumnIndexes[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

    printf("\n");
    for (int row = 0; row < 8; row++)
    {
        printf("%d   ", 8 - row);
        for (int column = 0; column < 8; column++)
        {
            (board[row][column][1] == '0') ? printf("\033[0;32m") : printf("\033[0;36m");
            (board[row][column][0] == ' ') ? printf("   ") : printf(" %c ", board[row][column][0]);
            printf("\033[0;37m");

            if (column != 7)
                printf("|");
        }

        printf("\n");

        if (row != 7)
            printf("    -------------------------------\n");
    }

    printf("\n     ");
    for (int i = 0; i < 8; i++)
        printf("%c   ", collumnIndexes[i]);

    printf("\n\n");
}

// Render board with posible moves
void renderBoardWithMoves(char board[8][8][2], int legalMoves[][2], int legalMovesLength)
{
    char collumnIndexes[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

    printf("\n");
    for (int row = 0; row < 8; row++)
    {
        printf("%d   ", 8 - row);

        for (int column = 0; column < 8; column++)
        {
            _Bool isLegal = 0;
            for (int i = 0; i < legalMovesLength; i++)
            {
                if (row == legalMoves[i][0] && column == legalMoves[i][1])
                {
                    printf("\033[0;31m");
                    (board[row][column][0] == ' ') ? printf(" # ") : printf(" %c ", board[row][column][0]);
                    isLegal = 1;
                    break;
                }
            }

            if (!isLegal)
            {
                (board[row][column][1] == '0') ? printf("\033[0;32m") : printf("\033[0;36m");
                (board[row][column][0] == ' ') ? printf("   ") : printf(" %c ", board[row][column][0]);
            }

            printf("\033[0;37m");

            if (column != 7)
                printf("|");
        }

        printf("\n");

        if (row != 7)
            printf("    -------------------------------\n");
    }

    printf("\n     ");
    for (int i = 0; i < 8; i++)
        printf("%c   ", collumnIndexes[i]);

    printf("\n\n");
}

// Parses input to coordinates array
// input: B3 -> output: [2, 5]
void parseInput(int inputRow, char inputColumn, int parsedPosition[2])
{
    char collumnIndexes[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

    int inputColumnIndex = -1;
    for (int i = 0; i < 8; i++)
    {
        if (inputColumn == collumnIndexes[i])
        {
            inputColumnIndex = i;
            break;
        }
    }

    parsedPosition[0] = 8 - inputRow;
    parsedPosition[1] = inputColumnIndex;
}

// Asks player to select figure to move
void askForPosition(int startingPosition[2], char prompt[])
{

    char move[2];

    (PLAYER == 0) ? printf("\033[0;32m") : printf("\033[0;36m");

    // Ask for starting position
    printf("%s", prompt);
    scanf("%s", move);

    printf("\033[0;37m");

    int row = move[1] - '0';
    char column = move[0];

    // Parse input to array of coordinates
    parseInput(row, column, startingPosition);
}

// Updates placement of the pawns on the board after move
void updateBoard(char board[8][8][2], int startingPosition[2], int endingPosition[2])
{

    board[endingPosition[0]][endingPosition[1]][0] = board[startingPosition[0]][startingPosition[1]][0];
    board[endingPosition[0]][endingPosition[1]][1] = PLAYER + '0';

    board[startingPosition[0]][startingPosition[1]][0] = ' ';
    board[startingPosition[0]][startingPosition[1]][1] = ' ';
}

// Deletes element from 2D array
void deleteElement(int array[][2], int indexOfElementToDelete, int *lastIndex) 
{
    for (int i = indexOfElementToDelete; i < *lastIndex; i++)
    {
        array[i][0] = array[i + 1][0];
        array[i][1] = array[i + 1][1];
    }
    --*lastIndex;
}

// Appends move to legal moves array starting from given index
void appendToLegalMoves(int legalMoves[][2], int *i, int row, int col)
{
    legalMoves[*i][0] = row;
    legalMoves[*i][1] = col;
    ++*i;
}

/*
 * MOVE GENERATORS FOR SELECTED PAWNS
*/

// Generates legal moves for a selected pawn and appends them to legalMoves array
void legalMovesForPawn(char board[8][8][2], int legalMoves[][2], int *i, int startRow, int startCol, char char_opositePlayer)
{
    int vector = (char_opositePlayer == '0') ? 1 : -1;

    if ((board[startRow + vector][startCol][1] == ' ') && startRow + vector < 8 && startRow + vector > -1)
        appendToLegalMoves(legalMoves, i, startRow + vector, startCol);

    if (startRow + vector < 8 && startRow + vector > -1)
    {

        if (board[startRow + vector][startCol + 1][1] == char_opositePlayer && startCol + 1 < 8)
            appendToLegalMoves(legalMoves, i, startRow + vector, startCol + 1);

        if (board[startRow + vector][startCol - 1][1] == char_opositePlayer && startCol - 1 > -1)
            appendToLegalMoves(legalMoves, i, startRow + vector, startCol - 1);
    }
}

// Checking function for legal move in a line (staight line o diagonal). Returns 1 if there are no more legal moves in a line
_Bool legalMoveInALine(char board[8][8][2], int legalMoves[][2], int *i, int row, int col, char char_currentPlayer, char char_opositePlayer) {
    if (board[row][col][1] != char_currentPlayer) {
        appendToLegalMoves(legalMoves, i, row, col);

        if (board[row][col][1] == char_opositePlayer)
            return 1;
    }
    else if (board[row][col][1] == char_currentPlayer)
        return 1;

    return 0;
}

// Generates legal moves for a selected rook and appends them to legalMoves array
void legalMovesForRook(char board[8][8][2], int legalMoves[][2], int *i, int startRow, int startCol, char char_currentPlayer, char char_opositePlayer)
{
    // Check upwards
    for (int row = startRow - 1; row >= 0; row--)
        if(legalMoveInALine(board, legalMoves, i, row, startCol, char_currentPlayer, char_opositePlayer))
            break;

    // Check downwards
    for (int row = startRow + 1; row < 8; row++)
        if(legalMoveInALine(board, legalMoves, i, row, startCol, char_currentPlayer, char_opositePlayer))
            break;

    // Check right
    for (int col = startCol + 1; col < 8; col++)
        if(legalMoveInALine(board, legalMoves, i, startRow, col, char_currentPlayer, char_opositePlayer))
            break;

    // Check left
    for (int col = startCol - 1; col >= 0; col--)
        if(legalMoveInALine(board, legalMoves, i, startRow, col, char_currentPlayer, char_opositePlayer))
            break;
}

// Generates legal moves for selected bishop and appends them to legalMoves array
void legalMovesForBishop(char board[8][8][2], int legalMoves[][2], int *i, int startRow, int startCol, char char_currentPlayer, char char_opositePlayer)
{
    // Check up/righ diagonal
    for (int row = startRow - 1, col = startCol + 1; row >= 0 && col < 8; row--, col++)
        if(legalMoveInALine(board, legalMoves, i, row, col, char_currentPlayer, char_opositePlayer))
            break;

    // Check up/left diagonal
    for (int row = startRow - 1, col = startCol - 1; row >= 0 && col >= 0; row--, col--)
        if(legalMoveInALine(board, legalMoves, i, row, col, char_currentPlayer, char_opositePlayer))
            break;

    // Check down/righ diagonal
    for (int row = startRow + 1, col = startCol + 1; row < 8 && col < 8; row++, col++)
        if(legalMoveInALine(board, legalMoves, i, row, col, char_currentPlayer, char_opositePlayer))
            break;

    // Check down/left diagonal
    for (int row = startRow + 1, col = startCol - 1; row < 8 && col >= 0; row++, col--)
        if(legalMoveInALine(board, legalMoves, i, row, col, char_currentPlayer, char_opositePlayer))
            break;
}

// Helping function for legalMovesForKnight function
void legalKnightMoveInAPosition(char board[8][8][2], int legalMoves[][2], int *i, char char_opositePlayer, int row, int col)
{
    if ((board[row][col][1] == ' ' || board[row][col][1] == char_opositePlayer) && row >= 0 && row < 8 && col >= 0 && col < 8)
        appendToLegalMoves(legalMoves, i, row, col);
}

// Generates legal moves for selected knight and appends them to legalMoves array
void legalMovesForKnight(char board[8][8][2], int legalMoves[][2], int *i, int startRow, int startCol, char char_opositePlayer)
{
    legalKnightMoveInAPosition(board, legalMoves, i, char_opositePlayer, startRow - 2, startCol + 1);
    legalKnightMoveInAPosition(board, legalMoves, i, char_opositePlayer, startRow - 2, startCol - 1);

    legalKnightMoveInAPosition(board, legalMoves, i, char_opositePlayer, startRow - 1, startCol + 2);
    legalKnightMoveInAPosition(board, legalMoves, i, char_opositePlayer, startRow - 1, startCol - 2);

    legalKnightMoveInAPosition(board, legalMoves, i, char_opositePlayer, startRow + 1, startCol + 2);
    legalKnightMoveInAPosition(board, legalMoves, i, char_opositePlayer, startRow + 1, startCol - 2);

    legalKnightMoveInAPosition(board, legalMoves, i, char_opositePlayer, startRow + 2, startCol + 1);
    legalKnightMoveInAPosition(board, legalMoves, i, char_opositePlayer, startRow + 2, startCol - 1);
}

// Helping function for legalMovesForKing function
void legalKingMoveInAPosition(char board[8][8][2], int legalMoves[][2], int *i, char char_opositePlayer, int moveRow, int moveCol)
{
    if (board[moveRow][moveCol][1] == ' ' || board[moveRow][moveCol][1] == char_opositePlayer)
        appendToLegalMoves(legalMoves, i, moveRow, moveCol);
}

// Generates basic 8 legal moves for king
void generateBasicLegalMovesForKing(char board[8][8][2], int legalMoves[][2], int *lastIndex, int kingsRow, int kingsCol, char char_opositePlayer)
{
    // Generate legal moves for king
    for (int n = -1; n <= 1; n++)
    {
        legalKingMoveInAPosition(board, legalMoves, lastIndex, char_opositePlayer, kingsRow - 1, kingsCol + n);
        legalKingMoveInAPosition(board, legalMoves, lastIndex, char_opositePlayer, kingsRow + 1, kingsCol + n);
        if (n)
            legalKingMoveInAPosition(board, legalMoves, lastIndex, char_opositePlayer, kingsRow, kingsCol + n);
    }

    legalMoves[*lastIndex][0] = -1;
}

// Generates legal moves for selected king and appends them to legalMoves array
void legalMovesForKing(char board[8][8][2], int legalMoves[][2], int *lastIndex, int kingsRow, int kingsCol, char char_currentPlayer, char char_opositePlayer)
{

    // Generate at most 8 basic legal moves for current players king
    generateBasicLegalMovesForKing(board, legalMoves, lastIndex, kingsRow, kingsCol, char_opositePlayer);

    // Generate all positions from which oponent can check
    int checkingMoves[200][2];
    int checkingMovesLength = 0;

    checkingPositionsGenerator(board, checkingMoves, &checkingMovesLength, kingsRow, kingsCol, char_currentPlayer, char_opositePlayer);  

    // Delete moves from the legalMoves array that are in checkingMoves array
    for (int legalMoveIndex = 0; legalMoves[legalMoveIndex][0] != -1; legalMoveIndex++)
    {
        for (int i = 0; i < checkingMovesLength; i++)
        {
            if (legalMoves[legalMoveIndex][0] == checkingMoves[i][0] && legalMoves[legalMoveIndex][1] == checkingMoves[i][1])
            {
                deleteElement(legalMoves, legalMoveIndex, lastIndex);
                legalMoveIndex--;
                break;
            }
        }
    }
}

// Appends all legal moves of a chosen player to moves array
void allLegalPlayerMoves(char board[8][8][2], int moves[][2], int *lastIndexInMoves, char char_player)
{
    for (int row = 0; row < 8; row++)
    {
        for (int column = 0; column < 8; column++)
        {
            if (board[row][column][1] == char_player)
            {
            }
        }
    }
}

// Appends all positions that check the currents player king to a positionsArray
void checkingPositionsGenerator(char board[8][8][2], int positionsArray[][2], int *arrayLength, int kingsRow, int kingsCol, char char_currentPlayer, char char_opositePlayer) {
    board[kingsRow][kingsCol][0] = ' ';
    board[kingsRow][kingsCol][1] = ' ';

    for (int row = 0; row < 8; row++)
    {
        for (int column = 0; column < 8; column++)
        {
            if (board[row][column][1] == char_opositePlayer)
            {
                int position[2] = {row, column};

                if (board[row][column][0] == 'P')
                {
                    char emptyBoard[8][8][2] = {
                        {{' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}},
                        {{' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}},
                        {{' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}},
                        {{' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}},
                        {{' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}},
                        {{' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}},
                        {{' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}},
                        {{' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}}};

                    emptyBoard[row][column][0] = 'P';
                    emptyBoard[row][column][1] = char_opositePlayer;

                    int vector = (char_opositePlayer == '0') ? -1 : 1;

                    if (row + vector < 8 && row + vector > -1)
                    {
                        if (column+1 < 8)
                            appendToLegalMoves(positionsArray, arrayLength, row + vector, column + 1);

                        if (column-1 > -1)
                            appendToLegalMoves(positionsArray, arrayLength, row + vector, column - 1);
                    }
                }
                else if (board[row][column][0] == 'K')
                {
                    generateBasicLegalMovesForKing(board, positionsArray, arrayLength, row, column, char_opositePlayer);
                }
                else
                {
                    legalMovesGenerator(board, position, positionsArray, arrayLength, char_currentPlayer);
                }
            }
        }
    }
    positionsArray[*arrayLength][0] = -1;

    board[kingsRow][kingsCol][0] = 'K';
    board[kingsRow][kingsCol][1] = char_currentPlayer;
}

// Appends legal moves of a chosen figure to array
void legalMovesGenerator(char board[8][8][2], int startingPosition[2], int legalMoves[][2], int *lastIndex, char char_currentPlayer)
{

    char pawnType = board[startingPosition[0]][startingPosition[1]][0];

    char char_opositePlayer = (char_currentPlayer == '0') ? '1' : '0';

    int startRow = startingPosition[0];
    int startCol = startingPosition[1];

    // Generate all moves
    switch (pawnType)
    {
    // 1) Legal moves for pawn
    case 'P':
        legalMovesForPawn(board, legalMoves, lastIndex, startRow, startCol, char_opositePlayer);
        break;
    // 2) Legal moves for rook
    case 'W':
        legalMovesForRook(board, legalMoves, lastIndex, startRow, startCol, char_currentPlayer, char_opositePlayer);
        break;
    // 3) Legal moves for bishop
    case 'G':
        legalMovesForBishop(board, legalMoves, lastIndex, startRow, startCol, char_currentPlayer, char_opositePlayer);
        break;
    // 4) Legal moves for knight
    case 'S':
        legalMovesForKnight(board, legalMoves, lastIndex, startRow, startCol, char_opositePlayer);
        break;
    // 5) Legal moves for hetman
    case 'H':
        legalMovesForRook(board, legalMoves, lastIndex, startRow, startCol, char_currentPlayer, char_opositePlayer);
        legalMovesForBishop(board, legalMoves, lastIndex, startRow, startCol, char_currentPlayer, char_opositePlayer);
        break;
    // 6) Legal moves for king
    case 'K':
        legalMovesForKing(board, legalMoves, lastIndex, startRow, startCol, char_currentPlayer, char_opositePlayer);
        break;
    }

    legalMoves[*lastIndex][0] = -1;
}

// Checks if players move was valid with the rules
_Bool isMoveLegal(char board[8][8][2], int startingPosition[2], int endingPosition[2], int legalMoves[][2])
{

    _Bool isLegal = 0;

    int startRow = startingPosition[0];
    int startCol = startingPosition[1];
    int endRow = endingPosition[0];
    int endCol = endingPosition[1];

    char startingPawn[2] = {board[startRow][startCol][0], board[startRow][startCol][1]}; // pawn ex. {'W', '0'}
    char endingPawn[2] = {board[endRow][endCol][0], board[endRow][endCol][1]};           // pawn ex. {' ', ' '}

    // Check if chosen position is inside the board
    if (endCol == -1 || endRow < 0 || endRow > 7) {
        printf("\n* Zaznaczyłeś pozycję poza planszą. Spróbuj pawnownie. *\n\n");
        return isLegal;
    }

    // Check if player moved his pawn on his pawn
    if (board[startRow][startCol][1] == board[endRow][endCol][1]) {
        printf("\n* Próbowałeś zbić swojego pionka. Spróbuj ponownie. *\n\n");
        return isLegal;
    }

    // Check if player tried moving his pawn on the same position
    if (startRow == endRow && startCol == endCol) {
        printf("\nPróbowałeś ruszyć się pionekiem w to samo miejsce. Spróbuj ponownie *\n\n");
        return isLegal;
    }

    // Check if chosen move is legal
    for (int i = 0; i < 30; i++)
    {
        // Check if move is legal
        if (legalMoves[i][0] != -1)
        {
            if (legalMoves[i][0] == endRow && legalMoves[i][1] == endCol)
            {
                isLegal = 1;
            }
        }
        else
        {
            break;
        }
    }

    if (!isLegal)
        printf("\n* Próbowałeś dokonać nielegalnego ruchu *\n\n");

    return isLegal;
}

// Checks if chosen pawn is legal
_Bool isSelectedFigureLegal(char board[8][8][2], int startingPosition[2])
{

    char startingPawn[2] = {
        board[startingPosition[0]][startingPosition[1]][0],
        board[startingPosition[0]][startingPosition[1]][1]}; // ex. startingPawn = {'W', '0'}

    // Check if starting position and ending position isn't outside the board
    if (startingPosition[1] == -1 || startingPosition[0] < 0 || startingPosition[0] > 7) {
        printf("* Zaznaczyłeś pozycję poza planszą. Spróbuj ponownie. *\n\n");
        return 0;
    }

    // Check if player chose his pawn
    if (startingPawn[1] - '0' == !PLAYER) {
        printf("\n* Próbowałeś ruszyć pionka przeciwnika. Spróbuj ponownie. *\n\n");
        return 0;
    }
    else if (startingPawn[1] == ' ') {
        printf("\n* Próbowałeś ruszyć puste pole. Spróbuj ponownie. *\n\n");
        return 0;
    }

    return 1;
}

int main()
{

    char board[8][8][2] = {
        {{'W', '1'}, {'S', '1'}, {'G', '1'}, {' ', ' '}, {' ', ' '}, {'G', '1'}, {'S', '1'}, {'W', '1'}},
        {{'P', '1'}, {' ', ' '}, {'P', '1'}, {' ', ' '}, {'P', '1'}, {'P', '1'}, {'P', '1'}, {'P', '1'}},
        {{' ', ' '}, {'P', '1'}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}},
        {{' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {'G', '1'}, {' ', ' '}, {' ', ' '}},
        {{' ', ' '}, {' ', ' '}, {' ', ' '}, {'W', '0'}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}},
        {{' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}, {' ', ' '}},
        {{'P', '0'}, {'P', '0'}, {' ', ' '}, {' ', ' '}, {'P', '0'}, {'P', '0'}, {'P', '0'}, {'P', '0'}},
        {{'W', '0'}, {'S', '0'}, {'G', '0'}, {' ', ' '}, {'H', '0'}, {'G', '0'}, {'S', '0'}, {'W', '0'}}};

    renderBoard(board);

    while (GAME_STATUS)
    {

        int startingPosition[2];
        int endingPosition[2];

        int legalMoves[30][2];
        int legalMovesLength = 0;

        if (1)
        {
            // 1) Ask player which figure to move
            do
                askForPosition(startingPosition, "Podaj kolumnę i rząd figury: ");
            while (!isSelectedFigureLegal(board, startingPosition)); // Check if chosen starting position is legal

            // 2) Generate legal moves for selected figure
            char char_currentPlayer = (PLAYER == 0) ? '0' : '1';

            legalMovesGenerator(board, startingPosition, legalMoves, &legalMovesLength, char_currentPlayer);

            // Check if any legal moves exist
            if (legalMoves[0][0] == -1) {
                printf("\n* Ta figura nie ma żadnych dostępnych ruchów *\n\n");
                continue;
            }

            // 3) Render board with posible legal moves highlighted
            renderBoardWithMoves(board, legalMoves, legalMovesLength);

            // 4) Ask player where to move a chosen pawn until move is legal
            do
                askForPosition(endingPosition, "Podaj kolumnę i rząd, na które chcesz się ruszyć: ");
            while (!isMoveLegal(board, startingPosition, endingPosition, legalMoves)); // Check if chosen ending position is legal
        }
        else
        {
            // 1) Best move generator function
        }

        // Update board
        updateBoard(board, startingPosition, endingPosition);

        // Render board
        renderBoard(board);

        // Check for win

        // Change player
        PLAYER = !PLAYER;
    }
}
