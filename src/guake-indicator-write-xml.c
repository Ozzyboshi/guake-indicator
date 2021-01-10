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
#include <guake-indicator.h>
#include <guake-indicator-read-json.h>
#include <guake-indicator-xml.h>
#include "guake-indicator-edit-menu.h"
#include <guake3.h>

int write_xml_cfg_file(GArray* grouphostlist)
{
	return write_xml_cfg_file_from_file(grouphostlist,customConfFile?customConfFile:NULL);
}

int write_xml_cfg_file_from_file(GArray* grouphostlist,char* file)
{
	char* filedir,*filecfg;
	
	if (file==NULL)
	{
		filedir=checkandcreatedefaultdir();
		if (asprintf(&filecfg,"%s/%s",filedir,GUAKE_INDICATOR_DEFAULT_FILEXML)==-1)
			return -1;
		free((void*)filedir);
		FILE * fd=fopen(filecfg,"w");
		if (fd==NULL)
		{
			free((void*)filecfg);
			return -1;
		}
		fclose(fd);
	}
	else
		filecfg=strdup(file);
	
	xmlDocPtr doc = NULL;
	xmlNodePtr node = NULL;
	xmlNodePtr node2 = NULL;
	xmlNodePtr root_node = NULL;/* node pointers */
	doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "Configuration");
    xmlDocSetRootElement(doc, root_node);
    
    guint count = 0;
	HostGroup* iterator;
	while ( iterator = g_array_index (grouphostlist, HostGroup*, count))
	{
		node=xmlNewChild(root_node, NULL, BAD_CAST "HostGroup", BAD_CAST "");
		if (iterator->title)
			xmlNewProp(node, BAD_CAST "name", BAD_CAST iterator->title);
		if (iterator->label)
			xmlNewProp(node, BAD_CAST "label", BAD_CAST "yes");
			
		Host* ptr;
		for (ptr=iterator->hostarray;ptr;ptr=ptr->next)
		{
			if (ptr->open_all) continue;
			node2=xmlNewChild(node, NULL, BAD_CAST "Host", BAD_CAST "");
			if (ptr->label)
				xmlNewProp(node2, BAD_CAST "label", BAD_CAST "yes");
			xmlNewTextChild(node2, NULL, BAD_CAST "menu_name", BAD_CAST ptr->menu_name);
			if (ptr->label==FALSE)
			{
				xmlNewTextChild(node2, NULL, BAD_CAST "tab_name", BAD_CAST ptr->tab_name);
				
				gchar* guakecmd=get_guake_cmd(ptr);

				xmlNewTextChild(node2, NULL, BAD_CAST "command_after_login",BAD_CAST guakecmd);
				g_free(guakecmd);
				xmlNewTextChild(node2, NULL, BAD_CAST "dont_show_guake", BAD_CAST ptr->dont_show_guake&&!strcmp((char*)ptr->dont_show_guake,"yes")?"yes":"no");
				if (ptr->open_in_tab)
				{
					xmlNodePtr open_in_tab_node = xmlNewTextChild(node2, NULL, BAD_CAST "open_in_tab", BAD_CAST ptr->open_in_tab);
					if (ptr->open_in_tab_named)
						xmlNewProp(open_in_tab_node, BAD_CAST "named", BAD_CAST "yes");
				}
				xmlNewTextChild(node2, NULL, BAD_CAST "lfcr", BAD_CAST ptr->lfcr&&!strcmp((char*)ptr->lfcr,"yes")?"yes":"no");
				xmlNewTextChild(node2, NULL, BAD_CAST "guakeindicatorscript", BAD_CAST ptr->guakeindicatorscript&&!strcmp((char*)ptr->guakeindicatorscript,"yes")?"yes":"no");
			}
		}
		count++;
	}
    xmlSaveFormatFileEnc(filecfg, doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    free((void*)filecfg);
	return 0;
}
