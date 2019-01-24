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
	PHP_ME(Selector, text,			arginfo_css_selector_void,			ZEND_ACC_PUBLIC)
	PHP_ME(Selector, ast,			arginfo_css_selector_void,			ZEND_ACC_PUBLIC)
	PHP_ME(Selector, valid,			arginfo_css_selector_void,			ZEND_ACC_PUBLIC)
	PHP_ME(Selector, length,		arginfo_css_selector_void,			ZEND_ACC_PUBLIC)
	PHP_ME(Selector, entry,			arginfo_css_selector_entry,			ZEND_ACC_PUBLIC)
	
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

void html5_dom_css_serialize_selector(html5_css_selector_t *self, mycss_selectors_list_t *selector, zval *result) {
	while (selector) {
		for (size_t i = 0; i < selector->entries_list_length; ++i) {
			mycss_selectors_entries_list_t *entries = &selector->entries_list[i];
			
			zval chain;
			array_init(&chain);
			html5_dom_css_serialize_entry(self, selector, entries->entry, &chain);
			add_next_index_zval(result, &chain);
			
		}
		selector = selector->next;
	}
}

void html5_dom_css_serialize_entry(html5_css_selector_t *self, mycss_selectors_list_t *selector, mycss_selectors_entry_t *entry, zval *result) {
	// combinators names
	static const struct {
		const char name[16];
		size_t len;
	} combinators[] = {
		{"", 0}, 
		{"descendant", 10},	// >>
		{"child", 5},		// >
		{"sibling", 7},		// +
		{"adjacent", 8},	// ~
		{"column", 6}		// ||
	};
	
	// attribute eq names
	static const struct {
		const char name[16];
		size_t len;
	} attr_match_names[] = {
		{"equal", 5},		// =
		{"include", 7},		// ~=
		{"dash", 4},		// |=
		{"prefix", 6},		// ^=
		{"suffix", 6},		// $=
		{"substring", 9}	// *=
	};
	
	while (entry) {
		if (entry->combinator != MyCSS_SELECTORS_COMBINATOR_UNDEF) {
			zval data;
			array_init(&data);
			add_assoc_stringl_ex(&data, "type", sizeof("type") - 1, "combinator", sizeof("combinator") - 1);
			add_assoc_stringl_ex(&data, "value", sizeof("value") - 1, combinators[entry->combinator].name, combinators[entry->combinator].len);
			add_next_index_zval(result, &data);
		}
		
		zval data;
		array_init(&data);
		
		if ((selector->flags) & MyCSS_SELECTORS_FLAGS_SELECTOR_BAD)
			add_assoc_bool_ex(&data, "invalid", sizeof("invalid") - 1, 1);
		
		switch (entry->type) {
			case MyCSS_SELECTORS_TYPE_ID:
			case MyCSS_SELECTORS_TYPE_CLASS:
			case MyCSS_SELECTORS_TYPE_ELEMENT:
			case MyCSS_SELECTORS_TYPE_PSEUDO_CLASS:
			case MyCSS_SELECTORS_TYPE_PSEUDO_ELEMENT:
			{
				switch (entry->type) {
					case MyCSS_SELECTORS_TYPE_ELEMENT:
						add_assoc_stringl_ex(&data, "type", sizeof("type") - 1, "tag", sizeof("tag") - 1);
					break;
					case MyCSS_SELECTORS_TYPE_ID:
						add_assoc_stringl_ex(&data, "type", sizeof("type") - 1, "id", sizeof("id") - 1);
					break;
					case MyCSS_SELECTORS_TYPE_CLASS:
						add_assoc_stringl_ex(&data, "type", sizeof("type") - 1, "class", sizeof("class") - 1);
					break;
					case MyCSS_SELECTORS_TYPE_PSEUDO_CLASS:
						add_assoc_stringl_ex(&data, "type", sizeof("type") - 1, "pseudo-class", sizeof("pseudo-class") - 1);
					break;
					case MyCSS_SELECTORS_TYPE_PSEUDO_ELEMENT:
						add_assoc_stringl_ex(&data, "type", sizeof("type") - 1, "pseudo-element", sizeof("pseudo-element") - 1);
					break;
				}
				
				if (entry->key)
					add_assoc_stringl_ex(&data, "value", sizeof("value") - 1, entry->key->data ? entry->key->data : "", entry->key->length);
			}
			break;
			case MyCSS_SELECTORS_TYPE_ATTRIBUTE:
			{
				add_assoc_stringl_ex(&data, "type", sizeof("type") - 1, "attribute", sizeof("attribute") - 1);
				
				/* key */
				if (entry->key)
					add_assoc_stringl_ex(&data, "name", sizeof("name") - 1, entry->key->data ? entry->key->data : "", entry->key->length);
				
				/* value */
				if (mycss_selector_value_attribute(entry->value)->value) {
					mycore_string_t *str_value = mycss_selector_value_attribute(entry->value)->value;
					add_assoc_stringl_ex(&data, "value", sizeof("value") - 1, str_value->data ? str_value->data : "", str_value->length);
				} else {
					add_assoc_stringl_ex(&data, "value", sizeof("value") - 1, "", 0);
				}
				
				/* match */
				int match = mycss_selector_value_attribute(entry->value)->match;
				add_assoc_stringl_ex(&data, "match", sizeof("match") - 1, attr_match_names[match].name, attr_match_names[match].len);
				
				/* modificator */
				if (mycss_selector_value_attribute(entry->value)->mod & MyCSS_SELECTORS_MOD_I) {
					add_assoc_bool_ex(&data, "ignoreCase", sizeof("ignoreCase") - 1, 1);
				} else {
					add_assoc_bool_ex(&data, "ignoreCase", sizeof("ignoreCase") - 1, 0);
				}
			}
			break;
			case MyCSS_SELECTORS_TYPE_PSEUDO_CLASS_FUNCTION:
			{
				add_assoc_stringl_ex(&data, "type", sizeof("type") - 1, "function", sizeof("function") - 1);
				
				switch (entry->sub_type) {
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_CONTAINS:
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_HAS:
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_NOT:
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_MATCHES:
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_CURRENT:
					{
						switch (entry->sub_type) {
							case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_CONTAINS:
								add_assoc_stringl_ex(&data, "name", sizeof("name") - 1, "contains", sizeof("contains") - 1);
							break;
							case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_HAS:
								add_assoc_stringl_ex(&data, "name", sizeof("name") - 1, "has", sizeof("has") - 1);
							break;
							case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_NOT:
								add_assoc_stringl_ex(&data, "name", sizeof("name") - 1, "not", sizeof("not") - 1);
							break;
							case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_MATCHES:
								add_assoc_stringl_ex(&data, "name", sizeof("name") - 1, "matches", sizeof("matches") - 1);
							break;
							case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_CURRENT:
								add_assoc_stringl_ex(&data, "name", sizeof("name") - 1, "current", sizeof("current") - 1);
							break;
						}
						
						zval value;
						array_init(&value);
						html5_dom_css_serialize_selector(self, entry->value, &value);
						add_assoc_zval_ex(&value, "value", sizeof("value") - 1, &value);
					}
					break;
					
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_NTH_CHILD:
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_NTH_LAST_CHILD:
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_NTH_COLUMN:
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_NTH_LAST_COLUMN:
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_NTH_OF_TYPE:
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_NTH_LAST_OF_TYPE:
					{
						mycss_an_plus_b_entry_t *a_plus_b = mycss_selector_value_an_plus_b(entry->value);
						
						add_assoc_stringl_ex(&data, "name", sizeof("name") - 1, "nth-child", sizeof("nth-child") - 1);
						add_assoc_long_ex(&data, "a", sizeof("a") - 1, a_plus_b->a);
						add_assoc_long_ex(&data, "b", sizeof("b") - 1, a_plus_b->b);
						
						if (a_plus_b->of) {
							zval of;
							array_init(&of);
							html5_dom_css_serialize_selector(self, a_plus_b->of, &of);
							add_assoc_zval_ex(&data, "of", sizeof("of") - 1, &of);
						}
					}
					break;
					
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_DIR:
					{
						add_assoc_stringl_ex(&data, "name", sizeof("name") - 1, "dir", sizeof("dir") - 1);
						if (entry->value) {
							mycore_string_t *str_fname = mycss_selector_value_string(entry->value);
							add_assoc_stringl_ex(&data, "value", sizeof("value") - 1, str_fname->data ? str_fname->data : "", str_fname->length);
						} else {
							add_assoc_stringl_ex(&data, "value", sizeof("value") - 1, "", 0);
						}
					}
					break;
					
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_DROP:
					{
						add_assoc_stringl_ex(&data, "name", sizeof("name") - 1, "drop", sizeof("drop") - 1);
						mycss_selectors_function_drop_type_t drop_val = mycss_selector_value_drop(entry->value);
						
						zval langs;
						array_init(&langs);
						
						if (drop_val & MyCSS_SELECTORS_FUNCTION_DROP_TYPE_ACTIVE)
							add_next_index_stringl(&langs, "active", sizeof("active") - 1);
						if (drop_val & MyCSS_SELECTORS_FUNCTION_DROP_TYPE_VALID)
							add_next_index_stringl(&langs, "valid", sizeof("valid") - 1);
						if (drop_val & MyCSS_SELECTORS_FUNCTION_DROP_TYPE_INVALID)
							add_next_index_stringl(&langs, "invalid", sizeof("invalid") - 1);
						
						add_assoc_zval_ex(&data, "value", sizeof("value") - 1, &langs);
					}
					break;
					
					case MyCSS_SELECTORS_SUB_TYPE_PSEUDO_CLASS_FUNCTION_LANG:
					{
						add_assoc_stringl_ex(&data, "name", sizeof("name") - 1, "lang", sizeof("lang") - 1);
						
						zval langs;
						array_init(&langs);
						
						if (entry->value) {
							mycss_selectors_value_lang_t *lang = mycss_selector_value_lang(entry->value);
							while (lang) {
								add_next_index_stringl(&langs, lang->str.data ? lang->str.data : "", lang->str.length);
								lang = lang->next;
							}
						}
						
						add_assoc_zval_ex(&data, "value", sizeof("value") - 1, &langs);
					}
					break;
					
					default:
						add_assoc_stringl_ex(&data, "name", sizeof("name") - 1, "unknown", sizeof("unknown") - 1);
					break;
				}
			}
			break;
			
			default:
				add_assoc_stringl_ex(&data, "type", sizeof("type") - 1, "unknown", sizeof("unknown") - 1);
			break;
		}
		
		add_next_index_zval(result, &data);
		
		entry = entry->next;
	}
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
	HTML5_DOM_METHOD_PARAMS(html5_css_selector_t);
	
	array_init(return_value);
	if (self->list)
		html5_dom_css_serialize_selector(self, self->list, return_value);
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
