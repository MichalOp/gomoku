#include "graphics.h"
#include "networking.h"
#include <glib.h>
#include <stdio.h>

#define NORMAL 0
#define PRO_1 1
#define PRO_2 2
#define PRO_3 3
#define SWAP_1 4
#define SWAP_2 5
#define SWAP_2_SWAP 6
#define SWAP_2_PLAY 7
#define SWAP_2_PLAY2 8
#define SWAP_3 9
#define SWAP_3_SWAP 10

static void play_button_press(GtkWidget *widget, GdkEvent* foo, gpointer data);
void to_main_menu(GtkWidget *widget, window_state *state);
void to_server_setup(GtkWidget *widget, window_state *state);
void to_client_setup(GtkWidget *widget, window_state *state);
void to_game(GtkWidget *widget, window_state *state);

int await_connection(void * data);
int on_connect(void * data);

void start_server(GtkWidget *widget, window_state *state);
void client_connect(GtkWidget *widget, window_state *state);
void quit(GtkWidget *widget, window_state *state);
void edit_address(GtkWidget* widget, GdkEvent  *event, window_state* state);
void try_play(window_state* window, int x, int y);
void select_game_mode(GtkWidget* widget, window_state* state);
void cancel_move(GtkWidget* widget, window_state* state);
void execute_move(GtkWidget* widget, window_state* state);
void update_status_change(int status,window_state* window);

void click_play_normally(GtkWidget* widget, window_state* state);
void click_swap(GtkWidget* widget, window_state* state);
void click_play_two(GtkWidget* widget, window_state* state);

int await_other_side(void * data);

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
    gtk_box_set_center_widget(GTK_BOX(box2),box1);

    gtk_stack_add_named(GTK_STACK(state->stack),box2,"main_menu");
}

void setup_server_menu(window_state* state){

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *radio_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    GtkWidget *label = gtk_label_new("Tryb gry");
    gtk_box_pack_start(GTK_BOX(box1),label, TRUE, TRUE, 0);
    GtkWidget *radio1 = gtk_radio_button_new_with_label(NULL,"standard");
    //gtk_container_add(GTK_RADIO_BUTTON(radio1), button);
    GtkWidget* radio2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1),"pro");
    GtkWidget* radio3 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio2),"swap2");

    g_signal_connect(G_OBJECT(radio1),"toggled",G_CALLBACK(select_game_mode),state);
    g_signal_connect(G_OBJECT(radio2),"toggled",G_CALLBACK(select_game_mode),state);
    g_signal_connect(G_OBJECT(radio3),"toggled",G_CALLBACK(select_game_mode),state);

    gtk_box_pack_start(GTK_BOX(radio_box), radio1, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(radio_box), radio2, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(radio_box), radio3, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(box1),radio_box, TRUE, TRUE, 20);

    GtkWidget* button = gtk_button_new_with_label("Utwórz podstawowy serwer");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(start_server),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);

    button = gtk_button_new_with_label("Powrót");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_main_menu),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);

    GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box1,GTK_ALIGN_CENTER);
    gtk_box_set_center_widget(GTK_BOX(box2),box1);

    gtk_stack_add_named(GTK_STACK(state->stack),box2,"server_menu");
}

void setup_client_menu(window_state* state){

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    server_connection_data* connection_data = malloc(sizeof(server_connection_data));
    memset(connection_data,0,sizeof(server_connection_data));
    connection_data->address = malloc(256);
    strcpy(connection_data->address,"127.0.0.1");
    state->game_state->mode = 0;

    GtkWidget* text = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(text), 50);
    
    gtk_entry_set_text(GTK_ENTRY(text), "127.0.0.1");
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
    gtk_box_set_center_widget(GTK_BOX(box2),box1);

    gtk_stack_add_named(GTK_STACK(state->stack),box2,"client_menu");
}

