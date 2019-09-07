#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

const char EMPTY = '.';
const char BLACK = 'b';
const char WHITE = 'w';
const int LEN = 5;
const int ROWS = 10;
const int COLS = 8;
char board[ROWS][COLS];
int filled[COLS];
char this_player = EMPTY;

int score[LEN] = {0,10,100,1000,10000};
int SEARCH_DEPTH = 4;

void init_board(char value)
{
   // loop over all rows and cols
   for (int c = 0; c < COLS; c++)
   for (int r = 0; r < ROWS; r++)
      board[r][c] = value;
   for (int c = 0; c < COLS; c++)
      filled[c] = 0;
}

bool check_move(int col)
{
   // check location is valid
   if ((col < 0) || (col >= COLS) || 
       (filled[col] < 0) || (filled[col] >= ROWS))
      return false;
   else
      return true;
}

void random_move(int &col)
{
   // generate random valid move
   col = random() % COLS;
   while (!check_move(col))
      col = random() % COLS;
}

void center_move(int &col)
{
   // loop over all starting positions
   float best_score = ROWS*ROWS + COLS*COLS;
   float center_row = (ROWS-1)/2.0;
   float center_col = (COLS-1)/2.0;
   for (int c = 0; c < COLS; c++)
   if (check_move(c))
   {
      // calculate distance to center
      int r = filled[c];
      float dr = r - center_row;
      float dc = c - center_col;
      float score = dr * dr + dc * dc;
      if (score < best_score)
      {
         col = c;
         best_score = score;
      }
   }
}

bool wrap(int drow, int dcol)
{
   if(dcol==0)
   {
      if(dcol==1 || dcol==-1)
         return true;
   }
   return false;
}

int score_sequence(bool wrap, int row, int col, int drow, int dcol, char player)
{
   // count black/white pieces
   int black_count = 0;
   int white_count = 0;
   for (int step = 0; step < LEN; step++)
   {
      if(wrap)
      {
         int r = (row + step * drow + ROWS) % ROWS;
         int c = (col + step * dcol + COLS) % COLS;
         if (board[r][c] == BLACK) black_count++;
         if (board[r][c] == WHITE) white_count++;
      }
      else
      {
         int r = row + step * drow;
         int c = col + step * dcol;
         if(r>=0 && r<ROWS && c>=0 && c<COLS)
         {
            if (board[r][c] == BLACK) black_count++;
            if (board[r][c] == WHITE) white_count++;
         }
      }
   }

   // calculate score
   if (player == WHITE)
   {
      if (black_count == 0) return score[white_count] * 2;
      if (white_count == 0) return score[black_count];
   }
   if (player == BLACK)
   {
      if (white_count == 0) return score[black_count] * 2;
      if (black_count == 0) return score[white_count];
   }
   return -1;
}

int score_move(int col, char player)
{
   int score = 0;
   int high_score = 0;
   if (check_move(col))
   {
      // check all five offsets
      int row = filled[col];
      for (int step = 0; step < LEN; step++)
      {
         // check all four directions
         score = score_sequence(false, row-step, col, 1, 0, player); 
         if (score > high_score) high_score = score;
         score = score_sequence(true, row, col-step, 0, 1, player); 
         if (score > high_score) high_score = score;
         score = score_sequence(false, row-step, col-step,  1, 1, player); 
         if (score > high_score) high_score = score;
         score = score_sequence(false, row+step, col-step, -1, 1, player); 
         if (score > high_score) high_score = score;
      }
   }
   return high_score; 
}

void clever_move(int &col, char player)
{
   // loop over all starting positions
   int best_score = 0;
   for (int c = 0; c < COLS; c++)
   if (check_move(c))
   {
      // calculate best score
      int score = score_move(c, player);
      if (score > best_score)
      {
         col = c;
         best_score = score;
      }
   }

   // make first move
   if (best_score == 0)
      random_move(col);
}

/*
void simulate_moves(int& moves, char firstPlayer)
{
   char player = firstPlayer;
   for(int i=0; i<moves.length; i++)
   {
      makeMove(player);
      // switch players
      if(player==BLACK)
         player=WHITE;
      else
         player=BLACK;
   }
}
*/

