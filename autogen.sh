#!/bin/sh

bail_out ()
{
    echo $1 failed. Exiting.
    exit 1
}

# Create configure.scan, and then convert it into configure.in
echo autoscan...
autoscan

echo aclocal...
aclocal_args=
aclocal $aclocal_args || bail_out aclocal
echo autoconf...
autoconf || bail_out autoconf
#echo autoheader...
#autoheader || bail_out autoheader

# To run automake, we need Makefile.am
echo automake...
automake -a -c --foreign
