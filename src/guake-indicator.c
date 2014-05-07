/*
Copyright (C) 2013-2014 Alessio Garzi <gun101@email.it>

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


#include <string.h>
#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include "guake-indicator.h"
#include "guake-indicator-read-json.h"


static const gchar* ui_start = "<ui>";
static const gchar* ui_end = "</ui>";
static const gchar* popup_start = "<popup name='IndicatorPopup'>";
static const gchar* popup_end = "</popup>";
static const gchar* separator = "<separator/>";
static const gchar* default_menuitems = "<menuitem action='Reload' />"
                                        "<menuitem action='Quit' />"
                                        "<menuitem action='About' />";


AppIndicator *indicator;
guint merge_id=0;

// Open a group of tabs
static void group_guake_open(GtkAction* action,gpointer user_data)
{
	Host host = *((Host*) user_data);
	Host* ptr = host.group_head;
	void (*guake_funct)(GtkAction*,gpointer);
	while (ptr)
	{
		// If it's the last host or the host after is the  "open all"
		// i call guake_open_with_show() for showing the terminal
		// otheerwise i call guake_open() that doesn't show the terminal
		if (ptr->next==NULL || ptr->next->group_head!=NULL)
			guake_funct=guake_open_with_show;
		else
			guake_funct=guake_open;
		if (ptr->group_head==NULL && ptr->label==FALSE)
			guake_funct(action,(gpointer)ptr);
		ptr=ptr->next;
	}
}

static void guake_open_with_show(GtkAction* action,gpointer user_data)
{
	// open new terminal and execute command
	guake_open(action,user_data);
	
	// try to show Guake
	guake_show();
}

// Open a new terminal tab on guake
static void guake_open(GtkAction* action,gpointer user_data)
{
	Host host = *((Host*) user_data);
	gchar* cmd = NULL;
	
	// set x_forwarded flag
	gchar* x_forwarded_flag;
	if (host.x_forwarded==NULL || g_strcmp0(host.x_forwarded,"yes"))
		x_forwarded_flag=g_strdup("");
	else
		x_forwarded_flag=g_strdup(" -X ");
		
	// open a new Guake tab
	guake_newtab();
		
	// perform a rename
	guake_renamecurrenttab(host.tab_name);
		
	// execute a command
	if (!strlen((char*)host.hostname))
		cmd = g_strjoin(NULL,host.command_after_login!=NULL?host.command_after_login:"",NULL);
	else if (host.command_after_login==NULL)
		cmd = g_strjoin(NULL,host.protocol," -l ",host.login," ",host.hostname,NULL);
	else
	{
		if (host.remote_command==NULL || g_strcmp0(host.remote_command,"yes"))
			cmd = g_strjoin(NULL,host.protocol,x_forwarded_flag," -t -l ",host.login," ",host.hostname," '",host.command_after_login,";/bin/bash'",NULL);
		else
			cmd = g_strjoin(NULL,host.protocol,x_forwarded_flag," -t -l ",host.login," ",host.hostname," ",host.command_after_login,NULL);
	}
	//printf("%s",cmd);fflush(stdout);
	guake_executecommand(cmd);
	
	g_free(x_forwarded_flag);
	g_free(cmd);
}

// Reload hosts reading them from the configuration file
static void reload(GtkAction* action,gpointer user_data)
{
	GtkActionGroup* action_group= ((GtkInfo*)user_data)->action_group;
	GtkActionGroup* new_action_group;
	GtkUIManager * uim = ((GtkInfo*)user_data)->uim;
	GError *error = NULL;
	GtkWidget *indicator_menu;
	GArray* grouphostlist =read_json_cfg_file(NULL);
	GList *actions, *iter;
	gint i=0;
		
	// Remove old action group and ui
	gtk_ui_manager_remove_action_group(uim,action_group);
	actions = gtk_action_group_list_actions (action_group);
	for (iter = actions; iter; iter = iter->next)
	{
		GtkAction *action = iter->data;
		gtk_action_group_remove_action  (action_group,action);
	}
	g_list_free (actions);
	
	gtk_ui_manager_remove_ui(uim,merge_id);
		
	// I create a new actionlist for each grouphostlist
	gchar* menuitems=create_actionlists(grouphostlist,uim,action_group);
	create_default_actions(action_group,(GtkInfo*)user_data);
		
	gtk_ui_manager_insert_action_group (uim, action_group, 0);
		
	gchar* ui_full_info=g_strjoin (NULL,
	ui_start,
	popup_start,
	menuitems,
	separator,
	default_menuitems,
	popup_end,
	ui_end,
	NULL
	);
		
	merge_id=gtk_ui_manager_add_ui_from_string (uim, ui_full_info, -1, &error);
	if (!merge_id)
	{
		g_message ("Failed to build menus: %s\n", error->message);
		g_error_free (error);
		error = NULL;
	}
        
	indicator_menu = gtk_ui_manager_get_widget (uim, "/ui/IndicatorPopup");
	app_indicator_set_menu (indicator, GTK_MENU (indicator_menu));
	gtk_ui_manager_ensure_update(uim);
	guake_notify("Guake indicator","Reload completed");
	return ;
}

// About page
static void about(GtkAction* action)
{
	GtkWidget *dialog;
	GdkPixbuf *logo;
	GError *error = NULL;
		
	gchar* license ="guake-indicator is free software; you can redistribute it and/or\n"
					" modify it under the terms of the GNU General Public License\n"
					" as published by the Free Software Foundation; either version 2\n"
					" of the License, or (at your option) any later version.\n\n"

					"This program is distributed in the hope that it will be useful,\n"
					"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
					" MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
					" GNU General Public License for more details.\n\n"

					"You should have received a copy of the GNU General Public License\n"
					"along with this program; if not, write to the Free Software\n"
					"Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.";
	
	const gchar *authors[] = {
		"Alessio Garzi",
		NULL
	};
		
	const gchar *documenters[] = {
		"Alessio Garzi",
		NULL
	};
	
	
	dialog = gtk_about_dialog_new ();
	logo = gdk_pixbuf_new_from_file ("/usr/share/icons/guake-indicator.png", &error);
	if (error == NULL)
		gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), logo);
	else
	{
		if (error->domain == GDK_PIXBUF_ERROR)
			g_print ("GdkPixbufError: %s\n", error->message);
		else if (error->domain == G_FILE_ERROR)
			g_print ("GFileError: %s\n", error->message);
		else
			g_print ("An error in the domain: %d has occurred!\n", error->domain);
		g_error_free (error);
	}
		
	gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (dialog), "guake-indicator");
	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), "0.2");
	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog),"(C) 2013-2014 Alessio Garzi");
	gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog),"A simple guake indicator that lets you ssh into your favourite hosts");
		
	gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), license);
	gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog),"http://guake-indicator.ozzyboshi.com");
	gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG (dialog),"http://guake-indicator.ozzyboshi.com");
		
	gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
	gtk_about_dialog_set_documenters (GTK_ABOUT_DIALOG (dialog), documenters);
	gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (dialog),"Alessio Garzi");
		
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	return ;
}

/*static void
activate_action (GtkAction* action)
{
        const gchar *name = gtk_action_get_name (action);
        GtkWidget *dialog;

        dialog = gtk_message_dialog_new (NULL,
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_INFO,
                                         GTK_BUTTONS_CLOSE,
                                         "You activated action: \"%s\"",
                                         name);

        g_signal_connect (dialog, "response",
                          G_CALLBACK (gtk_widget_destroy), NULL);

        gtk_widget_show (dialog);
}*/

