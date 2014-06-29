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

#include "guake-indicator.h"
#include "guake-indicator-read-json.h"
#include "guake-indicator-write-json.h"
#include <string.h>

const char* write_json_cfg_file_to_str(GArray* grouphostlist)
{
	size_t i=0;
	json_object * outer_label_string=NULL;
	json_object * j_outerobj = json_object_new_object();
	
	for (i=0;grouphostlist!=NULL && i<grouphostlist->len;i++)
	{
		HostGroup* hostgroup = g_array_index (grouphostlist, HostGroup* , i);
		
		// Case of label outside the data array
		if (hostgroup->title!=NULL && hostgroup->label==TRUE)
		{
			outer_label_string = json_object_new_string(hostgroup->title);
			continue;
		}
		
		if (hostgroup->title!=NULL)
		{
			json_object * data_array_obj = json_object_new_object();
			
			// Remember that there is a label outside the data array to apply
			if (outer_label_string!=NULL)
			{
				json_object_object_add(data_array_obj, "label",outer_label_string);
				outer_label_string=NULL;
			}
			
			json_object * data_array = json_object_new_array(); 
			Host* hostPtr;
			for (hostPtr=hostgroup->hostarray;hostPtr;hostPtr=hostPtr->next)
			{
				// Open all hosts don't count because they are automatically created thus not written in the cfg file
				if (hostPtr->open_all==TRUE)
					continue;
				
				// Label inside data array
				if (hostPtr->label==TRUE)
					json_object_array_add(data_array,build_json_label_obj(hostPtr));
				// Regular host
				else
					json_object_array_add(data_array,build_json_data_obj(hostPtr));
			}
			
			json_object_object_add(data_array_obj,"data",data_array);
			json_object_object_add(j_outerobj, hostgroup->title,data_array_obj);
		}
		
		// data array in the root object
		else
		{
			json_object * data_array = json_object_new_array(); 
			Host* hostPtr;
			for (hostPtr=hostgroup->hostarray;hostPtr;hostPtr=hostPtr->next)
			{
				// Label inside data array
				if (hostPtr->label==TRUE)
					json_object_array_add(data_array,build_json_label_obj(hostPtr));
				
				// Regular host
				else
					json_object_array_add(data_array,build_json_data_obj(hostPtr));
			}
			
			json_object_object_add(j_outerobj, "data",data_array);
		}
	}
	return json_object_to_json_string_ext(j_outerobj, JSON_C_TO_STRING_PRETTY);
}

json_object * build_json_label_obj(Host* hostPtr)
{
	json_object * label_obj = json_object_new_object();
	json_object_object_add(label_obj,"label",json_object_new_string(hostPtr->menu_name));
	return label_obj;
}

json_object * build_json_data_obj(Host* hostPtr)
{
	json_object * data_obj = json_object_new_object();
	json_object_object_add(data_obj,"hostname",json_object_new_string(hostPtr->hostname));
	json_object_object_add(data_obj,"login",json_object_new_string(hostPtr->login));
	json_object_object_add(data_obj,"menu_name",json_object_new_string(hostPtr->menu_name));
	json_object_object_add(data_obj,"tab_name",json_object_new_string(hostPtr->tab_name));
	if (hostPtr->command_after_login!=NULL)
		json_object_object_add(data_obj,"command_after_login",json_object_new_string(hostPtr->command_after_login));
		
	if (hostPtr->remote_command!=NULL)
		json_object_object_add(data_obj,"remote_command",json_object_new_string(hostPtr->remote_command));
		
	if (hostPtr->x_forwarded!=NULL)
		json_object_object_add(data_obj,"x_forwarded",json_object_new_string(hostPtr->x_forwarded));
		
	if (hostPtr->dont_show_guake!=NULL)
		json_object_object_add(data_obj,"dont_show_guake",json_object_new_string(hostPtr->dont_show_guake));
	
	return data_obj;
}

int write_cfg_file(GArray* grouphostlist)
{
	char* filedir,*filecfg;
	filedir=checkandcreatedefaultdir();
	asprintf(&filecfg,"%s/%s",filedir,GUAKE_INDICATOR_DEFAULT_FILEJSON);
	free((void*)filedir);
	FILE * fd=fopen(filecfg,"w");
	if (fd==NULL)
	{
		free((void*)filecfg);
		return -1;
	}
	const char* data = write_json_cfg_file_to_str(grouphostlist);
	fwrite((const void*)data,strlen(data),1,fd);
	fclose(fd);
	free((void*)filecfg);
	return 0;
}
