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
#include "guake-indicator.h"
#include "guake-indicator-new-entry.h"
#include "guake-indicator-read-json.h"
#include "guake-indicator-write-json.h"

static Host2GtkTree* SELECTED_HOST = NULL;

void print_new_entry_form(GtkAction* action,gpointer user_data)
{
	GtkWidget *window, *vbox, *hbox, *hbox1, *question, *label,*labellogin, *hostname,*login;
	GtkWidget * tree_view;
	GtkCellRenderer *cell_renderer;
	GtkTreeViewColumn* column;
	const gint HBOX_SPACING = 5;
	
	SELECTED_HOST=NULL;
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "New entry");
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);

	//question = gtk_label_new ("New entry");
	label = gtk_label_new ("Hostname :");
	labellogin = gtk_label_new ("Login    :");
	
	/* Create a new GtkEntry widget and hide its content from view. */
	hostname = gtk_entry_new ();
	gtk_entry_set_text((GtkEntry*)hostname,"127.0.0.1");
	login = gtk_entry_new ();
	char* user=getenv("LOGNAME");
	if (user!=NULL)
		gtk_entry_set_text((GtkEntry*)login,user);

	hbox = gtk_hbox_new (TRUE, HBOX_SPACING);
	hbox1 = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox), label);
	gtk_box_pack_start_defaults (GTK_BOX (hbox), hostname);
	gtk_box_pack_start_defaults (GTK_BOX (hbox1), labellogin);
	gtk_box_pack_start_defaults (GTK_BOX (hbox1), login);

	// Start of menu_name creation
	GtkWidget* hbox_menu_name,*menu_name,*labelmenu_name;
	menu_name = gtk_entry_new ();
	labelmenu_name = gtk_label_new ("Menu name :");
	hbox_menu_name = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_menu_name), labelmenu_name);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_menu_name), menu_name);
	// End of menu_name creation

	// Start of tab_name creation
	GtkWidget* hbox_tab_name,*tab_name,*labeltab_name;
	tab_name = gtk_entry_new ();
	labeltab_name = gtk_label_new ("Tab name :");
	hbox_tab_name = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_tab_name), labeltab_name);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_tab_name), tab_name);
	// End of tab_name creation

	// Start of command_after_login creation
	GtkWidget* hbox_command_after_login,*command_after_login,*labelcommand_after_login;
	command_after_login = gtk_entry_new ();
	labelcommand_after_login = gtk_label_new ("Command after login :");
	hbox_command_after_login = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_command_after_login), labelcommand_after_login);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_command_after_login), command_after_login);
	// End of command_after_login creation

	// Start of x_forwarded creation
	GtkWidget* hbox_x_forwarded,*x_forwarded,*labelx_forwarded;
	x_forwarded = gtk_check_button_new_with_label ("Check to enable");
	labelx_forwarded = gtk_label_new ("X Forwarded :");
	hbox_x_forwarded = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_x_forwarded), labelx_forwarded);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_x_forwarded), x_forwarded);
	// End of x_forwarded creation

	// Start of dont_show_guake creation
	GtkWidget* hbox_dont_show_guake,*dont_show_guake,*labeldont_show_guake;
	dont_show_guake = gtk_check_button_new_with_label ("Check to enable");
	labeldont_show_guake = gtk_label_new ("Don't show Guake after selected :");
	hbox_dont_show_guake = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_dont_show_guake), labeldont_show_guake);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_dont_show_guake), dont_show_guake);
	// End of dont_show_guake creation

	// Start of remote_command
	GtkWidget* hbox_remote_command,*remote_command,*labelremote_command;
	remote_command = gtk_check_button_new_with_label ("Check to enable");
	labelremote_command = gtk_label_new ("Disconnect after command executed :");
	hbox_remote_command = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_remote_command), labelremote_command);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_remote_command), remote_command);
	// End of remote_command
	
	// Start of label
	GtkWidget* hbox_label_cmd,*label_cmd,*labellabel_cmd;
	label_cmd = gtk_check_button_new_with_label ("Check to enable");
	labellabel_cmd = gtk_label_new ("Label :");
	hbox_label_cmd = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_label_cmd), labellabel_cmd);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_label_cmd), label_cmd);
	g_signal_connect (G_OBJECT (label_cmd), "toggled",G_CALLBACK (label_check_toggled),(gpointer) window);
	// End of label
	
	// Start of treeview
	SELECTED_HOST=NULL;
	GtkTreeStore* tree_store = gtk_tree_store_new(1, G_TYPE_STRING);
	tree_view=gtk_tree_view_new ();
	
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes("Ref", cell_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), column);

	GtkTreeIter iter;
	GtkTreeIter iter2;
	GArray* host2gtktreelist= g_array_new (TRUE, FALSE, sizeof (Host2GtkTree*));
	
	// Fetch data from the cfg file
	GArray* grouphostlist=read_json_cfg_file(NULL);
	int j=0;
	for (j=0;grouphostlist!=NULL && j<grouphostlist->len;j++)
	{
		HostGroup* hostgroup = g_array_index (grouphostlist, HostGroup* , j);
		if (hostgroup->label==TRUE) continue;
	
		gtk_tree_store_append(tree_store, &iter, NULL);
		gtk_tree_store_set(tree_store, &iter,0,hostgroup->title!=NULL?hostgroup->title:"Root node",-1);
		
		Host2GtkTree* rel=(Host2GtkTree*)malloc(sizeof(Host2GtkTree));
		bzero(rel,sizeof(Host2GtkTree));
		rel->user_data=iter.user_data;
		rel->hostgroup=hostgroup;
		rel->action=action;
		rel->gtk_user_data=user_data;
		g_array_append_val (host2gtktreelist, rel);
		
		int i=0;
		Host* ptr=NULL;
		for (ptr=hostgroup->hostarray;ptr;ptr=ptr->next)
		{
			if (ptr->open_all==TRUE) continue;
			gtk_tree_store_append(tree_store, &iter2, &iter);
			gtk_tree_store_set(tree_store, &iter2,
						0, ptr->menu_name,-1);
			Host2GtkTree* rel=(Host2GtkTree*)malloc(sizeof(Host2GtkTree));
			bzero(rel,sizeof(Host2GtkTree));
			rel->user_data=iter2.user_data;
			rel->host=ptr;
			rel->action=action;
			rel->gtk_user_data=user_data;
			g_array_append_val (host2gtktreelist, rel);
		}
	}
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (tree_store));
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	g_signal_connect(selection, "changed", G_CALLBACK(host_changed), (gpointer)host2gtktreelist);
	
	GtkWidget * scrollbar = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar),
		GTK_POLICY_AUTOMATIC,
		GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrollbar), tree_view);


	GtkWidget* hbox_treeview,*labeltreeview;
	labeltreeview = gtk_label_new ("Select where to place new entry :");
	hbox_treeview = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_widget_set_size_request (scrollbar,400,250);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_treeview), labeltreeview);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_treeview), scrollbar);
	// End of treeview

	vbox = gtk_vbox_new (FALSE, 10);

	GtkWidget* saveButton = gtk_button_new_from_stock (GTK_STOCK_SAVE);
	g_signal_connect_swapped (G_OBJECT (saveButton), "clicked",G_CALLBACK (crunch_new_entry_form_data),(gpointer) window);
	GtkWidget* hbox_buttons = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_buttons), saveButton);

	// Add to vbox
	//gtk_box_pack_start_defaults (GTK_BOX (vbox), question);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox1);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_menu_name);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_tab_name);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_command_after_login);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_x_forwarded);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_dont_show_guake);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_remote_command);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_label_cmd);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_treeview);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_buttons);

	gtk_container_add (GTK_CONTAINER (window), vbox);
	gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_widget_show_all (window);
	
	return;
}

