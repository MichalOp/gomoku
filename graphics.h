#include "engine.h"
#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct board_button_data_t{
    void* game_data;
    int x;
    int y;
    GtkImage* image;
    GdkPixbuf* template_images[3];
} board_button_data;

typedef struct connection_data_t{

    bool try_to_connect;
    int base_server_socket;
    int final_socket;
    char* address;

}server_connection_data;

typedef struct game_state_t{

    board bo;
    int mode;
    int status;
    int socket;   
    int player;
    int now_plays;
    board_button_data*** buttons;
    
}game_state;

typedef struct window_state_t{
    GtkWidget* window;
    GtkWidget* stack;
    server_connection_data* connection_data;
    game_state* game_state;

}window_state;

void setup_menus();

board_button_data* board_button_data_create(window_state* state,int x, int y, GtkImage* image, GdkPixbuf** template_images);
