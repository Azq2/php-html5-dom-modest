#ifndef PHP_HTML5_DOM_COLLECTION_H
#define PHP_HTML5_DOM_COLLECTION_H

#include "php.h"
#include "html5_dom.h"

zend_class_entry *html5_dom_collection_ce;

void html5_dom_collection_class_init();
void html5_dom_collection_class_unload();

static void html5_dom_collection_free_obj(zend_object *object TSRMLS_DC);
static zend_object *html5_dom_collection_create_object(zend_class_entry *ce TSRMLS_DC);

void html5_dom_collection_to_zval(myhtml_tree_t *tree, myhtml_collection_t *collection, zval *retval);

PHP_METHOD(Collection, __construct);

PHP_METHOD(Collection, array);
PHP_METHOD(Collection, first);
PHP_METHOD(Collection, last);

// Countable
PHP_METHOD(Collection, count);

// ArrayAccess
PHP_METHOD(Collection, offsetExists);
PHP_METHOD(Collection, offsetGet);
PHP_METHOD(Collection, offsetSet);
PHP_METHOD(Collection, offsetUnset);

// Iterator
PHP_METHOD(Collection, current);
PHP_METHOD(Collection, next);
PHP_METHOD(Collection, key);
PHP_METHOD(Collection, valid);
PHP_METHOD(Collection, rewind);

#endif // PHP_HTML5_DOM_COLLECTION_H
