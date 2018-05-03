/*
Copyright (C) 2013-2018 Alessio Garzi <gun101@email.it>
Copyright (C) 2013-2018 Francesco Minà <mina.francesco@gmail.com>

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
#include <string.h>

GArray* read_xml_cfg_file()
{
	return read_xml_cfg_file_from_file(NULL);
}

GArray* read_xml_cfg_file_from_file(char* filecfginput)
{
	char* filedir,*filecfg;
	GArray* hostgrouparray;
	Host* head=NULL;
	
	hostgrouparray = g_array_new (TRUE, FALSE, sizeof (HostGroup*));
	
	filedir=checkandcreatedefaultdir();
	if (filecfginput==NULL)
		asprintf(&filecfg,"%s/%s",filedir,GUAKE_INDICATOR_DEFAULT_FILEXML);
	else
		filecfg=strdup(filecfginput);
	free((void*)filedir);
	xmlNode *cur_node = NULL;
	
	LIBXML_TEST_VERSION
	xmlDoc *doc = NULL;
	doc = xmlReadFile(filecfg, NULL, 0);
	if (doc == NULL)
	{
		return NULL;
	}
	
	xmlNode *root_element = xmlDocGetRootElement(doc);
	cur_node = root_element->xmlChildrenNode;
	while (cur_node != NULL)
	{
		if (!xmlStrcmp(cur_node->name, (const xmlChar *)"HostGroup"))
		{
			xmlChar* label=xmlGetProp(cur_node,"label");
			head=NULL;
			
			HostGroup* hostgroup = malloc(sizeof(HostGroup));
			bzero((void*)hostgroup,sizeof(HostGroup));
			sethostgroupcounterid(hostgroup);
			xmlChar* hostgrouplabel = xmlGetProp(cur_node,"name");
			hostgroup->title=g_strdup((const gchar*)hostgrouplabel);
			xmlFree(hostgrouplabel);
			
			// read all the hosts
			xmlNode * host_node=cur_node->xmlChildrenNode;
			while (host_node != NULL)
			{
				
				if (!xmlStrcmp(host_node->name, (const xmlChar *)"Host"))
				{
					xmlChar* hostlabel=xmlGetProp(host_node,"label");
					xmlNode * data_node=host_node->xmlChildrenNode;
					xmlChar* menu_name=NULL,*tab_name=NULL,*command_after_login=NULL,*dont_show_guake=NULL,*open_in_tab=NULL,*lfcr=NULL,*guakeindicatorscript=NULL,*named=NULL;
					while (data_node != NULL)
					{
						if (!xmlStrcmp(data_node->name, (const xmlChar *)"menu_name"))
							menu_name=xmlNodeListGetString(doc, data_node->children, 1);
						if (!xmlStrcmp(data_node->name, (const xmlChar *)"tab_name"))
							tab_name=xmlNodeListGetString(doc, data_node->children, 1);
						if (!xmlStrcmp(data_node->name, (const xmlChar *)"command_after_login"))
							command_after_login=xmlNodeListGetString(doc, data_node->children, 1);
						if (!xmlStrcmp(data_node->name, (const xmlChar *)"dont_show_guake"))
							dont_show_guake=xmlNodeListGetString(doc, data_node->children, 1);
						if (!xmlStrcmp(data_node->name, (const xmlChar *)"open_in_tab"))
						{
							open_in_tab=xmlNodeListGetString(doc, data_node->children, 1);
							if (open_in_tab)
								named=xmlGetProp(data_node,"named");
						}
						if (!xmlStrcmp(data_node->name, (const xmlChar *)"lfcr"))
							lfcr=xmlNodeListGetString(doc, data_node->children, 1);
						if (!xmlStrcmp(data_node->name, (const xmlChar *)"guakeindicatorscript"))
							guakeindicatorscript=xmlNodeListGetString(doc, data_node->children, 1);
						data_node=data_node->next;
					}
					
					Host* newhost = create_new_host(hostgroup,
					head,
					menu_name,
					tab_name,
					command_after_login,
					dont_show_guake&&!strcasecmp((char*)dont_show_guake,"yes")?TRUE:FALSE,
					open_in_tab,hostlabel&&!strcasecmp((char*)hostlabel,"yes")?TRUE:FALSE,
					lfcr&&!strcasecmp((char*)lfcr,"yes")?TRUE:FALSE,\
					guakeindicatorscript&&!strcasecmp((char*)guakeindicatorscript,"yes")?TRUE:FALSE,
					named&&!strcasecmp((char*)named,"yes")?TRUE:FALSE,
					FALSE);
					head=host_queue(head,newhost);
					hostgroup->hostarray=head;
					xmlFree(hostlabel);
					xmlFree(command_after_login);
					xmlFree(menu_name);
					xmlFree(tab_name);
					xmlFree(dont_show_guake);
					xmlFree(open_in_tab);
					xmlFree(lfcr);
					xmlFree(guakeindicatorscript);
					xmlFree(named);
				}
				host_node=host_node->next;
			}
			
			if ((!xmlStrcmp(label, (const xmlChar *)"yes")))
			{
				hostgroup->label=TRUE;
			}
			// create a regular hostgroup
			else
			{
				// Set the open all row if we are not on the root node
				if (hostgroup->title)
				{
					Host* newhost = create_new_host( hostgroup,head,"Open all",NULL,NULL,FALSE,"",FALSE,FALSE,FALSE,FALSE,TRUE);
					head=host_queue(head,newhost);
					hostgroup->hostarray=head;
				}
			}
			xmlFree(label);
			g_array_append_val (hostgrouparray, hostgroup);
		}
		cur_node = cur_node->next;
	}
	xmlFree(cur_node);
	free(filecfg);
	xmlFreeDoc(doc);
	return hostgrouparray;
}

gboolean check_xml_cfg_file_presence()
{
	char* filedir,*filecfg;
	gboolean ret;
	filedir=checkandcreatedefaultdir();
	asprintf(&filecfg,"%s/%s",filedir,GUAKE_INDICATOR_DEFAULT_FILEXML);
	if( access( filecfg, F_OK ) != -1 )
		ret=TRUE;
	else
		ret=FALSE;
	free((void*)filedir);
	free((void*)filecfg);
	return ret;
}

// Function for new host creation
Host* create_new_host(HostGroup* parent, Host* head,const gchar* menu_name,const gchar* tab_name,const gchar* command,gboolean dont_show_guake,const gchar* open_in_tab,gboolean label,gboolean lfcr,gboolean guakeindicatorscript,gboolean named,gboolean open_all)
{
	Host* host = (Host*) malloc(sizeof(Host));
	bzero((void*)host,sizeof(Host));
	sethostcounterid(host);
	
	host->parent=parent;
	host->menu_name=g_strdup(menu_name);
	if (label==TRUE)
	{
		host->label=TRUE;
		return host;
	}
	if (open_all==TRUE)
	{
		host->group_head=head;
		host->open_all=TRUE;
		return host;
	}
	host->tab_name=g_strdup(tab_name);
	host->command_after_login=g_strdup(command);
	if (dont_show_guake==TRUE)
		host->dont_show_guake=g_strdup("yes");
	if (open_in_tab)
	{
		host->open_in_tab=g_strdup(open_in_tab);
		host->open_in_tab_named=named;
	}
	if (lfcr==TRUE)
		host->lfcr=g_strdup("yes");
	if (guakeindicatorscript==TRUE)
		host->guakeindicatorscript=g_strdup("yes");
	return host;
}
