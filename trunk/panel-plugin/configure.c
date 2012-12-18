/*  xfce4-netspeed-plugin
 *
 *  Copyright (c) 2011 Calin Crisan <ccrisan@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxfce4ui/libxfce4ui.h>

#include "configure.h"

#ifndef _
# include <libintl.h>
# define _(String) gettext (String)
#endif


static void net_speed_configure_response(GtkWidget *dialog, int response, NetSpeedConfigure *net_speed_configure);
static void net_speed_configure_changed(GtkWidget *widget, NetSpeedConfigure *net_speed_configure);


static void net_speed_configure_response(GtkWidget *dialog, int response, NetSpeedConfigure *net_speed_configure) {
    NetSpeedPlugin *net_speed_plugin = net_speed_configure->net_speed_plugin;
    
    xfce_panel_plugin_unblock_menu(net_speed_plugin->plugin);

    gtk_widget_destroy(dialog);

    net_speed_options_save(net_speed_plugin->options, net_speed_plugin->plugin);
    
    g_free(net_speed_configure);
}

static void net_speed_configure_changed(GtkWidget *widget, NetSpeedConfigure *net_speed_configure) {
    NetSpeedPlugin *net_speed_plugin = net_speed_configure->net_speed_plugin;
    
    /* device */
    if (net_speed_plugin->options->device) {
        g_free(net_speed_plugin->options->device);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(net_speed_configure->auto_device_check_button))) {
        gtk_widget_set_sensitive(net_speed_configure->device_combo, FALSE);
        net_speed_plugin->options->device = NULL;
    }
    else {
        gtk_widget_set_sensitive(net_speed_configure->device_combo, TRUE);
        net_speed_plugin->options->device =
            gtk_combo_box_get_active_text(GTK_COMBO_BOX(net_speed_configure->device_combo));
    }
    
    /* show frame */
    net_speed_plugin->options->show_frame =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(net_speed_configure->show_frame_check_button));

    /* show icon */
    net_speed_plugin->options->show_icon =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(net_speed_configure->show_icon_check_button));

    /* show bits */
    net_speed_plugin->options->show_bits =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(net_speed_configure->show_bits_check_button));

    /* show sum */
    net_speed_plugin->options->show_sum =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(net_speed_configure->show_sum_check_button));
    
    /* fixed width */
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(net_speed_configure->fixed_width_check_button))) {
        net_speed_plugin->options->fixed_width =
            gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(net_speed_configure->fixed_width_spin_button));
        gtk_widget_set_sensitive(net_speed_configure->fixed_width_spin_button, TRUE);
    }
    else {
        net_speed_plugin->options->fixed_width = 0;
        gtk_widget_set_sensitive(net_speed_configure->fixed_width_spin_button, FALSE);
    }

    net_speed_update_options(net_speed_plugin);
}

