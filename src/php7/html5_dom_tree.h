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
static int html5_dom_tree__parser(html5_dom_object_wrap *obj, zval *val, int write);

PHP_METHOD(Tree, __construct);
PHP_METHOD(Tree, createElement);
PHP_METHOD(Tree, createComment);
PHP_METHOD(Tree, createTextNode);
PHP_METHOD(Tree, parseFragment);
PHP_METHOD(Tree, document); // field
PHP_METHOD(Tree, root); // field
PHP_METHOD(Tree, head); // field
PHP_METHOD(Tree, body); // field
PHP_METHOD(Tree, at);
PHP_METHOD(Tree, querySelector);
PHP_METHOD(Tree, find);
PHP_METHOD(Tree, querySelectorAll);
PHP_METHOD(Tree, findId);
PHP_METHOD(Tree, getElementById);
PHP_METHOD(Tree, findTag);
PHP_METHOD(Tree, getElementsByTagName);
PHP_METHOD(Tree, findClass);
PHP_METHOD(Tree, getElementsByClassName);
PHP_METHOD(Tree, findAttr);
PHP_METHOD(Tree, getElementByAttribute);
PHP_METHOD(Tree, encoding); // field
PHP_METHOD(Tree, encodingId); // field
PHP_METHOD(Tree, tag2id);
PHP_METHOD(Tree, id2tag);
PHP_METHOD(Tree, namespace2id);
PHP_METHOD(Tree, id2namespace);
PHP_METHOD(Tree, parser); // field

#endif // PHP_HTML5_DOM_TREE_H