GtkWidget* setup_game_menu_stack(window_state* state){

    GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *button = gtk_button_new_with_label("Wyjście");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_main_menu),state);
    gtk_box_pack_start(GTK_BOX(main_box), button, TRUE, FALSE, 10);

    GtkWidget* label = gtk_label_new("empty label for now");
    state->game_state->info_label = label;
    gtk_box_pack_start(GTK_BOX(main_box), label, TRUE, TRUE, 10);

    GtkWidget* stack = gtk_stack_new();

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    label = gtk_label_new("Brak akcji specjalnych w tej turze");
    gtk_box_pack_start(GTK_BOX(box1), label, TRUE, TRUE, 0);
    gtk_stack_add_named(GTK_STACK(stack),box1,"normal");
    
    box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    label = gtk_label_new("Wybierz jeden z ruchów specjalnych:");

    gtk_box_pack_start(GTK_BOX(box1), label, TRUE, TRUE, 10);

    GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    button = gtk_button_new_with_label("Graj białymi");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(click_play_normally),state);
    gtk_box_pack_start(GTK_BOX(box2), button, TRUE, FALSE, 0);

    button = gtk_button_new_with_label("Zagraj dwa");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(click_play_two),state);
    gtk_box_pack_start(GTK_BOX(box2), button, TRUE, FALSE, 0);

    button = gtk_button_new_with_label("Graj czarnymi");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(click_swap),state);

    
    gtk_box_pack_start(GTK_BOX(box2), button, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box1), box2, TRUE, TRUE, 0);
    gtk_stack_add_named(GTK_STACK(stack),box1,"swap2_2");

    GtkWidget *box3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    label = gtk_label_new("Wybierz jeden z ruchów specjalnych:");
    gtk_box_pack_start(GTK_BOX(box1), label, TRUE, TRUE, 10);

    button = gtk_button_new_with_label("Graj białymi");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(click_play_normally),state);
    gtk_box_pack_start(GTK_BOX(box3), button, TRUE, FALSE, 0);

    button = gtk_button_new_with_label("Graj czarnymi");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(click_swap),state);
    
    gtk_box_pack_start(GTK_BOX(box3), button, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box1), box3, TRUE, TRUE, 0);
    gtk_stack_add_named(GTK_STACK(stack),box1,"swap2_1");

    state->game_state->game_menu_stack = stack;

    gtk_box_pack_start(GTK_BOX(main_box), stack, TRUE, FALSE, 10);

    GtkWidget* box4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    button = gtk_button_new_with_label("Cofnij");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(cancel_move),state);
    gtk_box_pack_start(GTK_BOX(box4), button, TRUE, FALSE, 10);

    button = gtk_button_new_with_label("Wykonaj ruch");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(execute_move),state);
    gtk_box_pack_start(GTK_BOX(box4), button, TRUE, FALSE, 10);

    gtk_box_pack_start(GTK_BOX(main_box), box4, TRUE, FALSE, 10);

    return main_box;

}

void setup_game(window_state* state){

    game_state* game_s = malloc(sizeof(game_state));
    memset(game_s,0,sizeof(game_state));
    game_s->buttons = malloc(sizeof(board_button_data**)*15);
    state->game_state = game_s;

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *gamebox = setup_game_menu_stack(state);
    gtk_box_pack_start(GTK_BOX(box1), gamebox, TRUE, TRUE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    
    gtk_box_pack_start(GTK_BOX(box1), grid, TRUE, TRUE, 0);
    GtkWidget * label;
    GError **err = NULL;
    GdkPixbuf* template_pictures[3];
    template_pictures[1] = gdk_pixbuf_new_from_file("./images/black.png",err);
    template_pictures[2] = gdk_pixbuf_new_from_file("./images/white.png",err);
    template_pictures[0] = gdk_pixbuf_new_from_file("./images/unknown.png",err);

    //board bo = board_init(15,15,5,4);
    for(int i = 0; i < 15; i++)
    {
        game_s->buttons[i] = malloc(sizeof(board_button_data*)*15);
        for(int j = 0; j < 15; j++){
            GtkWidget* event_box = gtk_event_box_new();
            label = gtk_image_new_from_pixbuf(template_pictures[0]);
            gtk_container_add(GTK_CONTAINER(event_box),label);
            board_button_data *data_to_pass = board_button_data_create(state,i,j,GTK_IMAGE(label),template_pictures);
            game_s->buttons[i][j] = data_to_pass;
            g_signal_connect(G_OBJECT(event_box),"button_press_event", G_CALLBACK(play_button_press),(gpointer)data_to_pass);
            gtk_grid_attach(GTK_GRID(grid), event_box, i, j, 1, 1);
        }
    }
    GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_widget_set_halign(box1,GTK_ALIGN_CENTER);
    gtk_box_set_center_widget(GTK_BOX(box2),box1);
    gtk_stack_add_named(GTK_STACK(state->stack),box2,"game");
}

void to_game(GtkWidget *widget, window_state *state){
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack),"game");
}

