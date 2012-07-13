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

#include "xfce4_netspeed_plugin.h"

#ifndef _
# include <libintl.h>
# define _(String) gettext (String)
#endif


    /* tcons for the network interfaces */
static const char* device_type_icons[DEVICE_TYPE_WIRELESS + 1] = {
    "network-workgroup",         /* unknown */
    "netspeed-loopback",         /* looppack */
    "netspeed-plip",             /* plip */
    "netspeed-slip",             /* slip */
    "netspeed-ppp",              /* ppp */
    "network-wired",             /* ethernet */
    "network-wireless"           /* wireless */
};


    /* local prototypes */
static void         net_speed_restart_timeout(NetSpeedPlugin *net_speed_plugin);
static gboolean     net_speed_orientation_changed(XfcePanelPlugin *plugin, GtkOrientation orientation, NetSpeedPlugin *net_speed_plugin);
static gboolean     net_speed_size_changed(XfcePanelPlugin *plugin, gint size, NetSpeedPlugin *net_speed_plugin);

static void         net_speed_create_widgets(NetSpeedPlugin *net_speed_plugin);

static void         net_speed_save_options(XfcePanelPlugin *plugin, NetSpeedPlugin *net_speed_plugin);

static void         net_speed_new(XfcePanelPlugin *plugin);
static void         net_speed_free(XfcePanelPlugin *plugin, NetSpeedPlugin *net_speed_plugin);


void net_speed_update_labels(NetSpeedPlugin *net_speed_plugin) {
    if (net_speed_plugin->device_info) { /* a device has been chosen */
        if (net_speed_plugin->device_info->up) { /* interface up */
            if (net_speed_plugin->options->show_sum) { /* rx + tx */
                gchar *text = bytes_to_string((double) (net_speed_plugin->rx_speed + net_speed_plugin->tx_speed), TRUE, 
                    net_speed_plugin->options->show_bits);

                gtk_label_set_markup(GTK_LABEL(net_speed_plugin->rx_label), text);
                gtk_label_set_markup(GTK_LABEL(net_speed_plugin->tx_label), "");
            
                g_free(text);
                
                gtk_widget_hide(GTK_WIDGET(net_speed_plugin->rx_icon));
                gtk_widget_hide(GTK_WIDGET(net_speed_plugin->tx_icon));
            }
            else { /* separate rx & tx */
                gchar *text = bytes_to_string((double) net_speed_plugin->rx_speed, TRUE, net_speed_plugin->options->show_bits);
                gtk_label_set_markup(GTK_LABEL(net_speed_plugin->rx_label), text);
                g_free(text);
            
                text = bytes_to_string((double) net_speed_plugin->tx_speed, TRUE, net_speed_plugin->options->show_bits);
                gtk_label_set_markup(GTK_LABEL(net_speed_plugin->tx_label), text);
                g_free(text);
                
                gtk_widget_show(GTK_WIDGET(net_speed_plugin->rx_icon));
                gtk_widget_show(GTK_WIDGET(net_speed_plugin->tx_icon));
            }
        }
        else { /* interface down */
            gtk_label_set_markup(GTK_LABEL(net_speed_plugin->rx_label), _("(network down)"));
            gtk_label_set_markup(GTK_LABEL(net_speed_plugin->tx_label), "");
            
            gtk_widget_hide(GTK_WIDGET(net_speed_plugin->rx_icon));
            gtk_widget_hide(GTK_WIDGET(net_speed_plugin->tx_icon));
        }
    }
    else { /* no device has been chosen in the configuration */
        gtk_label_set_markup(GTK_LABEL(net_speed_plugin->rx_label), _("(no device)"));
        gtk_label_set_markup(GTK_LABEL(net_speed_plugin->tx_label), "");
    }

    /* update label's orientation */
    if (xfce_panel_plugin_get_orientation(net_speed_plugin->plugin) == GTK_ORIENTATION_VERTICAL) {
        gtk_label_set_angle(GTK_LABEL(net_speed_plugin->rx_label), -90);
        gtk_label_set_angle(GTK_LABEL(net_speed_plugin->tx_label), -90);
    }
    else {
        gtk_label_set_angle(GTK_LABEL(net_speed_plugin->rx_label), 0);
        gtk_label_set_angle(GTK_LABEL(net_speed_plugin->tx_label), 0);
    }
}

