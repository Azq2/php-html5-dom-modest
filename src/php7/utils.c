#include "utils.h"

void *html5_dom_zend_object_alloc(size_t obj_size, zend_class_entry *ce) {
	void *obj = emalloc(obj_size + zend_object_properties_size(ce));
	/* Subtraction of sizeof(zval) is necessary, because zend_object_properties_size() may be
	 * -sizeof(zval), if the object has no properties. */
	memset(obj, 0, obj_size - sizeof(zval));
	return obj;
}

html5_dom_object_wrap *html5_dom_object_wrap_create(zend_class_entry *ce, zend_object_handlers *handlers) {
	html5_dom_object_wrap *intern = html5_dom_zend_object_alloc(sizeof(html5_dom_object_wrap), ce);
	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->std.handlers = handlers;
	return intern;
}

void html5_dom_object_wrap_free(html5_dom_object_wrap *object) {
	zend_object_std_dtor(&object->std);
}