void to_main_menu(GtkWidget *widget, window_state *state){
    int result = send(state->game_state->socket,"e",1,0);
    if(result == -1){
        printf("connection already closed\n");
    }

    if(state->game_state->socket){
        close(state->game_state->socket);
        state->game_state->socket = 0;
        printf("closing connection\n");
    }
    if(state->connection_data->base_server_socket){
        close(state->connection_data->base_server_socket);
        state->connection_data->base_server_socket = 0;
        printf("closing server\n");
    }
    state->connection_data->try_to_connect = false;
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack),"main_menu");
}

void to_client_setup(GtkWidget *widget, window_state *state){
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack),"client_menu");
}

void to_server_setup(GtkWidget *widget, window_state *state){
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack),"server_menu");
}

void click_swap(GtkWidget *widget, window_state *state){
    if(state->game_state->status == SWAP_2){
        update_status_change(SWAP_2_SWAP,state);
    }else{
        update_status_change(SWAP_3_SWAP,state);
    }
}

void click_play_two(GtkWidget *widget, window_state *state){
    update_status_change(SWAP_2_PLAY2,state);
}

void click_play_normally(GtkWidget *widget, window_state *state){
    update_status_change(NORMAL,state);
}

void select_game_mode(GtkWidget *widget, window_state *state){
    GSList* group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(widget));
    group = g_slist_copy(group);
    group = g_slist_reverse(group);
    int i = 0;
    do{
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(group->data))){
            state->game_state->mode = i;
        }
        i++;
        group = g_slist_next(group);
    }while(group != NULL);
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
    state->game_state->ended = 0;
    state->game_state->turn = 0;
    g_timeout_add(1000,await_connection,state);
}

int await_connection(void * data){

    window_state* window = (window_state*) data;
    //printf("awaiting connection\n");
    server_connection_data* connection = window->connection_data;

    if(window->connection_data->try_to_connect == false){
        close(connection->base_server_socket);
        return TRUE;
    }
    
    int new_sock = wait_for_connection(connection->base_server_socket,10);

    if(new_sock!= 0){
        connection->final_socket = new_sock;
        window->game_state->socket = new_sock;
        window->game_state->player = 1;
        window->game_state->now_plays = 1;
        g_timeout_add(0,on_connect, data);
        return FALSE;
    }
    return TRUE;
}

void update_status_change(int status,window_state* window){ //JESUS CHRIST
    game_state* state = window->game_state;
    state->status = status;
    switch(status){
        case NORMAL:
            gtk_stack_set_visible_child_name(GTK_STACK(state->game_menu_stack),"normal");
            state->plays_left = 1;
        break;
        case PRO_1:
            gtk_stack_set_visible_child_name(GTK_STACK(state->game_menu_stack),"normal");
            state->plays_left = 1;
            if(state->player == state->now_plays){
                try_play(window,7,7);
                execute_move(NULL,window);
            }
        break;
        case PRO_2:
            gtk_stack_set_visible_child_name(GTK_STACK(state->game_menu_stack),"normal");
            state->plays_left = 1;
        break;
        case PRO_3:
            gtk_stack_set_visible_child_name(GTK_STACK(state->game_menu_stack),"normal");
            state->plays_left = 1;
        break;
        case SWAP_1:
            gtk_stack_set_visible_child_name(GTK_STACK(state->game_menu_stack),"normal");
            state->plays_left = 3;
        break;
        case SWAP_2:
            gtk_stack_set_visible_child_name(GTK_STACK(state->game_menu_stack),"swap2_2");
            state->plays_left = -1;
            state->player = state->base_player;
            state->now_plays = state->base_player;
        break;
        case SWAP_2_PLAY2:
            gtk_stack_set_visible_child_name(GTK_STACK(state->game_menu_stack),"normal");
            state->plays_left = 2;
            state->player = 3-state->player;
            state->now_plays = state->player;
        break;
        case SWAP_2_PLAY:
            gtk_stack_set_visible_child_name(GTK_STACK(state->game_menu_stack),"normal");
            state->plays_left = 1;
        break;
        case SWAP_2_SWAP:
            gtk_stack_set_visible_child_name(GTK_STACK(state->game_menu_stack),"normal");
            state->plays_left = 0;
            state->player = 3-state->player;
            state->now_plays = state->player;
        break;
        case SWAP_3:
            gtk_stack_set_visible_child_name(GTK_STACK(state->game_menu_stack),"swap2_1");
            state->plays_left = -1;
            state->player = state->base_player;
            state->now_plays = state->base_player;
        break;
        case SWAP_3_SWAP:
            gtk_stack_set_visible_child_name(GTK_STACK(state->game_menu_stack),"normal");
            state->plays_left = 0;
            state->player = 3-state->player;
            state->now_plays = state->player;
        break;
        default: 
        printf("\nwhat?\n");
    }
}