void net_speed_update_tooltip(NetSpeedPlugin *net_speed_plugin) {
    gchar tooltip_text[256];
    
    if (net_speed_plugin->device_info) { /* a device has been chosen */
        gchar *rx_text = bytes_to_string((double) net_speed_plugin->rx_speed, TRUE, net_speed_plugin->options->show_bits);        
        gchar *tx_text = bytes_to_string((double) net_speed_plugin->tx_speed, TRUE, net_speed_plugin->options->show_bits);
        
        g_snprintf(tooltip_text, 256, "%s: %s\n%s: %s %s: %s",
            _("Interface"), net_speed_plugin->device_info->name, _("Download"), rx_text, _("Upload"), tx_text);

        g_free(rx_text);
        g_free(tx_text);
    }
    else { /* no device has been chosen in the configuration */
        g_snprintf(tooltip_text, 256, _("(no device)"));
    }

    gtk_tooltips_set_tip(net_speed_plugin->tooltip, net_speed_plugin->ebox, tooltip_text, NULL);
}

void net_speed_update_icon(NetSpeedPlugin *net_speed_plugin) {
    if (net_speed_plugin->icon) { /* destroy the previous icon */
        gtk_widget_destroy(net_speed_plugin->icon);
        net_speed_plugin->icon = NULL;
    }

    if (net_speed_plugin->options->show_icon) {
        GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
        GdkPixbuf *buf = NULL;

        if (net_speed_plugin->device_info) {
            if (net_speed_plugin->device_info->up) { /* interface up */
                buf = gtk_icon_theme_load_icon(icon_theme,
                    device_type_icons[net_speed_plugin->device_info->type], net_speed_plugin->icon_size, 0, NULL);
            }
            else { /* interface down */
                buf = gtk_icon_theme_load_icon(icon_theme, "gtk-dialog-error", net_speed_plugin->icon_size, 0, NULL);
            }
        }
        
        if (!buf) { /* no device selected / fallback */
            buf = gtk_icon_theme_load_icon(icon_theme,
                device_type_icons[DEVICE_TYPE_UNKNOWN], net_speed_plugin->icon_size, 0, NULL);
        }

        net_speed_plugin->icon = gtk_image_new_from_pixbuf(buf);
        g_object_unref(buf);

        gtk_box_pack_start(GTK_BOX(net_speed_plugin->box), net_speed_plugin->icon, FALSE, FALSE, 0);
        gtk_widget_show(net_speed_plugin->icon);
    }
}

void net_speed_update_options(NetSpeedPlugin *net_speed_plugin) {
    /* device */
    if (net_speed_plugin->device_info) {
        device_info_free(net_speed_plugin->device_info);
        net_speed_plugin->device_info = NULL;
    }

    if (net_speed_plugin->options->device) {
        net_speed_plugin->device_info = device_info_new(net_speed_plugin->options->device);
    }
    
    /* show/hide frame */
    gtk_frame_set_shadow_type(GTK_FRAME(net_speed_plugin->frame), 
        net_speed_plugin->options->show_frame ? GTK_SHADOW_IN : GTK_SHADOW_NONE);

    /* fixed width */    
    if (net_speed_plugin->options->fixed_width) {
        gtk_widget_set_size_request(net_speed_plugin->frame, net_speed_plugin->options->fixed_width, net_speed_plugin->options->fixed_width);
    }
    else {
        gtk_widget_set_size_request(net_speed_plugin->frame, -1, -1);
    }

    /* show/hide icon */
    net_speed_update_icon(net_speed_plugin);
    
    /* clear the history */
    net_speed_plugin->history_index = -1;
    net_speed_plugin->rx_speed = 0;
    net_speed_plugin->tx_speed = 0;
    
    /* restart the timer */
    net_speed_restart_timeout(net_speed_plugin);
    
    gtk_widget_set_size_request(GTK_WIDGET(net_speed_plugin->plugin), -1, -1);

    /* update the plugin as well */
    net_speed_update_plugin(net_speed_plugin);
}

