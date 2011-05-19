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

#ifndef _CONFIGURE_H
#define _CONFIGURE_H

#include "xfce4_netspeed_plugin.h"

typedef struct {

    GtkWidget *dialog;
    
    GtkWidget *device_combo;
    GtkWidget *refresh_interval_spinner;
    GtkWidget *show_frame_check_button;
    GtkWidget *show_icon_check_button;
    GtkWidget *show_bits_check_button;
    GtkWidget *show_sum_check_button;
    
    NetSpeedPlugin *net_speed_plugin;

} NetSpeedConfigure;


void    net_speed_configure_show(XfcePanelPlugin *plugin, NetSpeedPlugin *net_speed_plugin);

#endif /* _CONFIGURE_H */
