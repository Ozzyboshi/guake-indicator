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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
 

#include "guake-indicator.h"
#include "guake-indicator-read-json.h"

// This function reads the json cfg file and returns a linked list of host
GArray* read_json_cfg_file(char * customfilecfg)
{
	FILE* fd;
	struct json_object *new_obj;
	struct stat s;
	char* raw_data=NULL;
	char* filecfg;
	char* filedir;
	size_t numbytes;

	if (customfilecfg!=NULL)
		filecfg=strdup(customfilecfg);
	else
	{
			filedir=checkandcreatedefaultdir();
			asprintf(&filecfg,"%s/%s",filedir,GUAKE_INDICATOR_DEFAULT_FILEJSON);
			free((void*)filedir);
	}
	
	int err = stat(filecfg, &s);
	if (err==-1)
	{
			if (createdefaultfilecfg(filecfg))
				return NULL;
			err = stat(filecfg, &s);
	}
	
	if(!S_ISREG(s.st_mode) && !S_ISLNK(s.st_mode))
	{
		free((void*)filecfg);
		return NULL;
	}
	fd=fopen(filecfg,"r");
	free((void*)filecfg);
	if (fd==NULL) return NULL;
	raw_data=(char*)malloc(s.st_size+1);
	bzero((void*)raw_data,s.st_size+1);
	numbytes=fread((void*)raw_data,1,s.st_size,fd);
	if (numbytes<0)
	{
	        fclose(fd);
	        return NULL;
	}
	fclose(fd);
	new_obj = json_tokener_parse(raw_data);
	if (new_obj==NULL)
		return NULL;
	return json_parse(new_obj);
}

GArray* json_parse(json_object * jobj)
{
	GArray * hostgrouparray;
	Host* head=NULL;
	int i;
	struct json_object * new_obj ,* inner_jobj,*data_jobj,*label_obj;
	enum json_type type;
	HostGroup* hostgroup;
	hostgrouparray = g_array_new (TRUE, FALSE, sizeof (HostGroup*));
	json_object_object_foreach(jobj, key, val)
	{
		type = json_object_get_type(val);
		switch (type)
		{
			/*case json_type_string:  
						if (!strncmp(key,"label",strlen("label")))
						{
					        char* title=(char*)json_object_get_string(json_object_object_get(jobj, key));
					        hostgroup = malloc(sizeof(HostGroup));
					        bzero((void*)hostgroup,sizeof(HostGroup));
					        sethostgroupcounterid(hostgroup);
					        hostgroup->title=g_strdup((const gchar*)title);
					        hostgroup->label=TRUE;
					        g_array_append_val (hostgrouparray, hostgroup);
						}
						
						break;*/
			case json_type_array: 
						hostgroup = malloc(sizeof(HostGroup));
						bzero((void*)hostgroup,sizeof(HostGroup));
						sethostgroupcounterid(hostgroup);
						head=NULL;
						json_object_object_get_ex(jobj, "data",&data_jobj);
						int arraylen = json_object_array_length(data_jobj);
							
						for (i=0;i<arraylen;i++)
						{			
							Host* newhost = create_host_linkedlist(data_jobj,i);
							if (newhost!=NULL)
								head=host_queue(head,newhost);
						}
						hostgroup->hostarray=head;
						g_array_append_val (hostgrouparray, hostgroup);

						break;
			
			case json_type_object:  
						hostgroup = malloc(sizeof(HostGroup));
						bzero((void*)hostgroup,sizeof(HostGroup));
						hostgroup->title=g_strdup(key);
						json_object_object_get_ex(jobj, key,&inner_jobj);
						head=NULL;
						json_object_object_foreach(inner_jobj, key, val)
						{
							if (!strcmp(key,"label"))
							{
								struct json_object * lab_obj;
								json_object_object_get_ex(inner_jobj, key,&lab_obj);
								char* title=(char*)json_object_get_string(lab_obj);
								HostGroup* labelhostgroup = malloc(sizeof(HostGroup));
								bzero((void*)labelhostgroup,sizeof(HostGroup));
								sethostgroupcounterid(labelhostgroup);
								labelhostgroup->title=g_strdup((const gchar*)title);
								labelhostgroup->label=TRUE;
								g_array_append_val (hostgrouparray, labelhostgroup);
							}
							else
							{
								json_object_object_get_ex(inner_jobj, "data",&data_jobj);
								int arraylen = json_object_array_length(data_jobj);
								for (i=0;i<arraylen;i++)
								{
									Host* new_host = create_host_linkedlist(data_jobj,i);
									if (new_host!=NULL)
										head=host_queue(head,new_host);
								}
								
								// Set the open all row
								Host* all_host = (Host*) malloc(sizeof(Host));
								bzero((void*)all_host,sizeof(Host));
								sethostcounterid(all_host);
								all_host->menu_name=g_strdup((char*)"Open all");
								all_host->group_head=head;
								head=host_queue(head,all_host);
							}
						}
						
						hostgroup->hostarray=head;
						g_array_append_val (hostgrouparray, hostgroup);
						break;
					}
					
	}
	return hostgrouparray;
}

