#include "graphics.h"
#include "networking.h"

static void play_button_press(GtkWidget *widget, GdkEvent* foo, gpointer data);
void to_main_menu(GtkWidget *widget, window_state *state);
void to_server_setup(GtkWidget *widget, window_state *state);
void to_client_setup(GtkWidget *widget, window_state *state);
void to_game(GtkWidget *widget, window_state *state);

void start_server(GtkWidget *widget, window_state *state);
void client_connect(GtkWidget *widget, window_state *state);
void quit(GtkWidget *widget, window_state *state);

GtkWidget* make_window(){

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"Gomoku");
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit), NULL);
    
    return window;
}

void setup_main_menu(window_state* state){

    GtkWidget* window = make_window();

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box1);

    GtkWidget *button = gtk_button_new_with_label("Grać");

    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_game),state);

    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);

    button = gtk_button_new_with_label("Utwórz serwer");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_server_setup),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    
    button = gtk_button_new_with_label("Dołącz do serwera");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_client_setup),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    
    state->main_menu = window;
    gtk_widget_show_all(window);
    gtk_widget_hide(window);
}

void setup_server_menu(window_state* state){

    GtkWidget* window = make_window();

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box1);

    GtkWidget *button = gtk_button_new_with_label("Create basic Server");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(start_server),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);

    button = gtk_button_new_with_label("Wrócić");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_main_menu),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    
    state->server_setup_window = window;
    gtk_widget_show_all(window);
    gtk_widget_hide(window);

}

void setup_client_menu(window_state* state){

    GtkWidget* window = make_window();

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box1);

    GtkWidget *button = gtk_button_new_with_label("Connect to Local");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(client_connect),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);

    button = gtk_button_new_with_label("Wrócić");
    g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(to_main_menu),state);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    
    state->client_setup_window = window;
    gtk_widget_show_all(window);
    gtk_widget_hide(window);

}

void setup_game(window_state* state){
    GtkWidget* window = make_window();

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box1);
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
    for(int i = 0; i < 15; i++)
    {
        for(int j = 0; j < 15; j++){
            GtkEventBox* event_box = gtk_event_box_new();
            label = gtk_image_new_from_pixbuf(template_pictures[0]);
            gtk_container_add(event_box,label);
            board_button_data *data_to_pass = board_button_data_create(bo,i,j,label,template_pictures);
            g_signal_connect(G_OBJECT(event_box),"button_press_event", G_CALLBACK(play_button_press),(gpointer)data_to_pass);
            gtk_grid_attach(GTK_GRID(grid), event_box, i, j, 1, 1);
        }
    }
    state->game_window = window;
    gtk_widget_show_all(window);
    gtk_widget_hide(window);
}

void to_game(GtkWidget *widget, window_state *state){
    gtk_widget_hide(GTK_WIDGET(gtk_widget_get_toplevel(widget)));
    gtk_widget_show(state->game_window);
}

void to_main_menu(GtkWidget *widget, window_state *state){
    gtk_widget_hide(GTK_WIDGET(gtk_widget_get_toplevel(widget)));
    gtk_widget_show(state->main_menu);
}

void to_client_setup(GtkWidget *widget, window_state *state){    
    gtk_widget_hide(GTK_WIDGET(gtk_widget_get_toplevel(widget)));
    gtk_widget_show(state->client_setup_window);
}

void to_server_setup(GtkWidget *widget, window_state *state){
    gtk_widget_hide(GTK_WIDGET(gtk_widget_get_toplevel(widget)));
    gtk_widget_show(state->server_setup_window);
}

void setup_menus(){
    window_state* state = malloc(sizeof(window_state));
    setup_main_menu(state);
    setup_game(state);
    setup_client_menu(state);
    setup_server_menu(state);
    //gtk_widget_show_all(state->main_menu);
    gtk_widget_show(state->main_menu);

    //gtk_widget_show(state->main_menu);
    gtk_main();
}

void start_server(GtkWidget *widget, window_state *state){
    int sock = init_server();
    printf("%d\n",sock);
    char buffer[1024];
    recv(sock,buffer,1024,0);
    printf("%s\n",buffer);
}

void client_connect(GtkWidget *widget, window_state *state){
    int sock = init_client("");
    printf("%d\n",sock);
    char *msg = "Things";
    send(sock,msg,strlen(msg),0);
}

static void play_button_press(GtkWidget *widget, GdkEvent* foo, gpointer data)
{
    board_button_data* board_data = (board_button_data*)data;
    int color = 2;
    int result = board_play(board_data->bo,board_data->x,board_data->y,color);
    if(result!= INVALID_MOVE){
            gtk_image_set_from_pixbuf(board_data->image,board_data->template_images[color]);
    }else{
        printf("FAILED\n");
    }
    if(result == WHITE_WIN){
        printf("white won\n");
    }
    if(result == BLACK_WIN){
        printf("black won\n");
    }
    g_print("GTK-test - %d %d zastal nacisniety\n",board_data->x,board_data->y);
}

board_button_data* board_button_data_create(board bo, int x, int y, GtkImage* image, GdkPixbuf** images){
    board_button_data* data = malloc(sizeof(board_button_data));
    data->bo = bo;
    data->x = x;
    data->y = y;
    data->image = image;
    data->template_images[0] = images[0];
    data->template_images[1] = images[1];
    data->template_images[2] = images[2];
    return data;
}