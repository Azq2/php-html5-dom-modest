#include <common/utils.h>
#include <php7/utils.h>

#include "html5_dom_exception.h"
#include "html5_dom_css_selector.h"
#include "html5_dom_css_selector_entry.h"

#include "zend_exceptions.h"

#define HTML5_DOM_FIELD_METHOD_RO(___method, ___field) \
	PHP_METHOD(Entry, ___method) { \
		HTML5_DOM_METHOD_PARAMS(html5_css_selector_entry_t); \
		html5_dom_css_selector_entry__##___field(self_object, return_value, 0, 0); \
	}

/*
	Class HTML5\DOM\CSS
*/

static zend_object_handlers php_html5_dom_css_selector_entry_handlers;
static HashTable php_html5_dom_css_selector_entry_prop_handlers;

static zend_function_entry php_html5_dom_css_selector_entry_methods[] = {
	PHP_ME(Entry, __construct,			NULL,	ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(Entry, text,					NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Entry, ast,					NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Entry, pseudoElement,		NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Entry, valid,				NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Entry, specificity,			NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Entry, specificityArray,		NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Entry, pseudoElement,		NULL,	ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void html5_dom_css_selector_entry_class_init() {
	zend_class_entry ce;
	memcpy(&php_html5_dom_css_selector_entry_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_html5_dom_css_selector_entry_handlers.offset				= XtOffsetOf(html5_dom_object_wrap, std);
	php_html5_dom_css_selector_entry_handlers.free_obj				= html5_dom_css_selector_entry_free_obj;
	php_html5_dom_css_selector_entry_handlers.clone_obj				= NULL;
	
	php_html5_dom_css_selector_entry_handlers.read_property			= html5_dom_read_property;
	php_html5_dom_css_selector_entry_handlers.write_property		= html5_dom_write_property;
	php_html5_dom_css_selector_entry_handlers.get_property_ptr_ptr	= html5_dom_get_property_ptr_ptr;
	php_html5_dom_css_selector_entry_handlers.has_property			= html5_dom_has_property;
	php_html5_dom_css_selector_entry_handlers.get_debug_info		= html5_dom_get_debug_info;
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\CSS\\Selector\\Entry", php_html5_dom_css_selector_entry_methods);
	ce.create_object = html5_dom_css_selector_entry_create_object;
	html5_dom_css_selector_entry_ce = zend_register_internal_class(&ce);
	
	html5_dom_prop_handler_list handlers[] = {
		{"text",				html5_dom_css_selector_entry__text}, 
		{"valid",				html5_dom_css_selector_entry__valid}, 
		{"specificity",			html5_dom_css_selector_entry__specificity}, 
		{"specificityArray",	html5_dom_css_selector_entry__specificityArray}, 
		{"pseudoElement",		html5_dom_css_selector_entry__pseudoElement}, 
		{"",					NULL}, 
	};
	html5_dom_prop_handler_init(&php_html5_dom_css_selector_entry_prop_handlers, handlers);
}

static zend_object *html5_dom_css_selector_entry_create_object(zend_class_entry *ce TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_wrap_create(ce, &php_html5_dom_css_selector_entry_handlers);
	
	intern->prop_handler = &php_html5_dom_css_selector_entry_prop_handlers;
	
	DOM_GC_TRACE("DOM::CSS::Selector::Entry::new (refs=%d)", GC_REFCOUNT(&intern->std));
	
	return &intern->std;
}

static void html5_dom_css_selector_entry_free_obj(zend_object *object TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_unwrap(object);
	
	DOM_GC_TRACE("DOM::CSS::Selector::Entry::DESTROY (refs=%d)", GC_REFCOUNT(&intern->std));
	
	if (intern->ptr) {
		efree(intern->ptr);
		zval_ptr_dtor(&intern->parent);
	}
	
	html5_dom_object_wrap_free(intern);
}

void html5_dom_css_selector_entry_class_unload() {
	zend_hash_destroy(&php_html5_dom_css_selector_entry_prop_handlers);
}

/*
	PHP methods
*/
PHP_METHOD(Entry, __construct) {
	
}

PHP_METHOD(Entry, ast) {
	HTML5_DOM_METHOD_PARAMS(html5_css_selector_entry_t);
	
	array_init(return_value);
	if (self->list)
		html5_dom_css_serialize_entry(self->selector, self->selector->list, self->list->entry, return_value);
}

HTML5_DOM_FIELD_METHOD_RO(text, text);
HTML5_DOM_FIELD_METHOD_RO(valid, valid);
HTML5_DOM_FIELD_METHOD_RO(specificity, specificity);
HTML5_DOM_FIELD_METHOD_RO(specificityArray, specificityArray);
HTML5_DOM_FIELD_METHOD_RO(pseudoElement, pseudoElement);

/*
	Acessors
*/
static int html5_dom_css_selector_entry__text(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	html5_css_selector_entry_t *self = (html5_css_selector_entry_t *) obj->ptr;
	
	if (!write) {
		if (self->list) {
			smart_str buf = {0};
			mycss_selectors_serialization_chain(mycss_entry_selectors(self->selector->parser->entry), self->list->entry, html5_dom_serialization_callback, &buf);
			
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

static int html5_dom_css_selector_entry__valid(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	html5_css_selector_entry_t *self = (html5_css_selector_entry_t *) obj->ptr;
	
	if (!write) {
		ZVAL_BOOL(val, !(self->selector->list->flags & MyCSS_SELECTORS_FLAGS_SELECTOR_BAD));
		return 1;
	}
	
	return 0;
}


static int html5_dom_css_selector_entry__specificity(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	html5_css_selector_entry_t *self = (html5_css_selector_entry_t *) obj->ptr;
	
	if (!write) {
		array_init(val);
		add_assoc_long_ex(val, "a", sizeof("a") - 1, self->list->specificity.a);
		add_assoc_long_ex(val, "b", sizeof("b") - 1, self->list->specificity.b);
		add_assoc_long_ex(val, "c", sizeof("c") - 1, self->list->specificity.c);
		return 1;
	}
	
	return 0;
}


static int html5_dom_css_selector_entry__specificityArray(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	html5_css_selector_entry_t *self = (html5_css_selector_entry_t *) obj->ptr;
	
	if (!write) {
		array_init(val);
		add_next_index_long(val, self->list->specificity.a);
		add_next_index_long(val, self->list->specificity.b);
		add_next_index_long(val, self->list->specificity.c);
		return 1;
	}
	
	return 0;
}

static int html5_dom_css_selector_entry__pseudoElement(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	html5_css_selector_entry_t *self = (html5_css_selector_entry_t *) obj->ptr;
	
	if (!write) {
		ZVAL_NULL(val);
		
		mycss_selectors_entry_t *entry = self->list->entry;
		while (entry) {
			if (entry->type == MyCSS_SELECTORS_TYPE_PSEUDO_ELEMENT) {
				ZVAL_STRINGL(val, entry->key->data ? entry->key->data : "", entry->key->length);
				break;
			}
			entry = entry->next;
		}
		
		return 1;
	}
	
	return 0;
}
