#!/bin/sh
VERSION=`git describe`
echo "#define VERSION \"$VERSION\"" 2> /dev/null
