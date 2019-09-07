// Brian Gauch - CS360

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

int move_count = 0;
float time_remaining = 10.0;
int score[LEN] = {0,10,100,1000,10000};
int SEARCH_DEPTH = 4;

void init_board(char value)
{
   move_count = 0;
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
      move_count++;
   }
}

// for use only when searching
void undo_move(int col)
{
   int row = --filled[col];
   board[row][col] = EMPTY;
   move_count--;
}

void recursive_DFS(int& score, int depth, int move, char player)
{
   int moveScore = score_move(move, player);
   score += moveScore;
   
   if(depth < SEARCH_DEPTH)
   {
      int bestChildScore = 0;
      for(int i=0; i<COLS; i++)
      {
         if(check_move(move))
         {
            make_move(move, player);
            // switch players
            if(player==BLACK)
               player=WHITE;
            else
               player=BLACK;
            int childScore = 0;
            recursive_DFS(childScore, depth+1, i, player);
            undo_move(move);
            if(childScore > bestChildScore)
            {
               bestChildScore = childScore;
            }
         }
      }
      score -= bestChildScore;
      /*
      if (depth % 2 == 1)
         score += bestChildScore;
      else
         score -= bestChildScore;
      */
   }
}

void DFS_move(int& col, char player)
{
   int bestScore = -1000000;
   int bestMove = 0;
   for(int i=0; i<COLS; i++)
   {
      if(check_move(i))
      {
         int score=0; int depth=0;
         recursive_DFS(score, depth, i, player);
         // cout << "d:" << depth << ", c:" << i << ", s:" << score << endl;
         if(score > bestScore)
         {
            bestScore = score;
            bestMove = i;
         }
      }
   }
   col=bestMove;
   // cout << "BEST " << bestMove << " " << bestScore << endl;
}

void test()
{
   // initialize game
   int col = 0;
   init_board(EMPTY);

   // make first move
   if (this_player == BLACK)
   {
      DFS_move(col, BLACK);
      make_move(col, BLACK);
      print_board();
      if (check_win(BLACK)) { cerr << "BLACK wins\n"; return; }
   }

   // loop until someone wins game
   while (move_count < ROWS*COLS)
   {
      // white moves
      DFS_move(col, WHITE);
      make_move(col, WHITE);
      print_board();
      if (check_win(WHITE)) { cerr << "WHITE wins\n"; return; }

      // black moves
      DFS_move(col, BLACK);
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
      // process command
      if (command == "protocol_version")
      {
         cout << "= 2 " << endl << endl;
      }

      // process command
      else if (command == "name")
      {
         cout << "= Brian Gauch " << endl << endl;
      }
      
      // process command
      else if (command == "version")
      {
         cout << "= 1.0 " << endl << endl;
      }
      
      // process command
      else if (command == "known_command")
      {
         string argument = "";
         cin >> argument;
         cout << "= false " << endl << endl;
      }
      
      // process command
      else if (command == "list_commands")
      {
         cout << "= " << endl << endl;
      }
      
      // process command
      else if (command == "quit")
      {
         cout << "= " << endl << endl;
         exit(0);
      }
      
      // process command
      else if (command == "boardsize")
      {
         int size = 0;
         cin >> size;
         cout << "= " << endl << endl;
      }
      
      // process command
      else if (command == "clear_board")
      {
         init_board(EMPTY);
         cout << "= " << endl << endl;
      }

      // process command
      else if (command == "komi")
      {
         float value = 0;
         cin >> value;
         cout << "= " << endl << endl;
      }

      // process command
      else if (command == "play")
      {
	  string player = "";
	  cin >> player >> col;
	  if (player == "white")
	     make_move(col, WHITE);
	  else if (player == "black")
	     make_move(col, BLACK);
	  cout << "= " << endl << endl;
      }
	   
      // process command
      else if (command == "genmove")
      {
         float cpu = 0;
         int move = 0;
         string player = "";
         cin >> player >> cpu >> move;
         time_remaining = cpu;
	 if (move_count >= ROWS*COLS)
	    cout << "= pass" << endl << endl;
         else if (player == "white")
         {
	        if (check_win(BLACK))  
	           cout << "= resign " << endl << endl;
	        else 
	        {
               DFS_move(col, WHITE);
	           make_move(col, WHITE);
	           cout << "= " << col << endl << endl;
	        }
         }
         else if (player == "black")
         {
	        if (check_win(WHITE))  
	           cout << "= resign " << endl << endl;
	        else 
	        {
               DFS_move(col, BLACK);
	           make_move(col, BLACK);
	           cout << "= " << col << endl << endl;
	        }
         }
      }

      // process play command
      else if (command == "showboard")
      {
         print_board();
	 cout << "= " << endl << endl;
      }

      else if (command == "final_score")
      {
	     cout << "= " << time_remaining << endl << endl;
      }

      // ignore other commands
      else
         cerr << "ignore command: " << command << endl;

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
   // test();
   play();
   return 0;
}
