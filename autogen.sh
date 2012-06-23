libtoolize
aclocal -I m4
intltoolize --force
autoheader
automake --add-missing --copy --force-missing --gnu
autoconf --force