gboolean net_speed_update_plugin(NetSpeedPlugin *net_speed_plugin) {
    /* if the is automatically chosen,
     * we need to iterate through the device list
     * with each plugin update, and find the most "active" one */
    if (net_speed_plugin->options->device == NULL) {
        if (net_speed_plugin->device_info) {
            device_info_free(net_speed_plugin->device_info);
            net_speed_plugin->device_info = NULL;
        }

        GList *device, *devices = device_info_list_get();
        DeviceInfo *device_info, *running_device_info = NULL;
        for (device = devices; device != NULL; device = g_list_next(device)) {
            device_info = device_info_new(device->data);
            device_info_fill(device_info);

            if (device_info->running && device_info->type != DEVICE_TYPE_LOOPBACK) {
                running_device_info = device_info;
                break;
            }
            else {
                device_info_free(device_info);
            }
        }
        device_info_list_free(devices);
        
        if (net_speed_plugin->device_info) {
            device_info_free(net_speed_plugin->device_info);
        }
        
        net_speed_plugin->device_info = running_device_info;
    }

    if (net_speed_plugin->device_info) { /* a device has been chosen */
        gint i;
        
        if (net_speed_plugin->history_index >= HISTORY_LENGTH - 1) {
            /* shift the history by one position */
            for (i = 0; i < HISTORY_LENGTH - 1; i++) {
                net_speed_plugin->history_rx[i] = net_speed_plugin->history_rx[i + 1];
                net_speed_plugin->history_tx[i] = net_speed_plugin->history_tx[i + 1];
            }
        }
        else {
            net_speed_plugin->history_index++;
        }
        
        gboolean old_up = net_speed_plugin->device_info->up;
        if (net_speed_plugin->options->device != NULL) { /* if device was guessed, the device_info is already filled */
            device_info_fill(net_speed_plugin->device_info);
        }

        net_speed_plugin->history_rx[net_speed_plugin->history_index] = net_speed_plugin->device_info->rx;
        net_speed_plugin->history_tx[net_speed_plugin->history_index] = net_speed_plugin->device_info->tx;
        
        if (net_speed_plugin->history_index >= 1) { /* we have at least two values */
            guint64 delta_rx = net_speed_plugin->history_rx[net_speed_plugin->history_index] - net_speed_plugin->history_rx[0];
            guint64 delta_tx = net_speed_plugin->history_tx[net_speed_plugin->history_index] - net_speed_plugin->history_tx[0];
            
            net_speed_plugin->rx_speed = (delta_rx) * 1000 / (net_speed_plugin->options->refresh_interval * net_speed_plugin->history_index);
            net_speed_plugin->tx_speed = (delta_tx) * 1000 / (net_speed_plugin->options->refresh_interval * net_speed_plugin->history_index);
        }
        
        net_speed_update_icon(net_speed_plugin);
    }

    net_speed_update_labels(net_speed_plugin);
    net_speed_update_tooltip(net_speed_plugin);

    return TRUE;
}


void net_speed_restart_timeout(NetSpeedPlugin *net_speed_plugin) {
    if (net_speed_plugin->timeout_handle) {
        g_source_remove(net_speed_plugin->timeout_handle);
    }
    
    net_speed_plugin->timeout_handle = g_timeout_add(net_speed_plugin->options->refresh_interval,
        (GSourceFunc) net_speed_update_plugin, net_speed_plugin);
}

static gboolean net_speed_orientation_changed(XfcePanelPlugin *plugin, GtkOrientation orientation, NetSpeedPlugin *net_speed_plugin) {
    gtk_orientable_set_orientation(GTK_ORIENTABLE(net_speed_plugin->box), orientation);
    net_speed_update_options(net_speed_plugin);

    return TRUE;
}

static gboolean net_speed_size_changed(XfcePanelPlugin *plugin, gint size, NetSpeedPlugin *net_speed_plugin) {
    net_speed_plugin->icon_size = size - 4;
    net_speed_update_options(net_speed_plugin);

    return TRUE;
}

