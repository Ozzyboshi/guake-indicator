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

#ifndef _GUAKE_INDICATOR_XML_H
#define _GUAKE_INDICATOR_XML_H

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <strings.h>

#define GUAKE_INDICATOR_DEFAULT_FILEXML "guake-indicator.xml"
#define ENCODING "UTF-8"

Host* create_new_host(HostGroup*, Host* ,const gchar* ,const gchar* ,const gchar* ,gboolean ,const gchar*,gboolean ,gboolean,gboolean,gboolean,gboolean );
gboolean check_xml_cfg_file_presence();
int write_xml_cfg_file(GArray*);
int write_xml_cfg_file_from_file(GArray*,char*);
GArray* read_xml_cfg_file();
GArray* read_xml_cfg_file_from_file(char* );

#endif //_GUAKE_INDICATOR_XML_H