#include "graphics.h"
#include "networking.h"
#include <glib.h>

static void play_button_press(GtkWidget *widget, GdkEvent* foo, gpointer data);
void to_main_menu(GtkWidget *widget, window_state *state);
void to_server_setup(GtkWidget *widget, window_state *state);
void to_client_setup(GtkWidget *widget, window_state *state);
void to_game(GtkWidget *widget, window_state *state);

bool await_connection(window_state* data);
bool on_connect(window_state* state);

void start_server(GtkWidget *widget, window_state *state);
void client_connect(GtkWidget *widget, window_state *state);
void quit(GtkWidget *widget, window_state *state);
void edit_address(GtkWidget* widget, GdkEvent  *event, window_state* state);
void try_play(window_state* window, int x, int y);
bool await_other_side(window_state* window);

void start_game(window_state* window);

void show_info(GtkWidget* window, char *komunikat)
{
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new (GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,
    GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",komunikat);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

GtkWidget* make_window(){

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"Gomoku");
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit), NULL);
    
    return window;
}

void setup_main_menu(window_state* state){

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *button = gtk_button_new_with_label("Utwórz serwer");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_server_setup),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    
    button = gtk_button_new_with_label("Dołącz do serwera");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_client_setup),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    
    GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_widget_set_halign(box1,GTK_ALIGN_CENTER);
    gtk_box_set_center_widget(box2,box1);

    gtk_stack_add_named(state->stack,box2,"main_menu");
}

void setup_server_menu(window_state* state){

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *button = gtk_button_new_with_label("Utwórz podstawowy serwer");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(start_server),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);

    button = gtk_button_new_with_label("Powrót");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_main_menu),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);

    GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box1,GTK_ALIGN_CENTER);
    gtk_box_set_center_widget(box2,box1);

    gtk_stack_add_named(state->stack,box2,"server_menu");
}

void setup_client_menu(window_state* state){

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    server_connection_data* connection_data = malloc(sizeof(server_connection_data));
    connection_data->address = malloc(256);
    strcpy(connection_data->address,"127.0.0.1");

    GtkWidget* text = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(text), 50);
    
    gtk_entry_set_text(GTK_ENTRY(text), "127.0.0.1");
    //gint tmp_pos = gtk_entry_get_text_length(GTK_ENTRY(text));
    //gtk_editable_insert_text(GTK_EDITABLE(text), " jakis tekst", -1, &tmp_pos);
    gtk_editable_select_region(GTK_EDITABLE(text), 0, gtk_entry_get_text_length(GTK_ENTRY(text)));
    g_signal_connect(G_OBJECT(text), "focus-out-event", G_CALLBACK(edit_address),state);
    gtk_box_pack_start(GTK_BOX(box1), text, TRUE, TRUE, 0);

    GtkWidget *button = gtk_button_new_with_label("Dołącz do serwera");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(client_connect),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);

    button = gtk_button_new_with_label("Powrót");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_main_menu),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    
    state->connection_data = connection_data;
    GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_widget_set_halign(box1,GTK_ALIGN_CENTER);
    gtk_box_set_center_widget(box2,box1);

    gtk_stack_add_named(state->stack,box2,"client_menu");
}

void setup_game(window_state* state){

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *button = gtk_button_new_with_label("Wrócić");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_main_menu),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    
    gtk_box_pack_start(GTK_BOX(box1), grid, TRUE, TRUE, 0);
    GtkImage * label;
    GError **err;
    GdkPixbuf* template_pictures[3];
    template_pictures[1] = gdk_pixbuf_new_from_file("./images/black.png",err);
    template_pictures[2] = gdk_pixbuf_new_from_file("./images/white.png",err);
    template_pictures[0] = gdk_pixbuf_new_from_file("./images/unknown.png",err);

    board bo = board_init(15,15,5,4);

    game_state* game_s = malloc(sizeof(game_state));
    game_s->buttons = malloc(sizeof(board_button_data**)*15);

    for(int i = 0; i < 15; i++)
    {
        game_s->buttons[i] = malloc(sizeof(board_button_data*)*15);
        for(int j = 0; j < 15; j++){
            GtkEventBox* event_box = gtk_event_box_new();
            label = gtk_image_new_from_pixbuf(template_pictures[0]);
            gtk_container_add(event_box,label);
            board_button_data *data_to_pass = board_button_data_create(state,i,j,label,template_pictures);
            game_s->buttons[i][j] = data_to_pass;
            g_signal_connect(G_OBJECT(event_box),"button_press_event", G_CALLBACK(play_button_press),(gpointer)data_to_pass);
            gtk_grid_attach(GTK_GRID(grid), event_box, i, j, 1, 1);
        }
    }
    state->game_state = game_s;
    gtk_stack_add_named(state->stack,box1,"game");
}

void to_game(GtkWidget *widget, window_state *state){
    gtk_stack_set_visible_child_name(state->stack,"game");
}

void to_main_menu(GtkWidget *widget, window_state *state){
    gtk_stack_set_visible_child_name(state->stack,"main_menu");
}

void to_client_setup(GtkWidget *widget, window_state *state){
    gtk_stack_set_visible_child_name(state->stack,"client_menu");
}

void to_server_setup(GtkWidget *widget, window_state *state){
    gtk_stack_set_visible_child_name(state->stack,"server_menu");
}

