#include <gtk/gtk.h>

#include "bmp.h"

struct button_pressed_data {
    GtkFileDialog *file_dialog;
    GtkWindow *window;
};

static void finish_open_file_dialog(GObject *self, GAsyncResult *res, gpointer data) {
	GtkWindow *window = data;
	GtkFileDialog *file_dialog = (GtkFileDialog*)self;
    GFile *file = gtk_file_dialog_open_finish(file_dialog, res, NULL);
    char *path = g_file_get_path(file);

	char *error;
	struct image_data *buffer = read_image(path, &error);

	if (buffer) {
		g_free(buffer);
	} else {
		GtkAlertDialog *dialog = gtk_alert_dialog_new(
			"An error occured: %s",
			error
		);
		gtk_alert_dialog_show(dialog, window);
	}

    g_free(path);
    g_free(file);
}

static void button_pressed_callback(GtkWidget *button, struct button_pressed_data *data) {
    gtk_file_dialog_open(data->file_dialog, data->window, NULL, finish_open_file_dialog, NULL);
}

static void activate (GtkApplication *app, gpointer user_data) {
    GtkWindow *window = (GtkWindow*)gtk_application_window_new(app);

    GtkBox *box = (GtkBox*)gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkFileDialog *file_dialog = gtk_file_dialog_new();
	gtk_file_dialog_set_modal(file_dialog, true);

    struct button_pressed_data *data = g_new0(struct button_pressed_data, 1);
    data->window = window;
    data->file_dialog = file_dialog;
    
    GtkButton *read_button = (GtkButton*)gtk_button_new_with_label("Read");
    g_signal_connect(read_button, "clicked", G_CALLBACK (button_pressed_callback), data);
    gtk_box_append(box, (GtkWidget*)read_button);
    
    gtk_window_set_child (GTK_WINDOW (window), (GtkWidget*)box);
    
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("net.dranoel.bmp_file", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

      return status;
}
