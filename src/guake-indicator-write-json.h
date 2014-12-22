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

#include "json.h"

const char* write_json_cfg_file_to_str(GArray*);
json_object * build_json_data_obj(Host*);
json_object * build_json_label_obj(Host*);
int write_cfg_file(GArray*);