// This function parses a "data" json array
Host* create_host_linkedlist(struct json_object * data_jobj,int i)
{
	json_object * jvalue;
	jvalue = json_object_array_get_idx(data_jobj, i);
	
	// check if it's a label
	struct json_object * new_obj_label;
	json_object_object_get_ex(jvalue, "label",&new_obj_label);
	if (new_obj_label!=NULL)
	{
		Host* newhost = malloc(sizeof(Host));
		bzero((void*)newhost,sizeof(Host));
		sethostcounterid(newhost);
		newhost->menu_name=strdup((char*)json_object_get_string(new_obj_label));
		newhost->label=TRUE;
		return newhost;
	}
	
	// regular host entry 										
	struct json_object * new_obj_protocol;
	json_object_object_get_ex(jvalue, "protocol",&new_obj_protocol);	
	struct json_object * new_obj_hostname;
	json_object_object_get_ex(jvalue, "hostname",&new_obj_hostname);
	struct json_object * new_obj_login;
	json_object_object_get_ex(jvalue, "login",&new_obj_login);
	struct json_object * new_obj_menu_name;
	json_object_object_get_ex(jvalue, "menu_name",&new_obj_menu_name);
	struct json_object * new_obj_tab_name;
	json_object_object_get_ex(jvalue, "tab_name",&new_obj_tab_name);
	struct json_object * new_obj_cmd_name;
	json_object_object_get_ex(jvalue, "command_after_login",&new_obj_cmd_name);
	struct json_object * new_obj_remote_command;
	json_object_object_get_ex(jvalue, "remote_command",&new_obj_remote_command);
								
	/*printf("##%s##\n",json_object_get_string(new_obj_hostname));
	printf("##%s##\n",json_object_get_string(new_obj_login));
	printf("##%s##\n",json_object_get_string(new_obj_menu_name));
	printf("##%s##\n",json_object_get_string(new_obj_tab_name));*/
	//printf("caaa%s\n",json_object_get_string(new_obj));
	// These are all mandatory fields
								
	if  (
		(char*)json_object_get_string(new_obj_hostname)==NULL 	  || 
		(char*)json_object_get_string(new_obj_login)	==NULL	  ||
		(char*)json_object_get_string(new_obj_menu_name)==NULL	  ||
		(char*)json_object_get_string(new_obj_tab_name)==NULL
	)
		return NULL;
															
	Host* newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	sethostcounterid(newhost);
	const char* protocol = json_object_get_string(new_obj_protocol);
	if (protocol == NULL || (strcmp(protocol,"ssh") && strcmp(protocol,"telnet")) )
		newhost->protocol=g_strdup("ssh");
	else
		newhost->protocol=g_strdup(protocol);
	newhost->hostname=strdup((char*)json_object_get_string(new_obj_hostname));
	newhost->login=strdup((char*)json_object_get_string(new_obj_login));
	newhost->menu_name=strdup((char*)json_object_get_string(new_obj_menu_name));
	newhost->tab_name=strdup((char*)json_object_get_string(new_obj_tab_name));
	if ((char*)json_object_get_string(new_obj_cmd_name))
		newhost->command_after_login=strdup((char*)json_object_get_string(new_obj_cmd_name));
	if (new_obj_remote_command!=NULL)
		newhost->remote_command=strdup((char*)json_object_get_string(new_obj_remote_command));
	return newhost;
}

// This function puts a new host and the end of a queue
Host* host_queue(Host* head,Host* newhost)
{
	Host* ptr;
	if (head==NULL)
		return newhost;
	
	ptr=head;
	while (ptr->next)
		ptr=ptr->next;
	ptr->next=newhost;
	return head;
}

// Create a directory for hosting a new configuration file if it doesn't exist
char* checkandcreatedefaultdir()
{
	char* fulldirpath;
	struct stat s;
		
	asprintf(&fulldirpath,"%s/%s",getenv("HOME"),GUAKE_INDICATOR_DEFAULT_DIR);
	int err = stat(fulldirpath, &s);
	if(ENOENT == err)
	{
		if (mkdir(fulldirpath,0744)==-1)
			fprintf(stderr,"Can't create %s (%d - %s)",GUAKE_INDICATOR_DEFAULT_DIR,errno,strerror(errno));
	}
	else
	{
		if(!S_ISDIR(s.st_mode))
		{
			if (mkdir(fulldirpath,0744))
				fprintf(stderr,"Can't create %s (%d - %s)",GUAKE_INDICATOR_DEFAULT_DIR,errno,strerror(errno));
		}
	}
	return fulldirpath;
}

