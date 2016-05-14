/*
Copyright (C) 2013-2015 Alessio Garzi <gun101@email.it>
Copyright (C) 2013-2015 Francesco Minà <mina.francesco@gmail.com>

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
#include <gconf/gconf-client.h>
#include "guake-indicator.h"
#include "guake-indicator-read-json.h"
#include "guake-indicator-write-json.h"
#include "guake-indicator-edit-menu.h"
#include "guake-indicator-xml.h"

static const gchar* ui_start = "<ui>";
static const gchar* ui_end = "</ui>";
static const gchar* popup_start = "<popup name='IndicatorPopup'>";
static const gchar* popup_end = "</popup>";
static const gchar* separator = "<separator/>";
static const gchar* default_menuitems = "<separator/>"
										"<menuitem action='Edit Menu'/>"
										"<menuitem action='Reload' />"
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
		// otherwise i call guake_open() that doesn't show the terminal
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
	gint32 numtabs;
	gchar* uuid = NULL;
	
	// open a new Guake tab
	if (guake_gettabcount(&numtabs)==FALSE)
		guake_newtab();
	// if current guake tab is selected i skip this part
	else if (host.open_in_tab && atol((char*)host.open_in_tab)==-1)
	{
		printf("eseguo nella current tab");
		if (guake_getcurrenttab_uuid(&uuid)==FALSE) uuid=NULL;
	}
	else if (host.open_in_tab==NULL || !strlen(host.open_in_tab) || ((long)numtabs<=atol((char*)host.open_in_tab) && host.open_in_tab_named==FALSE))
	{
		if (guake_newtab(&uuid)==FALSE) uuid=NULL;
	}
	else
	{
		if (host.open_in_tab_named==FALSE)
			guake_selecttab(host.open_in_tab);
		// Search a tab by name
		else
		{
			gint32 i =0;
			gchar* name;
			for (i=0;i<numtabs;i++)
			{
				gboolean dbus_gtktabname = guake_getgtktabname(i,&name);
				if (dbus_gtktabname && !strcmp(name,host.open_in_tab))
				{
					gchar *istr = g_strdup_printf("%i", i);
					guake_selecttab(istr);
					g_free(istr);
					g_free(name);
					break;
				}
				if (dbus_gtktabname)
					g_free(name);
			}
			
			// tab not found
			if (i==numtabs)
			{
				gchar* tabnotfoundmsg = g_strjoin(NULL,"Tab named '",host.open_in_tab,"' not found",NULL);
				error_modal_box(tabnotfoundmsg);
				g_free(tabnotfoundmsg);
				return ;
				//guake_newtab();
			}
		}
	}
	
	// set x_forwarded flag
	gchar* x_forwarded_flag;
	if (host.x_forwarded==NULL || g_strcmp0(host.x_forwarded,"yes"))
		x_forwarded_flag=g_strdup("");
	else
		x_forwarded_flag=g_strdup(" -X ");
		
	// perform a rename
	if (host.tab_name && strlen((char*) host.tab_name)) guake_renamecurrenttab(host.tab_name);
		
	// execute a command
	if (host.hostname==NULL || !strlen((char*)host.hostname))
		cmd = g_strjoin(NULL,host.command_after_login!=NULL?host.command_after_login:"",NULL);
	else if (host.command_after_login==NULL || !strlen((char*)host.command_after_login))
		cmd = g_strjoin(NULL,"ssh"," -l ",host.login," ",host.hostname,NULL);
	else
	{
		if (host.remote_command==NULL || g_strcmp0(host.remote_command,"yes"))
			cmd = g_strjoin(NULL,"ssh",x_forwarded_flag," -t -l ",host.login," ",host.hostname," '",host.command_after_login,";/bin/bash'",NULL);
		else
			cmd = g_strjoin(NULL,"ssh",x_forwarded_flag," -t -l ",host.login," ",host.hostname," ",host.command_after_login,NULL);
	}
	//guake_executecommand(cmd);
	GString * newstring =  g_string_new (NULL);
	size_t i=0;
	for (i=0;i<strlen((char*)cmd);i++)
	{
		//Manage the <#tag
		if (host.guakeindicatorscript && !strcmp(host.guakeindicatorscript,"yes") && cmd[i]=='<' && cmd[i+1]=='#')
		{
			i+=2;
			GString * systemstring =  g_string_new (NULL);
			while ((cmd[i])&&(cmd[i]!=10)&&(cmd[i]!=13))
			{
				g_string_append_c (systemstring,cmd[i]);
				i++;
			}
			if (system(systemstring->str)==-1)
			{
				guake_notify("Guake indicator","Cannot spawn system call");
			}
			g_string_free (systemstring,TRUE);
			continue;
		}
		
		//Manage the <! and !> tags
		else if (host.guakeindicatorscript && !strcmp(host.guakeindicatorscript,"yes") && cmd[i]=='<' && cmd[i+1]=='!')
		{
			i+=2;
			gchar* start=cmd+(i*sizeof(gchar));
			gchar* end = g_strstr_len(start,-1,"!>");
			if (end>start)
			{
				gchar* envstring = g_strndup(start,(end-start)*sizeof(gchar));
				GConfClient *client = gconf_client_get_default ();
				gchar* gschema = g_strjoin(NULL,GUAKE_INDICATOR_GCONF_SCHEMA_ROOT,envstring,NULL);
				char* str = gconf_client_get_string (client, gschema, NULL);
				g_free(gschema);
				if (str)
				{
					g_string_append (newstring,str);
					free(str);
				}
				g_free(envstring);
				i=i+((end-start)*sizeof(gchar))+1;
				continue;
			}
		}
		
		g_string_append_c (newstring,cmd[i]);
		if (cmd[i]==10)
		{
			// Add a cr to the end line will be lfcr
			if (host.lfcr && !g_strcmp0(host.lfcr,"yes")) g_string_append_c (newstring,13);
			if (uuid) guake_executecommand_by_uuid(uuid,newstring->str);
			else guake_executecommand(newstring->str);
			g_string_free (newstring,TRUE);
			newstring = g_string_new (NULL);
		}
	}
	if (newstring->len>0)
	{
		if (uuid) guake_executecommand_by_uuid(uuid,newstring->str);
		else guake_executecommand(newstring->str);
	}
	g_string_free (newstring,TRUE);
	if (uuid) g_free(uuid);
	
	g_free(x_forwarded_flag);
	g_free(cmd);
}

// Reload hosts reading them from the configuration file
void reload(GtkAction* action,gpointer user_data)
{
	GArray* oldgrouphostlist = ((GtkInfo*)user_data)->grouphostlist;
	GtkActionGroup* action_group = ((GtkInfo*)user_data)->action_group;
	GtkUIManager * uim = ((GtkInfo*)user_data)->uim;
	GError *error = NULL;
	GtkWidget *indicator_menu;
	GArray* grouphostlist =NULL;
	GList *actions, *iter;
	
	// Fetch data from the cfg file
    if (check_xml_cfg_file_presence())
		grouphostlist = read_xml_cfg_file();
	else
		grouphostlist = read_json_cfg_file(NULL);
		
	if (grouphostlist==NULL)
	{
		error_modal_box("Couldn't retrieve host from your guake indicator configuration file");
		return ;
	}
	
	// free the old grouphostlist and use the new one
	grouphostlist_free(oldgrouphostlist);
	((GtkInfo*)user_data)->grouphostlist=grouphostlist;
	
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
	g_free(menuitems);
		
	merge_id=gtk_ui_manager_add_ui_from_string (uim, ui_full_info, -1, &error);
	if (!merge_id)
	{
		g_message ("Failed to build menus: %s\n", error->message);
		g_error_free (error);
		g_free(ui_full_info);
		error = NULL;
	}
	g_free(ui_full_info);

	indicator_menu = gtk_ui_manager_get_widget (uim, "/ui/IndicatorPopup");
	app_indicator_set_menu (indicator, GTK_MENU (indicator_menu));
	gtk_ui_manager_ensure_update(uim);
	
	guake_notify("Guake indicator","Reload completed");
	return ;
}

// About page
static void about (GtkAction* action)
{
	GError *error = NULL;
	const gchar *authors[] = {
		"Alessio Garzi <gun101@email.it>",
		"Francesco Minà <mina.francesco@gmail.com>",
		NULL
	};
	const gchar* license ="guake-indicator is free software; you can redistribute it and/or\n"
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
	
	GdkPixbuf *logo = gdk_pixbuf_new_from_file (DATADIR"/"GUAKE_INDICATOR_ICON_DIR"/guake-indicator.png", &error);	
	if (error != NULL)
	{
		if (error->domain == GDK_PIXBUF_ERROR)
			g_print ("GdkPixbufError: %s\n", error->message);
		else if (error->domain == G_FILE_ERROR)
			g_print ("GFileError: %s\n", error->message);
		else
			g_print ("An error in the domain: %d has occurred!\n", error->domain);
		g_error_free (error);
	}

	gtk_show_about_dialog(NULL,
							"program-name", "guake-indicator",
							"authors", authors,
							"comments", "A simple indicator that lets you send custom commands to Guake.",
							"copyright", "(C) 2013-2015 Alessio Garzi\n(C) 2013-2015 Francesco Mina\n\nDedicated to my daughters\n Ludovica and newborn Mariavittoria",
							"logo", logo,
							"version", "1.1", 
							"website", "http://guake-indicator.ozzyboshi.com",
							"license",license,
							NULL);
}

void error_modal_box (const char* alerttext)
{
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new (NULL,
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_INFO,
									GTK_BUTTONS_CLOSE,
									"%s",
									alerttext);

	g_signal_connect (dialog, "response",G_CALLBACK (gtk_widget_destroy), NULL);
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
		
		// Draw a inner label
		else if (ptr->label==TRUE)
		{
			action = gtk_action_new(ptr->id, ptr->menu_name, NULL, NULL);
			gtk_action_set_sensitive(action,FALSE);
		}
		
		// Regular row is clicked
		else
		{
			void (*funct_ptr)(GtkAction*,gpointer);
			if (ptr->dont_show_guake==NULL || g_strcmp0(ptr->dont_show_guake,"yes"))
				funct_ptr=guake_open_with_show;
			else
				funct_ptr=guake_open;

			if (ptr->open_in_tab==NULL)
				action = gtk_action_new(ptr->id, ptr->menu_name, NULL, NULL);
			else
			{
				gchar* menu_desc;
				if (atol((char*)ptr->open_in_tab)==-1)
					menu_desc=g_strjoin(NULL,ptr->menu_name," (Current Tab)",NULL);
				else
					menu_desc=g_strjoin(NULL,ptr->menu_name," (Tab ",ptr->open_in_tab,")",NULL);
				action = gtk_action_new(ptr->id, menu_desc, NULL, NULL);
				g_free(menu_desc);
			}
			g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(funct_ptr), (gpointer)ptr);
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
			gchar* xmlcode=add_host_to_menu(hostgroup->hostarray,action_group);
			menuitems=g_strconcat(p,xmlcode,NULL);
			g_free(xmlcode);
			g_free(p);
			continue;
		}
		gchar* gtk_name=g_strdup_printf("%d",i);
		
		// Create a new action group
		GtkActionGroup* host_action_group = gtk_action_group_new (hostgroup->title);
		GtkAction *newaction=gtk_action_new(gtk_name, hostgroup->title, NULL, NULL);
		gtk_action_group_add_action(action_group,newaction);
		g_object_unref(newaction);
		gchar* p = menuitems;
		
		// Case of a label
		if (hostgroup->label==TRUE)
		{
			gchar* xmlcode=add_lable_to_menu(hostgroup,host_action_group);
			menuitems=g_strconcat(p,xmlcode,NULL);
			g_free(xmlcode);
		}
		//Case of a Guake link
		else
		{
			gchar* xmlcode=add_host_to_menu(hostgroup->hostarray,host_action_group);
			menuitems=g_strconcat(p,"<menu action='",gtk_name,"'>",xmlcode,"</menu>",NULL);
			g_free(xmlcode);
		}
		
		g_free(p);
		g_free(gtk_name);
		gtk_ui_manager_insert_action_group (uim, host_action_group , 0);
	}
	return menuitems;
}

// Free a grouphostlist
void grouphostlist_free(GArray* grouphostlist)
{
	Host* ptr,*newptr;
	gint i;
	for (i=0;grouphostlist!=NULL && i<grouphostlist->len;i++)
	{
		HostGroup* hostgroup = g_array_index (grouphostlist, HostGroup* , i);
		for (ptr=hostgroup->hostarray;ptr;ptr=newptr)
		{
			host_free(ptr);
			newptr=ptr->next;
			free(ptr);
		}
		if (hostgroup->id) free(hostgroup->id);
		if (hostgroup->title) free(hostgroup->title);
		free(hostgroup);
	}
	g_array_free(grouphostlist,TRUE);
}

// Free a host structure
void host_free(Host* ptr)
{
	if (ptr->menu_name) free(ptr->menu_name);
	if (ptr->tab_name) free(ptr->tab_name);
	if (ptr->command_after_login) free(ptr->command_after_login);
	if (ptr->dont_show_guake) free(ptr->dont_show_guake);
	if (ptr->lfcr) free(ptr->lfcr);
	if (ptr->guakeindicatorscript) free(ptr->guakeindicatorscript);
	if (ptr->id) free(ptr->id);
	if (ptr->open_in_tab) free(ptr->open_in_tab);
}

// Free a hostgroup structure
void hostgroup_free(HostGroup* ptr)
{
	if (ptr->id) free(ptr->id);
	if (ptr->title) free(ptr->title);
}

// close the indicator
static void close_guake ( GtkWidget *widget, gpointer user_data)
{	
	GArray* grouphostlist= ((GtkInfo*)user_data)->grouphostlist;
	grouphostlist_free(grouphostlist);
	gtk_main_quit();
	return ;
}

int main (int argc, char **argv)
{
	GtkWidget *indicator_menu;
	GtkActionGroup *action_group;
	GtkUIManager *uim;
	GError *error = NULL;
	
	if (!findguakepid()) 
		if (system("guake &")==-1)
			return -1;
			
	gtk_init (&argc, &argv);
		
	GArray* grouphostlist;
	if (check_xml_cfg_file_presence())
		grouphostlist = read_xml_cfg_file();
	else
		grouphostlist = read_json_cfg_file(NULL);
		
	if (grouphostlist==NULL)
		error_modal_box("Couldn't retrieve host from your guake indicator configuration file");
	
	// Create action group for refresh quit and about menuitems
	action_group = gtk_action_group_new ("AppActions");
		
	uim = gtk_ui_manager_new ();
	
	// I create a new actionlist for each grouphostlist
	gchar* menuitems=create_actionlists(grouphostlist,uim,action_group);
		
	// Add reload_action
	GtkInfo gtkinfo;
	gtkinfo.action_group = action_group;
	gtkinfo.uim = uim;
	gtkinfo.grouphostlist=grouphostlist;
		
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
	// Add Edit menu
	GtkAction* edit_menu_action = gtk_action_new("Edit Menu", "Edit Menu", NULL, NULL);
	g_signal_connect(G_OBJECT(edit_menu_action), "activate", G_CALLBACK(print_edit_menu_form), (gpointer) gtkinfo);
	gtk_action_group_add_action(action_group, edit_menu_action);

	// Add reload_action
	GtkAction* reload_action = gtk_action_new("Reload", "Reload", NULL, NULL);
	g_signal_connect(G_OBJECT(reload_action), "activate", G_CALLBACK(reload), (gpointer) gtkinfo);
	gtk_action_group_add_action(action_group, reload_action);
		
	// Add quit_action
	GtkAction* quit_action = gtk_action_new("Quit", "Quit", NULL, NULL);
	g_signal_connect(G_OBJECT(quit_action), "activate", G_CALLBACK(close_guake), (gpointer) gtkinfo);
	gtk_action_group_add_action(action_group, quit_action);
		
	// Add about_action
	GtkAction* about_action = gtk_action_new("About", "About", NULL, NULL);
	g_signal_connect(G_OBJECT(about_action), "activate", G_CALLBACK(about), NULL);
	gtk_action_group_add_action(action_group, about_action);
}

int findguakepid()
{
	const char* directory = "/proc";
	size_t taskNameSize = 1024;
	char* taskName = calloc(1, taskNameSize);
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
				if (cmdline==NULL) continue;

				if (getline(&taskName, &taskNameSize, cmdline) > 0)
				{
					taskName[strlen((char*)taskName)-1]=0;
					if (!strcmp(taskName, guakestr) != 0)
					{
						fclose(cmdline);
						closedir(dir);
						free(taskName);
						return pid;
					}
				}

				fclose(cmdline);
			}
		}
		closedir(dir);
	}
	free(taskName);
	return 0;
}
