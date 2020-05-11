
[![Total alerts](https://img.shields.io/lgtm/alerts/g/Ozzyboshi/guake-indicator.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/Ozzyboshi/guake-indicator/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/Ozzyboshi/guake-indicator.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/Ozzyboshi/guake-indicator/context:cpp)

# Guake indicator
Guake indicator is a compact and convenient Appindicator that lets you send commands to [Guake](http://guake-project.org/)
 terminal.

Guake indicator is free software, you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation.

![guake indicator img](http://guake-indicator.ozzyboshi.com/images/out.gif)
![guake indicator img](http://guake-indicator.ozzyboshi.com/images/ubuntu18.png)

### Description of the project
Guake indicator is a compact and convenient Ubuntu Appindicator that lets you send commands and perform customized tasks to [Guake virtual terminal](http://guake-project.org/).You can specify the target tab or to create a new one at the same time.
Guake indicator comes with a convenient menu editor to create and manage your entries out of the box.

Initial versions of Guake indicator was meant for Ubuntu it but later versions are targeting the Debian Operating System with GTK based desktop managers.

From version 1.4, Guake indicator relies on Ayatana indicators and GTK3 rather than old libappindicator and GTK2.

Guake-indicator sticks to your "Indicator Area" and displays your customized command retrieved from ~/.guake.indicator/guake-indicator.xml. If this file doesn't exist, Guake Indicator will create a small default one that you can use as a base to build your own configuration file depending on your needs.

You can customize the configuration file using the Guake indicator built-in GUI under the "Edit Menu" section or, as an alternative, you can do it on your own with your favorite text/xml editor. The guake-indicator.xml fields are quite self-explanatory, however, I'm going to give you a more in-depth description of each one:

- menu_name :the name that will show up in the indicator itself
- tab_name : the name of the Guake terminal tab once it is opened (leave blank if you don't want automatic autorenaming)
- command_after_login : command to send to Guake through his DBUS interface
- dont_show_guake : if set to yes Guake is not shown after command execution
- open_in_tab : The index of the Guake tab where Guake indicator will execute the command. Guake assigns index 0 to the first tab so put in this field the nth tab-1 number. If this tag has the attribute named="yes" it contains the name of the Guake tab (see Guake indicator select tab by name issue for more informations) 
If this field contains the value '-1', Guake indicator will execute the command in the currently selected Guake tab (only from version 1.1)
- lfcr : if set to yes a LF (Line Feed) + CR (Carriage return) is sent to Guake after each line. Otherwise, only a line feed (ascii code 10) is sent.
- guakeindicatorscript : if set to yes enables the Guake indicator scripting function (see related scripting section)

### Requirements for Guake indicator
For proper compilation and functionality of Guake indicator, the following packages are REQUIRED:
- libayatana-appindicator3-dev
- libcairo2-dev (>= 1.10)
- libdbus-1-dev
- libdbus-glib-1-dev
- libglib2.0-dev (>=2.35.4)
- libgtk-3-dev (>=3.1.4)
- libjson-c-dev (for backward compatibility)
- libxml2-dev

### Credits
Guake indicator was conceived and initially created by Alessio Garzi <gun101@email.it>

Guake indicator was developed By Alessio Garzi <gun101@email.it> and Francesco Minà <girardengo@gmail.com>

Guake indicator debian package is mantained by Alessio Garzi <gun101@email.it>.

Debian webpage : https://packages.debian.org/sid/guake-indicator

We are looking for any type of contribution (code, bug fixing, documentation, translation or just new ideas), if you have an interest in Guake indicator please email me.