static void  host_changed(GtkWidget *widget,gpointer data) 
{
	GArray* host2gtktreelist = (GArray*)data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	char *value;

	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter))
	{
		gtk_tree_model_get(model, &iter, 0, &value,  -1);
	
		int i=0;
		for (i=0;host2gtktreelist!=NULL && i<host2gtktreelist->len;i++)
		{
			Host2GtkTree* ref = g_array_index (host2gtktreelist, Host2GtkTree* , i);
			if (ref->user_data==iter.user_data)
			{
				SELECTED_HOST=ref;
			}
		}
    
		g_free(value);
	}
}

static void crunch_new_entry_form_data(gpointer window_pointer)
{
	if (SELECTED_HOST==NULL)
	{
		error_modal_box("Select where to place the new entry");
		return ;
	}

	GtkWidget *window = (GtkWidget *)window_pointer;
	GtkWidget *vbox = gtk_bin_get_child(GTK_BIN(window));
	
	GList *vbox_children = gtk_container_get_children(GTK_CONTAINER(vbox));
	//vbox_children = g_list_next(vbox_children);
	
	// Start fetching hostname
	GtkWidget* hbox_hostname = (GtkWidget*) vbox_children->data;
	GList* hostname_reference = gtk_container_get_children(GTK_CONTAINER(hbox_hostname));
	hostname_reference = g_list_next(hostname_reference);
	GtkWidget* hostname_entry = (GtkWidget*) hostname_reference->data;
	const gchar* testohost=gtk_entry_get_text((GtkEntry *)hostname_entry);
	// End fetching hostname
 
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching login
	GtkWidget* hbox_login = (GtkWidget*) vbox_children->data;
	GList* login_reference = gtk_container_get_children(GTK_CONTAINER(hbox_login));
	login_reference = g_list_next(login_reference);
	GtkEntry* login_entry = (GtkEntry*) login_reference->data;
	const gchar* testologin=gtk_entry_get_text(login_entry);
	// End fetching login
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching menu_name
	GtkWidget* hbox_menuname = (GtkWidget*) vbox_children->data;
	GList* menuname_reference = gtk_container_get_children(GTK_CONTAINER(hbox_menuname));
	menuname_reference = g_list_next(menuname_reference);
	GtkEntry* menuname_entry = (GtkEntry*) menuname_reference->data;
	const gchar* testomenuname=gtk_entry_get_text(menuname_entry);
	// End fetching menu_name
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching tab_name
	GtkWidget* hbox_tabname = (GtkWidget*) vbox_children->data;
	GList* tabname_reference = gtk_container_get_children(GTK_CONTAINER(hbox_tabname));
	tabname_reference = g_list_next(tabname_reference);
	GtkEntry* tabname_entry = (GtkEntry*) tabname_reference->data;
	const gchar* testotabname=gtk_entry_get_text(tabname_entry);
	// End fetching tab_name
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching command_after_login
	GtkWidget* hbox_command_after_login = (GtkWidget*) vbox_children->data;
	GList* command_after_login_reference = gtk_container_get_children(GTK_CONTAINER(hbox_command_after_login));
	command_after_login_reference = g_list_next(command_after_login_reference);
	GtkEntry* command_after_login_entry = (GtkEntry*) command_after_login_reference->data;
	const gchar* testocommand_after_login=gtk_entry_get_text(command_after_login_entry);
	// End fetching command_after_login
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching x_forwarded
	GtkWidget* hbox_x_forwarded = (GtkWidget*) vbox_children->data;
	GList* x_forwarded_reference = gtk_container_get_children(GTK_CONTAINER(hbox_x_forwarded));
	x_forwarded_reference = g_list_next(x_forwarded_reference);
	GtkWidget* x_forwarded_entry = (GtkWidget*) x_forwarded_reference->data;
	const gchar* testox_forwarded;
	 if (GTK_TOGGLE_BUTTON (x_forwarded_entry)->active) 
		testox_forwarded=g_strdup("yes");
	else
		testox_forwarded=g_strdup("no");
	// End fetching x_forwarded
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching dont_show_guake
	GtkWidget* hbox_dont_show_guake = (GtkWidget*) vbox_children->data;
	GList* dont_show_guake_reference = gtk_container_get_children(GTK_CONTAINER(hbox_dont_show_guake));
	dont_show_guake_reference = g_list_next(dont_show_guake_reference);
	GtkWidget* dont_show_guake_entry = (GtkWidget*) dont_show_guake_reference->data;
	const gchar* testodont_show_guake;
	if (GTK_TOGGLE_BUTTON (dont_show_guake_entry)->active) 
		testodont_show_guake=g_strdup("yes");
	else
		testodont_show_guake=g_strdup("no");
	// End fetching dont_show_guake
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching remote_command
	GtkWidget* hbox_remote_command = (GtkWidget*) vbox_children->data;
	GList* remote_command_reference = gtk_container_get_children(GTK_CONTAINER(hbox_remote_command));
	remote_command_reference = g_list_next(remote_command_reference);
	GtkWidget* remote_command_entry = (GtkWidget*) remote_command_reference->data;
	const gchar* testoremote_command;
	 if (GTK_TOGGLE_BUTTON (remote_command_entry)->active) 
		testoremote_command=g_strdup("yes");
	else
		testoremote_command=g_strdup("no");
	// End fetching remote_command
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching label cmd
	GtkWidget* hbox_label_cmd = (GtkWidget*) vbox_children->data;
	GList* label_cmd_reference = gtk_container_get_children(GTK_CONTAINER(hbox_label_cmd));
	label_cmd_reference = g_list_next(label_cmd_reference);
	GtkWidget* label_cmd_entry = (GtkWidget*) label_cmd_reference->data;
	gboolean testolabel_cmd;
	if (GTK_TOGGLE_BUTTON (label_cmd_entry)->active)
		testolabel_cmd=TRUE;
	else
		testolabel_cmd=FALSE;
		
	// End fetching label cmd
	
	Host* newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	newhost->label=testolabel_cmd;
	newhost->hostname=g_strdup(testohost);
	newhost->login=g_strdup(testologin);
	newhost->menu_name=g_strdup(testomenuname);
	newhost->tab_name=g_strdup(testotabname);
	newhost->command_after_login=g_strdup(testocommand_after_login);
	newhost->x_forwarded=g_strdup(testox_forwarded);
	newhost->dont_show_guake=g_strdup(testodont_show_guake);
	newhost->remote_command=g_strdup(testoremote_command);
	
	// Fetch data from the cfg file
	GArray* grouphostlist=read_json_cfg_file(NULL);
	
	int j=0;
	for (j=0;grouphostlist!=NULL && j<grouphostlist->len;j++)
	{
		HostGroup* hostgroup = g_array_index (grouphostlist, HostGroup* , j);
		Host* ptr=NULL;
		
		// Insert at the first position
		if (SELECTED_HOST->hostgroup!=NULL)
		{
			if (!g_strcmp0(hostgroup->id,SELECTED_HOST->hostgroup->id))
			{
				newhost->next=hostgroup->hostarray;
				hostgroup->hostarray=newhost;
			}
		}
		
		// Insert in the middle or at the end
		else
		{
			for (ptr=hostgroup->hostarray;ptr;ptr=ptr->next)
			{
				if (!g_strcmp0(ptr->id,SELECTED_HOST->host->id))
				{
					if (ptr->next==NULL)
						host_queue(hostgroup->hostarray,newhost);
					else
					{
						newhost->next=ptr->next;
						ptr->next=newhost;
					}
				}
			}
		}
	}
	
	/*printf("%s\n",(char*)write_json_cfg_file_to_str(grouphostlist));*/
	
	char* filedir,*filecfg;
	filedir=checkandcreatedefaultdir();
	asprintf(&filecfg,"%s/%s",filedir,GUAKE_INDICATOR_DEFAULT_FILEJSON);
	free((void*)filedir);
	FILE * fd=fopen(filecfg,"w");
	if (fd==NULL)
	{
			error_modal_box ("Error saving cfg file");
			gtk_widget_destroy(window);
			free((void*)filecfg);
			return ;
	}
	char* data = (char*)write_json_cfg_file_to_str(grouphostlist);
	fwrite((const void*)data,strlen(data),1,fd);
	fclose(fd);
	free((void*)filecfg);
	
	gtk_widget_destroy(window);
	error_modal_box ("Host saved successfully");
	reload(SELECTED_HOST->action, SELECTED_HOST->gtk_user_data);
}

