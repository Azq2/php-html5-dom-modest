#include <common/utils.h>
#include <php7/utils.h>

#include "html5_dom_exception.h"
#include "html5_dom_tree.h"

#include "zend_exceptions.h"

/*
	Class HTML5\DOM\Tree
*/

static zend_object_handlers php_html5_dom_tree_handlers;
static HashTable php_html5_dom_tree_prop_handlers;

static zend_function_entry php_html5_dom_tree_methods[] = {
	PHP_ME(Tree, __construct,			NULL,	ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_FE_END
};

void html5_dom_tree_class_init() {
	memcpy(&php_html5_dom_tree_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_html5_dom_tree_handlers.offset					= XtOffsetOf(html5_dom_object_wrap, std);
	php_html5_dom_tree_handlers.free_obj				= html5_dom_tree_free_obj;
	php_html5_dom_tree_handlers.clone_obj				= NULL;
	
	php_html5_dom_tree_handlers.read_property			= html5_dom_read_property;
	php_html5_dom_tree_handlers.write_property			= html5_dom_write_property;
	php_html5_dom_tree_handlers.get_property_ptr_ptr	= html5_dom_get_property_ptr_ptr;
	php_html5_dom_tree_handlers.has_property			= html5_dom_has_property;
	
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Tree", php_html5_dom_tree_methods);
	ce.create_object = html5_dom_tree_create_object;
	
	html5_dom_tree_ce = zend_register_internal_class(&ce);
	
	html5_dom_prop_handler_list handlers[] = {
		{"encoding",		html5_dom_tree__encoding}, 
		{"encodingId",		html5_dom_tree__encodingId}, 
		{"document",		html5_dom_tree__document}, 
		{"root",			html5_dom_tree__root}, 
		{"html",			html5_dom_tree__root}, 
		{"head",			html5_dom_tree__head}, 
		{"body",			html5_dom_tree__body}, 
		{"parser",			html5_dom_tree_parser}, 
		{"",				NULL}, 
	};
	html5_dom_prop_handler_init(&php_html5_dom_tree_prop_handlers, handlers);
}

static zend_object *html5_dom_tree_create_object(zend_class_entry *ce TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_wrap_create(ce, &php_html5_dom_tree_handlers);
	
	intern->prop_handler = &php_html5_dom_tree_prop_handlers;
	
	DOM_GC_TRACE("DOM::Tree::new (refs=%d)", GC_REFCOUNT(&intern->std));
	
	return &intern->std;
}

static void html5_dom_tree_free_obj(zend_object *object TSRMLS_DC) {
	html5_dom_object_wrap *obj = html5_dom_object_unwrap(object);
	
	DOM_GC_TRACE("DOM::Tree::DESTROY (refs=%d, obj->ptr=%p)", GC_REFCOUNT(&obj->std), obj->ptr);
	
	html5_dom_tree_t *tree_obj = (html5_dom_tree_t *) obj->ptr;
	if (tree_obj) {
		if (tree_obj->used) {
			tree_obj->tree->context = NULL;
		} else {
			myhtml_tree_destroy(tree_obj->tree);
		}
		
		efree(tree_obj);
	}
	
	zval_ptr_dtor(&obj->parent);
	html5_dom_object_wrap_free(obj);
}

void html5_dom_tree_class_unload() {
	zend_hash_destroy(&php_html5_dom_tree_prop_handlers);
}

/*
	PHP methods
*/
PHP_METHOD(Tree, __construct) {
	
}

/*
	Property acessors
*/
static int html5_dom_tree__encoding(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_dom_tree_t *self = (html5_dom_tree_t *) obj->ptr;
	
	if (!write) {
		size_t length;
		const char *name = myencoding_name_by_id(self->tree->encoding, &length);
		
		ZVAL_STRINGL(val, name, length);
		
		return 1;
	}
	
	return 0;
}

static int html5_dom_tree__encodingId(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_dom_tree_t *self = (html5_dom_tree_t *) obj->ptr;
	
	if (!write) {
		ZVAL_LONG(val, self->tree->encoding);
		return 1;
	}
	
	return 0;
}

static int html5_dom_tree__document(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_dom_tree_t *self = (html5_dom_tree_t *) obj->ptr;
	
	if (!write) {
		html5_dom_node_to_zval(myhtml_tree_get_document(self->tree), val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_tree__root(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_dom_tree_t *self = (html5_dom_tree_t *) obj->ptr;
	
	if (!write) {
		html5_dom_node_to_zval(myhtml_tree_get_node_html(self->tree), val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_tree__head(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_dom_tree_t *self = (html5_dom_tree_t *) obj->ptr;
	
	if (!write) {
		html5_dom_node_to_zval(myhtml_tree_get_node_head(self->tree), val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_tree__body(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_dom_tree_t *self = (html5_dom_tree_t *) obj->ptr;
	
	if (!write) {
		html5_dom_node_to_zval(myhtml_tree_get_node_body(self->tree), val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_tree_parser(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_dom_tree_t *self = (html5_dom_tree_t *) obj->ptr;
	
	if (!write) {
		zval *parent = self->parent;
		ZVAL_COPY(val, parent);
		return 1;
	}
	
	return 0;
}
