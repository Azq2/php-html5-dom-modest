#include <common/utils.h>
#include <php7/utils.h>

#include "html5_dom_exception.h"
#include "html5_dom_tree.h"

#include "zend_exceptions.h"

/*
	Class HTML5\DOM\Tree
*/

static zend_object_handlers php_html5_dom_tree_handlers;

static zend_function_entry php_html5_dom_tree_methods[] = {
	PHP_ME(DOM, __construct,			NULL,	ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_FE_END
};

void html5_dom_tree_class_init() {
	memcpy(&php_html5_dom_tree_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_html5_dom_tree_handlers.offset		= XtOffsetOf(html5_dom_object_wrap, std);
	php_html5_dom_tree_handlers.free_obj	= html5_dom_tree_free_obj;
	php_html5_dom_tree_handlers.clone_obj	= NULL;
	
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Tree", php_html5_dom_tree_methods);
	ce.create_object = html5_dom_tree_create_object;
	
	html5_dom_tree_ce = zend_register_internal_class(&ce);
}

static zend_object *html5_dom_tree_create_object(zend_class_entry *ce TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_wrap_create(ce, &php_html5_dom_tree_handlers);
	
	DOM_GC_TRACE("DOM::Tree::new (refs=%d)", GC_REFCOUNT(&intern->std));
	
	return &intern->std;
}

static void html5_dom_tree_free_obj(zend_object *object TSRMLS_DC) {
	html5_dom_object_wrap *obj = html5_dom_object_unwrap(object);
	
	DOM_GC_TRACE("DOM::Tree::DESTROY (refs=%d, obj->ptr=%p)", GC_REFCOUNT(&obj->std), obj->ptr);
	
	html5_dom_tree_t *tree_obj = (html5_dom_tree_t *) obj->ptr;
	if (tree_obj) {
		zval *parent = (zval *) tree_obj->parent;
		
		fprintf(stderr, "parent=%p\n", parent);
		
		if (tree_obj->used) {
			tree_obj->tree->context = NULL;
		} else {
			myhtml_tree_destroy(tree_obj->tree);
		}
		
		fprintf(stderr, "GC_REFCOUNT(&parent->std)=%d\n", Z_REFCOUNT_P(parent));
		
		zval_ptr_dtor(parent);
		efree(parent);
		efree(tree_obj);
	}
	
	html5_dom_object_wrap_free(obj);
}

void html5_dom_tree_class_unload() {
	// Nothing todo...
}
