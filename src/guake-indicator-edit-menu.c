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
#define _GNU_SOURCE 
#include "guake-indicator.h"
#include "guake-indicator-edit-menu.h"
#include "guake-indicator-read-json.h"
#include "guake-indicator-xml.h"
#include "guake-indicator-dbus.h"
#include "guake-indicator-notify.h"
#include "guake-indicator-write-json.h"
#include "guake3.h"

GtkTreeStore* global_tree_view;
gboolean is_print_edit_menu_form_opened=FALSE;
gboolean is_print_custom_form_opened=FALSE;

// Function to print edit menu form window
void print_edit_menu_form(GtkAction* action, gpointer user_data)
{
	GtkCellRenderer *cell_name;
	GtkTreeViewColumn* column_name;   
	GError *error = NULL;
	static EditMenuDialog widgets;

	GtkBuilder* builder;
	
	if (is_print_edit_menu_form_opened) return;

	builder = gtk_builder_new ();
	GError *err = NULL; 

	if(0 == gtk_builder_add_from_file (builder, DATADIR GUAKE_INDICATOR_DATADIR "/gi_edit_menu_dialog.glade", &err))
	{
		fprintf(stderr, "[print_edit_menu_form]: Error adding build from file. Error: %s\n", err->message);
		return ;
	}

	// get widget data references from glade file
	widgets.window = GTK_WIDGET (gtk_builder_get_object (builder, "edit_menu_window"));

	widgets.entry_menu_name = GTK_WIDGET (gtk_builder_get_object (builder, "entry_menu_name"));
	widgets.entry_tab_name = GTK_WIDGET (gtk_builder_get_object (builder, "entry_tab_name"));
	widgets.entry_command = GTK_WIDGET (gtk_builder_get_object (builder, "entry_command"));
	widgets.cb_show_guake = GTK_WIDGET (gtk_builder_get_object (builder, "cb_show_guake"));
	widgets.current_guake_tab = GTK_WIDGET (gtk_builder_get_object (builder, "current_guake_tab"));
	widgets.new_guake_tab = GTK_WIDGET (gtk_builder_get_object (builder, "new_guake_tab"));
	widgets.existing_guake_tab = GTK_WIDGET (gtk_builder_get_object (builder, "existing_guake_tab"));
	widgets.nth_guake_tab = GTK_WIDGET (gtk_builder_get_object (builder, "nth_guake_tab"));
	/*GtkObject * adj = gtk_adjustment_new( 0,0,99,1,1,0);
	gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widgets.nth_guake_tab),GTK_ADJUSTMENT(adj));*/
	widgets.existing_guake_tab_named=GTK_WIDGET (gtk_builder_get_object (builder, "existing_guake_tab_named"));
	widgets.named_guake_tab = GTK_WIDGET (gtk_builder_get_object (builder, "named_guake_tab"));
	widgets.lfcr = GTK_WIDGET (gtk_builder_get_object (builder, "lfcr"));
	widgets.guakeindicatorscript = GTK_WIDGET (gtk_builder_get_object (builder, "guakeindicatorscript"));

	widgets.btn_edit_menu_remove = GTK_WIDGET (gtk_builder_get_object (builder, "btn_edit_menu_remove"));
	widgets.btn_edit_menu_save = GTK_WIDGET (gtk_builder_get_object (builder, "btn_edit_menu_save"));
	widgets.btn_edit_menu_add_group = GTK_WIDGET (gtk_builder_get_object (builder, "btn_edit_menu_add_group"));
	widgets.btn_edit_menu_add_host = GTK_WIDGET (gtk_builder_get_object (builder, "btn_edit_menu_add_host"));
	widgets.btn_edit_menu_add_host_label = GTK_WIDGET (gtk_builder_get_object (builder, "btn_edit_menu_add_host_lbl"));
	widgets.btn_edit_menu_add_group_label = GTK_WIDGET (gtk_builder_get_object (builder, "btn_edit_menu_add_group_lbl"));
	widgets.btn_edit_menu_export = GTK_WIDGET (gtk_builder_get_object (builder, "btn_edit_menu_export"));
	widgets.btn_edit_menu_import = GTK_WIDGET (gtk_builder_get_object (builder, "btn_edit_menu_import"));

	widgets.btn_edit_menu_close_dialog = GTK_WIDGET (gtk_builder_get_object (builder, "btn_edit_menu_close_dialog"));

	widgets.action=action;
	widgets.user_data=user_data;

	widgets.status = STATUS_EDIT;
	// remove button
	g_signal_connect (G_OBJECT (widgets.btn_edit_menu_remove), "clicked",G_CALLBACK (remove_edit_menu),&widgets);

	// save button
	g_signal_connect (G_OBJECT (widgets.btn_edit_menu_save), "clicked",G_CALLBACK (save_edit_menu),&widgets);

	// cancel button
	g_signal_connect (G_OBJECT (widgets.btn_edit_menu_close_dialog), "clicked",G_CALLBACK (close_dialog),&widgets);

	// add group button
	g_signal_connect (G_OBJECT (widgets.btn_edit_menu_add_group), "clicked",G_CALLBACK (add_group),&widgets);
 
	// add host button
	g_signal_connect (G_OBJECT (widgets.btn_edit_menu_add_host), "clicked",G_CALLBACK (add_host),&widgets);
	
	// add host label button
	g_signal_connect (G_OBJECT (widgets.btn_edit_menu_add_host_label), "clicked",G_CALLBACK (add_host_label),&widgets);
	
	// add group label button
	g_signal_connect (G_OBJECT (widgets.btn_edit_menu_add_group_label), "clicked",G_CALLBACK (add_group_label),&widgets);
	
	// add export button
	g_signal_connect (G_OBJECT (widgets.btn_edit_menu_export), "clicked",G_CALLBACK (export),&widgets);

	// add import button
	g_signal_connect (G_OBJECT (widgets.btn_edit_menu_import), "clicked",G_CALLBACK (import),&widgets);
	
	// set signal for clearing gtkentry
	g_signal_connect (G_OBJECT (widgets.entry_menu_name), "icon-press",G_CALLBACK (clear_gtkentry),&widgets);
	g_signal_connect (G_OBJECT (widgets.entry_tab_name), "icon-press",G_CALLBACK (clear_gtkentry),&widgets);
	
	//set expand and collapse triggers
	GtkWidget* expandwidget = GTK_WIDGET (gtk_builder_get_object (builder, "expand"));
	g_signal_connect (G_OBJECT (expandwidget), "button-release-event",G_CALLBACK (expand),&widgets);
	
	GtkWidget* collapsewidget = GTK_WIDGET (gtk_builder_get_object (builder, "collapse"));
	g_signal_connect (G_OBJECT (collapsewidget), "button-release-event",G_CALLBACK (collapse),&widgets);
	
	//set ctrl+s keyboard shortcut for entry command
	g_signal_connect (G_OBJECT (widgets.entry_command), "key_press_event", G_CALLBACK (manage_ctrl_s), &widgets);
	
	// move button 
	
	// top button
	widgets.topButton = GTK_WIDGET (gtk_builder_get_object(builder,"btn_edit_top"));
	g_signal_connect (G_OBJECT (widgets.topButton), "clicked",G_CALLBACK (move_top),&widgets);

	// up button
	widgets.upButton = GTK_WIDGET (gtk_builder_get_object(builder,"btn_edit_up"));
	g_signal_connect (G_OBJECT (widgets.upButton), "clicked",G_CALLBACK (move_up),&widgets);

	// down button
	widgets.downButton = GTK_WIDGET (gtk_builder_get_object(builder,"btn_edit_down"));
	g_signal_connect (G_OBJECT (widgets.downButton), "clicked",G_CALLBACK (move_down),&widgets);

	// bottom button
	widgets.bottomButton = GTK_WIDGET (gtk_builder_get_object(builder,"btn_edit_bottom"));
	g_signal_connect (G_OBJECT (widgets.bottomButton), "clicked",G_CALLBACK (move_bottom),&widgets);
	
	// Start of treeview
	// treestore
	widgets.tree_store = gtk_tree_store_new(N_COLUMNS, G_TYPE_POINTER,GDK_TYPE_PIXBUF,G_TYPE_STRING);

	// treeview
	widgets.tree_view = GTK_WIDGET (gtk_builder_get_object(builder,"edit_menu_treeview"));

	/* Set treeview as the source of the Drag-N-Drop operation */
	if (DND_DISABLED!=TRUE)
	{
		static const GtkTargetEntry drag_targets = { 
		"STRING", GTK_TARGET_SAME_APP,0
		};

		/** it doesn't work well for now disabled **/
		gtk_drag_source_set(widgets.tree_view,GDK_BUTTON1_MASK, &drag_targets,1,
			GDK_ACTION_COPY|GDK_ACTION_MOVE);
		gtk_drag_dest_set(widgets.tree_view,GTK_DEST_DEFAULT_ALL,&drag_targets,1,
			GDK_ACTION_COPY|GDK_ACTION_MOVE);
	
		/* Attach a "drag-data-get" signal to send out the dragged data */
		g_signal_connect (widgets.tree_view, "drag-motion",G_CALLBACK (drag_motion_handl), &widgets);
		g_signal_connect (widgets.tree_view, "drag-begin",G_CALLBACK (drag_begin_handl), &widgets);
		g_signal_connect (widgets.tree_view, "drag-end",G_CALLBACK (drag_end_handl), &widgets);
	}

	// Handling copy cut and paste popup menu
	g_signal_connect(widgets.tree_view, "button-press-event", (GCallback) view_onButtonPressed, &widgets);
	g_signal_connect(widgets.tree_view, "popup-menu", (GCallback) view_onPopupMenu, &widgets);

	// cell
	cell_name = gtk_cell_renderer_text_new();
	widgets.labelicon = gdk_pixbuf_new_from_file (DATADIR GUAKE_INDICATOR_DATADIR "/labelicon.png",&error);
	if (error)
	{
		g_warning ("Could not load icon: %s\n", error->message);
		g_error_free(error);
		error = NULL;
		return;
	}

	widgets.hosticon = gdk_pixbuf_new_from_file (DATADIR GUAKE_INDICATOR_DATADIR "/hosticon.png",&error);
	if (error)
	{
		g_warning ("Could not load icon: %s\n", error->message);
		g_error_free(error);
		error = NULL;
		return;
	}

	widgets.hostgroupicon = gdk_pixbuf_new_from_file (DATADIR GUAKE_INDICATOR_DATADIR "/hostgroupicon.png",&error);
	if (error)
	{
		g_warning ("Could not load icon: %s\n", error->message);
		g_error_free(error);
		error = NULL;
		return;
	}
	column_name=gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column_name, "Name");

	GtkCellRenderer *renderer;
	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(column_name, renderer, FALSE);
	gtk_tree_view_column_set_attributes(column_name, renderer,"pixbuf", COL_ICON,NULL);
	gtk_tree_view_column_pack_start(column_name, cell_name, TRUE);
	gtk_tree_view_column_set_attributes(column_name, cell_name,"text", NAME_COLUMN,NULL);

	gtk_tree_view_append_column(GTK_TREE_VIEW (widgets.tree_view), column_name);

	// Fetch data from the cfg file
	if (customConfFile)
	{
		widgets.grouphostlist = read_xml_cfg_file_from_file(customConfFile);
	}
	else
	{
		if (check_xml_cfg_file_presence())
			widgets.grouphostlist = read_xml_cfg_file();
		else
			widgets.grouphostlist = read_json_cfg_file(NULL);
	}

	reload_model_view(&widgets);

	// store the callback function "selection_func" for the selection
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets.tree_view));
	gtk_tree_selection_set_select_function(selection, selection_func, (gpointer)&widgets,NULL);

	// manage the double click event that causes the row to auto expand
	g_signal_connect(widgets.tree_view, "row-activated", (GCallback) view_onRowActivated, NULL);
	
	// manage the right key press event that causes the row to auto expan
	g_signal_connect(widgets.tree_view, "key_press_event", (GCallback) on_key_press, &widgets);

	set_widget_sensitivity(&widgets,FALSE);
	set_new_widget_sensitivity(&widgets,TRUE);

	g_object_unref (G_OBJECT (builder));

	widgets.reset_flag=&is_print_edit_menu_form_opened;
	g_signal_connect(widgets.window, "destroy",G_CALLBACK(close_dialog), (gpointer)&widgets);
	gtk_widget_show_all( widgets.window );
	is_print_edit_menu_form_opened=TRUE;
	
	// move the window on top
	gtk_window_present(GTK_WINDOW(widgets.window));
	
	return;
}

