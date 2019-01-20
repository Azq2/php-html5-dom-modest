#ifndef PHP_HTML5_DOM_TREE_H
#define PHP_HTML5_DOM_TREE_H

#include "php.h"
#include "html5_dom.h"

zend_class_entry *html5_dom_tree_ce;

void html5_dom_tree_class_init();
void html5_dom_tree_class_unload();

static void html5_dom_tree_free_obj(zend_object *object TSRMLS_DC);
static zend_object *html5_dom_tree_create_object(zend_class_entry *ce TSRMLS_DC);

PHP_METHOD(DOM, __construct);
PHP_METHOD(DOM, parse);
PHP_METHOD(DOM, parseChunkStart);
PHP_METHOD(DOM, parseChunk);
PHP_METHOD(DOM, parseChunkTree);
PHP_METHOD(DOM, parseChunkEnd);
PHP_METHOD(DOM, parseAsync);

#endif // PHP_HTML5_DOM_TREE_H
