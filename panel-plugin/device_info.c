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


#include <glibtop/netlist.h>
#include <glibtop/netload.h>

#include "device_info.h"


DeviceInfo *device_info_new(char *device) {
    DeviceInfo *device_info = g_new0(DeviceInfo, 1);
    
    device_info->name = g_strdup(device);
    device_info_fill(device_info);    
    
    return device_info;
}

void device_info_free(DeviceInfo *device_info) {
    g_free(device_info->name);
    g_free(device_info);
}

void device_info_fill(DeviceInfo *device_info) {
    glibtop_netload netload;
    glibtop_get_netload(&netload, device_info->name);
    
    device_info->tx = netload.bytes_out;
    device_info->rx = netload.bytes_in;

    device_info->up = (netload.if_flags & (1L << GLIBTOP_IF_FLAGS_UP) ? TRUE : FALSE);
    
    if (netload.if_flags & (1L << GLIBTOP_IF_FLAGS_LOOPBACK)) {
        device_info->type = DEVICE_TYPE_LOOPBACK;
    }
    else if (netload.if_flags & (1L << GLIBTOP_IF_FLAGS_WIRELESS)) {
        device_info->type = DEVICE_TYPE_WIRELESS;
    }
    else if (netload.if_flags & (1L << GLIBTOP_IF_FLAGS_POINTOPOINT)) {
        if (g_str_has_prefix(device_info->name, "plip")) {
            device_info->type = DEVICE_TYPE_PLIP;
        }
        else if (g_str_has_prefix(device_info->name, "sl")) {
            device_info->type = DEVICE_TYPE_SLIP;
        }
        else {
            device_info->type = DEVICE_TYPE_PPP;
        }
    }
    else {
        device_info->type = DEVICE_TYPE_ETHERNET;
    }
}

GList* device_info_list_get() {
    glibtop_netlist netlist;
    char **devices, **dev;
    GList *device_glist = NULL;

    devices = glibtop_get_netlist(&netlist);

    for (dev = devices; *dev; ++dev) {
        device_glist = g_list_prepend(device_glist, g_strdup(*dev));
    }

    g_strfreev(devices);

    return device_glist;
}

void device_info_list_free(GList *list) {
    g_list_foreach(list, (GFunc) g_free, NULL);
    g_list_free(list);
}