void view_onRowActivated (GtkTreeView* treeview,GtkTreePath* path,GtkTreeViewColumn *col,gpointer userdata)
{
	if (gtk_tree_view_row_expanded (treeview,path)==FALSE)
		gtk_tree_view_expand_row (treeview,path,FALSE);
	else
		gtk_tree_view_collapse_row (treeview,path);
}

static void build_cmd ( GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	GArray* hbox=dialog->hbox;
	guint count=0;
	GtkWidget* iterator;
	gchar* cmd = g_strdup("");
	gchar* app;
	while ( iterator = g_array_index (hbox, GtkWidget*, count))
	{
		GList* children = gtk_container_get_children(GTK_CONTAINER(iterator));
		if (children)
		{
			GList* data = g_list_next(children);
			if (data)
			{
				if (!g_strcmp0(gtk_widget_get_name(GTK_WIDGET(data->data)),"GtkEntry"))
				{
					GtkEntry* widget_data = GTK_ENTRY(data->data);
					const gchar* name = gtk_buildable_get_name(GTK_BUILDABLE(widget_data));
					if (strlen((char*)gtk_entry_get_text(widget_data)))
					{
						app=cmd;
						if (!strncasecmp((char*)name,"empty",strlen("empty")))
							cmd=g_strconcat(cmd," '",gtk_entry_get_text(widget_data),"'",NULL);
						else
							cmd=g_strconcat(cmd," ",name," ",gtk_entry_get_text(widget_data),NULL);
						g_free(app);
					}
				}
				else if (!g_strcmp0(gtk_widget_get_name(GTK_WIDGET(data->data)),"GtkCheckButton"))
				{
					GtkCheckButton* widget_data = GTK_CHECK_BUTTON(data->data);
					if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget_data)))
					{
						app=cmd;
						cmd=g_strconcat(cmd," ",gtk_buildable_get_name(GTK_BUILDABLE(widget_data)),NULL);
						g_free(app);
					}
					//printf("%s %d\n",gtk_buildable_get_name(GTK_BUILDABLE(widget_data)),gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget_data)));fflush(stdout);
				}
				else if (!g_strcmp0(gtk_widget_get_name(GTK_WIDGET(data->data)),"GtkLabel"))
				{
					GtkLabel* widget_data = GTK_LABEL(data->data);
					app=cmd;
					cmd=g_strconcat(cmd," ",gtk_buildable_get_name(GTK_BUILDABLE(widget_data)),NULL);
					g_free(app);
				}
			}
			g_list_free(children);
		}
		count++;
	}
	
	gtk_widget_destroy(dialog->window);
	TEXTVIEW_SET_TEXT(GET_ENTRY_COMMAND(dialog),cmd)
	g_free(cmd);
	return;
}