void edit_address(GtkWidget* widget, GdkEvent  *event, window_state *state){
    strcpy(state->connection_data->address, gtk_entry_get_text(GTK_ENTRY(widget)));
    printf("%s\n", state->connection_data->address);
    //return FALSE;
}

void setup_menus(){
    window_state* state = malloc(sizeof(window_state));
    GtkWidget* window = make_window();
    GtkWidget* stack = gtk_stack_new();
    gtk_container_add(GTK_CONTAINER(window), stack);
    state->stack = stack;
    state->window = window;
    
    
    setup_main_menu(state);
    setup_game(state);
    setup_client_menu(state);
    setup_server_menu(state);
    //gtk_widget_show_all(state->main_menu);
    to_main_menu(NULL,state);
    //gtk_widget_show(state->main_menu);
    gtk_widget_show_all(window);
    gtk_main();
}

void start_server(GtkWidget *widget, window_state *state){
    int sock = init_server();
    server_connection_data* connection_data = state->connection_data;
    connection_data->try_to_connect = true;
    connection_data->base_server_socket = sock;
    connection_data->final_socket = 0;

    state->connection_data = connection_data;

    g_timeout_add(1000,G_CALLBACK(await_connection),state);
}

bool await_connection(window_state* data){
    //printf("awaiting connection\n");
    server_connection_data* connection = data->connection_data;

    if(data->connection_data->try_to_connect == false){
        close(connection->base_server_socket);
        return false;
    }
    
    int new_sock = wait_for_connection(connection->base_server_socket,10);

    if(new_sock!= 0){
        connection->final_socket = new_sock;
        data->game_state->socket = new_sock;
        data->game_state->player = 1;
        data->game_state->now_plays = 1;
        g_timeout_add(0,G_CALLBACK(on_connect), data);
        return false;
    }
    return true;
}

void start_game(window_state* window){
    game_state* state = window->game_state;
    //board_free(state->bo);
    state->bo = board_init(15,15,5,4);
    for(int i = 0; i< state->bo.size_x; i++){
        for(int j = 0; j< state->bo.size_y; j++){
            gtk_image_set_from_pixbuf(state->buttons[i][j]->image,state->buttons[i][j]->template_images[0]);
        }
    }
    if(state->player != (state->now_plays)){
        g_timeout_add(100,G_CALLBACK(await_other_side),window);
        g_print("awaiting");
    }
}

bool on_connect(window_state* state){

    printf("that actually worked, damn sonnnnn\n");
    start_game(state);
    to_game(NULL,state);
    
    return false;

}

void client_connect(GtkWidget *widget, window_state *state){
    state->game_state->player = 2;
    state->game_state->now_plays = 1;
    int sock = init_client(state->connection_data->address);
    printf("%d\n",sock);
    state->game_state->socket = sock;
    g_timeout_add(0,G_CALLBACK(on_connect), state);
}

static void play_button_press(GtkWidget *widget, GdkEvent* foo, gpointer data)
{
    board_button_data* board_data = (board_button_data*)data;
    try_play(board_data->game_data,board_data->x,board_data->y);
}

void try_play(window_state* window, int x, int y){
    game_state* game = window->game_state;
    if((game->player) == (game->now_plays)){
        int result = board_play(game->bo,x,y,game->player);
        g_print("played");
        if(result != INVALID_MOVE){
            
            char message[256];
            sprintf(message,"%d %d",x,y);
            if(send(game->socket,message,strlen(message),0)==-1){
                g_print("FAILED TO SEND");
            }

            gtk_image_set_from_pixbuf(game->buttons[x][y]->image,game->buttons[x][y]->template_images[game->player]);

            if(result == PLAYING){
                game->now_plays = 3-(game->now_plays);
                g_timeout_add(100,G_CALLBACK(await_other_side),window);
            }else{

                if(game->now_plays==game->player){
                    show_info(window->window,"Wygrałeś");
                }else{
                    show_info(window->window,"Przegrałeś");
                }
                g_print("someone won");
            }
        }
    }   
}

bool await_other_side(window_state* window){
    game_state* game = window->game_state;

    char buffer[256];
    int result = try_read(game->socket,buffer,256,10);
    buffer[result] = 0;
    if(result>0){
        
        printf("%s", buffer);

        int x, y;

        sscanf(buffer,"%d %d",&x,&y);

        int result = board_play(game->bo,x,y,game->now_plays);
        if(result != INVALID_MOVE){

            gtk_image_set_from_pixbuf(game->buttons[x][y]->image,game->buttons[x][y]->template_images[game->now_plays]);

            if(result == PLAYING){
                game->now_plays = 3-(game->now_plays);
            }else{
                if(game->now_plays==game->player){
                    show_info(window->window,"Wygrałeś");
                }else{
                    show_info(window->window,"Przegrałeś");
                }
                g_print("someone won");
            }
        }
        return false;
    }
    return true;
}

board_button_data* board_button_data_create(window_state* state, int x, int y, GtkImage* image, GdkPixbuf** images){
    board_button_data* data = malloc(sizeof(board_button_data));
    data->game_data = (void*)state;
    data->x = x;
    data->y = y;
    data->image = image;
    data->template_images[0] = images[0];
    data->template_images[1] = images[1];
    data->template_images[2] = images[2];
    return data;
}