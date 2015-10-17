#-----GPL----------------------------------------------------------------------
#
# This file is part of adcirc2hash
# Copyright (C) 2015  Zach Cobell
#
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#------------------------------------------------------------------------------
#
#  File: build.sh
#
#------------------------------------------------------------------------------

#...User set the location of qmake
#   by default, just grab what is in
#   the command line
qmake_local=$(which qmake)

if [ "x$1" == "x--gui" ] ; then
    if [ "x$2" == "xclean" ] ; then
        cd gui
        make distclean
    else
        cd gui
        $qmake_local adcirc2hash_gui.pro
        make
    fi
elif [ "x$1" == "x--cmd" ] ; then
    if [ "x$2" == "xclean" ] ; then
        cd cmd
        make distclean
    else
        cd cmd
        $qmake_local adcirc2hash_cmd.pro
        make
    fi
else
    echo "Build script for adcirc2hash"
    echo ""
    echo "USAGE: ./build.sh [OPTION]"
    echo ""
    echo "OPTIONS:"
    echo "   --cmd         build the command line version of the code."
    echo "   --cmd clean   clean the command line version of the code."
    echo ""
    echo "   --gui         build the gui version of the code."
    echo "   --gui clean   clean the gui version of the code."
    echo ""
    echo "   --help        display this message."
    echo ""
fi