// This function is called each time one item of the gtk treeview has been selected
gboolean selection_func (GtkTreeSelection *selection, GtkTreeModel *model, GtkTreePath *path, gboolean path_currently_selected, gpointer userdata)
{
	GtkTreeIter iter;
	EditMenuDialog* widgets = (EditMenuDialog*)userdata;
	gpointer* host;
	
	if (gtk_tree_model_get_iter(model, &iter, path) && !path_currently_selected)
	{
		gtk_tree_model_get(model, &iter, ID_COLUMN, &host, -1);
		
		gchar* pathstring=gtk_tree_path_to_string(path);
		set_selected_path(widgets,pathstring);
		g_free(pathstring);
		
		if (widgets->status == STATUS_EDIT)
		{
			switch( gtk_tree_path_get_depth(path) )
			{
				case 1: // A hostgroup has been selected
					// Enable all widgets with host retreived data
					set_widget_sensitivity(widgets,TRUE);
					activate_label_sensitivity(widgets);
					ENTRY_SET_TEXT(GET_ENTRY_MENUNAME(widgets),((HostGroup*)host)->title)
					ENTRY_SET_TEXT(GET_ENTRY_TABNAME(widgets),NULL)
					TEXTVIEW_SET_TEXT(GET_ENTRY_COMMAND(widgets),"")
					gtk_toggle_button_set_active((GtkToggleButton *)widgets->cb_show_guake,0);
					gtk_toggle_button_set_active((GtkToggleButton *)widgets->lfcr,0);
					gtk_toggle_button_set_active((GtkToggleButton *)widgets->guakeindicatorscript,0);
					widgets->selected_hostgroup=(HostGroup*)host;
					widgets->selected_host=NULL;
					break;

				case 2: // A host has been selected
					set_widget_sensitivity(widgets,TRUE);
					ENTRY_SET_TEXT(GET_ENTRY_MENUNAME(widgets),((Host*)host)->menu_name)
					ENTRY_SET_TEXT(GET_ENTRY_TABNAME(widgets),((Host*)host)->tab_name)

					// use get_guake_cmd for backward compatibility (up until guake 0.4)
					gchar* cmd = get_guake_cmd((Host*)host);
					TEXTVIEW_SET_TEXT(GET_ENTRY_COMMAND(widgets),cmd)
					if ( ((Host*)host)->dont_show_guake && !strcmp(((Host*)host)->dont_show_guake,"yes"))
						gtk_toggle_button_set_active( (GtkToggleButton *)widgets->cb_show_guake,1);
					else
						gtk_toggle_button_set_active( (GtkToggleButton *)widgets->cb_show_guake,0 );
						
					if ( ((Host*)host)->lfcr && !strcmp(((Host*)host)->lfcr,"yes"))
						gtk_toggle_button_set_active( (GtkToggleButton *)widgets->lfcr,1);
					else
						gtk_toggle_button_set_active( (GtkToggleButton *)widgets->lfcr,0 );
						
					if ( ((Host*)host)->guakeindicatorscript && !strcmp(((Host*)host)->guakeindicatorscript,"yes"))
						gtk_toggle_button_set_active( (GtkToggleButton *)widgets->guakeindicatorscript,1);
					else
						gtk_toggle_button_set_active( (GtkToggleButton *)widgets->guakeindicatorscript,0 );
					
					gchar* open_in_tab=((Host*)host)->open_in_tab;
					gboolean named=((Host*)host)->open_in_tab_named;
					if (open_in_tab==NULL || (atol((char*)open_in_tab)<0 && named==FALSE))
					{
						if (open_in_tab && atol((char*)open_in_tab)==-1)
						{
							gtk_toggle_button_set_active( (GtkToggleButton *)widgets->new_guake_tab,0);
							gtk_toggle_button_set_active( (GtkToggleButton *)widgets->current_guake_tab,1);
						}
						else
						{
							gtk_toggle_button_set_active( (GtkToggleButton *)widgets->new_guake_tab,1);
							gtk_toggle_button_set_active( (GtkToggleButton *)widgets->current_guake_tab,0);
						}
						gtk_toggle_button_set_active( (GtkToggleButton *)widgets->existing_guake_tab,0);
						gtk_entry_set_text(GTK_ENTRY(widgets->nth_guake_tab),"0");
						gtk_entry_set_text(GTK_ENTRY(widgets->named_guake_tab),"");
					}
					else
					{
						gtk_toggle_button_set_active( (GtkToggleButton *)widgets->new_guake_tab,0);
						if (named==FALSE)
						{
							gtk_toggle_button_set_active( (GtkToggleButton *)widgets->existing_guake_tab,1);
							gtk_toggle_button_set_active( (GtkToggleButton *)widgets->existing_guake_tab_named,0);
							gtk_entry_set_text(GTK_ENTRY(widgets->nth_guake_tab),open_in_tab);
							gtk_entry_set_text(GTK_ENTRY(widgets->named_guake_tab),"");
						}
						else
						{
							gtk_toggle_button_set_active( (GtkToggleButton *)widgets->existing_guake_tab,0);
							gtk_toggle_button_set_active( (GtkToggleButton *)widgets->existing_guake_tab_named,1);
							gtk_entry_set_text(GTK_ENTRY(widgets->nth_guake_tab),"0");
							gtk_entry_set_text(GTK_ENTRY(widgets->named_guake_tab),open_in_tab);
						}
					}
					
					// disable selection by tab name if the dbus interface is not read (guake has not been patched)
					gchar* name;
					if (guake_getgtktabname(0,&name)==FALSE)
					{
						gtk_widget_set_sensitive(widgets->existing_guake_tab_named,FALSE);
						gtk_widget_set_sensitive(widgets->named_guake_tab,FALSE);
						/*GtkTooltips *tooltip;
						tooltip = gtk_tooltips_new ();
						gtk_tooltips_set_tip (tooltip, widgets->named_guake_tab,GUAKE_INDICATOR_DBUS_GTKLABEL_MISSING_ERRMSG, NULL);
						gtk_tooltips_set_tip (tooltip, widgets->existing_guake_tab_named, GUAKE_INDICATOR_DBUS_GTKLABEL_MISSING_ERRMSG, NULL);*/
					}
					else
						g_free(name);
						
					widgets->selected_host=(Host*)host;
					widgets->selected_hostgroup=NULL;
					if (((Host*)host)->label)
					{
						activate_label_sensitivity(widgets);
					}
					g_free(cmd);
					break;

					default: break;
			}
		}
		else
		{
			switch( gtk_tree_path_get_depth(path) )
			{
				case 1:
					widgets->selected_hostgroup=(HostGroup*)host;
					widgets->selected_host=NULL;
					break;
				case 2:
					widgets->selected_host=(Host*)host;
					widgets->selected_hostgroup=NULL;
					break;
			}
		}
	}
	return TRUE; /* allow selection state to change */
}

static void close_dialog ( GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;

	if (dialog->tree_store)
		g_object_unref (dialog->tree_store);
	set_selected_path(dialog,NULL);
	if (dialog->grouphostlist)
		grouphostlist_free(dialog->grouphostlist);
	gtk_widget_hide(GTK_WIDGET(dialog->window));
	if (dialog->reset_flag)
		*dialog->reset_flag=FALSE;
	return ;
}

// Function behind remove button
static void remove_edit_menu ( GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	
	switch (dialog->status)
	{
		case 0:
			if (dialog->selected_host)
			{
				Host* deletehost = dialog->selected_host;

				// Delete host in first position
				if (deletehost->previous==NULL)
				{
					deletehost->parent->hostarray=dialog->selected_host->next;
					host_free(dialog->selected_host);
				}
				else
				{
					deletehost->previous->next=dialog->selected_host->next;
					host_free(dialog->selected_host);
				}
				free(dialog->selected_host);
				write_and_reload(dialog,"Entry removed successfully");
			}
			else
			{
				// Deny "root node" removal
				if (dialog->selected_hostgroup->title==NULL)
				{
					error_modal_box ("You can't remove the Root node");
					return;
				}

				// Get the root grouphost
				guint count = 0;
				gint root_index=-1;
				HostGroup* iterator;
				while ( iterator = g_array_index (dialog->grouphostlist, HostGroup*, count))
				{
					if (iterator->title==NULL)
					{
						root_index = get_grouphost_index(dialog->grouphostlist,iterator);
						break;
					}
					count++;
				}
				if (root_index>=0)
				{
					// Delete the open_all host
					Host* ptr;
					for (ptr=dialog->selected_hostgroup->hostarray;ptr;ptr=ptr->next)
					{
						if (ptr->next && ptr->next->open_all)
						{
							host_free(ptr->next);
							free(ptr->next);
							ptr->next=NULL;
						}
					}
						
					HostGroup* root = (HostGroup*)g_array_index (dialog->grouphostlist, HostGroup*, (guint)root_index);

					if (root->hostarray==NULL) root->hostarray=dialog->selected_hostgroup->hostarray;
					else
					{
						for (ptr=root->hostarray;ptr;ptr=ptr->next)
						{
							if (ptr->next==NULL)
							{
								ptr->next = dialog->selected_hostgroup->hostarray;
								break;
							}
						}
					}
				}

				guint index = get_grouphost_index(dialog->grouphostlist,dialog->selected_hostgroup);
				hostgroup_free(dialog->selected_hostgroup);
				g_array_remove_index(dialog->grouphostlist,index);
				free(dialog->selected_hostgroup);

				write_and_reload(dialog,"Entrygroup removed successfully");
			}
		case 1: // cancel add group
		case 2: // cancel add host
		case 3: // cancel host label
		case 4 : // cancel group label
			set_widget_sensitivity(dialog,FALSE);
			unselect_treeview(dialog);
			set_new_widget_sensitivity(dialog,TRUE);
			gtk_button_set_label(GTK_BUTTON(dialog->btn_edit_menu_remove), "Remove");
			break;
		default:
			break;
	}
	dialog->status = STATUS_EDIT;
}

static void add_group ( GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	
	// clear all field, enable required field
	clear_widget(dialog);
	activate_grouphosts_sensitivity(dialog);
	unselect_treeview(dialog);

	// enable only save button
	gtk_widget_set_sensitive(dialog->btn_edit_menu_remove,TRUE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host_label,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group_label,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_save,TRUE);
	
	gtk_button_set_label(GTK_BUTTON(dialog->btn_edit_menu_remove), "Cancel");
	dialog->status = STATUS_ADD_GROUP;
}
	
static void add_host ( GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;

	clear_widget(dialog);
	set_widget_sensitivity(dialog,FALSE);
	unselect_treeview(dialog);

	// enable only save button
	set_form_widget_sensitivity(dialog,TRUE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_remove,TRUE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_save,TRUE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_export,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_import,TRUE);
	gtk_button_set_label(GTK_BUTTON(dialog->btn_edit_menu_remove), "Cancel");
	gtk_toggle_button_set_active( (GtkToggleButton *)dialog->new_guake_tab,1);
	
	// disable selection by tab name if the dbus interface is not read (guake has not been patched)
	gchar* name;
	if (guake_getgtktabname(0,&name)==FALSE)
	{
		gtk_widget_set_sensitive(dialog->existing_guake_tab_named,FALSE);
		gtk_widget_set_sensitive(dialog->named_guake_tab,FALSE);
		/*GtkTooltips *tooltip;
		tooltip = gtk_tooltips_new ();
		gtk_tooltips_set_tip (tooltip, dialog->named_guake_tab,GUAKE_INDICATOR_DBUS_GTKLABEL_MISSING_ERRMSG, NULL);
		gtk_tooltips_set_tip (tooltip, dialog->existing_guake_tab_named, GUAKE_INDICATOR_DBUS_GTKLABEL_MISSING_ERRMSG, NULL);*/
	}
	else
		g_free(name);

	dialog->status = STATUS_ADD_HOST;
}

