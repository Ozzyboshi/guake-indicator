/*
Copyright (C) 2013-2019 Alessio Garzi <gun101@email.it>
Copyright (C) 2013-2019 Francesco Minà <mina.francesco@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#include <guake-indicator.h>
#include <guake-indicator-ayatana.h>
#include <string.h>
#include <stdlib.h>

void build_menu_ayatana(int argc, char **argv,GtkInfo* gtkinfo)
{
	GtkWidget *menu = NULL;
	AppIndicator *ci = NULL;
	GtkStyleContext *menu_context;
	GtkBorder padding,border;
	GtkWidget *submenu = NULL;
	int i=0;
	GtkWidget *item;
	Host* ptr;

	gtk_init (&argc, &argv);

	apply_css();

	ci = app_indicator_new ("guake-indicator",DEFAULT_ICON,APP_INDICATOR_CATEGORY_APPLICATION_STATUS);

    g_assert (IS_APP_INDICATOR (ci));
	g_assert (G_IS_OBJECT (ci));

    app_indicator_set_status (ci, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_icon_full(ci,"guake-indicator","guake-indicator");
	app_indicator_set_title (ci, "Guake indicator");
    g_signal_connect (ci, "scroll-event",G_CALLBACK (scroll_event_cb), NULL);

        menu = gtk_menu_new ();

        menu_context = gtk_widget_get_style_context (menu);
    	gtk_style_context_add_class (menu_context, "titlebar");

    	//gtk_style_context_add_class (menu_context, "titlebar");
    	
    	//GList* classes = gtk_style_context_list_classes (menu_context);

    	gtk_style_context_get_padding (menu_context, gtk_style_context_get_state (menu_context), &padding);
    	gtk_style_context_get_border (menu_context, gtk_style_context_get_state (menu_context), &border);

    	printf("\n\n%d %d %d %d %d %d\n",padding.left,border.left,padding.top,border.top,padding.bottom,border.bottom);

    	for (i=0;gtkinfo->grouphostlist!=NULL && i<gtkinfo->grouphostlist->len;i++)
        {
                HostGroup* hostgroup = g_array_index (gtkinfo->grouphostlist, HostGroup* , i);
                if (hostgroup->title)
                {
                        GtkWidget *toggle_item = gtk_menu_item_new_with_label (hostgroup->title);
                        menu_context = gtk_widget_get_style_context (toggle_item);
                        gtk_style_context_add_class (menu_context, "titlebar");
                        if (hostgroup->label==TRUE) gtk_widget_set_sensitive ((GtkWidget *)toggle_item,FALSE);
                        else
                        {
                                submenu = gtk_menu_new ();
                                for (ptr=hostgroup->hostarray;ptr;ptr=ptr->next)
                                {
                                    printf("%s\n",ptr->menu_name);
                                    append_submenu (submenu,ptr);
                                }
                                gtk_widget_show_all (submenu);

                               gtk_menu_item_set_submenu (GTK_MENU_ITEM (toggle_item), submenu);

                        }
                        gtk_menu_shell_append (GTK_MENU_SHELL (menu), toggle_item);
                        gtk_widget_show (toggle_item);
                }
                else
                {
                	for (ptr=hostgroup->hostarray;ptr;ptr=ptr->next)
                        {
                                if (ptr->open_in_tab==NULL)
                                {
                                        item = gtk_menu_item_new_with_label (ptr->menu_name);
                                        menu_context = gtk_widget_get_style_context (item);
                                        gtk_style_context_add_class (menu_context, "titlebar");
                                }
                                else
                                {
                                        gchar* menu_desc;
                                        if (atol((char*)ptr->open_in_tab)==-1)
                                                menu_desc=g_strjoin(NULL,ptr->menu_name," (Current Tab)",NULL);
                                        else
                                                menu_desc=g_strjoin(NULL,ptr->menu_name," (Tab ",ptr->open_in_tab,")",NULL);
                                        item = gtk_menu_item_new_with_label (menu_desc);
                                        menu_context = gtk_widget_get_style_context (item);
                                        gtk_style_context_add_class (menu_context, "titlebar");
                                        g_free(menu_desc);
                                }

                                if (ptr->label==TRUE) gtk_widget_set_sensitive ((GtkWidget *)item,FALSE);
                                else 
                                {
                                        void (*guake_funct)(GtkAction*,gpointer)=guake_open;
                                        g_signal_connect (item, "activate",G_CALLBACK (guake_funct), (gpointer) ptr);
                                }

                                gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

                                gtk_widget_show (item);
                        }

                }
        }

        /*GtkWidget *item = gtk_check_menu_item_new_with_label ("1");
        g_signal_connect (item, "activate",
                          G_CALLBACK (item_clicked_cb), "1");
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
        gtk_widget_show (item);

        item = gtk_radio_menu_item_new_with_label (NULL, "2");
        g_signal_connect (item, "activate",
                          G_CALLBACK (item_clicked_cb), "2");
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
        gtk_widget_show (item);

        item = gtk_menu_item_new_with_label ("3");
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
        append_submenu (item);
        gtk_widget_show (item);

        GtkWidget *toggle_item = gtk_menu_item_new_with_label ("Toggle 3");
        g_signal_connect (toggle_item, "activate",
                          G_CALLBACK (toggle_sensitivity_cb), item);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), toggle_item);
        gtk_widget_show(toggle_item);

        item = gtk_image_menu_item_new_from_stock (GTK_STOCK_NEW, NULL);
        g_signal_connect (item, "activate",
                          G_CALLBACK (image_clicked_cb), NULL);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
        gtk_widget_show(item);

        item = gtk_menu_item_new_with_label ("Get Attention");
        g_signal_connect (item, "activate",
                          G_CALLBACK (activate_clicked_cb), ci);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
        gtk_widget_show(item);
        app_indicator_set_secondary_activate_target(ci, item);

        item = gtk_menu_item_new_with_label ("Show label");
        label_toggle_cb(item, ci);
        g_signal_connect (item, "activate",
                          G_CALLBACK (label_toggle_cb), ci);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
        gtk_widget_show(item);

        item = gtk_check_menu_item_new_with_label ("Set Local Icon");
        g_signal_connect (item, "activate",
                          G_CALLBACK (local_icon_toggle_cb), ci);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
        gtk_widget_show(item);*/

        // Build default actions
		item=gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
		gtk_widget_show (item);
		for (i=0;i<GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY_SIZE;i++)
		{
			item = gtk_menu_item_new_with_label (GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY_GTK3[i].label);
			menu_context = gtk_widget_get_style_context (item);
	                gtk_style_context_add_class (menu_context, "titlebar");
			if (GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY_GTK3[i].action_function) g_signal_connect (item, "activate",G_CALLBACK (GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY_GTK3[i].action_function), (gpointer) gtkinfo);
			gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
	        gtk_widget_show (item);
		}



        app_indicator_set_menu (ci, GTK_MENU (menu));

    mainloop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainloop);

    return ;
}

