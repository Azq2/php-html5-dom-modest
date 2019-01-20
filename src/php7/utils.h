#ifndef PHP7_UTILS_H
#define PHP7_UTILS_H

#define DOM_GC_TRACE(msg, ...) fprintf(stderr, "[GC] " msg "\n", ##__VA_ARGS__);
// #define DOM_GC_TRACE(...)

#include "php.h"

#define html5_dom_object_unwrap(obj) ((html5_dom_object_wrap *)((char *)(obj) - XtOffsetOf(html5_dom_object_wrap, std)));

#define HTML5_DOM_METHOD_PARAMS(T) \
	zval *self_php_object = getThis(); \
	html5_dom_object_wrap *self_object = html5_dom_object_unwrap(Z_OBJ_P(self_php_object)); \
	T *self = (T *) self_object->ptr;

typedef struct {
	void *ptr;
	zend_object std;
} html5_dom_object_wrap;

void *html5_dom_zend_object_alloc(size_t obj_size, zend_class_entry *ce);

html5_dom_object_wrap *html5_dom_object_wrap_create(zend_class_entry *ce, zend_object_handlers *handlers);
void html5_dom_object_wrap_free(html5_dom_object_wrap *object);

#endif // PHP7_UTILS_H