static void add_group_label ( GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	gtk_widget_set_sensitive(dialog->btn_edit_menu_save,TRUE);
	
	// clear all field, enable required field
	clear_widget(dialog);
	activate_grouphosts_sensitivity(dialog);
	unselect_treeview(dialog);

	// enable only save button
	gtk_widget_set_sensitive(dialog->btn_edit_menu_remove,TRUE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host_label,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group_label,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_save,TRUE);

	gtk_button_set_label(GTK_BUTTON(dialog->btn_edit_menu_remove), "Cancel");
	dialog->status = STATUS_ADD_GROUP_LABEL;
}

static void add_host_label ( GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	gtk_widget_set_sensitive(dialog->btn_edit_menu_save,TRUE);
	
	// clear all field, enable required field
	clear_widget(dialog);
	activate_grouphosts_sensitivity(dialog);
	unselect_treeview(dialog);

	// enable only save button
	gtk_widget_set_sensitive(dialog->btn_edit_menu_remove,TRUE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host_label,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group_label,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_save,TRUE);

	gtk_button_set_label(GTK_BUTTON(dialog->btn_edit_menu_remove), "Cancel");
	dialog->status = STATUS_ADD_HOST_LABEL;
}

static void import (GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* widgets = (EditMenuDialog*) user_data;
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Open File",
                                      GTK_WINDOW(widgets->window),
                                      action,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_Open"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);
		//open_file (filename);
		GArray* imported_hostlist = read_xml_cfg_file_from_file(filename);
		Host* imported_host=((HostGroup*)g_array_index(imported_hostlist,HostGroup*,0))->hostarray;
		if (GTK_IS_TEXT_VIEW((GtkTextView*)widgets->entry_command))
			TEXTVIEW_SET_TEXT(GET_ENTRY_COMMAND(widgets),imported_host->command_after_login)
		if (GTK_IS_ENTRY((GtkEntry*)widgets->entry_tab_name))
			ENTRY_SET_TEXT(GET_ENTRY_TABNAME(widgets),imported_host->tab_name);
		if (GTK_IS_ENTRY((GtkEntry*)widgets->entry_menu_name))
			ENTRY_SET_TEXT(GET_ENTRY_MENUNAME(widgets),imported_host->menu_name);
		if (GTK_IS_TOGGLE_BUTTON((GtkToggleButton *)widgets->cb_show_guake))
			if ( ((Host*)imported_host)->dont_show_guake && !strcmp(((Host*)imported_host)->dont_show_guake,"yes"))
				gtk_toggle_button_set_active( (GtkToggleButton *)widgets->cb_show_guake,1);
			else
				gtk_toggle_button_set_active( (GtkToggleButton *)widgets->cb_show_guake,0 );
		if (GTK_IS_TOGGLE_BUTTON((GtkToggleButton *)widgets->lfcr))
			if ( ((Host*)imported_host)->guakeindicatorscript && !strcmp(((Host*)imported_host)->guakeindicatorscript,"yes"))
				gtk_toggle_button_set_active( (GtkToggleButton *)widgets->guakeindicatorscript,1);
			else
				gtk_toggle_button_set_active( (GtkToggleButton *)widgets->guakeindicatorscript,0 );
		gtk_toggle_button_set_active( (GtkToggleButton *)widgets->new_guake_tab,1);
		grouphostlist_free(imported_hostlist);
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
	return ;
}

static void export ( GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* widgets = (EditMenuDialog*) user_data;
	GtkWidget *dialog;

	GtkFileChooser *chooser;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Save File",
                                      GTK_WINDOW(widgets->window),
                                      action,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_Save"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);
	chooser = GTK_FILE_CHOOSER (dialog);

	gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
	gtk_file_chooser_set_current_name (chooser,(DEFAULT_EXPORT_FILENAME));
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		Host* head;
		
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		GArray* exportarrray = g_array_new (TRUE, FALSE, sizeof (HostGroup*));
		HostGroup* hostgroup = malloc(sizeof(HostGroup));
		bzero((void*)hostgroup,sizeof(HostGroup));
		sethostgroupcounterid(hostgroup);
		Host* newhost = create_new_host(hostgroup,
		head,
		widgets->selected_host->menu_name,
		widgets->selected_host->tab_name,
		widgets->selected_host->command_after_login,
		widgets->selected_host->dont_show_guake&&!strcasecmp((char*)widgets->selected_host->dont_show_guake,"yes")?TRUE:FALSE,
		NULL,
		FALSE,
		widgets->selected_host->lfcr&&!strcasecmp((char*)widgets->selected_host->lfcr,"yes")?TRUE:FALSE,
		widgets->selected_host->guakeindicatorscript&&!strcasecmp((char*)widgets->selected_host->guakeindicatorscript,"yes")?TRUE:FALSE,
		FALSE,
		FALSE);
		head=host_queue(head,newhost);
		hostgroup->hostarray=head;
		g_array_append_val (exportarrray, hostgroup);
		write_xml_cfg_file_from_file(exportarrray,filename);
		char* msg;
		if (asprintf(&msg,"Export to file %s complete",filename)==-1)
		{
			guake_notify("Guake indicator",msg);
			free(msg);
		}
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

// Function to handle the edit button
static void save_edit_menu ( GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;

	switch (dialog->status)
	{
		case STATUS_EDIT: // save edit item
		{
			// if root menu:
			if ( !g_strcmp0(dialog->selected_path,"0"))
			{
				error_modal_box ("You're not allowed to save root menu");
				return;
			}
			if (!strlen((char*)gtk_entry_get_text(GET_ENTRY_MENUNAME(dialog))))
			{
				error_modal_box ("Menu name is blank");
				return ;
			}

			// A host object has been selected so a Host* object will be updated
			if (dialog->selected_host)
			{
				// Only for backward compatibility
				UPDATE_ENTRY(dialog->selected_host->hostname,NULL)
				UPDATE_ENTRY(dialog->selected_host->login,NULL)
				UPDATE_ENTRY(dialog->selected_host->remote_command,NULL)
				UPDATE_ENTRY(dialog->selected_host->x_forwarded,NULL)
				UPDATE_ENTRY(dialog->selected_host->menu_name,gtk_entry_get_text(GET_ENTRY_MENUNAME(dialog)))
				UPDATE_ENTRY(dialog->selected_host->tab_name,gtk_entry_get_text(GET_ENTRY_TABNAME(dialog)))
				gchar* text;
				GET_GTKVIEW_TEXT(GET_ENTRY_COMMAND(dialog),text)
				UPDATE_ENTRY(dialog->selected_host->command_after_login,text)
				g_free(text);
				UPDATE_ENTRY(dialog->selected_host->dont_show_guake,gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_SHOWGUAKE(dialog))?"yes":"no")
				if (gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_EXISTINGGUAKETAB(dialog)))
				{
					UPDATE_ENTRY(dialog->selected_host->open_in_tab,gtk_entry_get_text(GET_ENTRY_NTHGUAKETAB(dialog)))
					dialog->selected_host->open_in_tab_named=FALSE;
				}
				else if (gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_EXISTINGGUAKETABNAMED(dialog)))
				{
					UPDATE_ENTRY(dialog->selected_host->open_in_tab,gtk_entry_get_text(GET_ENTRY_NAMEDGUAKETAB(dialog)))
					dialog->selected_host->open_in_tab_named=TRUE;
				}
				else if (gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_EXISTINGGUAKETABCURRENT(dialog)))
				{
					UPDATE_ENTRY(dialog->selected_host->open_in_tab,"-1")
				}
				else
				{
					dialog->selected_host->open_in_tab_named=FALSE;
					dialog->selected_host->open_in_tab=NULL;
				}
				UPDATE_ENTRY(dialog->selected_host->lfcr,gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_LFCR(dialog))?"yes":"no")
				UPDATE_ENTRY(dialog->selected_host->guakeindicatorscript,gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_GUAKEINDICATORSCRIPT(dialog))?"yes":"no")
				
				write_and_reload(dialog,"Entry saved successfully");
				
				// if the "open in nth guake tab" option is selected i check if the dbus call to the tab counter succeed, if not i warn the user
				check_guake_get_tab_count(dialog);
			}
			// A hostgroup object has been selected so a Host* object will be updated
			else if (dialog->selected_hostgroup)
			{
				UPDATE_ENTRY(dialog->selected_hostgroup->title,gtk_entry_get_text(GET_ENTRY_MENUNAME(dialog)))
				write_and_reload(dialog,"Entry saved successfully");
			}
			else
			{
				error_modal_box("Entry or Entry group not selected");
			}
			break;
		}
		case STATUS_ADD_GROUP_LABEL:
		case STATUS_ADD_GROUP: // add group
		{
			if (!strlen((char*)gtk_entry_get_text(GET_ENTRY_MENUNAME(dialog))))
			{
				error_modal_box ("Menu name is blank");
				return ;
			}

			if (dialog->selected_hostgroup == NULL && dialog->grouphostlist->len )
			{
				error_modal_box ("Select a group first. \nThe new group will be added after the selected group");
				return;
			}
			// save new group
			gint groupindex = (dialog->grouphostlist->len ) ? get_grouphost_index(dialog->grouphostlist,dialog->selected_hostgroup) : -1;

			HostGroup* newhostgroup=malloc(sizeof(HostGroup));
			bzero((void*)newhostgroup,sizeof(HostGroup));
			sethostgroupcounterid(newhostgroup);
			newhostgroup->title=g_strdup((const gchar*)gtk_entry_get_text(GET_ENTRY_MENUNAME(dialog)));
			if (dialog->status==STATUS_ADD_GROUP_LABEL)
				newhostgroup->label=TRUE;
			g_array_insert_val(dialog->grouphostlist,++groupindex,newhostgroup);

			if (dialog->status==STATUS_ADD_GROUP_LABEL)
				write_and_reload(dialog,"Label saved successfully");
			else
				write_and_reload(dialog,"Group saved successfully");

			set_widget_sensitivity(dialog,FALSE);
			gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group,TRUE);
			gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host,TRUE);
			gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host_label,TRUE);
			gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group_label,TRUE);
			gtk_button_set_label(GTK_BUTTON(dialog->btn_edit_menu_remove), "Remove");
			break;
		}
		case STATUS_ADD_HOST_LABEL:
		case STATUS_ADD_HOST: // add host
		{
			if (!strlen((char*)gtk_entry_get_text(GET_ENTRY_MENUNAME(dialog))))
			{
				error_modal_box ("Menu name is blank");
				return ;
			}
			if (!dialog->grouphostlist->len)
			{
				error_modal_box ("Create a new group first.");
				return;
			}
			if (dialog->selected_hostgroup == NULL && dialog->selected_host == NULL)
			{
				error_modal_box ("Select a group or a host.The new host will be placed after the selected element");
				return;
			}
			// check if the selected hostgroup is a label
			if (dialog->selected_hostgroup != NULL && dialog->selected_hostgroup->label)
			{
				error_modal_box ("Selected host group is a label");
				return;
			}

			Host* newhost = malloc(sizeof(Host));
			bzero((void*)newhost,sizeof(Host));
			sethostcounterid(newhost);
			newhost->menu_name=strdup(gtk_entry_get_text(GET_ENTRY_MENUNAME(dialog)));
			if (dialog->status==STATUS_ADD_HOST_LABEL)
				newhost->label=TRUE;
			else
			{
				newhost->tab_name=strdup(gtk_entry_get_text(GET_ENTRY_TABNAME(dialog)));
				gchar* text;
				GET_GTKVIEW_TEXT(GET_ENTRY_COMMAND(dialog),text)
				newhost->command_after_login=g_strdup(text);
				g_free(text);
				if (gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_SHOWGUAKE(dialog)))
					newhost->dont_show_guake=strdup("yes");
				if (gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_EXISTINGGUAKETAB(dialog)))
				{
					newhost->open_in_tab_named=FALSE;
					newhost->open_in_tab=strdup(gtk_entry_get_text(GET_ENTRY_NTHGUAKETAB(dialog)));
				}
				else if (gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_EXISTINGGUAKETABNAMED(dialog)))
				{
					newhost->open_in_tab_named=TRUE;
					newhost->open_in_tab=strdup(gtk_entry_get_text(GET_ENTRY_NAMEDGUAKETAB(dialog)));
				}
				else if (gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_EXISTINGGUAKETABCURRENT(dialog)))
				{
					newhost->open_in_tab=strdup("-1");
				}
				if (gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_LFCR(dialog)))
					newhost->lfcr=strdup("yes");
				if (gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_GUAKEINDICATORSCRIPT(dialog)))
					newhost->guakeindicatorscript=strdup("yes");
			}
			if (dialog->selected_hostgroup)
			{
				newhost->next=dialog->selected_hostgroup->hostarray;
				dialog->selected_hostgroup->hostarray=newhost;
			}
			else
			{
				if (dialog->selected_host->previous==NULL)
				{
					newhost->next=dialog->selected_host->parent->hostarray->next;
					dialog->selected_host->parent->hostarray->next=newhost;
				}
				else
				{
					newhost->next=dialog->selected_host->next;
					dialog->selected_host->next=newhost;
				}
			}
			write_and_reload(dialog,"Host saved successfully");
			
			// if the "open in nth guake tab" option is selected i check if the dbus call to the tab counter succeed, if not i warn the user
			check_guake_get_tab_count(dialog);
			
			set_widget_sensitivity(dialog,FALSE);
			gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group,TRUE);
			gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host,TRUE);
			gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host_label,TRUE);
			gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group_label,TRUE);
			gtk_button_set_label(GTK_BUTTON(dialog->btn_edit_menu_remove), "Remove");
			break;
		}
		default:
		break;
	}
	dialog->status = STATUS_EDIT;
}