void label_check_toggled (GtkToggleButton *check,gpointer data)
{
	gboolean state = TRUE;
	if (gtk_toggle_button_get_active (check))
		state=FALSE;

	GtkWidget *window = (GtkWidget *)data;
	GtkWidget *vbox = gtk_bin_get_child(GTK_BIN(window));
	
	GList *vbox_children = gtk_container_get_children(GTK_CONTAINER(vbox));
	vbox_children = g_list_next(vbox_children);
		
	GtkWidget* hbox_hostname = (GtkWidget*) vbox_children->data;
	GList* hostname_reference = gtk_container_get_children(GTK_CONTAINER(hbox_hostname));
	hostname_reference = g_list_next(hostname_reference);
	GtkWidget* hostname_entry = (GtkWidget*) hostname_reference->data;
	gtk_widget_set_sensitive (hostname_entry,state);
		
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching login
	GtkWidget* hbox_login = (GtkWidget*) vbox_children->data;
	GList* login_reference = gtk_container_get_children(GTK_CONTAINER(hbox_login));
	login_reference = g_list_next(login_reference);
	GtkEntry* login_entry = (GtkEntry*) login_reference->data;
	gtk_widget_set_sensitive ((GtkWidget*)login_entry,state);
	// End fetching login
	
	vbox_children = g_list_next(vbox_children);
	
	//vbox_children = g_list_next(vbox_children);
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching tab_name
	GtkWidget* hbox_tabname = (GtkWidget*) vbox_children->data;
	GList* tabname_reference = gtk_container_get_children(GTK_CONTAINER(hbox_tabname));
	tabname_reference = g_list_next(tabname_reference);
	GtkEntry* tabname_entry = (GtkEntry*) tabname_reference->data;
	gtk_widget_set_sensitive ((GtkWidget*)tabname_entry,state);
	// End fetching tab_name
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching command_after_login
	GtkWidget* hbox_command_after_login = (GtkWidget*) vbox_children->data;
	GList* command_after_login_reference = gtk_container_get_children(GTK_CONTAINER(hbox_command_after_login));
	command_after_login_reference = g_list_next(command_after_login_reference);
	GtkEntry* command_after_login_entry = (GtkEntry*) command_after_login_reference->data;
	gtk_widget_set_sensitive ((GtkWidget*)command_after_login_entry,state);
	// End fetching command_after_login
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching x_forwarded
	GtkWidget* hbox_x_forwarded = (GtkWidget*) vbox_children->data;
	GList* x_forwarded_reference = gtk_container_get_children(GTK_CONTAINER(hbox_x_forwarded));
	x_forwarded_reference = g_list_next(x_forwarded_reference);
	GtkWidget* x_forwarded_entry = (GtkWidget*) x_forwarded_reference->data;
	gtk_widget_set_sensitive ((GtkWidget*)x_forwarded_entry,state);
	// End fetching x_forwarded
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching dont_show_guake
	GtkWidget* hbox_dont_show_guake = (GtkWidget*) vbox_children->data;
	GList* dont_show_guake_reference = gtk_container_get_children(GTK_CONTAINER(hbox_dont_show_guake));
	dont_show_guake_reference = g_list_next(dont_show_guake_reference);
	GtkWidget* dont_show_guake_entry = (GtkWidget*) dont_show_guake_reference->data;
	gtk_widget_set_sensitive ((GtkWidget*)dont_show_guake_entry,state);
	// End fetching dont_show_guake
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching remote_command
	GtkWidget* hbox_remote_command = (GtkWidget*) vbox_children->data;
	GList* remote_command_reference = gtk_container_get_children(GTK_CONTAINER(hbox_remote_command));
	remote_command_reference = g_list_next(remote_command_reference);
	GtkWidget* remote_command_entry = (GtkWidget*) remote_command_reference->data;
	gtk_widget_set_sensitive ((GtkWidget*)remote_command_entry,state);
	// End fetching remote_command
}

