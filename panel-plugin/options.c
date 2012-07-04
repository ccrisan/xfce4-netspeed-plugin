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

#include <libxfce4util/libxfce4util.h>

#include "options.h"

NetSpeedOptions *net_speed_options_new() {
    NetSpeedOptions *options = g_new0(NetSpeedOptions, 1);
    
    return options;
}

void net_speed_options_free(NetSpeedOptions *options) {
    if (options->device) {
        g_free(options->device);
    }
    
    g_free(options);
}

gboolean net_speed_options_load(NetSpeedOptions *options, XfcePanelPlugin *plugin) {
    gchar *file = xfce_panel_plugin_save_location(plugin, FALSE);
    if (G_UNLIKELY(!file))
        return FALSE;

    XfceRc *rc = xfce_rc_simple_open(file, FALSE);
    g_free(file);

    options->refresh_interval = REFRESH_INTERVAL;
    
    options->show_frame = xfce_rc_read_bool_entry(rc, "show_frame",  TRUE);
    options->show_icon = xfce_rc_read_bool_entry(rc, "show_icon",  TRUE);
    options->show_bits = xfce_rc_read_bool_entry(rc, "show_bits",  FALSE);
    options->show_sum = xfce_rc_read_bool_entry(rc, "show_sum",  FALSE);
    options->device = (gchar *) xfce_rc_read_entry(rc, "device",  NULL);
    options->device = g_strdup(options->device);
    options->fixed_width = xfce_rc_read_int_entry(rc, "fixed_width",  0);
    
    xfce_rc_close(rc);
    
    return TRUE;
}

gboolean net_speed_options_save(NetSpeedOptions *options, XfcePanelPlugin *plugin) {
    gchar *file = xfce_panel_plugin_save_location(plugin, TRUE);
    if (G_UNLIKELY(!file))
        return FALSE;

    XfceRc *rc = xfce_rc_simple_open(file, FALSE);
    g_free(file);

    xfce_rc_write_bool_entry(rc, "show_frame", options->show_frame);
    xfce_rc_write_bool_entry(rc, "show_icon", options->show_icon);
    xfce_rc_write_bool_entry(rc, "show_bits", options->show_bits);
    xfce_rc_write_bool_entry(rc, "show_sum", options->show_sum);
    xfce_rc_write_entry(rc, "device", options->device ? options->device : "");
    xfce_rc_write_int_entry(rc, "fixed_width", options->fixed_width);

    xfce_rc_close(rc);
    
    return TRUE;
}
