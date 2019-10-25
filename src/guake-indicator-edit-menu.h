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

#ifndef _GUAKE_INDICATOR_EDIT_MENU_H
#define	_GUAKE_INDICATOR_EDIT_MENU_H

#define DND_DISABLED TRUE

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <string.h>

enum
{
	ID_COLUMN,
	COL_ICON,
	NAME_COLUMN,
	N_COLUMNS
};

enum
{
	STATUS_EDIT,
	STATUS_ADD_GROUP,
	STATUS_ADD_HOST,
	STATUS_ADD_HOST_LABEL,
	STATUS_ADD_GROUP_LABEL,
};

#define HOSTGROUP 1
#define HOST 2

#define GUAKE_INDICATOR_DATADIR "/guake-indicator"
#define GUAKE_INDICATOR_PLUGIN_MANAGER "/guake-indicator-plugin-manager"
#define GUAKE_INDICATOR_DBUS_GTKLABEL_MISSING_ERRMSG "Patch Guake to enable this feature: see http://guake-indicator.ozzyboshi.com/index.html#findtabbynameissue"

#define ENTRY_SET_TEXT(var,var2) gtk_entry_set_text(var,var2?var2:"");
#define TEXTVIEW_SET_TEXT(var,var2) gtk_text_buffer_set_text(gtk_text_view_get_buffer (var),var2?var2:"",strlen((char*)var2));

// get entry definitions
#define GET_ENTRY_MENUNAME(var) (GtkEntry*)var->entry_menu_name
#define GET_ENTRY_TABNAME(var) (GtkEntry*)var->entry_tab_name
#define GET_ENTRY_COMMAND(var) (GtkTextView*)var->entry_command
#define GET_ENTRY_SHOWGUAKE(var) (GtkToggleButton*)var->cb_show_guake
#define GET_ENTRY_EXISTINGGUAKETAB(var) (GtkToggleButton*)var->existing_guake_tab
#define GET_ENTRY_EXISTINGGUAKETABNAMED(var) (GtkToggleButton*)var->existing_guake_tab_named
#define GET_ENTRY_EXISTINGGUAKETABCURRENT(var) (GtkToggleButton*)var->current_guake_tab
#define GET_ENTRY_NTHGUAKETAB(var) (GtkEntry*)var->nth_guake_tab
#define GET_ENTRY_NAMEDGUAKETAB(var) (GtkEntry*)var->named_guake_tab
#define GET_ENTRY_LFCR(var)	(GtkToggleButton*)var->lfcr
#define GET_ENTRY_GUAKEINDICATORSCRIPT(var) (GtkToggleButton*)var->guakeindicatorscript

#define GET_GTKVIEW_TEXT(var,var2) GtkTextIter start, end;\
							GtkTextBuffer *buffer = gtk_text_view_get_buffer (var);\
							gtk_text_buffer_get_bounds (buffer, &start, &end);\
							var2 = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

// update entry definitions
#define UPDATE_ENTRY(var,var2)	g_free(var);\
								var=g_strdup(var2);

#define DEFAULT_EXPORT_FILENAME "Untitled document.xml"

typedef struct edit_menu_dialog_type
{
	// main window
	GtkWidget *window;

	// entries
	GtkWidget *entry_menu_name;

	GtkWidget *entry_tab_name;
	GtkWidget *entry_command;

	// checkboxes and radiobox
	GtkWidget *cb_show_guake;
	GtkWidget *current_guake_tab;
	GtkWidget *new_guake_tab;
	GtkWidget *existing_guake_tab;
	GtkWidget *nth_guake_tab;
	GtkWidget *existing_guake_tab_named;
	GtkWidget *named_guake_tab;
	GtkWidget *lfcr;
	GtkWidget *guakeindicatorscript;

	// buttons
	GtkWidget *btn_edit_menu_save;
	GtkWidget *btn_edit_menu_remove;
	GtkWidget *btn_edit_menu_add_group;
	GtkWidget *btn_edit_menu_add_host;
	GtkWidget *btn_edit_menu_add_host_label;
	GtkWidget *btn_edit_menu_add_group_label;
	GtkWidget *btn_edit_menu_export;
	GtkWidget *btn_edit_menu_import;
	GtkWidget *topButton;
	GtkWidget *upButton;
	GtkWidget *downButton;
	GtkWidget *bottomButton;

	GtkWidget *btn_edit_menu_close_dialog;

	GArray* hbox;

	// selection data
	Host* selected_host;
	HostGroup* selected_hostgroup;
	gchar* selected_glade_file;

	// selection data for copy cut and paste operations
	Host* selected_host_for_operation;
	HostGroup* selected_hostgroup_for_operation;
	gboolean type_operation; // true = copy false = cut
	Host* copied_host;
	HostGroup* copied_hostgroup;

	gchar * selected_path;
	// treestore
	GtkTreeStore *tree_store;
	GtkWidget *tree_view;

	// action status
	gint status;
	gboolean* reset_flag;

	//Cfg file data
	GArray* grouphostlist;
	GtkAction* action;
	gpointer user_data;

	//Icons
	GdkPixbuf	*labelicon;
	GdkPixbuf	*hosticon;
	GdkPixbuf	*hostgroupicon;
	
	//Drag data
	HostGroup* starthostgroupdrag;
	Host* starthostdrag;
	HostGroup* endhostgroupdrag;
	Host* endhostdrag;
	
} EditMenuDialog;

