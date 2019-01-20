#ifndef PHP_HTML5_DOM_TREE_H
#define PHP_HTML5_DOM_TREE_H

#include "php.h"
#include "html5_dom.h"

zend_class_entry *html5_dom_tree_ce;

void html5_dom_tree_class_init();
void html5_dom_tree_class_unload();

static void html5_dom_tree_free_obj(zend_object *object TSRMLS_DC);
static zend_object *html5_dom_tree_create_object(zend_class_entry *ce TSRMLS_DC);

static int html5_dom_tree__encoding(html5_dom_object_wrap *obj, zval *val, int write);
static int html5_dom_tree__encodingId(html5_dom_object_wrap *obj, zval *val, int write);
static int html5_dom_tree__document(html5_dom_object_wrap *obj, zval *val, int write);
static int html5_dom_tree__root(html5_dom_object_wrap *obj, zval *val, int write);
static int html5_dom_tree__head(html5_dom_object_wrap *obj, zval *val, int write);
static int html5_dom_tree__body(html5_dom_object_wrap *obj, zval *val, int write);
static int html5_dom_tree_parser(html5_dom_object_wrap *obj, zval *val, int write);

PHP_METHOD(DOM, __construct);
PHP_METHOD(DOM, createElement);
PHP_METHOD(DOM, createComment);
PHP_METHOD(DOM, createTextNode);
PHP_METHOD(DOM, parseFragment);
PHP_METHOD(DOM, document); // field
PHP_METHOD(DOM, root); // field
PHP_METHOD(DOM, head); // field
PHP_METHOD(DOM, body); // field
PHP_METHOD(DOM, at);
PHP_METHOD(DOM, querySelector);
PHP_METHOD(DOM, find);
PHP_METHOD(DOM, querySelectorAll);
PHP_METHOD(DOM, findId);
PHP_METHOD(DOM, getElementById);
PHP_METHOD(DOM, findTag);
PHP_METHOD(DOM, getElementsByTagName);
PHP_METHOD(DOM, findClass);
PHP_METHOD(DOM, getElementsByClassName);
PHP_METHOD(DOM, findAttr);
PHP_METHOD(DOM, getElementByAttribute);
PHP_METHOD(DOM, encoding); // field
PHP_METHOD(DOM, encodingId); // field
PHP_METHOD(DOM, tag2id);
PHP_METHOD(DOM, id2tag);
PHP_METHOD(DOM, namespace2id);
PHP_METHOD(DOM, id2namespace);
PHP_METHOD(DOM, parser); // field

#endif // PHP_HTML5_DOM_TREE_H