/*
void search_and_move(int &col, char player)
{
   int best_score = 0;
   int moveSequence[SEARCH_DEPTH];
   // Move sequence with optimal play from both players
   int bestMoveSequence[SEARCH_DEPTH];

   for(int i=0; i<SEARCH_DEPTH; i++)
   {
      // Depth 1 means 1 move for both players
      for(int j=0; j<2; j++)
      {
         for(int move=0; move<COLS; move++)
         {
            if(check_move(move))
            {
               moveSequence[(2*i)+j] = move;
               simulate_moves(searchBoard, moves, player);
               int score = score_board(searchBoard, player);
               if (score > best_score)
               {
                  memcpy(bestMoveSequence, moveSequence, sizeof(bestMoveSequence));
                  col = bestMoveSequence[0];
                  best_score = score;
               }
            }
         }
         // switch players
         if(player==BLACK)
            player=WHITE;
         else
            player=BLACK;
      }
   }
   
   // make first move
   if (best_score == 0)
      random_move(col);
}
*/

void print_board()
{
   // print all rows and cols
   cerr << "\n";
   for (int r = 0; r < ROWS; r++)
   {
      cerr << "+";
      for (int c = 0; c < COLS; c++)
         cerr << "---+";
      cerr << "\n";
      cerr << "| ";
      for (int c = 0; c < COLS; c++)
         cerr << board[r][c] << " | ";
      cerr << "\n";
   }
   cerr << "+";
   for (int c = 0; c < COLS; c++)
      cerr << "---+";
   cerr << "\n";
}

bool check_win(char player)
{
   // loop over all starting positions
   bool win = false;
   for (int c = 0; c < COLS; c++)
   for (int r = 0; r < ROWS; r++)
   if (board[r][c] == player)
   {
      // check row
      int count = 0;
      for (int d = 0; d < LEN; d++)
         if (r+d < ROWS && board[r+d][c] == player) count++;
      if (count == LEN) win = true;
    
      // check col
      count = 0;
      for (int d = 0; d < LEN; d++)
         // wrap in this direction
         if (board[r][(c+d+COLS)%COLS] == player) count++;
      if (count == LEN) win = true;
    
      // check diagonal
      count = 0;
      for (int d = 0; d < LEN; d++)
         if (c+d<COLS && r+d<ROWS && board[r+d][c+d] == player) count++;
      if (count == LEN) win = true;
    
      // check diagonal
      count = 0;
      for (int d = 0; d < LEN; d++)
         if (c+d < COLS && r-d >= 0 && board[r-d][c+d] == player) count++;
      if (count == LEN) win = true;
   }
   return win;
}

void make_move(int col, char player)
{
   // make move
   if (check_move(col))
   {
      int row = filled[col]++;
      board[row][col] = player; 
   }
}

// for use only when searching
void undo_move(int col)
{
   int row = --filled[col];
   board[row][col] = EMPTY;
}


void recursive_DFS2(int& score, int depth, int move, char player)
{
   
   //print_board();
   int moveScore = score_move(move, player);
   score += moveScore;
   //cout << "d:" << depth << ", c:" << move << ", s:" << score << endl;
   
   if(depth < SEARCH_DEPTH)
   {
      int bestChildScore = 0;
      for(int i=0; i<COLS; i++)
      {
         if(check_move(i))
         {
            make_move(move, player);
            // switch players
            if(player==BLACK)
               player=WHITE;
            else
               player=BLACK;
            int childScore = 0;
            recursive_DFS2(childScore, depth+1, i, player);
            undo_move(move);
            if(childScore > bestChildScore)
            {
               bestChildScore = childScore;
            }
         }
      }
      score -= bestChildScore;
   }   
}