void apply_css()
{
	GtkCssProvider *provider;

	provider = gtk_css_provider_new ();
    GtkWidget *window;
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_css_provider_load_from_data (provider, CSS, -1, NULL);
    gtk_style_context_add_provider_for_screen (gtk_widget_get_screen (window),GTK_STYLE_PROVIDER (provider),GTK_STYLE_PROVIDER_PRIORITY_USER);
}

static void
label_toggle_cb (GtkWidget * widget, gpointer data)
{
    can_haz_label = !can_haz_label;

    if (can_haz_label) {
        gtk_menu_item_set_label(GTK_MENU_ITEM(widget), "Hide label");
    } else {
        gtk_menu_item_set_label(GTK_MENU_ITEM(widget), "Show label");
    }

    return;
}

static void
activate_clicked_cb (GtkWidget *widget, gpointer data)
{
    AppIndicator * ci = APP_INDICATOR(data);

    if (active) {
        app_indicator_set_status (ci, APP_INDICATOR_STATUS_ATTENTION);
        gtk_menu_item_set_label(GTK_MENU_ITEM(widget), "I'm okay now");
        active = FALSE;
    } else {
        app_indicator_set_status (ci, APP_INDICATOR_STATUS_ACTIVE);
        gtk_menu_item_set_label(GTK_MENU_ITEM(widget), "Get Attention");
        active = TRUE;
    }

}

