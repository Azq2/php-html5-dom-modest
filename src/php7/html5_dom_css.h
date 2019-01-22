#ifndef PHP_HTML5_DOM_CSS_H
#define PHP_HTML5_DOM_CSS_H

#include "php.h"
#include "html5_dom.h"

zend_class_entry *html5_dom_css_ce;

void html5_dom_css_class_init();
void html5_dom_css_class_unload();

static void html5_dom_css_free_obj(zend_object *object TSRMLS_DC);
static zend_object *html5_dom_css_create_object(zend_class_entry *ce TSRMLS_DC);

PHP_METHOD(CSS, __construct);
PHP_METHOD(CSS, parseSelector);

#endif // PHP_HTML5_DOM_CSS_H
