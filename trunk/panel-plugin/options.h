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

#ifndef _OPTIONS_H
#define _OPTIONS_H

#include <glib.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#define REFRESH_INTERVAL    1000 /* 1000 milliseconds */


typedef struct {

    guint refresh_interval;
    gchar *device;
    gboolean show_frame;
    gboolean show_icon;
    gboolean show_bits; /* instead of bytes */
    gboolean show_sum; /* instead of separate rx, tx */

} NetSpeedOptions;


NetSpeedOptions *   net_speed_options_new();
void                net_speed_options_free(NetSpeedOptions *options);
gboolean            net_speed_options_load(NetSpeedOptions *options, XfcePanelPlugin *plugin);
gboolean            net_speed_options_save(NetSpeedOptions *options, XfcePanelPlugin *plugin);

#endif /* _OPTIONS_H */
