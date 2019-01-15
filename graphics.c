#include "graphics.h"

board_button_data* board_button_data_create(board bo, int x, int y, GtkImage* image){
    board_button_data* data = malloc(sizeof(board_button_data));
    data->bo = bo;
    data->x = x;
    data->y = y;
    data->image = image;
    return data;
}

board_graphics board_graphics_create(GtkWidget *attach_to, board bo, void (*click_event)(int x, int y, void* data)){

    board_graphics bg;
    
    bg.root = attach_to;

    bg.click_event = click_event;

    return bg;
}