void start_game(window_state* window){
    game_state* state = window->game_state;
    //board_free(state->bo);
    state->bo = board_init(15,15,5,WIN_RULE_GREATER_OR_EQUAL);
    for(int i = 0; i< state->bo.size_x; i++){
        for(int j = 0; j< state->bo.size_y; j++){
            gtk_image_set_from_pixbuf(state->buttons[i][j]->image,state->buttons[i][j]->template_images[0]);
        }
    }

    if(state->now_plays == state->player){
        update_status_change(state->status, window);
    }

    g_timeout_add(100,await_other_side,window);
}

void reset_board_to_data(window_state* window){
    game_state *game = window->game_state;
    board bo = game->bo;
    for(int x = 0; x < bo.size_x; x++){
        for(int y = 0; y<bo.size_y; y++){
            gtk_image_set_from_pixbuf(game->buttons[x][y]->image,game->buttons[x][y]->template_images[bo.cells[x][y].color]);
        }
    }
}

int on_connect(void * data){

    window_state* state = (window_state*) data;

    int side; 

    char start_msg[256];

    if(state->connection_data->base_server_socket != 0){ //server
        side = rand()%2;
        sprintf(start_msg,"%d %d",1-side,state->game_state->mode);
        send(state->game_state->socket,start_msg,strlen(start_msg),0);
    }else{  //client
        recv(state->game_state->socket,start_msg,256,0);
        sscanf(start_msg,"%d %d",&side,&state->game_state->mode);
    }
    game_state* game = state->game_state;
    game->executed = 0;
    game->now_plays = 1;
    if(side == 0){
        game->player = 1;
        gtk_label_set_text(GTK_LABEL(game->info_label),"Twoja tura");
    }else{
        game->player = 2;
        gtk_label_set_text(GTK_LABEL(game->info_label),"Gra przeciwnik");
    }
    if(game->mode == 0){
        game->status = NORMAL;
        game->recv_status = NORMAL;
    }
    if(game->mode == 1){
        game->status = PRO_1;
        game->recv_status = PRO_1;
    }
    if(game->mode == 2){
        game->status = SWAP_1;
        game->recv_status = SWAP_1;
    }
    start_game(state);
    to_game(NULL,state);

    return FALSE;
}

void client_connect(GtkWidget *widget, window_state *state){
    state->game_state->player = 2;
    state->game_state->now_plays = 1;
    int sock = init_client(state->connection_data->address);
    if(sock == 0){
        return;
    }
    printf("%d\n",sock);
    state->game_state->socket = sock;
    g_timeout_add(0,on_connect, state);
}

static void play_button_press(GtkWidget *widget, GdkEvent* foo, gpointer data)
{
    board_button_data* board_data = (board_button_data*)data;
    try_play(board_data->game_data,board_data->x,board_data->y);
}

void try_play(window_state* window, int x, int y){
    game_state* game = window->game_state;
    if(game->ended)
        return;
    
    if(game->status == PRO_3){
        if(x>=4&&x<=10&&y>=4&&y<=10)
        return;
    }

    if((game->player) == (game->now_plays)&& game->plays_left>0){
        int color = game->player;
        if(game->plays_left == 2)
            color = 2; 
        int result = board_play(game->bo,x,y,color,1);

        if(result != INVALID_MOVE){
            game->plays_left--;
            gtk_image_set_from_pixbuf(game->buttons[x][y]->image,game->buttons[x][y]->template_images[color]);
        }
    }   
}

