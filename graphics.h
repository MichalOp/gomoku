#include "engine.h"
#include <gtk/gtk.h>

typedef struct board_button_data_t{
    board bo;
    int x;
    int y;
    GtkImage* image;
} board_button_data;

board_button_data* board_button_data_create(board bo, int x, int y, GtkImage* image);

typedef struct board_graphics_t{

    GtkWidget* root;
    GtkWidget* parent;
    void (*click_event)(int x, int y, void* data);

}board_graphics;

board_graphics board_graphics_create(GtkWidget* attach_to,board bo, void (*click_event_func)(int x, int y, void* data));
void draw_board(board_graphics bg, board bo);
void destory_board_graphics(board_graphics bg);