#ifndef PHP7_UTILS_H
#define PHP7_UTILS_H

#define DOM_GC_TRACE(msg, ...) fprintf(stderr, "[GC] " msg "\n", ##__VA_ARGS__);
// #define DOM_GC_TRACE(...)

#include "php.h"
#include <common/utils.h>

#define html5_dom_object_unwrap(obj) ((html5_dom_object_wrap *)((char *)(obj) - XtOffsetOf(html5_dom_object_wrap, std)));

#define HTML5_DOM_METHOD_PARAMS(T) \
	zval *self_php_object = getThis(); \
	html5_dom_object_wrap *self_object = html5_dom_object_unwrap(Z_OBJ_P(self_php_object)); \
	T *self = (T *) self_object->ptr;

typedef struct {
	void *ptr;					// custom payload
	HashTable *prop_handler;	// handlers for class properties
	zval parent;				// reference to parent object
	zend_object std;			// self object
} html5_dom_object_wrap;

typedef int (*html5_dom_prop_handler)(html5_dom_object_wrap *obj, zval *val, int write);

typedef struct {
	const char name[64];
	html5_dom_prop_handler func;
} html5_dom_prop_handler_list;

void html5_dom_node_to_zval(myhtml_tree_node_t *node, zval *retval);

void html5_dom_prop_handler_init(HashTable *hash, html5_dom_prop_handler_list *handlers);
void html5_dom_prop_handler_free(HashTable *hash);

zval *html5_dom_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot);
zval *html5_dom_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv);

#if PHP_VERSION_ID > 70400
// In php 7.4 write_property must return value
zval *html5_dom_write_property(zval *object, zval *member, zval *value, void **cache_slot);
#else
void html5_dom_write_property(zval *object, zval *member, zval *value, void **cache_slot);
#endif

int html5_dom_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot);

void *html5_dom_zend_object_alloc(size_t obj_size, zend_class_entry *ce);

html5_dom_object_wrap *html5_dom_object_wrap_create(zend_class_entry *ce, zend_object_handlers *handlers);
void html5_dom_object_wrap_free(html5_dom_object_wrap *object);

#endif // PHP7_UTILS_H
