#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <string.h>

//gcc -o main main.c `pkg-config --libs --cflags gtk+-3.0`
static void button_clicked(GtkWidget *widget, gpointer data) {
    g_print("%s\n", gtk_entry_get_text(GTK_ENTRY(data)));
    gtk_entry_set_text(GTK_ENTRY(data), "");
}

void close_window() {
    // check tabs are saved
    gtk_main_quit();
}

void make_text(GtkWidget *vbox) {
    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *text = gtk_text_view_new();
    //gtk_widget_set_sensitive(GTK_WIDGET(text),FALSE);
    gtk_container_add(GTK_CONTAINER(scrolledWindow), text);
    gtk_widget_show_all(scrolledWindow);
    gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 0);
}

void enter_text(GtkWidget *vbox) {
    GtkWidget *input = gtk_entry_new();
    //GtkWidget *enter = gtk_button_new_with_mnemonic("Enter");
    g_signal_connect(input, "activate", G_CALLBACK(button_clicked), input);
    gtk_box_pack_start(GTK_BOX(vbox), input, FALSE, TRUE, 0);
    //gtk_box_pack_start(GTK_BOX(vbox), enter, TRUE, TRUE, 0);
}


void make_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    //set start position center of screen
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    //set window size
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);

    //window titel
    gtk_window_set_title(GTK_WINDOW(window), "Shell OS-24");

    //signal to destroy window
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(close_window), NULL);

    //need to add box to add more widgets
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    //add box to window
    gtk_container_add(GTK_CONTAINER(window), vbox);

    //make menu maybe...

    //make shell tabs
    make_text(vbox);

    //enter text
    enter_text(vbox);

    //make visible
    gtk_widget_show_all(window);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    make_window();

    gtk_main();
    return 0;
}