static void
error_modal_box (const char* alerttext)
{
        GtkWidget *dialog;

        dialog = gtk_message_dialog_new (NULL,
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_INFO,
                                         GTK_BUTTONS_CLOSE,
                                         "%s",
                                         alerttext);

        g_signal_connect (dialog, "response",
                          G_CALLBACK (gtk_widget_destroy), NULL);

        gtk_widget_show (dialog);
}

// Add a lable to an action group
gchar* add_lable_to_menu(HostGroup* hostgroup,GtkActionGroup *action_group)
{
	gchar* title=hostgroup->title;
	gchar* id=hostgroup->id;
	gchar* ui_tmp;
	gchar* ui_full_info=g_strjoin(NULL,"",NULL);
	GtkAction* action = gtk_action_new(id, title, NULL, NULL);
	gtk_action_set_sensitive(action,FALSE);
	gtk_action_group_add_action(action_group, action);
	ui_tmp=ui_full_info;
	ui_full_info = g_strconcat(ui_tmp,"<separator/><menuitem action='",id,"' />",NULL);
	g_free(ui_tmp);
	return ui_full_info;
}

// Add a host to an action group
gchar* add_host_to_menu(Host* head,GtkActionGroup *action_group)
{
	GtkAction* action;
	Host* ptr = head;
	gchar* ui_tmp;
	gchar* ui_full_info=g_strjoin(NULL,"",NULL);
	
	for (ptr=head;ptr;ptr=ptr->next)
	{
		// if the open_all row is clicked
		if (ptr->group_head!=NULL)
		{
			action = gtk_action_new(ptr->id, ptr->menu_name, NULL, NULL);
			g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(group_guake_open), (gpointer)ptr);
		}
		else if (ptr->label==TRUE)
		{
			action = gtk_action_new(ptr->id, ptr->menu_name, NULL, NULL);
			gtk_action_set_sensitive(action,FALSE);
		}
		else
		{
			action = gtk_action_new(ptr->id, ptr->menu_name, NULL, NULL);
			g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(guake_open_with_show), (gpointer)ptr);	
		}
			
		gtk_action_group_add_action(action_group, action);
		ui_tmp=ui_full_info;
		
		// for the open all row and labels I set a separator before printing it
		if (ptr->group_head!=NULL || ptr->label==TRUE)
			ui_full_info = g_strconcat(ui_tmp,"<separator/><menuitem action='",ptr->id,"' />",NULL);
		else
			ui_full_info = g_strconcat(ui_tmp,"<menuitem action='",ptr->id,"' />",NULL);
		g_free(ui_tmp);
	}
	return ui_full_info;
}