static void
local_icon_toggle_cb (GtkWidget *widget, gpointer data)
{
    AppIndicator * ci = APP_INDICATOR(data);

    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
        app_indicator_set_icon_full(ci, "simple-client-test-icon.png", "Local Icon");
    } else {
        app_indicator_set_icon_full(ci, "indicator-messages", "System Icon");
    }

    return;
}

static void
item_clicked_cb (GtkWidget *widget, gpointer data)
{
  const gchar *text = (const gchar *)data;

  g_print ("%s clicked!\n", text);
}

static void
toggle_sensitivity_cb (GtkWidget *widget, gpointer data)
{
  GtkWidget *target = (GtkWidget *)data;

  gtk_widget_set_sensitive (target, !gtk_widget_is_sensitive (target));
}

static void
image_clicked_cb (GtkWidget *widget, gpointer data)
{
  /*gtk_image_set_from_stock (GTK_IMAGE (gtk_image_menu_item_get_image (
                            GTK_IMAGE_MENU_ITEM (widget))),
                            GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU);*/
}

static void
scroll_event_cb (AppIndicator * ci, gint delta, guint direction, gpointer data)
{
    g_print("Got scroll event! delta: %d, direction: %d\n", delta, direction);
}

static void
append_submenu2 (GtkWidget *item)
{
  GtkWidget *menu;
  GtkWidget *mi;
  GtkWidget *prev_mi;

  menu = gtk_menu_new ();

  mi = gtk_menu_item_new_with_label ("Sub 1");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
  g_signal_connect (mi, "activate",
                    G_CALLBACK (item_clicked_cb), "Sub 1");

  prev_mi = mi;
  mi = gtk_menu_item_new_with_label ("Sub 2");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
  g_signal_connect (mi, "activate",
                    G_CALLBACK (toggle_sensitivity_cb), prev_mi);

  mi = gtk_menu_item_new_with_label ("Sub 3");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
  g_signal_connect (mi, "activate",
                    G_CALLBACK (item_clicked_cb), "Sub 3");

  gtk_widget_show_all (menu);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), menu);
}

static void append_submenu (GtkWidget *menu,Host* ptr)
{
  GtkWidget *mi;
  mi = gtk_menu_item_new_with_label (ptr->menu_name);
  if (ptr->label==TRUE) gtk_widget_set_sensitive ((GtkWidget *)mi,FALSE);
  else 
  {
    void (*guake_funct)(GtkAction*,gpointer)=guake_open;
    g_signal_connect (mi, "activate",G_CALLBACK (guake_funct), (gpointer) ptr);
  }
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
  g_signal_connect (mi, "activate",G_CALLBACK (item_clicked_cb), "Sub a");
}


guint percentage = 0;

static gboolean
percent_change (gpointer user_data)
{
    percentage = (percentage + 1) % 100;
    if (can_haz_label) {
        gchar * percentstr = g_strdup_printf("%d%%", percentage + 1);
        app_indicator_set_label (APP_INDICATOR(user_data), percentstr, "100%");
        g_free(percentstr);
    } else {
        app_indicator_set_label (APP_INDICATOR(user_data), NULL, NULL);
    }
    return TRUE;
}

// Function to close the indicator
static void close_guake_gtk ( GtkAction *action, gpointer user_data)
{
        GArray* grouphostlist= ((GtkInfo*)user_data)->grouphostlist;
        grouphostlist_free(grouphostlist);
        g_main_loop_quit(mainloop);
        return ;
}
