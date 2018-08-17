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

#include <string.h>
#include <gtk/gtk.h>
#include "guake-indicator.h"
#include "guake-indicator-read-json.h"
#include "guake-indicator-write-json.h"
#include "guake-indicator-edit-menu.h"
#include "guake-indicator-xml.h"
#include "guake-indicator-dbus.h"
#include "guake-indicator-notify.h"


int GUAKE3;

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
void guake_open(GtkAction* action,gpointer user_data)
{
	Host host = *((Host*) user_data);
	gchar* cmd = NULL;
	gint32 numtabs;
	gchar* uuid = NULL;
	
	// open a new Guake tab
	if (guake_gettabcount(&numtabs)==FALSE)
		guake_newtab(NULL);
	// if current guake tab is selected i skip this part
	else if (host.open_in_tab && atol((char*)host.open_in_tab)==-1)
	{
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
				GSettings* editor_settings = g_settings_new (GUAKE_INDICATOR_DCONF_SCHEMA_ROOT);
				gchar* str =NULL;
				if ( !strcmp(envstring,"param0")||!strcmp(envstring,"param1")||!strcmp(envstring,"param2")||!strcmp(envstring,"param3")||!strcmp(envstring,"param4")||!strcmp(envstring,"param5")||!strcmp(envstring,"param6")||!strcmp(envstring,"param7")||!strcmp(envstring,"param8")||!strcmp(envstring,"param9") )
					str = g_settings_get_string (editor_settings, envstring);
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
//void reload(GtkAction* action,gpointer user_data)
void reload(GtkInfo* gtkinfo)
{
	if (gtkinfo->grouphostlist) grouphostlist_free(gtkinfo->grouphostlist);
	if (check_xml_cfg_file_presence())
		gtkinfo->grouphostlist = read_xml_cfg_file();
	else
		gtkinfo->grouphostlist = read_json_cfg_file(NULL);
	if (gtkinfo->grouphostlist==NULL)
	{
		error_modal_box("Couldn't retrieve host from your guake indicator configuration file");
		return ;
	}


	
	//guake_notify("Guake indicator","Reload completed");
	return ;
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
	
}

// Add a host to an action group
gchar* add_host_to_menu(Host* head,GtkActionGroup *action_group)
{
	
}

// Create actionslist according to the result of a json file
gchar* create_actionlists(GArray* grouphostlist,GtkUIManager* uim,GtkActionGroup* action_group)
{
	
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
        GArray* grouphostlist;
        GtkInfo gtkinfo;

        guake_notify("Guake indicator","Guake indicator is running");

        if (argc>1 && strlen(argv[1])>0)
                grouphostlist=read_xml_cfg_file_from_file(argv[1]);
        else if (check_xml_cfg_file_presence())
                grouphostlist = read_xml_cfg_file();
        else
                grouphostlist = read_json_cfg_file(NULL);

        if (grouphostlist==NULL)
                error_modal_box("Couldn't retrieve host from your guake indicator configuration file");

        gtkinfo.grouphostlist=grouphostlist;

        build_menu_ayatana(argc,argv,&gtkinfo);
        return 0;
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
