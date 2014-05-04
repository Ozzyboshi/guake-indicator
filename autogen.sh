#!/bin/sh

PKG_NAME="guake-indicator"

which gnome-autogen.sh || {
	echo "You need gnome-common from GNOME Git"
	exit 1
}

USE_GNOME2_MACROS=1 \
. gnome-autogen.sh $@