// Create a new default cfg file
int createdefaultfilecfg(const char* path)
{
	FILE* fd;
	char* user=getenv("LOGNAME");
	fd=fopen(path,"w");
	if (fd==NULL) return -1;
	fprintf(fd,\
		"{\n"
		"	\"data\": [\n"
		"	{\"label\" : \"SSH Hosts\"},\n"
		"	{\n"
		"		\"hostname\": \"localhost\",\n"
		"		\"login\": \"%s\",\n"
		"		\"menu_name\": \"SSH on localhost\",\n"
		"		\"tab_name\": \"Localhost\"\n"
		"    }\n"
		"	],\n"
		"	\"MYSQL\": {\n"
		"	\"label\" : \"Mysql servers\",\n"
		"    	\"data\": [\n"
		"		{\"label\" : \"Local mysql servers\"},\n"
		"		{\n"
		"			\"hostname\": \"localhost\",\n"
		"			\"login\": \"%s\",\n"
		"			\"menu_name\": \"Mysql on localhost\",\n"
		"			\"tab_name\": \"Mysql\",\n"
		"			\"command_after_login\" : \"mysql -u root --pass --host localhost -A mysql\",\n"
		"			\"remote_command\":\"yes\"\n"
		"        },\n"
		"    	{\n"
		"			\"hostname\": \"127.0.0.1\",\n"
		"			\"login\": \"%s\",\n"
		"			\"menu_name\": \"Mysql on 127.0.0.1\",\n"
		"			\"tab_name\": \"Mysql on 127.0.0.1\",\n"
		"			\"command_after_login\" : \"mysql -u root --pass --host 127.0.0.1 -A mysql\",\n"
		"			\"remote_command\":\"yes\"\n"
		"        },\n"
		"		{\"label\" : \"Remote mysql servers\"},\n"
		"		{\n"
		"			\"hostname\": \"192.168.1.1\",\n"
		"			\"login\": \"%s\",\n"
		"			\"menu_name\": \"Mysql on 192.168.1.1\",\n"
		"			\"tab_name\": \"Mysql\",\n"
		"			\"command_after_login\" : \"mysql -u root --pass --host 192.168.1.1 -A mysql\",\n"
		"			\"remote_command\":\"yes\"\n"
		"        }\n"
		"    	]\n"
		"	},\n"
		"	\"Label 3\" : \"Telnet Hosts\",\n"
		"	\"Telnet hosts\": {\n"
		"    	\"data\": [\n"
		"    	{\n"
		"			\"protocol\": \"telnet\",\n"
		"			\"hostname\": \"localhost\",\n"
		"			\"login\": \"%s\",\n"
		"			\"menu_name\": \"Telnet on localhost\",\n"
		"			\"tab_name\": \"Telnet\"\n"
		"    	}\n"
		"    ]\n"
		"    },\n"
		"	\"Localhost tasks\":\n"
		"	{\n"
		"		\"label\" : \"MISC\",\n"
		"		\"data\" : [\n"
		"		{\n"
		"			\"hostname\": \"\",\n"
		"			\"login\": \"\",\n"
		"			\"menu_name\": \"tmp folder\",\n"
		"			\"tab_name\": \"tmp folder\",\n"
		"			\"command_after_login\" : \"cd /tmp\"\n"
		"		},\n"
		"		{\n"
		"			\"hostname\": \"\",\n"
		"			\"login\": \"\",\n"
		"			\"menu_name\": \"top\",\n"
		"			\"tab_name\": \"top\",\n"
		"			\"command_after_login\" : \"top\"\n"
		"		},\n"
		"		{\n"
		"			\"hostname\": \"\",\n"
		"			\"login\": \"\",\n"
		"			\"menu_name\": \"ping www.google.com\",\n"
		"			\"tab_name\": \"ping google\",\n"
		"			\"command_after_login\" : \"ping www.google.com\"\n"
		"		}]\n"
		"	}\n"
		"}\n"\
		,user,user,user,user,user);
		fclose(fd);
		return(0);
}
void sethostcounterid(Host* host)
{
	static int counter=0;
	host->id = g_strdup_printf("host%d", counter++);
}
void sethostgroupcounterid(HostGroup* hostgroup)
{
	static int counter=0;
	hostgroup->id = g_strdup_printf("hostgroup%d", counter++);
}
