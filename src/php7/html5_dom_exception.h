#ifndef PHP_HTML5_DOM_EXCEPTION_H
#define PHP_HTML5_DOM_EXCEPTION_H

#include "php.h"
#include "html5_dom.h"

zend_class_entry *html5_dom_exception_ce;

void html5_dom_exception_class_init();
void html5_dom_exception_class_unload();

#endif // PHP_HTML5_DOM_EXCEPTION_H
