PHP_ARG_ENABLE(html5-dom, for html5-dom, 
	[AS_HELP_STRING([--enable-html5-dom], [enable html5-dom (default is no)])])

if test "${PHP_HTML5_DOM}" != "no"; then
	MODEST_CFLAGS=""
	
	case $host_alias in
		*freebsd*)
			MODEST_CFLAGS=" -DMyCORE_USE_SEMAPHORE_INSTEAD_OF_MUTEX ";;
		*netbsd*)
			MODEST_CFLAGS=" -DMyCORE_USE_SEMAPHORE_INSTEAD_OF_MUTEX ";;
		*openbsd*)
			MODEST_CFLAGS=" -DMyCORE_USE_SEMAPHORE_INSTEAD_OF_MUTEX ";;
	esac
	
	AC_DEFINE(HAVE_HTML5_DOM, 1, [ ])
	
	PHP_ADD_INCLUDE(src)
	PHP_ADD_INCLUDE(third_party/modest/include)
	
	PHP_NEW_EXTENSION(html5_dom, [
		src/php7/html5_dom.c \
		src/php7/html5_dom_parser.c \
		src/php7/html5_dom_tree.c \
		src/php7/html5_dom_node.c \
		src/php7/html5_dom_collection.c \
		src/php7/html5_dom_exception.c \
		src/php7/utils.c \
		src/common/utils.c \
		src/modest/modest_myurl.c \
		src/modest/modest_mycss.c \
		src/modest/modest_myencoding.c \
		src/modest/modest_mycore.c \
		src/modest/modest_myfont.c \
		src/modest/modest_modest.c \
		src/modest/modest_myhtml.c \
		src/modest/modest_myport.c \
	], $ext_shared)
	
	PHP_ADD_LIBRARY(pthread, 1, HTML5_DOM_SHARED_LIBADD)
	PHP_SUBST(HTML5_DOM_SHARED_LIBADD)
	PHP_SUBST(MODEST_CFLAGS)
fi