void execute_move(GtkWidget *widget, window_state *state){

    if(state->game_state->plays_left != 0||state->game_state->player!= state->game_state->now_plays){
        return;
    }

    

    char message[1024];
    message[0] = 0;
    
    sprintf(message,"%d;",state->game_state->status);

    int result = board_execute_marked(state->game_state->bo,message);

    state->game_state->executed = 1;

    if(send(state->game_state->socket,message,strlen(message),0)==-1){
        show_info(state->window,"Utracono połączenie z drugim graczem");
        to_main_menu(NULL,state);
    }

    if(result == PLAYING){
        state->game_state->now_plays = 3-(state->game_state->now_plays);
        g_timeout_add(100,await_other_side,state);

    }else{

        if(state->game_state->now_plays==state->game_state->player){
            show_info(state->window,"Wygrałeś");
        }else{
            show_info(state->window,"Przegrałeś");
        }
        state->game_state->ended = 1;
    }
    gtk_label_set_text(GTK_LABEL(state->game_state->info_label),"Gra przeciwnik");
}

void cancel_move(GtkWidget *widget, window_state *state){
    if(state->game_state->executed)
        return;
    board_clear_marked(state->game_state->bo);
    reset_board_to_data(state);
    update_status_change(state->game_state->recv_status,state);
}

void react_for_received_special(int status,window_state* window){
    game_state* state = window->game_state;
    switch(status){
        case NORMAL:
            state->recv_status = NORMAL;
            break;
        case PRO_1:
            state->recv_status = PRO_2;
        break;
        case PRO_2:
            state->recv_status = PRO_3;
        break;
        case PRO_3:
            state->recv_status = NORMAL;
        break;
        case SWAP_1:
            state->recv_status = SWAP_2;
            state->base_player = state->player;
        break;
        case SWAP_2:
            printf("\nwhat? invalid state\n");
        break;
        case SWAP_2_PLAY2:
            state->player = 3-state->player;
            state->now_plays = state->player;
            state->recv_status = SWAP_3;
            state->base_player = state->player;
            show_info(window->window,"Drugi gracz zagrał dwa kamienie, wybierz kolor, którym chcesz grać");
        break;
        case SWAP_2_PLAY:
            state->recv_status = NORMAL;
        break;
        case SWAP_2_SWAP:
            state->player = 3-state->player;
            state->now_plays = state->player;
            state->recv_status = NORMAL;
            show_info(window->window,"Drugi gracz zamienił kolory, grasz teraz białymi");
        break;
        case SWAP_3:
            printf("\nwhat? invalid state\n");
        case SWAP_3_SWAP:
            state->player = 3-state->player;
            state->now_plays = state->player;
            state->recv_status = NORMAL;
            show_info(window->window,"Drugi gracz zamienił kolory, grasz teraz czarnymi");
        break;
        default: 
        printf("\nwhat?\n");
    }
}

int await_other_side(void * data){
    window_state* window = (window_state*) data;
    game_state* game = window->game_state;

    char buffer[1024];
    int result = try_read(game->socket,buffer,1024,10);
    if(result == -1){
        show_info(window->window,"Utracono połączenie z drugim graczem");
        to_main_menu(NULL,window);
        return FALSE;
    }
    if(result>0){
        if(buffer[0]=='e'){
            show_info(window->window,"Drugi gracz opuścił grę");
            to_main_menu(NULL,window);
            return FALSE;
        }

        buffer[result] = 0;
        game->executed = 0;

        int special;
        sscanf(buffer,"%d",&special);

        react_for_received_special(special,window);
        update_status_change(game->recv_status,window);

        char* data_start = buffer;

        while(*data_start!=';'){
            data_start = data_start + 1;
        }
        data_start = data_start + 1;

        int result = board_execute_from_char(game->bo,data_start);
        reset_board_to_data(window);

        if(result != INVALID_MOVE){

            if(result == PLAYING){
                game->now_plays = game->player;
            }else{
                if(game->now_plays==game->player){
                    show_info(window->window,"Wygrałeś");
                }else{
                    show_info(window->window,"Przegrałeś");
                }
                game->ended = TRUE;
                return FALSE;
            }
        }
        gtk_label_set_text(GTK_LABEL(game->info_label),"Twoja tura");
    }
    return TRUE;
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