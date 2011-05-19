plugindir = $(libexecdir)/xfce4/panel-plugins
plugin_PROGRAMS = xfce4-netspeed-plugin

xfce4_netspeed_plugin_CFLAGS =					\
	-I$(top_srcdir)						\
	@LIBXFCE4PANEL_CFLAGS@					\
	@LIBXFCE4UTIL_CFLAGS@					\
	@LIBXFCEGUI4_CFLAGS@					\
	@LIBGTOP_CFLAGS@					\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"

xfce4_netspeed_plugin_LDADD =					\
	@LIBXFCE4PANEL_LIBS@					\
	@LIBXFCE4UTIL_LIBS@					\
	@LIBXFCEGUI4_LIBS@					\
	@LIBGTOP_LIBS@

xfce4_netspeed_plugin_SOURCES =					\
	xfce4_netspeed_plugin.h					\
	xfce4_netspeed_plugin.c					\
	options.h						\
	options.c						\
	device_info.h						\
	device_info.c						\
	utils.h							\
	utils.c							\
	configure.h						\
	configure.c

desktop_in_in_files = xfce4-netspeed-plugin.desktop.in.in
desktop_in_files = $(desktop_in_in_files:.desktop.in.in=.desktop.in)
	
desktopdir = $(datadir)/xfce4/panel-plugins
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = 								\
	$(desktop_in_in_files)

DISTCLEANFILES =							\
	$(desktop_DATA) $(desktop_in_files)
	
# get full path into .desktop file
%.desktop.in: %.desktop.in.in
	sed -e "s^@PLUGIN_PATH@^$(libexecdir)/xfce4/panel-plugins^"	\
		$< > $@