// Function to set sensitivity of all entry data widget 
static void set_widget_sensitivity(EditMenuDialog* dialog,gboolean flag)
{
	set_form_widget_sensitivity(dialog,flag);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_remove,flag);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_save,flag);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_export,flag);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_import,flag);
	
	set_new_widget_sensitivity(dialog,flag);
	set_move_widget_sensitivity(dialog,flag);
}

static void set_form_widget_sensitivity(EditMenuDialog* dialog,gboolean flag)
{
	gtk_widget_set_sensitive(dialog->entry_menu_name,flag);
	gtk_widget_set_sensitive(dialog->entry_tab_name,flag);
	gtk_widget_set_sensitive(dialog->entry_command,flag);
	gtk_widget_set_sensitive(dialog->cb_show_guake,flag);
	gtk_widget_set_sensitive(dialog->current_guake_tab,flag);
	gtk_widget_set_sensitive(dialog->new_guake_tab,flag);
	gtk_widget_set_sensitive(dialog->existing_guake_tab,flag);
	gtk_widget_set_sensitive(dialog->nth_guake_tab,flag);
	gtk_widget_set_sensitive(dialog->existing_guake_tab_named,flag);
	gtk_widget_set_sensitive(dialog->named_guake_tab,flag);
	gtk_widget_set_sensitive(dialog->lfcr,flag);
	gtk_widget_set_sensitive(dialog->guakeindicatorscript,flag);
}

static void set_new_widget_sensitivity(EditMenuDialog* dialog,gboolean flag)
{
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group,flag);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host,flag);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_group_label,flag);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_add_host_label,flag);
}
static void set_move_widget_sensitivity(EditMenuDialog* dialog,gboolean flag)
{
	gtk_widget_set_sensitive(dialog->topButton,flag);
	gtk_widget_set_sensitive(dialog->upButton,flag);
	gtk_widget_set_sensitive(dialog->downButton,flag);
	gtk_widget_set_sensitive(dialog->bottomButton,flag);
}

static void activate_grouphosts_sensitivity(EditMenuDialog* dialog)
{
	set_widget_sensitivity(dialog,FALSE);
	gtk_widget_set_sensitive(dialog->entry_menu_name,TRUE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_save,TRUE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_remove,TRUE);
}

static void activate_label_sensitivity(EditMenuDialog* dialog)
{
	set_form_widget_sensitivity(dialog,FALSE);
	gtk_widget_set_sensitive(dialog->entry_menu_name,TRUE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_save,TRUE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_remove,TRUE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_export,FALSE);
	gtk_widget_set_sensitive(dialog->btn_edit_menu_import,FALSE);
}

static void clear_widget(EditMenuDialog* dialog)
{
	ENTRY_SET_TEXT(GET_ENTRY_MENUNAME(dialog),NULL)
	ENTRY_SET_TEXT(GET_ENTRY_TABNAME(dialog),NULL)
	TEXTVIEW_SET_TEXT(GET_ENTRY_COMMAND(dialog),"")
	gtk_toggle_button_set_active(GET_ENTRY_SHOWGUAKE(dialog),0);
	gtk_toggle_button_set_active(GET_ENTRY_LFCR(dialog),0);
	gtk_toggle_button_set_active(GET_ENTRY_GUAKEINDICATORSCRIPT(dialog),0);
}

static void unselect_treeview(EditMenuDialog* dialog)
{
	// unselect all
	gtk_tree_selection_unselect_all (gtk_tree_view_get_selection(GTK_TREE_VIEW (dialog->tree_view)));
	set_selected_path(dialog,NULL);
	dialog->selected_hostgroup = NULL;
	dialog->selected_host = NULL;
}

