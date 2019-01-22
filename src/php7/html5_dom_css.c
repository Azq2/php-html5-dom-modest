#include <common/utils.h>
#include <php7/utils.h>

#include "html5_dom_exception.h"
#include "html5_dom_parser.h"
#include "html5_dom_css.h"
#include "html5_dom_css_selector.h"

#include "zend_exceptions.h"

/*
	Class HTML5\DOM\CSS
*/

static zend_object_handlers php_html5_dom_css_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_css_constructor, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_css_parse_selector, 0, 0, 1)
	ZEND_ARG_INFO(0, selector)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

static zend_function_entry php_html5_dom_css_methods[] = {
	PHP_ME(CSS, __construct,			arginfo_css_constructor,		ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(CSS, parseSelector,			arginfo_css_parse_selector,		ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void html5_dom_css_class_init() {
	zend_class_entry ce;
	memcpy(&php_html5_dom_css_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_html5_dom_css_handlers.offset				= XtOffsetOf(html5_dom_object_wrap, std);
	php_html5_dom_css_handlers.free_obj				= html5_dom_css_free_obj;
	php_html5_dom_css_handlers.clone_obj			= NULL;
	
	php_html5_dom_css_handlers.read_property		= html5_dom_read_property;
	php_html5_dom_css_handlers.write_property		= html5_dom_write_property;
	php_html5_dom_css_handlers.get_property_ptr_ptr	= html5_dom_get_property_ptr_ptr;
	php_html5_dom_css_handlers.has_property			= html5_dom_has_property;
	php_html5_dom_css_handlers.get_debug_info		= html5_dom_get_debug_info;
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\CSS", php_html5_dom_css_methods);
	ce.create_object = html5_dom_css_create_object;
	html5_dom_css_ce = zend_register_internal_class(&ce);
}

static zend_object *html5_dom_css_create_object(zend_class_entry *ce TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_wrap_create(ce, &php_html5_dom_css_handlers);
	
	DOM_GC_TRACE("DOM::CSS::new (refs=%d)", GC_REFCOUNT(&intern->std));
	
	return &intern->std;
}

static void html5_dom_css_free_obj(zend_object *object TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_unwrap(object);
	html5_css_parser_t *self = (html5_css_parser_t *) intern->ptr;
	
	DOM_GC_TRACE("DOM::CSS::DESTROY (refs=%d)", GC_REFCOUNT(&intern->std));
	
	if (self) {
		mycss_entry_destroy(self->entry, 1);
		mycss_destroy(self->mycss, 1);
		efree(self);
	}
}

void html5_dom_css_class_unload() {
	// Nothing todo...
}

/*
	PHP methods
*/
PHP_METHOD(CSS, __construct) {
	HTML5_DOM_METHOD_PARAMS(html5_css_parser_t);
	
	zval *options = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a", &options) != SUCCESS)
		WRONG_PARAM_COUNT; 
	
	mystatus_t status;
	
	html5_dom_options_t opts = {0};
	html5_dom_parse_options(&opts, NULL, options);
	
	if (!html5_dom_check_options(&opts))
		RETURN_NULL();
	
	mycss_t *mycss = mycss_create();
	status = mycss_init(mycss);
	if (status) {
		mycss_destroy(mycss, 1);
		zend_throw_exception_ex(html5_dom_exception_ce, status, "mycss_init failed: %d (%s)", status, modest_strerror(status));
		RETURN_NULL();
	}
	
	mycss_entry_t *entry = mycss_entry_create();
	status = mycss_entry_init(mycss, entry);
	if (status) {
		mycss_destroy(mycss, 1);
		mycss_entry_destroy(entry, 1);
		zend_throw_exception_ex(html5_dom_exception_ce, status, "mycss_entry_init failed: %d (%s)", status, modest_strerror(status));
		RETURN_NULL();
	}
	
	self = (html5_css_parser_t *) emalloc(sizeof(html5_css_parser_t));
	self->mycss = mycss;
	self->entry = entry;
	self->encoding = MyENCODING_UTF_8;
	memcpy(&self->opts, &opts, sizeof(html5_dom_options_t));
	
	self_object->ptr = self;
}

PHP_METHOD(CSS, parseSelector) {
	HTML5_DOM_METHOD_PARAMS(html5_css_parser_t);
	
	zend_string *selector_text = NULL;
	zval *options = NULL;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|a", &selector_text, &options) != SUCCESS)
		WRONG_PARAM_COUNT;
	
	mystatus_t status;
	
	html5_dom_options_t opts;
	html5_dom_parse_options(&opts, &self->opts, options);
	if (!html5_dom_check_options(&opts))
		RETURN_NULL();
	
	mycss_selectors_list_t *list = mycss_selectors_parse(mycss_entry_selectors(self->entry), MyENCODING_UTF_8, selector_text->val, selector_text->len, &status);
	html5_css_selector_t *selector = (html5_css_selector_t *) emalloc(sizeof(html5_css_selector_t));
	selector->list = list;
	selector->parser = self;
	
	// new HTML5\DOM\CSS\Selector
	object_init_ex(return_value, html5_dom_css_selector_ce);
	html5_dom_object_wrap *intern = html5_dom_object_unwrap(Z_OBJ_P(return_value));
	intern->ptr = selector;
	
	// Save reference to parent
	// That would not deleted tree object while at least one node object live
	GC_REFCOUNT(&self_object->std)++;
	ZVAL_OBJ(&intern->parent, &self_object->std);
}