// Create actionslist according to the result of a json file
gchar* create_actionlists(GArray* grouphostlist,GtkUIManager* uim,GtkActionGroup* action_group)
{
	gint i=0;
	gchar* menuitems=g_strdup("");
	for (i=0;grouphostlist!=NULL && i<grouphostlist->len;i++)
	{
		HostGroup* hostgroup = g_array_index (grouphostlist, HostGroup* , i);
		if (hostgroup->title==NULL || !strlen((char*)hostgroup->title))
		{
			gchar* p = menuitems;
			menuitems=g_strconcat(p,add_host_to_menu(hostgroup->hostarray,action_group),NULL);
			g_free(p);
			continue;
		}
		gchar* gtk_name=g_strdup_printf("%d",i);
		
		// Create a new action group
		GtkActionGroup* host_action_group = gtk_action_group_new (hostgroup->title);
		gtk_action_group_add_action(action_group, gtk_action_new(gtk_name, hostgroup->title, NULL, NULL)); 
		gchar* p = menuitems;
		
		// Case of a label
		if (hostgroup->label==TRUE)
			menuitems=g_strconcat(p,add_lable_to_menu(hostgroup,host_action_group),NULL);
		//Case of a Guake link
		else
			menuitems=g_strconcat(p,"<menu action='",gtk_name,"'>",add_host_to_menu(hostgroup->hostarray,host_action_group),"</menu>",NULL);
		
		g_free(p);
		g_free(gtk_name);
		    
		gtk_ui_manager_insert_action_group (uim, host_action_group , 0);
	}
	return menuitems;
}