gchar* get_guake_cmd(Host* host)
{
	gchar* cmd = NULL;
	
	// set x_forwarded flag
	gchar* x_forwarded_flag;
	if (host->x_forwarded==NULL || g_strcmp0(host->x_forwarded,"yes"))
		x_forwarded_flag=g_strdup("");
	else
		x_forwarded_flag=g_strdup(" -X ");
		
	// get the command
	if (host->hostname == NULL || !strlen((char*)host->hostname))
		cmd = g_strjoin(NULL,host->command_after_login!=NULL?host->command_after_login:"",NULL);
	else if (host->command_after_login==NULL || !strlen((char*)host->command_after_login))
		cmd = g_strjoin(NULL,"ssh"," -l ",host->login," ",host->hostname,NULL);
	else
	{
		if (host->remote_command==NULL || g_strcmp0(host->remote_command,"yes"))
			cmd = g_strjoin(NULL,"ssh",x_forwarded_flag," -t -l ",host->login," ",host->hostname," '",host->command_after_login,";/bin/bash'",NULL);
		else
			cmd = g_strjoin(NULL,"ssh",x_forwarded_flag," -t -l ",host->login," ",host->hostname," ",host->command_after_login,NULL);
	}
	g_free(x_forwarded_flag);
	return cmd;
}

guint get_grouphost_index(GArray* grouphostlist,HostGroup* hostgroup)
{
	guint count = 0;
	HostGroup* iterator;
	while ( iterator = g_array_index (grouphostlist, HostGroup*, count))
	{
		if (!g_strcmp0(iterator->id,hostgroup->id))
			return count;
		count++;
	}
	return -1;
}

guint get_grouphost_size(GArray* grouphostlist)
{
	guint count = 0;
	HostGroup* iterator;
	while ( iterator = g_array_index (grouphostlist, HostGroup*, count))
	{
		count++;
	}
	return count;
}

void reload_model_view(EditMenuDialog *dialog)
{
	GtkTreeIter iter;
	GtkTreeIter iter2;

	dialog->tree_store = gtk_tree_store_new(N_COLUMNS, G_TYPE_POINTER,GDK_TYPE_PIXBUF,G_TYPE_STRING);
	gtk_tree_store_clear(GTK_TREE_STORE(dialog->tree_store));
	int j=0;

	for (j=0;dialog->grouphostlist!=NULL && j<dialog->grouphostlist->len;j++)
	{
		HostGroup* hostgroup = g_array_index (dialog->grouphostlist, HostGroup* , j);

		gtk_tree_store_append(dialog->tree_store, &iter, NULL);
		GdkPixbuf* icon;
		if (hostgroup->label)
			icon=dialog->labelicon;
		else
			icon=dialog->hostgroupicon;
		
		gtk_tree_store_set (dialog->tree_store, &iter,
			ID_COLUMN,(gpointer*)hostgroup,
			COL_ICON, icon,
			NAME_COLUMN, hostgroup->title!=NULL?hostgroup->title:"Root node",
			-1);

		Host* ptr=NULL;
		for (ptr=hostgroup->hostarray;ptr;ptr=ptr->next)
		{
			if (ptr->open_all==TRUE) continue;
			gtk_tree_store_append(dialog->tree_store, &iter2, &iter);
			
			if (ptr->label)
				icon=dialog->labelicon;
			else
				icon=dialog->hosticon;
			
			gtk_tree_store_set (dialog->tree_store, &iter2,
				ID_COLUMN,(gpointer*)ptr,
				COL_ICON, icon,
				NAME_COLUMN,ptr->menu_name,
				-1);
		}
	}
	
	gtk_tree_view_set_model (GTK_TREE_VIEW (dialog->tree_view), GTK_TREE_MODEL (dialog->tree_store));
	
	/* Tell the theme engine we would like differentiated row colour */
	//gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(dialog->tree_view),TRUE);
	//dialog->tree_view->set_rules_hint(TRUE);
	
	// TODO expand and select row
	if (dialog->selected_path !=NULL)
	{
		GtkTreeSelection *sel;
		GtkTreePath *path;

		sel = gtk_tree_view_get_selection(GTK_TREE_VIEW (dialog->tree_view));
		path = gtk_tree_path_new_from_string(dialog->selected_path);
	
		if (gtk_tree_path_get_depth(path)>1)
		{
			// expand
			gtk_tree_view_expand_to_path (GTK_TREE_VIEW (dialog->tree_view), path);
		}
		gtk_tree_selection_select_path(sel, path);
		gtk_tree_path_free(path);
	}
	unselect_treeview(dialog);
	set_widget_sensitivity(dialog,FALSE);
	set_new_widget_sensitivity(dialog,TRUE);
}

void move_down(GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	
	// Moving a host
	if (dialog->selected_host)
	{
		
		//If the host is the last element i can't move it down
		if (dialog->selected_host->next==NULL || dialog->selected_host->next->open_all )
			return;
		
		Host* nexthost=dialog->selected_host->next;
		
		if (dialog->selected_host->previous==NULL)
		{
			
			dialog->selected_host->next=nexthost->next;
			dialog->selected_host->parent->hostarray=nexthost;
			nexthost->next=dialog->selected_host;
		}
		else
		{
			dialog->selected_host->next=nexthost->next;
			dialog->selected_host->previous->next=nexthost;
			nexthost->next=dialog->selected_host;
		}
		write_and_reload(dialog,"Entry moved down successfully");
	}
	// Hostgroup movedown
	else
	{
		// Get the new hostgroup reference, if it doesn't exist we are at the end so i exit
		guint index=get_grouphost_index(dialog->grouphostlist,dialog->selected_hostgroup);
		if (index+1>=get_grouphost_size(dialog->grouphostlist))
			return;
		g_array_insert_val(dialog->grouphostlist,index+2,dialog->selected_hostgroup);
		g_array_remove_index(dialog->grouphostlist,index);
		
		write_and_reload(dialog,"Entry moved down successfully");
	}
}

void move_up(GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	
	// Moving a host
	if (dialog->selected_host)
	{
		//If the host is the first element i can't move it up
		if (dialog->selected_host->previous==NULL || dialog->selected_host->previous->open_all )
			return;
		
		Host* previoushost=dialog->selected_host->previous;
		
		if (previoushost->previous==NULL)
		{			
			dialog->selected_host->parent->hostarray=dialog->selected_host;
			previoushost->next=dialog->selected_host->next;
			dialog->selected_host->next=previoushost;
		}
		else
		{
			previoushost->previous->next=dialog->selected_host;
			previoushost->next=dialog->selected_host->next;
			dialog->selected_host->next=previoushost;
		}
		
		write_and_reload(dialog,"Entry moved up successfully");
	}
	// Hostgroup movedown
	else
	{
		// Get the new hostgroup reference, if it doesn't exist we are at the end so i exit
		guint index=get_grouphost_index(dialog->grouphostlist,dialog->selected_hostgroup);
		if (index<=0)
			return ;
		g_array_insert_val(dialog->grouphostlist,index-1,dialog->selected_hostgroup);
		g_array_remove_index(dialog->grouphostlist,index+1);
		write_and_reload(dialog,"Entrygroup moved up successfully");
	}
}

// move a host or hostgroup on the bottom of the list
void move_bottom(GtkWidget *widget, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;

	// Moving a host
	if (dialog->selected_host)
	{
		//If the host is the last element i can't move it down
		if (dialog->selected_host->next==NULL || dialog->selected_host->next->open_all )
			return;
		
		Host* lasthost=dialog->selected_host;
		while (lasthost->next && lasthost->next->open_all==FALSE)
		{
			lasthost=lasthost->next;
		}
		if (lasthost==dialog->selected_host)
			return ;
		if (dialog->selected_host->previous==NULL)
			dialog->selected_host->parent->hostarray=dialog->selected_host->next;
		else
			dialog->selected_host->previous->next=dialog->selected_host->next;
		
		Host* ptr=lasthost->next;
		lasthost->next=dialog->selected_host;
		dialog->selected_host->next=ptr;
		write_and_reload(dialog,"Entry moved bottom successfully");
	}
	else
	{
		// Get the new hostgroup reference, if it doesn't exist we are at the end so i exit
		guint index=get_grouphost_index(dialog->grouphostlist,dialog->selected_hostgroup);
		if (index+1>=get_grouphost_size(dialog->grouphostlist))
			return;
		g_array_insert_val(dialog->grouphostlist,get_grouphost_size(dialog->grouphostlist),dialog->selected_hostgroup);
		g_array_remove_index(dialog->grouphostlist,index);		
		write_and_reload(dialog,"Entrygroup moved down successfully");
	}
}

// expand a treeview
static void expand(GtkWidget *widget,GdkEvent  *event,gpointer user_data )
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	gtk_tree_view_expand_all( GTK_TREE_VIEW(dialog->tree_view));
}

// collapse a treeview
static void collapse(GtkWidget *widget,GdkEvent  *event,gpointer user_data )
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	gtk_tree_view_collapse_all(GTK_TREE_VIEW(dialog->tree_view));
}

// move a host or hostgroup on top of the list
void move_top(GtkWidget *widget, gpointer user_data )
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	
	// Moving a host
	if (dialog->selected_host)
	{
		//If the host is the first element i can't move it up
		if (dialog->selected_host->previous==NULL || dialog->selected_host->previous->open_all )
			return;
		dialog->selected_host->previous->next=dialog->selected_host->next;
		dialog->selected_host->next=dialog->selected_host->parent->hostarray;
		dialog->selected_host->parent->hostarray=dialog->selected_host;
		write_and_reload(dialog,"Entry moved top successfully");
	}
	else
	{
		// Get the new hostgroup reference, if it doesn't exist we are at the end so i exit
		guint index=get_grouphost_index(dialog->grouphostlist,dialog->selected_hostgroup);
		if (index<=0)
			return ;
		g_array_insert_val(dialog->grouphostlist,0,dialog->selected_hostgroup);
		g_array_remove_index(dialog->grouphostlist,index+1);
		write_and_reload(dialog,"Entrygroup moved top successfully");
	}
}

