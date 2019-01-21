#include <common/utils.h>
#include <php7/utils.h>

#include "html5_dom_exception.h"
#include "html5_dom_css_selector.h"

#include "zend_exceptions.h"

/*
	Class HTML5\DOM\CSS\Selector
*/

static zend_object_handlers php_html5_dom_css_selector_handlers;
static HashTable php_html5_dom_css_selector_prop_handlers;

static zend_function_entry php_html5_dom_css_selector_methods[] = {
	PHP_ME(Selector, __construct,			NULL,	ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_FE_END
};

void html5_dom_css_selector_class_init() {
	zend_class_entry ce;
	memcpy(&php_html5_dom_css_selector_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_html5_dom_css_selector_handlers.offset				= XtOffsetOf(html5_dom_object_wrap, std);
	php_html5_dom_css_selector_handlers.free_obj				= html5_dom_css_selector_free_obj;
	php_html5_dom_css_selector_handlers.clone_obj			= NULL;
	
	php_html5_dom_css_selector_handlers.read_property		= html5_dom_read_property;
	php_html5_dom_css_selector_handlers.write_property		= html5_dom_write_property;
	php_html5_dom_css_selector_handlers.get_property_ptr_ptr	= html5_dom_get_property_ptr_ptr;
	php_html5_dom_css_selector_handlers.has_property			= html5_dom_has_property;
	php_html5_dom_css_selector_handlers.get_debug_info		= html5_dom_get_debug_info;
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\CSS\\Selector", php_html5_dom_css_selector_methods);
	ce.create_object = html5_dom_css_selector_create_object;
	html5_dom_css_selector_ce = zend_register_internal_class(&ce);
	
	html5_dom_prop_handler_list handlers[] = {
		{"",				NULL}, 
	};
	html5_dom_prop_handler_init(&php_html5_dom_css_selector_prop_handlers, handlers);
}

static zend_object *html5_dom_css_selector_create_object(zend_class_entry *ce TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_wrap_create(ce, &php_html5_dom_css_selector_handlers);
	
	intern->prop_handler = &php_html5_dom_css_selector_prop_handlers;
	
	DOM_GC_TRACE("DOM::CSS::Selector::new (refs=%d)", GC_REFCOUNT(&intern->std));
	
	return &intern->std;
}

static void html5_dom_css_selector_free_obj(zend_object *object TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_unwrap(object);
	
	DOM_GC_TRACE("DOM::CSS::Selector::DESTROY (refs=%d)", GC_REFCOUNT(&intern->std));
}

void html5_dom_css_selector_class_unload() {
	zend_hash_destroy(&php_html5_dom_css_selector_prop_handlers);
}

/*
	PHP methods
*/
PHP_METHOD(Selector, __construct) {
	
}