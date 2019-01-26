#include "engine.h"
#include <gtk/gtk.h>

typedef struct board_button_data_t{
    board bo;
    int x;
    int y;
    GtkImage* image;
    GdkPixbuf* template_images[3];
} board_button_data;

typedef struct window_state_t{

    GtkWidget* main_menu;
    GtkWidget* server_setup_window;
    GtkWidget* client_setup_window;
    GtkWidget* game_window;

}window_state;

typedef struct game_state_t{

    window_state* window_s;
    int mode;
    int status;
    int socket;   
    int player;
    board_button_data** buttons;

}game_state;

typedef struct game_window_state_t{
    window_state* main_window_state;
};

void setup_menus();

board_button_data* board_button_data_create(board bo, int x, int y, GtkImage* image, GdkPixbuf** template_images);