void net_speed_configure_show(XfcePanelPlugin *plugin, NetSpeedPlugin *net_speed_plugin) {
    GtkWidget *dialog;
    GtkWidget *dialog_vbox;
    GtkWidget *hbox;
    GtkWidget *label;
    GtkWidget *combo;
    GtkWidget *button;
    
    /* net speed configure */
    NetSpeedConfigure *net_speed_configure = g_new0(NetSpeedConfigure, 1);
    net_speed_configure->net_speed_plugin = net_speed_plugin;

    /* dialog */
    xfce_panel_plugin_block_menu(net_speed_plugin->plugin);
    net_speed_configure->dialog = dialog = xfce_titled_dialog_new_with_buttons(_("Network Speed Plugin"),
        NULL, GTK_DIALOG_NO_SEPARATOR, GTK_STOCK_CLOSE, GTK_RESPONSE_OK, NULL);
    xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog), _("Configuration"));
    
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    gtk_window_set_icon_name(GTK_WINDOW(dialog), "xfce4-netspeed-plugin");
    gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);
    gtk_window_stick(GTK_WINDOW(dialog));

    dialog_vbox = GTK_DIALOG(dialog)->vbox;

    /* auto-device */    
    hbox = gtk_hbox_new(FALSE, 3);
    gtk_container_add(GTK_CONTAINER(dialog_vbox), hbox);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 3);
    
    net_speed_configure->auto_device_check_button = button = gtk_check_button_new_with_mnemonic(_("Automatically Choose Network Device"));
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), net_speed_plugin->options->device == NULL);
    g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(net_speed_configure_changed), net_speed_configure);
    
    /* device */
    hbox = gtk_hbox_new(FALSE, 3);
    gtk_container_add(GTK_CONTAINER(dialog_vbox), hbox);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 3);
    
    label = gtk_label_new(_("Network Device:"));
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
    
    net_speed_configure->device_combo = combo = gtk_combo_box_new_text();
    gtk_box_pack_start(GTK_BOX(hbox), combo, FALSE, TRUE, 0);
    
    GList *device, *devices = device_info_list_get();
    gint index = 0, sel_index = -1;
    for (device = devices; device != NULL; device = g_list_next(device)) {
        gtk_combo_box_append_text(GTK_COMBO_BOX(combo), device->data);
        if (net_speed_plugin->options->device && !strcmp(device->data, net_speed_plugin->options->device)) {
            sel_index = index;
        }
        index++;
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), sel_index);
    g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(net_speed_configure_changed), net_speed_configure);
    device_info_list_free(devices);
    
    gtk_widget_set_sensitive(net_speed_configure->device_combo, net_speed_plugin->options->device != NULL);

    /* show frame */
    hbox = gtk_hbox_new(FALSE, 3);
    gtk_container_add(GTK_CONTAINER(dialog_vbox), hbox);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 3);
    
    net_speed_configure->show_frame_check_button = button = gtk_check_button_new_with_mnemonic(_("Show Frame"));
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), net_speed_plugin->options->show_frame);
    g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(net_speed_configure_changed), net_speed_configure);

    /* show icon */
    hbox = gtk_hbox_new(FALSE, 3);
    gtk_container_add(GTK_CONTAINER(dialog_vbox), hbox);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 3);
    
    net_speed_configure->show_icon_check_button = button = gtk_check_button_new_with_mnemonic(_("Show Icon"));
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), net_speed_plugin->options->show_icon);
    g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(net_speed_configure_changed), net_speed_configure);

    /* show bits */
    hbox = gtk_hbox_new(FALSE, 3);
    gtk_container_add(GTK_CONTAINER(dialog_vbox), hbox);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 3);
    
    net_speed_configure->show_bits_check_button = button = gtk_check_button_new_with_mnemonic(_("Show Bits Instead of Bytes"));
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), net_speed_plugin->options->show_bits);
    g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(net_speed_configure_changed), net_speed_configure);

    /* show sum */
    hbox = gtk_hbox_new(FALSE, 3);
    gtk_container_add(GTK_CONTAINER(dialog_vbox), hbox);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 3);
    
    net_speed_configure->show_sum_check_button = button = gtk_check_button_new_with_mnemonic(_("Show Sum of Download and Upload"));
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), net_speed_plugin->options->show_sum);
    g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(net_speed_configure_changed), net_speed_configure);

    /* fixed width check button */
    hbox = gtk_hbox_new(FALSE, 3);
    gtk_container_add(GTK_CONTAINER(dialog_vbox), hbox);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 3);
    
    net_speed_configure->fixed_width_check_button = button = gtk_check_button_new_with_mnemonic(_("Fixed Width"));
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), (gboolean) net_speed_plugin->options->fixed_width);
    g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(net_speed_configure_changed), net_speed_configure);

    net_speed_configure->fixed_width_spin_button = button = gtk_spin_button_new_with_range(20, 500, 10);
    gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(button), (gdouble) net_speed_plugin->options->fixed_width);
    g_signal_connect(G_OBJECT(button), "value-changed", G_CALLBACK(net_speed_configure_changed), net_speed_configure);

    g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(net_speed_configure_response), net_speed_configure);

    gtk_widget_show_all(dialog);
}
