#include <string.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include "graphics.h"
#include <stdbool.h>

//GtkLabel * labels[256];

GdkPixbuf *black;
GdkPixbuf *white;
GdkPixbuf *unset;
int n = 15;
// Prototypy własnych funkcji obsługi zdarzen´ srodowiska GTK+ 3.0 ´
static void play_button_press(GtkWidget *widget, GdkEvent* foo, gpointer data);
static void test_nacisniecia( GtkWidget *widget, gpointer data);
static void wyjscie_przyciskiem( GtkWidget *widget, gpointer data);
static void wypisz_wprowadzony_tekst( GtkWidget *widget, gpointer data);
static void dodaj_do_text( GtkWidget *widget, gpointer data);
int main( int argc,char *argv[] )
{
    gtk_init(&argc, &argv);
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"GTK - rysowanie kwadratów");
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit), NULL);
    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box1);

    GtkWidget *button = gtk_button_new_with_label("Następna permutacja");

    g_signal_connect(G_OBJECT(button), "clicked",
                     G_CALLBACK(test_nacisniecia),(gpointer)"Przycisk");
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    gtk_box_pack_start(GTK_BOX(box1), grid, TRUE, TRUE, 0);
    GtkImage * label;
    GError **err;
    black = gdk_pixbuf_new_from_file("./images/black.png",err);
    white = gdk_pixbuf_new_from_file("./images/white.png",err);
    unset = gdk_pixbuf_new_from_file("./images/unknown.png",err);

    board bo = board_init(15,15,5,4);
    for(int i = 0; i < 15; i++)
    {
        for(int j = 0; j < 15; j++){
            GtkEventBox* event_box = gtk_event_box_new();
            label = gtk_image_new_from_pixbuf(unset);
            gtk_container_add(event_box,label);
            board_button_data *data_to_pass = board_button_data_create(bo,i,j,label);
            g_signal_connect(G_OBJECT(event_box),"button_press_event", G_CALLBACK(play_button_press),(gpointer)data_to_pass);
            gtk_grid_attach(GTK_GRID(grid), event_box, i, j, 1, 1);
        }
    }
    GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    button = gtk_button_new();
    board_button_data data_to_pass;
    data_to_pass.x = 23;
    data_to_pass.y = 23;
    
    g_signal_connect(G_OBJECT(button), "button_press_event", G_CALLBACK(wyjscie_przyciskiem), (gpointer*)&"seriously what the hell");
    gtk_box_pack_start(GTK_BOX(box2), button, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box1), box2, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
static void play_button_press(GtkWidget *widget, GdkEvent* foo, gpointer data)
{
    board_button_data* board_data = (board_button_data*)data;
    int color = 2;
    int result = board_play(board_data->bo,board_data->x,board_data->y,color);
    if(result!= INVALID_MOVE){
        if(color == 1)
            gtk_image_set_from_pixbuf(board_data->image,black);
        else
            gtk_image_set_from_pixbuf(board_data->image,white);
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
// definicje własnych funkcji obsługi zdarzen´
static void test_nacisniecia( GtkWidget *widget, gpointer data)
{
   g_print("GTK-test - %s zastal nacisniety\n", (gchar *) data);
}
static void wyjscie_przyciskiem( GtkWidget *widget, gpointer data)
{
    board_button_data* board_data = data;
    printf("%s\n", (gchar*)data);
    gtk_main_quit();
}
static void wypisz_wprowadzony_tekst( GtkWidget *widget, gpointer data)
{
    
    g_print ("GTK-test wprowadzono: %s\n", gtk_entry_get_text (GTK_ENTRY (data)));
}
