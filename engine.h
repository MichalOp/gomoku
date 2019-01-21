#define BLACK_WIN 1
#define WHITE_WIN 2
#define PLAYING 0
#define DRAW 3
#define INVALID_MOVE -1
#define WIN_RULE_EXACT 1
#define WIN_RULE_GREATER_OR_EQUAL 0

typedef struct cell_t{
    int color;
}cell;

typedef struct board_t{
    
    int size_x;
    int size_y;
    cell **cells;
    int* filled_cells;
    int win_rule;
    int win_count;
    
}board;

void board_free(board b);
board board_init(int sizex, int sizey, int win_count,int win_rule);
int board_play(board b, int x, int y, int color);
