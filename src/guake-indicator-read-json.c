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
		free((void*)raw_data);
		fclose(fd);
		return NULL;
	}
	fclose(fd);
	new_obj = json_tokener_parse(raw_data);
	free((void*)raw_data);
	if (new_obj==NULL)
		return NULL;
	GArray* returnarray=json_parse(new_obj);
	json_object_put(new_obj);
	return returnarray;
}

GArray* json_parse(json_object * jobj)
{
	GArray * hostgrouparray;
	Host* head=NULL;
	int i;
	struct json_object * new_obj ,* inner_jobj,*data_jobj,*label_obj;
	enum json_type type;
	HostGroup* hostgroup;
	sethostcounterid(NULL); // reset counter id for single hosts
	sethostgroupcounterid(NULL); // reset counter id for host groups
	hostgrouparray = g_array_new (TRUE, FALSE, sizeof (HostGroup*));
	json_object_object_foreach(jobj, key, val)
	{
		type = json_object_get_type(val);
		switch (type)
		{
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
						sethostgroupcounterid(hostgroup);
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
								all_host->open_all=TRUE;
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
	struct json_object * new_obj_x_forwarded;
	json_object_object_get_ex(jvalue, "x_forwarded",&new_obj_x_forwarded);
	struct json_object * new_obj_dont_show_guake;
	json_object_object_get_ex(jvalue, "dont_show_guake",&new_obj_dont_show_guake);
								
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
	newhost->hostname=strdup((char*)json_object_get_string(new_obj_hostname));
	newhost->login=strdup((char*)json_object_get_string(new_obj_login));
	newhost->menu_name=strdup((char*)json_object_get_string(new_obj_menu_name));
	newhost->tab_name=strdup((char*)json_object_get_string(new_obj_tab_name));
	if ((char*)json_object_get_string(new_obj_cmd_name))
		newhost->command_after_login=strdup((char*)json_object_get_string(new_obj_cmd_name));
	if (new_obj_remote_command!=NULL)
		newhost->remote_command=strdup((char*)json_object_get_string(new_obj_remote_command));
	if (new_obj_x_forwarded!=NULL)
		newhost->x_forwarded=strdup((char*)json_object_get_string(new_obj_x_forwarded));
	if (new_obj_dont_show_guake!=NULL)
		newhost->dont_show_guake=strdup((char*)json_object_get_string(new_obj_dont_show_guake));
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
	char* user=getenv("LOGNAME");
	Host* head;
	HostGroup* hostgroup;
	GArray* hostgrouparray;
	
	// Start root section
	hostgrouparray = g_array_new (TRUE, FALSE, sizeof (HostGroup*));
	hostgroup = malloc(sizeof(HostGroup));
	bzero((void*)hostgroup,sizeof(HostGroup));
	sethostgroupcounterid(hostgroup);
	
	Host* newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	sethostcounterid(newhost);
	newhost->label=TRUE;
	newhost->menu_name=g_strdup("SSH Hosts");
	head=host_queue(head,newhost);
	
	newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	sethostcounterid(newhost);
	newhost->hostname=g_strdup("localhost");
	newhost->login=g_strdup(user!=NULL?user:"");
	newhost->menu_name=g_strdup("SSH on localhost");
	newhost->tab_name=g_strdup("LocalHost");
	head=host_queue(head,newhost);
	
	hostgroup->hostarray=head;
	g_array_append_val (hostgrouparray, hostgroup);
	//End root section
	
	// Start mysql server section
	head=NULL;
	hostgroup = malloc(sizeof(HostGroup));
	bzero((void*)hostgroup,sizeof(HostGroup));
	sethostgroupcounterid(hostgroup);
	hostgroup->title=g_strdup("Mysql Servers");
	hostgroup->label=TRUE;
	g_array_append_val (hostgrouparray, hostgroup);
	
	hostgroup = malloc(sizeof(HostGroup));
	bzero((void*)hostgroup,sizeof(HostGroup));
	sethostgroupcounterid(hostgroup);
	hostgroup->title=g_strdup("MYSQL");
	
	newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	sethostcounterid(newhost);
	newhost->label=TRUE;
	newhost->hostname=g_strdup("");
	newhost->login=g_strdup("");
	newhost->tab_name=g_strdup("");
	newhost->menu_name=g_strdup("Local Mysql Servers");
	head=host_queue(head,newhost);
	
	newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	sethostcounterid(newhost);
	newhost->hostname=g_strdup("localhost");
	newhost->login=g_strdup(user!=NULL?user:"");
	newhost->menu_name=g_strdup("Mysql on localhost");
	newhost->tab_name=g_strdup("Mysql");
	newhost->command_after_login=g_strdup("mysql -u root --pass --host localhost -A mysql");
	newhost->remote_command=g_strdup("yes");
	head=host_queue(head,newhost);
	
	newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	sethostcounterid(newhost);
	newhost->hostname=g_strdup("127.0.0.1");
	newhost->login=g_strdup(user!=NULL?user:"");
	newhost->menu_name=g_strdup("Mysql on 127.0.0.1");
	newhost->tab_name=g_strdup("Mysql on 127.0.0.1");
	newhost->command_after_login=g_strdup("mysql -u root --pass --host 127.0.0.1 -A mysql");
	newhost->remote_command=g_strdup("yes");
	head=host_queue(head,newhost);
	
	newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	sethostcounterid(newhost);
	newhost->label=TRUE;
	newhost->hostname=g_strdup("");
	newhost->login=g_strdup("");
	newhost->tab_name=g_strdup("");
	newhost->menu_name=g_strdup("Remote Mysql Servers");
	head=host_queue(head,newhost);
	
	newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	sethostcounterid(newhost);
	newhost->hostname=g_strdup("192.168.1.1");
	newhost->login=g_strdup(user!=NULL?user:"");
	newhost->menu_name=g_strdup("Mysql on 192.168.1.1");
	newhost->tab_name=g_strdup("Mysql on 192.168.1.1");
	newhost->command_after_login=g_strdup("mysql -u root --pass --host 192.168.1.1 -A mysql");
	newhost->remote_command=g_strdup("yes");
	head=host_queue(head,newhost);
	
	hostgroup->hostarray=head;
	g_array_append_val (hostgrouparray, hostgroup);
	// End of mysql server section
	
	//Start of localhost tasks
	head=NULL;
	hostgroup = malloc(sizeof(HostGroup));
	bzero((void*)hostgroup,sizeof(HostGroup));
	sethostgroupcounterid(hostgroup);
	hostgroup->title=g_strdup("Localhost tasks");
	hostgroup->label=TRUE;
	g_array_append_val (hostgrouparray, hostgroup);
	
	hostgroup = malloc(sizeof(HostGroup));
	bzero((void*)hostgroup,sizeof(HostGroup));
	sethostgroupcounterid(hostgroup);
	hostgroup->title=g_strdup("MISC");
	
	newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	sethostcounterid(newhost);
	newhost->hostname=g_strdup("");
	newhost->login=g_strdup("");
	newhost->menu_name=g_strdup("tmp folder");
	newhost->tab_name=g_strdup("tmp folder");
	newhost->command_after_login=g_strdup("cd /tmp");
	head=host_queue(head,newhost);
	
	newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	sethostcounterid(newhost);
	newhost->hostname=g_strdup("");
	newhost->login=g_strdup("");
	newhost->menu_name=g_strdup("top");
	newhost->tab_name=g_strdup("top");
	newhost->command_after_login=g_strdup("top");
	head=host_queue(head,newhost);
	
	newhost = malloc(sizeof(Host));
	bzero((void*)newhost,sizeof(Host));
	sethostcounterid(newhost);
	newhost->hostname=g_strdup("");
	newhost->login=g_strdup("");
	newhost->menu_name=g_strdup("ping www.google.com");
	newhost->tab_name=g_strdup("Ping google");
	newhost->command_after_login=g_strdup("ping www.google.com");
	head=host_queue(head,newhost);
	
	hostgroup->hostarray=head;
	g_array_append_val (hostgrouparray, hostgroup);
	// End of localhost tasks
	
	return write_cfg_file(hostgrouparray);
	
	return 0;
}
void sethostcounterid(Host* host)
{
	static int counter=0;
	if (host==NULL)
	{
		counter=0;
		return ;
	}
	host->id = g_strdup_printf("host%d", counter++);
}
void sethostgroupcounterid(HostGroup* hostgroup)
{
	static int counter=0;
	if (hostgroup==NULL)
	{
		counter=0;
		return;
	}
	hostgroup->id = g_strdup_printf("hostgroup%d", counter++);
}