int main (int argc, char **argv)
{
	GtkWidget *window;
	GtkWidget *sw;
	GtkWidget *indicator_menu;
	GtkActionGroup *action_group;
	GtkUIManager *uim;
	GError *error = NULL;
	
	if (!findguakepid()) 
		if (system("guake &")==-1)
			return -1;
			
	gtk_init (&argc, &argv);
		
	GArray* grouphostlist = read_json_cfg_file(NULL);
	if (grouphostlist==NULL)
		error_modal_box("Couldn't retrieve host from your guake indicator json file (is it a valid json file?)");
	
	// Create action group for refresh quit and about menuitems
	action_group = gtk_action_group_new ("AppActions");
		
	uim = gtk_ui_manager_new ();
	
	// I create a new actionlist for each grouphostlist
	gchar* menuitems=create_actionlists(grouphostlist,uim,action_group);
		
	// Add reload_action
	GtkInfo gtkinfo;
	gtkinfo.action_group = action_group;
	gtkinfo.uim = uim;
		
	// Create default actions
	create_default_actions(action_group,&gtkinfo);
		
	gtk_ui_manager_insert_action_group (uim, action_group, 0);
		
	gchar* ui_full_info=g_strjoin (NULL,
	ui_start,
	popup_start,
	menuitems,
	separator,
	default_menuitems,
	popup_end,
	ui_end,
	NULL
	);
				
	merge_id=gtk_ui_manager_add_ui_from_string (uim, ui_full_info, -1, &error);
	if (!merge_id)
	{
		g_message ("Failed to build menus: %s\n", error->message);
		g_error_free (error);
		error = NULL;
	}
	g_free(ui_full_info);
	g_free(menuitems);
		
	/* Indicator */
	indicator = app_indicator_new ("guake-indicator",
	"guake-indicator",
	APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
	
	indicator_menu = gtk_ui_manager_get_widget (uim, "/ui/IndicatorPopup");
		
	app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ACTIVE);
		
	app_indicator_set_menu (indicator, GTK_MENU (indicator_menu));
		
	guake_notify("Guake indicator","Guake indicator is running");
		
	gtk_main ();
		
	return 0;
}

void create_default_actions(GtkActionGroup* action_group,GtkInfo* gtkinfo)
{

	// Add reload_action
	GtkAction* reload_action = gtk_action_new("Reload", "Reload", NULL, NULL);
	g_signal_connect(G_OBJECT(reload_action), "activate", G_CALLBACK(reload), (gpointer) gtkinfo);
	gtk_action_group_add_action(action_group, reload_action);
		
	// Add quit_action
	GtkAction* quit_action = gtk_action_new("Quit", "Quit", NULL, NULL);
	g_signal_connect(G_OBJECT(quit_action), "activate", G_CALLBACK(gtk_main_quit), NULL);
	gtk_action_group_add_action(action_group, quit_action);
		
	// Add about_action
	GtkAction* about_action = gtk_action_new("About", "About", NULL, NULL);
	g_signal_connect(G_OBJECT(about_action), "activate", G_CALLBACK(about), NULL);
	gtk_action_group_add_action(action_group, about_action);
}

int findguakepid()
{
	const char* directory = "/proc";
	size_t      taskNameSize = 1024;
	char*       taskName = calloc(1, taskNameSize);
	const char* guakestr ="guake";

	DIR* dir = opendir(directory);

	if (dir)
	{
		struct dirent* de = 0;
		while ((de = readdir(dir)) != 0)
		{
			if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

			int pid = -1;
			int res = sscanf(de->d_name, "%d", &pid);

			if (res == 1)
			{

				char cmdline_file[1024] = {0};
				sprintf(cmdline_file, "%s/%d/comm", directory, pid);

				FILE* cmdline = fopen(cmdline_file, "r");

				if (getline(&taskName, &taskNameSize, cmdline) > 0)
				{
					taskName[strlen((char*)taskName)-1]=0;
					if (!strcmp(taskName, guakestr) != 0)
					{
						fclose(cmdline);
						closedir(dir);
						return pid;
					}
				}

				fclose(cmdline);
			}
		}

		closedir(dir);
   }
   return 0;
}