GArray* get_custom_glade_files();
static void build_cmd ( GtkWidget *, gpointer);
static void close_dialog ( GtkWidget *, gpointer );
static void save_edit_menu ( GtkWidget *, gpointer );
static void remove_edit_menu ( GtkWidget *, gpointer );
static void add_group ( GtkWidget *, gpointer );
static void add_host ( GtkWidget *, gpointer );
static void add_host_label ( GtkWidget *, gpointer);
static void add_group_label ( GtkWidget *, gpointer);
static void export ( GtkWidget *, gpointer);
static void import ( GtkWidget *, gpointer);

static void populate_dialog(gpointer);
static void set_widget_sensitivity(EditMenuDialog*,gboolean);
static void set_form_widget_sensitivity(EditMenuDialog*,gboolean);
static void activate_grouphosts_sensitivity(EditMenuDialog*);
static void activate_label_sensitivity(EditMenuDialog*);
static void clear_widget(EditMenuDialog*);
static void set_new_widget_sensitivity(EditMenuDialog* ,gboolean);
static void set_move_widget_sensitivity(EditMenuDialog* ,gboolean );
static void unselect_treeview(EditMenuDialog*);
void write_and_reload(EditMenuDialog*,const char* );

gboolean selection_func (GtkTreeSelection *, GtkTreeModel *, GtkTreePath *,gboolean, gpointer);
gboolean gladefile_selection_func (GtkTreeSelection *, GtkTreeModel *, GtkTreePath *, gboolean, gpointer);
gchar* get_guake_cmd(Host*);
guint get_grouphost_index(GArray*,HostGroup*);
guint get_grouphost_size(GArray* grouphostlist);

void reload_model_view(EditMenuDialog *dialog);

static void move_top(GtkWidget *, gpointer );
static void move_up(GtkWidget *, gpointer );
static void move_down(GtkWidget *, gpointer );
static void move_bottom(GtkWidget *, gpointer );
static void expand(GtkWidget *, GdkEvent*,gpointer  );
static void collapse(GtkWidget *, GdkEvent*,gpointer  );

static gboolean drag_motion_handl (GtkWidget *, GdkDragContext *, gint , gint , guint ,gpointer );
static void drag_begin_handl(GtkWidget *, GdkDragContext *, gpointer );
static void drag_end_handl (GtkWidget *, GdkDragContext *, gpointer );
void view_onRowActivated (GtkTreeView* ,GtkTreePath* ,GtkTreeViewColumn *,gpointer);
gboolean view_onButtonPressed (GtkWidget *, GdkEventButton *, gpointer );
void view_popup_menu (GtkWidget *, GdkEventButton *, gpointer );
void view_popup_menu_oncopy (GtkWidget *, gpointer);
void view_popup_menu_onpaste (GtkWidget *, gpointer);
void view_popup_menu_oncut (GtkWidget *, gpointer);
void view_popup_menu_onperformcutcopyaction (GtkWidget *, gpointer);
gboolean view_onPopupMenu (GtkWidget *, gpointer);
void clear_gtkentry (GtkEntry* ,GtkEntryIconPosition ,GdkEvent *,gpointer);
gboolean on_key_press (GtkWidget *, GdkEventKey *, gpointer );
gboolean manage_ctrl_s (GtkWidget *, GdkEventKey *, gpointer );
void set_selected_path(EditMenuDialog*,gchar*);
void check_guake_get_tab_count(EditMenuDialog*);

#endif	/* _GUAKE_INDICATOR_EDIT_MENU_H */