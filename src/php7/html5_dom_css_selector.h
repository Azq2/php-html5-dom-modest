#ifndef PHP_HTML5_DOM_CSS_SELECTOR_H
#define PHP_HTML5_DOM_CSS_SELECTOR_H

#include "php.h"
#include "html5_dom.h"

zend_class_entry *html5_dom_css_selector_ce;

void html5_dom_css_selector_class_init();
void html5_dom_css_selector_class_unload();

static void html5_dom_css_selector_free_obj(zend_object *object TSRMLS_DC);
static zend_object *html5_dom_css_selector_create_object(zend_class_entry *ce TSRMLS_DC);

PHP_METHOD(Selector, __construct);

#endif // PHP_HTML5_DOM_CSS_SELECTOR_H
