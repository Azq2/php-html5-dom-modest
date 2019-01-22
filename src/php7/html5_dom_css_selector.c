#include <common/utils.h>
#include <php7/utils.h>

#include "html5_dom_exception.h"
#include "html5_dom_css_selector.h"
#include "html5_dom_css_selector_entry.h"

#include "zend_exceptions.h"
#include "zend_interfaces.h"

#define HTML5_DOM_FIELD_METHOD_RO(___method, ___field) \
	PHP_METHOD(Selector, ___method) { \
		HTML5_DOM_METHOD_PARAMS(html5_css_selector_t); \
		html5_dom_css_selector__##___field(self_object, return_value, 0); \
	}

/*
	Class HTML5\DOM\CSS\Selector
*/

static zend_object_handlers php_html5_dom_css_selector_handlers;
static HashTable php_html5_dom_css_selector_prop_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_css_selector_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_css_selector_entry, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

static zend_function_entry php_html5_dom_css_selector_methods[] = {
	PHP_ME(Selector, __construct,	arginfo_css_selector_void,			ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(Selector, text,			arginfo_css_selector_void,			ZEND_ACC_PRIVATE)
	PHP_ME(Selector, valid,			arginfo_css_selector_void,			ZEND_ACC_PRIVATE)
	PHP_ME(Selector, length,		arginfo_css_selector_void,			ZEND_ACC_PRIVATE)
	PHP_ME(Selector, entry,			arginfo_css_selector_entry,			ZEND_ACC_PRIVATE)
	
	// aliases
	PHP_MALIAS(Selector,	count,		length,				arginfo_css_selector_void,			ZEND_ACC_PUBLIC)
	
	PHP_FE_END
};

void html5_dom_css_selector_class_init() {
	zend_class_entry ce;
	memcpy(&php_html5_dom_css_selector_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_html5_dom_css_selector_handlers.offset				= XtOffsetOf(html5_dom_object_wrap, std);
	php_html5_dom_css_selector_handlers.free_obj			= html5_dom_css_selector_free_obj;
	php_html5_dom_css_selector_handlers.clone_obj			= NULL;
	
	php_html5_dom_css_selector_handlers.read_property			= html5_dom_read_property;
	php_html5_dom_css_selector_handlers.write_property			= html5_dom_write_property;
	php_html5_dom_css_selector_handlers.get_property_ptr_ptr	= html5_dom_get_property_ptr_ptr;
	php_html5_dom_css_selector_handlers.has_property			= html5_dom_has_property;
	php_html5_dom_css_selector_handlers.get_debug_info			= html5_dom_get_debug_info;
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\CSS\\Selector", php_html5_dom_css_selector_methods);
	ce.create_object = html5_dom_css_selector_create_object;
	html5_dom_css_selector_ce = zend_register_internal_class(&ce);
	
	zend_class_implements(html5_dom_css_selector_ce, 1, zend_ce_countable);
	
	html5_dom_prop_handler_list handlers[] = {
		{"text",		html5_dom_css_selector__text}, 
		{"valid",		html5_dom_css_selector__valid}, 
		{"length",		html5_dom_css_selector__length}, 
		{"entries",		html5_dom_css_selector__entries}, 
		{"",			NULL}, 
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
	
	if (intern->ptr) {
		html5_css_selector_t *self = (html5_css_selector_t *) intern->ptr;
		
		if (self->list)
			mycss_selectors_list_destroy(mycss_entry_selectors(self->parser->entry), self->list, true);
		
		efree(self);
		
		zval_ptr_dtor(&intern->parent);
	}
	
	html5_dom_object_wrap_free(intern);
}

void html5_dom_css_selector_class_unload() {
	zend_hash_destroy(&php_html5_dom_css_selector_prop_handlers);
}

static void html5_dom_selector_entry_to_zval(mycss_selectors_entries_list_t *list, html5_css_selector_t *selector, html5_dom_object_wrap *parent_intern, zval *retval) {
	html5_css_selector_entry_t *entry = (html5_css_selector_entry_t *) emalloc(sizeof(html5_css_selector_entry_t));
	entry->selector = selector;
	entry->list = list;
	
	// new HTML5\DOM\CSS\Selector
	object_init_ex(retval, html5_dom_css_selector_entry_ce);
	html5_dom_object_wrap *intern = html5_dom_object_unwrap(Z_OBJ_P(retval));
	intern->ptr = entry;
	
	// Save reference to parent
	// That would not deleted tree object while at least one node object live
	GC_REFCOUNT(&parent_intern->std)++;
	ZVAL_OBJ(&intern->parent, &parent_intern->std);
}

/*
	Serialization to AST
*/


/*
	PHP methods
*/
PHP_METHOD(Selector, __construct) {
	
}

PHP_METHOD(Selector, ast) {
	
}

PHP_METHOD(Selector, entry) {
	HTML5_DOM_METHOD_PARAMS(html5_css_selector_t);
	
	zend_long index;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) != SUCCESS)
		WRONG_PARAM_COUNT;
	
	if (!self->list || index < 0 || index >= self->list->entries_list_length) {
		RETURN_NULL();
	} else {
		html5_dom_selector_entry_to_zval(&self->list->entries_list[index], self, self_object, return_value);
	}
}

HTML5_DOM_FIELD_METHOD_RO(length, length);
HTML5_DOM_FIELD_METHOD_RO(text, text);
HTML5_DOM_FIELD_METHOD_RO(valid, valid);
HTML5_DOM_FIELD_METHOD_RO(entries, entries);

/*
	Acessors
*/

static int html5_dom_css_selector__text(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_css_selector_t *self = (html5_css_selector_t *) obj->ptr;
	
	if (!write) {
		if (self->list) {
			smart_str buf = {0};
			mycss_selectors_serialization_list(mycss_entry_selectors(self->parser->entry), self->list, html5_dom_serialization_callback, &buf);
			
			if (buf.s) {
				ZSTR_VAL(buf.s)[ZSTR_LEN(buf.s)] = '\0';
				ZVAL_NEW_STR(val, buf.s);
			} else {
				ZVAL_EMPTY_STRING(val);
			}
		} else {
			ZVAL_NULL(val);
		}
		return 1;
	}
	
	return 0;
}

static int html5_dom_css_selector__valid(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_css_selector_t *self = (html5_css_selector_t *) obj->ptr;
	
	if (!write) {
		ZVAL_BOOL(val, self->list ? !(self->list->flags & MyCSS_SELECTORS_FLAGS_SELECTOR_BAD) : 0);
		return 1;
	}
	
	return 0;
}

static int html5_dom_css_selector__length(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_css_selector_t *self = (html5_css_selector_t *) obj->ptr;
	
	if (!write) {
		ZVAL_LONG(val, self->list ? self->list->entries_list_length : 0);
		return 1;
	}
	
	return 0;
}

static int html5_dom_css_selector__entries(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_css_selector_t *self = (html5_css_selector_t *) obj->ptr;
	
	if (!write) {
		array_init(val);
		
		if (self->list) {
			for (size_t i = 0; i < self->list->entries_list_length; ++i) {
				zval tmp;
				html5_dom_selector_entry_to_zval(&self->list->entries_list[i], self, obj, &tmp);
				add_index_zval(val, i, &tmp);
			}
		}
		
		return 1;
	}
	
	return 0;
}