// Write the current configuration to a file (json and xml), print a message, 
//reload the configuration from the file and reload the model view for the treeview
void write_and_reload(EditMenuDialog* dialog,const char* msg)
{
	dialog->copied_host=NULL;
	dialog->copied_hostgroup=NULL;
	write_cfg_file(dialog->grouphostlist);
	write_xml_cfg_file(dialog->grouphostlist);
	error_modal_box (msg);
	refresh_indicator(dialog->user_data);
	grouphostlist_free(dialog->grouphostlist);
	if (customConfFile) dialog->grouphostlist=read_xml_cfg_file_from_file(customConfFile);
	else dialog->grouphostlist=read_xml_cfg_file();
	reload_model_view(dialog);
}

// Get all the plugin files into ~/.guake-indicator/plugins
GArray* get_custom_glade_files()
{
	char* plugindir;
	char* pluginpath;
	GArray* res=g_array_new (TRUE, FALSE, sizeof (char*));
	plugindir=checkandcreatedefaultdir();
	if (asprintf(&pluginpath,"%s/%s",plugindir,GUAKE_INDICATOR_PLUGIN_DIR)==-1)
		return NULL;
	DIR* dir = opendir(pluginpath);
	free(pluginpath);
	free(plugindir);
	
	if (dir)
	{
		struct dirent* de = 0;
		while ((de = readdir(dir)) != 0)
		{
			if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
				continue;
			
			if ((rindex(de->d_name,'.') && !strcmp(rindex(de->d_name,'.'),".glade"))
			|| (rindex(de->d_name,'.') && !strcmp(rindex(de->d_name,'.'),".xml")))
			{
				char* filename=strdup(de->d_name);
				g_array_append_val(res,filename);
			}
		}
	}
	closedir(dir);
	return res;
}

// Selection funct for the select glade file treeview
gboolean gladefile_selection_func (GtkTreeSelection *selection, GtkTreeModel *model, GtkTreePath *path, gboolean path_currently_selected, gpointer userdata)
{
	gpointer* item;
	GtkTreeIter iter;
	EditMenuDialog* widgets = (EditMenuDialog*)userdata;
	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gtk_tree_model_get(model, &iter, 0, &item, -1);
		widgets->selected_glade_file=g_strdup((gchar*)item);
		g_free(item);
	}
	return TRUE;
}


static void drag_begin_handl (GtkWidget *widget, GdkDragContext *context, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	GtkTreeIter iter;
	GtkTreeModel *list_store;
	gpointer* generic_data;
	GtkTreeSelection *selector;
	
	if (dialog->status!=STATUS_EDIT) return ;
	
	selector = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
	gtk_tree_selection_get_selected(selector,&list_store,&iter);
	gtk_tree_model_get(GTK_TREE_MODEL(dialog->tree_store), &iter, ID_COLUMN, &generic_data, -1);
	GtkTreePath * path = gtk_tree_model_get_path (GTK_TREE_MODEL(dialog->tree_store),&iter);
	if (gtk_tree_path_get_depth(path)==1)
	{
		dialog->starthostgroupdrag = (HostGroup*) generic_data;
		dialog->starthostdrag = NULL;
	}
	else
	{
		dialog->starthostgroupdrag = NULL ;
		dialog->starthostdrag = (Host*) generic_data;
	}
	gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(widget),GTK_TREE_VIEW_GRID_LINES_BOTH);
	gtk_tree_path_free(path);
}

static gboolean drag_motion_handl (GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint t,gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	GtkTreeIter iter;
	GtkTreePath *path;
	GtkTreeSelection *select;
	gpointer* generic_data;
	static int scrolly=0;
	
	if (dialog->status!=STATUS_EDIT) return FALSE;

	gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (widget), x, y-25, &path, 0, 0, 0);
	if (path==NULL) return FALSE;
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
	gtk_tree_selection_select_path (select, path);
	gtk_tree_model_get_iter  (GTK_TREE_MODEL(dialog->tree_store),&iter,path);
	gtk_tree_model_get(GTK_TREE_MODEL(dialog->tree_store), &iter, ID_COLUMN, &generic_data, -1);
	if (gtk_tree_path_get_depth(path)==1)
	{
		if (dialog->starthostdrag && gtk_tree_view_row_expanded (GTK_TREE_VIEW (widget),path)==FALSE)
		{
			gtk_tree_view_expand_row (GTK_TREE_VIEW (widget),path,FALSE);
		}
		dialog->endhostgroupdrag = (HostGroup*) generic_data;
		dialog->endhostdrag = NULL;
	}
	else if (gtk_tree_path_get_depth(path)==2)
	{
		dialog->endhostgroupdrag = NULL ;
		dialog->endhostdrag = (Host*) generic_data;
	}
	if (y>300)
	{
		if (scrolly<300) gtk_tree_view_scroll_to_point (GTK_TREE_VIEW (widget),0,scrolly+=15);
	}
	else if (y<30)
	{
		if (scrolly>0) gtk_tree_view_scroll_to_point (GTK_TREE_VIEW (widget),0,scrolly-=15);
	}
	gtk_tree_path_free (path);
	return  TRUE;
}

static void drag_end_handl (GtkWidget *widget, GdkDragContext *context, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(widget),0);
	if (dialog->status!=STATUS_EDIT) return ;
	
	// Move an hostgroup after another
	if (dialog->starthostgroupdrag!=NULL && dialog->endhostgroupdrag && dialog->starthostgroupdrag!=dialog->endhostgroupdrag)
	{
		guint indexstart=get_grouphost_index(dialog->grouphostlist,dialog->starthostgroupdrag);
		guint indexend=get_grouphost_index(dialog->grouphostlist,dialog->endhostgroupdrag);
		if (indexstart-1==indexend) 
			return;
		g_array_insert_val(dialog->grouphostlist,indexend+1,dialog->starthostgroupdrag);
		if (indexend>indexstart)
			g_array_remove_index(dialog->grouphostlist,indexstart);
		else
			g_array_remove_index(dialog->grouphostlist,indexstart+1);
		write_and_reload(dialog,"Entrygroup moved successfully");
	}
	// Move an host after another
	else if (dialog->starthostdrag!=NULL && dialog->endhostdrag!=NULL && dialog->starthostdrag!=dialog->endhostdrag && dialog->starthostdrag->previous!=dialog->endhostdrag)
	{
		Host* ptr = dialog->endhostdrag->next;
		if (dialog->starthostdrag->previous==NULL)
			dialog->starthostdrag->parent->hostarray=dialog->starthostdrag->next;
		else
			dialog->starthostdrag->previous->next=dialog->starthostdrag->next;
		dialog->endhostdrag->next=dialog->starthostdrag;
		dialog->starthostdrag->next=ptr;
		write_and_reload(dialog,"Entry moved successfully");
	}
	// Move an host in first position of a hostgroup
	else if (dialog->starthostdrag!=NULL && dialog->endhostgroupdrag!=NULL && dialog->endhostgroupdrag->label==FALSE)
	{
		if (dialog->starthostdrag->previous==NULL)
			dialog->starthostdrag->parent->hostarray=dialog->starthostdrag->next;
		else
			dialog->starthostdrag->previous->next=dialog->starthostdrag->next;
		dialog->starthostdrag->next=dialog->endhostgroupdrag->hostarray;
		dialog->endhostgroupdrag->hostarray=dialog->starthostdrag;
		write_and_reload(dialog,"Entry moved successfully");
	}
}

void clear_gtkentry (GtkEntry* entry,GtkEntryIconPosition icon_pos,GdkEvent *event,gpointer user_data)
{
	gtk_entry_set_text( entry, "" );
}

// Expand/collapse a hostgroup when the right/left key is pressed
gboolean on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	EditMenuDialog* dialog = (EditMenuDialog*) user_data;
	GtkTreePath *path;
	
	switch (event->keyval)
	{
		case GDK_KEY_Right:	path = gtk_tree_path_new_from_string (dialog->selected_path);
						gtk_tree_view_expand_row (GTK_TREE_VIEW(dialog->tree_view),path,FALSE);
						gtk_tree_path_free (path);
						break;
						
		case GDK_KEY_Left:	path = gtk_tree_path_new_from_string (dialog->selected_path);
						gtk_tree_view_collapse_row (GTK_TREE_VIEW(dialog->tree_view),path);
						gtk_tree_path_free (path);
						break;
	}
	return FALSE;
}
// Manage ctrl+s for save host
gboolean manage_ctrl_s (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	switch (event->keyval)
	{
		case GDK_KEY_S:
		case GDK_KEY_s:		
						if (event->state & GDK_CONTROL_MASK)
						{
							save_edit_menu ( NULL,user_data);
						}
						break;
	}
	return FALSE;
}

