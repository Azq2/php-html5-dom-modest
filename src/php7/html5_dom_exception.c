#include <common/utils.h>
#include <php7/utils.h>

#include "zend_exceptions.h"
#include "html5_dom_exception.h"

/*
	Class HTML5\DOM\Exception
*/

void html5_dom_exception_class_init() {
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Exception", NULL);
	html5_dom_exception_ce = zend_register_internal_class_ex(&ce, zend_ce_exception);
}

void html5_dom_exception_class_unload() {
	// Nothing todo...
}
