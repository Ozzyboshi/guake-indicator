iconthemedir = $(datadir)/icons/hicolor

appsiconxpmdir=$(datadir)/pixmaps/guake-indicator
appsicon16dir = $(iconthemedir)/16x16/apps
appsicon22dir = $(iconthemedir)/22x22/apps
appsicon24dir = $(iconthemedir)/24x24/apps
appsicon32dir = $(iconthemedir)/32x32/apps
appsicon48dir = $(iconthemedir)/48x48/apps
appsicon256dir = $(iconthemedir)/256x256/apps

dist_appsiconxpm_DATA = guake-indicator.xpm 
dist_appsicon16_DATA = 16x16/apps/guake-indicator.png 
dist_appsicon22_DATA = 22x22/apps/guake-indicator.png 
dist_appsicon24_DATA = 24x24/apps/guake-indicator.png 
dist_appsicon32_DATA = 32x32/apps/guake-indicator.png 
dist_appsicon48_DATA = 48x48/apps/guake-indicator.png 
dist_appsicon256_DATA = 256x256/apps/guake-indicator.png

update_icon_cache = gtk-update-icon-cache --ignore-theme-index --force

install-data-hook: install-update-icon-cache
uninstall-hook: uninstall-update-icon-cache

install-update-icon-cache:
		$(AM_V_at)$(POST_INSTALL)
		test -n "$(DESTDIR)" || $(update_icon_cache) "$(iconthemedir)"

uninstall-update-icon-cache:
		$(AM_V_at)$(POST_UNINSTALL)
		test -n "$(DESTDIR)" || $(update_icon_cache) "$(iconthemedir)"
		