void set_selected_path(EditMenuDialog* widgets,gchar* value)
{
	if (widgets->selected_path)
	{
		g_free(widgets->selected_path);
	}
	if (value)
		widgets->selected_path=g_strdup(value);
	else
		widgets->selected_path=NULL;
	return ;
}

// check if guake_tab_count dbus interface is available, if not a warning pops up
void check_guake_get_tab_count(EditMenuDialog* dialog)
{
	gint32 numtabs;
	if ((gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_EXISTINGGUAKETAB(dialog)) || gtk_toggle_button_get_active((GtkToggleButton*)GET_ENTRY_EXISTINGGUAKETABNAMED(dialog))) && guake_gettabcount(&numtabs)==FALSE)
	{
		char msg[1000];
		sprintf(msg,"Guake indicator detected a Guake version prior to 0.4.5 (dbus call to get_tab_count failed), it's very likely that this host will be opened in a new tab instead of reusing tab number %s.\nI suggest you upgrading Guake to version 0.4.5 or higher to use this feature",gtk_entry_get_text(GET_ENTRY_NTHGUAKETAB(dialog)));
		error_modal_box(msg);
	}
}

gboolean view_onButtonPressed (GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
	EditMenuDialog* dialog = (EditMenuDialog*) userdata;
	gpointer host;
	GtkTreeIter iter;
	
	if (dialog->status!=STATUS_EDIT)
		return FALSE;
	
	/* single click with the right mouse button */
	if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3)
	{
		GtkTreeSelection *selection;
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
		
		GtkTreePath *path;
 
		/* Get tree path for row that was clicked */
		if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview),
											(gint) event->x, 
											(gint) event->y,
											&path, NULL, NULL, NULL))
		{
			gtk_tree_selection_unselect_all(selection);
			gtk_tree_model_get_iter(GTK_TREE_MODEL (dialog->tree_store), &iter, path);
			gtk_tree_model_get(GTK_TREE_MODEL (dialog->tree_store), &iter, ID_COLUMN, &host, -1);
			switch( gtk_tree_path_get_depth(path) )
			{
				case 1:
					dialog->selected_hostgroup_for_operation=(HostGroup*)host;
					dialog->selected_host_for_operation=NULL;
					break;
				case 2:
					dialog->selected_host_for_operation=(Host*)host;
					dialog->selected_hostgroup_for_operation=NULL;
					break;
			}
			gtk_tree_selection_select_path(selection, path);
			gtk_tree_path_free(path);
		}
		view_popup_menu(treeview, event, userdata);
	}
	return FALSE;
}
  
void view_popup_menu (GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
	EditMenuDialog* dialog = (EditMenuDialog*) userdata;
	GtkWidget *menu, *copy,*cut,*paste;
	menu = gtk_menu_new();
	copy = gtk_menu_item_new_with_label("Copy");
	cut = gtk_menu_item_new_with_label("Cut");
	paste = gtk_menu_item_new_with_label("Paste");
	
	// set widget sensitivity
	gtk_widget_set_sensitive(GTK_WIDGET(paste),dialog->copied_host||dialog->copied_hostgroup?TRUE:FALSE);
 
	g_signal_connect(copy, "activate",(GCallback) view_popup_menu_oncopy, userdata);
	g_signal_connect(cut, "activate",(GCallback) view_popup_menu_oncut, userdata);
	g_signal_connect(paste, "activate",(GCallback) view_popup_menu_onpaste, userdata);

	if (dialog->selected_hostgroup_for_operation==NULL)
	{
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), copy);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), cut);
	}
	
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), paste);

	gtk_widget_show_all(menu);
 
	//gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,(event != NULL) ? event->button : 0,event->time);
	gtk_menu_popup_at_widget (GTK_MENU (menu),
                            menu,
                            GDK_GRAVITY_SOUTH_WEST,
                            GDK_GRAVITY_NORTH_WEST,
                            (GdkEvent *)event);

}

// Callback function of copy operations
void view_popup_menu_oncopy (GtkWidget *menuitem, gpointer userdata)
{
	EditMenuDialog* dialog = (EditMenuDialog*) userdata;
	dialog->type_operation=TRUE;
	view_popup_menu_onperformcutcopyaction (menuitem,userdata);
}

// Callback function of cut operations
void view_popup_menu_oncut (GtkWidget *menuitem, gpointer userdata)
{
	EditMenuDialog* dialog = (EditMenuDialog*) userdata;
	dialog->type_operation=FALSE;
	view_popup_menu_onperformcutcopyaction (menuitem,userdata);
}

void view_popup_menu_onperformcutcopyaction (GtkWidget *menuitem, gpointer userdata)
{
	EditMenuDialog* dialog = (EditMenuDialog*) userdata;
	
	if (dialog->selected_host_for_operation)
	{
		dialog->copied_host=dialog->selected_host_for_operation;
		gchar* message=g_strjoin(NULL,"Menu item ",dialog->copied_host->menu_name," stored into the clipboard",NULL);
		guake_notify("Guake indicator",message );
		g_free(message);
	}
	else if (dialog->selected_hostgroup_for_operation)
	{
		dialog->copied_hostgroup=dialog->selected_hostgroup_for_operation;
		gchar* message=g_strjoin(NULL,"Menu ",dialog->copied_host->menu_name," stored into the clipboard",NULL);
		guake_notify("Guake indicator",message );
		g_free(message);
	}
	else
		if (dialog->type_operation)
			error_modal_box("Nothing to copy");
		else
			error_modal_box("Nothing to cut");
}

void view_popup_menu_onpaste (GtkWidget *menuitem, gpointer userdata)
{
	EditMenuDialog* dialog = (EditMenuDialog*) userdata;
	
	// Manage the copy and paste (duplicate element)
	if (dialog->type_operation==TRUE)
	{
		// Manage the host copy
		if (dialog->copied_host)
		{
			// Create a new host based on the copied element
			Host*newhost=create_new_host(NULL,
			NULL,
			dialog->copied_host->menu_name,dialog->copied_host->tab_name,dialog->copied_host->command_after_login,
			dialog->copied_host->dont_show_guake&&!strcasecmp(dialog->copied_host->dont_show_guake,"yes")?TRUE:FALSE,
			dialog->copied_host->open_in_tab,
			dialog->copied_host->label,
			dialog->copied_host->lfcr&&!strcasecmp(dialog->copied_host->lfcr,"yes")?TRUE:FALSE,
			dialog->copied_host->guakeindicatorscript&&!strcasecmp(dialog->copied_host->guakeindicatorscript,"yes")?TRUE:FALSE,
			dialog->copied_host->open_in_tab_named,
			FALSE);
			
			// Host after another host
			if (dialog->selected_host_for_operation)
			{
				newhost->parent=dialog->selected_host_for_operation->parent;
				newhost->group_head=dialog->selected_host_for_operation->group_head;
				newhost->next=dialog->selected_host_for_operation->next;
				dialog->selected_host_for_operation->next=newhost;
				write_and_reload(dialog,"Entry copied successfully");
			}
			
			//Host as a first element of a group
			else if (dialog->selected_hostgroup_for_operation)
			{
				newhost->parent=dialog->selected_hostgroup_for_operation;
				newhost->group_head=newhost;
				newhost->next=dialog->selected_hostgroup_for_operation->hostarray;
				dialog->selected_hostgroup_for_operation->hostarray=newhost;
				write_and_reload(dialog,"Host copied successfully");
			}
			else
				error_modal_box("Nothing to paste");
		}
		
		// Manage the hostgroup copy (not implemented because probably useless)
		/*else
		{
			
		}*/
	}
	// Manage the cut and paste
	else
	{
		if (dialog->copied_host)
		{
			// Host after another host
			if (dialog->selected_host_for_operation)
			{
				Host* ptr = dialog->selected_host_for_operation->next;
				if (dialog->copied_host->previous==NULL)
					dialog->copied_host->parent->hostarray=dialog->copied_host->next;
				else
					dialog->copied_host->previous->next=dialog->copied_host->next;
				dialog->selected_host_for_operation->next=dialog->copied_host;
				dialog->copied_host->next=ptr;
				write_and_reload(dialog,"Entry cut successfully");
			}
			//Host as a first element of a group
			else if (dialog->selected_hostgroup_for_operation)
			{
				if (dialog->copied_host->previous==NULL)
					dialog->copied_host->parent->hostarray=dialog->copied_host->next;
				else
					dialog->copied_host->previous->next=dialog->copied_host->next;
				dialog->copied_host->next=dialog->selected_hostgroup_for_operation->hostarray;
				dialog->selected_hostgroup_for_operation->hostarray=dialog->copied_host;
				write_and_reload(dialog,"Entry moved successfully");
			}
			else
				error_modal_box("Nothing to paste");
		}
	}
}

gboolean view_onPopupMenu (GtkWidget *treeview, gpointer userdata)
{
	view_popup_menu(treeview, NULL, userdata);
	return TRUE;
}
