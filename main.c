#include <gtk/gtk.h>

#include "bmp.h"

struct button_pressed_data {
    GtkFileDialog *file_dialog;
    GtkWindow *window;
	GtkImage *image;
	struct image_data *image_data;
};

static void update_image(struct button_pressed_data *data) {
	struct image_data *image_data = data->image_data;
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(image_data->data, GDK_COLORSPACE_RGB, true, 8, image_data->width, image_data->height, image_data->width * 4, NULL, NULL);
	GdkTexture *texture = gdk_texture_new_for_pixbuf(pixbuf);
	gtk_image_set_from_paintable(data->image, (GdkPaintable*)texture);
}

static void finish_open_file_dialog(GObject *self, GAsyncResult *res, gpointer user_data) {
	struct button_pressed_data *data = user_data;

	GtkFileDialog *file_dialog = (GtkFileDialog*)self;
    GFile *file = gtk_file_dialog_open_finish(file_dialog, res, NULL);
    char *path = g_file_get_path(file);

	char *error;
	struct image_data *image_data = read_image(path, &error);

	if (image_data) {
		if (data->image_data) {
			g_free(data->image_data);
		}

		data->image_data = image_data;
		update_image(data);
	} else {
		GtkAlertDialog *dialog = gtk_alert_dialog_new(
			"An error occured: %s",
			error
		);
		gtk_alert_dialog_show(dialog, data->window);

		g_free(dialog);
	}

    g_free(path);
    g_free(file);
}

static void button_pressed_callback(GtkWidget *button, struct button_pressed_data *data) {
    gtk_file_dialog_open(data->file_dialog, data->window, NULL, finish_open_file_dialog, data);
}

const char *choices[] = {"Hello", "Space Invader", NULL};

static void activate (GtkApplication *app, gpointer user_data) {
    GtkWindow *window = (GtkWindow*)gtk_application_window_new(app);

	GtkImage *image = (GtkImage*)gtk_image_new();
	gtk_widget_set_size_request((GtkWidget*)image, 500, 500);
    
    GtkFileDialog *file_dialog = gtk_file_dialog_new();
	gtk_file_dialog_set_modal(file_dialog, true);

    struct button_pressed_data *data = g_new0(struct button_pressed_data, 1);
    data->window = window;
    data->file_dialog = file_dialog;
	data->image = image;
	data->image_data = NULL;
    
    GtkButton *read_button = (GtkButton*)gtk_button_new_with_label("Read");
    g_signal_connect(read_button, "clicked", G_CALLBACK (button_pressed_callback), data);

	GtkButton *load_button = (GtkButton*)gtk_button_new_with_label("Load");
	
	GtkDropDown *dropdown = (GtkDropDown*)gtk_drop_down_new_from_strings(choices);

	GtkGrid *grid = (GtkGrid*)gtk_grid_new();
	gtk_grid_attach(grid, (GtkWidget*)image, 0, 0, 2, 1);
	gtk_grid_attach(grid, (GtkWidget*)load_button, 0, 1, 1, 1);
	gtk_grid_attach(grid, (GtkWidget*)dropdown, 0, 2, 1, 1);
	gtk_grid_attach(grid, (GtkWidget*)read_button, 1, 1, 1, 1);

    gtk_window_set_child (GTK_WINDOW (window), (GtkWidget*)grid);
    
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
