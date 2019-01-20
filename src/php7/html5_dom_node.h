#ifndef PHP_HTML5_DOM_NODE_H
#define PHP_HTML5_DOM_NODE_H

#include "php.h"
#include "html5_dom.h"

zend_class_entry *html5_dom_node_ce;

void html5_dom_node_class_init();
void html5_dom_node_class_unload();

static void html5_dom_node_free_obj(zend_object *object TSRMLS_DC);
static zend_object *html5_dom_node_create_object(zend_class_entry *ce TSRMLS_DC);

void html5_dom_node_to_zval(myhtml_tree_node_t *node, zval *retval);

PHP_METHOD(Node, __construct);

#endif // PHP_HTML5_DOM_NODE_H
