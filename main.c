#include <string.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include "graphics.h"
#include <stdbool.h>

int n = 15;

int main( int argc,char *argv[] )
{
    gtk_init(&argc, &argv);
    setup_menus();
    return 0;
}