static void net_speed_create_widgets(NetSpeedPlugin *net_speed_plugin) {
    GtkOrientation orientation = xfce_panel_plugin_get_orientation(net_speed_plugin->plugin);
    net_speed_plugin->icon_size = xfce_panel_plugin_get_size(net_speed_plugin->plugin) - 4;
    GtkIconTheme *icon_theme = gtk_icon_theme_get_default();

    /* event box */
    net_speed_plugin->ebox = gtk_event_box_new();
    xfce_panel_plugin_add_action_widget(net_speed_plugin->plugin, net_speed_plugin->ebox);
    gtk_container_add(GTK_CONTAINER(net_speed_plugin->plugin), net_speed_plugin->ebox);

    /* frame */
    net_speed_plugin->frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(net_speed_plugin->frame), net_speed_plugin->options->show_frame ? GTK_SHADOW_IN : GTK_SHADOW_NONE);
    gtk_container_add(GTK_CONTAINER(net_speed_plugin->ebox), net_speed_plugin->frame);

    /* box */
    net_speed_plugin->box = gtk_hbox_new(FALSE, WIDGET_SPACING);
    gtk_container_set_border_width(GTK_CONTAINER(net_speed_plugin->box), WIDGET_BORDER);
    gtk_container_add(GTK_CONTAINER(net_speed_plugin->frame), net_speed_plugin->box);

    /* tx label */
    net_speed_plugin->tx_label = gtk_label_new(NULL);
    gtk_box_pack_end(GTK_BOX(net_speed_plugin->box), net_speed_plugin->tx_label, FALSE, FALSE, 0);
    gtk_widget_show(net_speed_plugin->tx_label);
    
    /* tx icon */
    GdkPixbuf *tx_arrow_buf = gtk_icon_theme_load_icon(icon_theme, "gtk-go-up", 16, 0, NULL);
    if (tx_arrow_buf) {
        net_speed_plugin->tx_icon = gtk_image_new_from_pixbuf(tx_arrow_buf);
        g_object_unref(tx_arrow_buf);
        
        gtk_box_pack_end(GTK_BOX(net_speed_plugin->box), net_speed_plugin->tx_icon, FALSE, FALSE, 0);
        gtk_widget_show(net_speed_plugin->tx_icon);
    }

    /* rx label */
    net_speed_plugin->rx_label = gtk_label_new(NULL);
    gtk_box_pack_end(GTK_BOX(net_speed_plugin->box), net_speed_plugin->rx_label, FALSE, FALSE, 0);
    gtk_widget_show(net_speed_plugin->rx_label);

    /* rx icon */
    GdkPixbuf *rx_arrow_buf = gtk_icon_theme_load_icon(icon_theme, "gtk-go-down", 16, 0, NULL);
    if (rx_arrow_buf) {
        net_speed_plugin->rx_icon = gtk_image_new_from_pixbuf(rx_arrow_buf);
        g_object_unref(rx_arrow_buf);

        gtk_box_pack_end(GTK_BOX(net_speed_plugin->box), net_speed_plugin->rx_icon, FALSE, FALSE, 0);
        gtk_widget_show(net_speed_plugin->rx_icon);
    }

    /* tooltip */
    net_speed_plugin->tooltip = gtk_tooltips_new();
    g_object_ref(G_OBJECT(net_speed_plugin->tooltip));
    gtk_tooltips_set_tip(net_speed_plugin->tooltip, net_speed_plugin->ebox, "", NULL);

    /* this will create, add & show the main icon;
     * it will also update the label and the tooltip */
    net_speed_orientation_changed(net_speed_plugin->plugin, orientation, net_speed_plugin);
    
    /* show the containers */
    gtk_widget_show(net_speed_plugin->frame);
    gtk_widget_show(net_speed_plugin->box);
    gtk_widget_show(net_speed_plugin->ebox);
}

static void net_speed_save_options(XfcePanelPlugin *plugin, NetSpeedPlugin *net_speed_plugin) {
    net_speed_options_save(net_speed_plugin->options, plugin);
}

static void net_speed_new(XfcePanelPlugin *plugin) {
    /* translations */
    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    /* plugin */
    NetSpeedPlugin *net_speed_plugin = g_new0(NetSpeedPlugin, 1);
    net_speed_plugin->plugin = plugin;

    /* options */
    net_speed_plugin->options = net_speed_options_new();
    net_speed_options_load(net_speed_plugin->options, plugin);
    
    net_speed_create_widgets(net_speed_plugin);

    /* apply the options */
    net_speed_update_options(net_speed_plugin);
    
    /* start the update timer */
    net_speed_restart_timeout(net_speed_plugin);

    /* signals */
    g_signal_connect(plugin, "free-data", G_CALLBACK(net_speed_free), net_speed_plugin);
    g_signal_connect(plugin, "save", G_CALLBACK(net_speed_save_options), net_speed_plugin);
    g_signal_connect(plugin, "size-changed", G_CALLBACK(net_speed_size_changed), net_speed_plugin);
    g_signal_connect(plugin, "orientation-changed", G_CALLBACK(net_speed_orientation_changed), net_speed_plugin);
    g_signal_connect(plugin, "configure-plugin", G_CALLBACK(net_speed_configure_show), net_speed_plugin);

    /* the configure and about menu items are hidden by default */
    xfce_panel_plugin_menu_show_configure(plugin);
}

static void net_speed_free(XfcePanelPlugin *plugin, NetSpeedPlugin *net_speed_plugin) {
    if (net_speed_plugin->timeout_handle)
        g_source_remove(net_speed_plugin->timeout_handle);

    gtk_tooltips_set_tip(net_speed_plugin->tooltip, net_speed_plugin->ebox, NULL, NULL);
    g_object_unref(net_speed_plugin->tooltip);
    
    net_speed_plugin->plugin = NULL;
    
    net_speed_options_free(net_speed_plugin->options);
    if (net_speed_plugin->device_info) {
        device_info_free(net_speed_plugin->device_info);
    }
    
    g_free(net_speed_plugin);
}

XFCE_PANEL_PLUGIN_REGISTER_EXTERNAL(net_speed_new);