void recursive_DFS(int& score, int depth, int move, char player)
{
   int moveScore = score_move(move, player);
   score += moveScore;
   make_move(move, player);
   
   print_board();
   cout << "d:" << depth << ", c:" << move << ", s:" << score << endl;
   
   if(depth < SEARCH_DEPTH)
   {
      // switch players
      if(player==BLACK)
         player=WHITE;
      else
         player=BLACK;

      int bestChildScore = 0;
      for(int i=0; i<COLS; i++)
      {
         if(check_move(i))
         {
            int childScore = score;
            recursive_DFS(childScore, depth+1, i, player);
            if(childScore > bestChildScore)
            {
               bestChildScore = childScore;
            }
         }
      }
      score = bestChildScore;
   }   
   undo_move(move);
}
//brian
void DFS_move(int& col, char player)
{
   int bestScore = 0;
   int bestMove = 0;
   for(int i=0; i<COLS; i++)
   {
      if(check_move(i))
      {
         int score=0; int depth=0;
         recursive_DFS2(score, depth, i, player);
         cout << "c:" << i << ", s:" << score << endl;
         if(score > bestScore)
         {
            bestScore = score;
            bestMove = i;
         }
      }
   }
   col=bestMove;
   //make_move(bestMove, this_player);
}

void test()
{
   // initialize game
   int col = 0, count = 0;
   init_board(EMPTY);

   // make first move
   if (this_player == BLACK)
   {
      random_move(col);
      make_move(col, BLACK);
      print_board();
      if (check_win(BLACK)) { cerr << "BLACK wins\n"; return; }
   }

   // loop until someone wins game
   while (count < ROWS*COLS)
   {
      // white moves
      //clever_move(col, WHITE);
      DFS_move(col, WHITE);
      make_move(col, WHITE);
      print_board();
      if (check_win(WHITE)) { cerr << "WHITE wins\n"; return; }

      // black moves
      DFS_move(col, BLACK);
      //clever_move(col, BLACK);
      //random_move(col);
      make_move(col, BLACK);
      print_board();
      if (check_win(BLACK)) { cerr << "BLACK wins\n"; return; }
   }

   // print final message
   cerr << "Sorry, no one wins\n";
}

void play()
{
   // initialize game
   int col = 0;
   init_board(EMPTY);

   // loop reading and processing commands
   string command = "";
   cin >> command;
   while (!cin.eof())   
   {
      // process protocol command
      if (command == "protocol_version")
      {
         cout << "= 2 " << endl << endl;
      }

      // process boardsize command
      if (command == "boardsize")
      {
         int size = 0;
         cin >> size;
         cout << "= " << endl << endl;
      }

      // process clear_board command
      if (command == "clear_board")
      {
         init_board(EMPTY);
         cout << "= " << endl << endl;
      }

      // process komi command
      if (command == "komi")
      {
         float value = 0;
         cin >> value;
         cout << "= " << endl << endl;
      }

      // process genmove command
      else if (command == "genmove")
      {
         float cpu = 0;
         int move = 0;
         string player = "";
         cin >> player >> cpu >> move;
         if (player == "white")
         {
            clever_move(col, WHITE);
            make_move(col, WHITE);
         }
         else if (player == "black")
         {
            random_move(col);
            make_move(col, BLACK);
         }
         cout << "= " << col << endl << endl;
      }

      // process play command
      else if (command == "play")
      {
         string player = "";
         cin >> player >> col;
         if (player == "white")
         {
            make_move(col, WHITE);
            if (check_win(WHITE))  
               cout << "resign" << endl << endl;
         }
         else if (player == "black")
         {
            make_move(col, BLACK);
            if (check_win(BLACK))  
               cout << "resign" << endl << endl;
         }
         cout << "= " << endl << endl;
      }

      // process play command
      else if (command == "showboard")
         print_board();

      // read next command
      cin >> command;
   }
}

int main(int argc, char *argv[])
{
   // process command arguments
   if (argc == 2)
   {
      if ((argv[1][0] == 'b') || (argv[1][0] == 'B'))
         this_player = BLACK;
      if ((argv[1][0] == 'w') || (argv[1][0] == 'W'))
         this_player = WHITE;
   }
   
   // play game
   srandom(time(NULL));
   test();
   //play();
   
   //init_board(EMPTY);
   //make_DFS_move();
   return 0;
}
