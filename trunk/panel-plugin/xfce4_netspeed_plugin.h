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

#ifndef _XFCE4_NETSPEED_PLUGIN_H
#define _XFCE4_NETSPEED_PLUGIN_H

#include <unistd.h>

#include <libxfce4panel/xfce-panel-plugin.h>

#include "options.h"
#include "device_info.h"
#include "utils.h"

#define WIDGET_SPACING  2 /* space between the widgets */
#define WIDGET_BORDER   1 /* space between the frame and the widgets */

#define HISTORY_LENGTH  5 /* number of rx/tx samples to keep */


typedef struct {

    XfcePanelPlugin *plugin;

    /* Widgets */
    GtkWidget *ebox;
    GtkWidget *frame;
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *rx_icon;
    GtkWidget *tx_icon;
    GtkWidget *rx_label;
    GtkWidget *tx_label;
    GtkTooltips *tooltip;

    gint icon_size;

    NetSpeedOptions *options;
    gint timeout_handle;
    
    DeviceInfo *device_info;
    
    guint64 history_rx[HISTORY_LENGTH];
    guint64 history_tx[HISTORY_LENGTH];
    gint history_index;
    
    guint64 rx_speed;
    guint64 tx_speed;

} NetSpeedPlugin;

#include "configure.h"


void            net_speed_update_labels(NetSpeedPlugin *net_speed_plugin);
void            net_speed_update_tooltip(NetSpeedPlugin *net_speed_plugin);
void            net_speed_update_icon(NetSpeedPlugin *net_speed_plugin);
void            net_speed_update_options(NetSpeedPlugin *net_speed_plugin);
gboolean        net_speed_update_plugin(NetSpeedPlugin *net_speed_plugin);


#endif /* _XFCE4_NETSPEED_PLUGIN_H */
