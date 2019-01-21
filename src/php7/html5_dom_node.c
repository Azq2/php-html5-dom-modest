#include <common/utils.h>
#include <php7/utils.h>

#include "html5_dom_exception.h"
#include "html5_dom_node.h"

#include "zend_exceptions.h"

/*
	Class HTML5\DOM\Node
*/

static zend_object_handlers php_html5_dom_node_handlers;
static HashTable php_html5_dom_node_prop_handlers;

static zend_function_entry php_html5_dom_node_methods[] = {
	PHP_ME(Node, __construct,			NULL,	ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_FE_END
};

void html5_dom_node_class_init() {
	zend_class_entry ce;
	memcpy(&php_html5_dom_node_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_html5_dom_node_handlers.offset					= XtOffsetOf(html5_dom_object_wrap, std);
	php_html5_dom_node_handlers.free_obj				= html5_dom_node_free_obj;
	php_html5_dom_node_handlers.clone_obj				= NULL;
	
	php_html5_dom_node_handlers.read_property			= html5_dom_read_property;
	php_html5_dom_node_handlers.write_property			= html5_dom_write_property;
	php_html5_dom_node_handlers.get_property_ptr_ptr	= html5_dom_get_property_ptr_ptr;
	php_html5_dom_node_handlers.has_property			= html5_dom_has_property;
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Node", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_ce = zend_register_internal_class(&ce);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Element", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_element_ce = zend_register_internal_class_ex(&ce, html5_dom_node_ce);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Document", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_document_ce = zend_register_internal_class_ex(&ce, html5_dom_node_ce);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Fragment", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_fragment_ce = zend_register_internal_class_ex(&ce, html5_dom_node_ce);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Text", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_text_ce = zend_register_internal_class_ex(&ce, html5_dom_node_ce);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Comment", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_comment_ce = zend_register_internal_class_ex(&ce, html5_dom_node_ce);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\DocType", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_doctype_ce = zend_register_internal_class_ex(&ce, html5_dom_node_ce);
	
	html5_dom_prop_handler_list handlers[] = {
		{"",				NULL}, 
	};
	html5_dom_prop_handler_init(&php_html5_dom_node_prop_handlers, handlers);
}

static zend_object *html5_dom_node_create_object(zend_class_entry *ce TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_wrap_create(ce, &php_html5_dom_node_handlers);
	
	intern->prop_handler = &php_html5_dom_node_prop_handlers;
	
	DOM_GC_TRACE("DOM::Node::new (refs=%d)", GC_REFCOUNT(&intern->std));
	
	return &intern->std;
}

static void html5_dom_node_free_obj(zend_object *object TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_unwrap(object);
	
	DOM_GC_TRACE("DOM::Node::DESTROY (refs=%d)", GC_REFCOUNT(&intern->std));
	
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) intern->ptr;
	
	myhtml_node_set_data(self, NULL);
	if (!myhtml_node_parent(self) && self != myhtml_tree_get_document(self->tree)) {
		if (self == self->tree->node_html) {
			self->tree->node_html = NULL;
		} else if (self == self->tree->node_body) {
			self->tree->node_body = NULL;
		} else if (self == self->tree->node_head) {
			self->tree->node_head = NULL;
		} else if (self == self->tree->node_form) {
			self->tree->node_form = NULL;
		} else if (self == self->tree->fragment) {
			self->tree->fragment = NULL;
		} else if (self == self->tree->document) {
			self->tree->document = NULL;
		}
		DOM_GC_TRACE("=> DOM::Node::FREE");
		html5_tree_node_delete_recursive(self);
	}
	
	zval_ptr_dtor(&intern->parent);
}

void html5_dom_node_class_unload() {
	zend_hash_destroy(&php_html5_dom_node_prop_handlers);
}

static inline zend_class_entry *html5_dom_get_node_class(myhtml_tree_node_t *node) {
	html5_dom_tree_t *context = (html5_dom_tree_t *) node->tree->context;
	
	if (node->tag_id != MyHTML_TAG__UNDEF) {
		if (node->tag_id == MyHTML_TAG__TEXT) {
			return html5_dom_node_text_ce;
		} else if (node->tag_id == MyHTML_TAG__COMMENT) {
			return html5_dom_node_comment_ce;
		} else if (node->tag_id == MyHTML_TAG__DOCTYPE) {
			return html5_dom_node_doctype_ce;
		} else if (context->fragment_tag_id && node->tag_id == context->fragment_tag_id) {
			return html5_dom_node_fragment_ce;
		}
		return html5_dom_node_element_ce;
	}
	
	// Modest myhtml bug - document node has tag_id == MyHTML_TAG__UNDEF
	if (node_is_document(node))
		return html5_dom_node_document_ce;
	
	return html5_dom_node_ce;
}

void html5_dom_node_to_zval(myhtml_tree_node_t *node, zval *retval) {
	html5_dom_object_wrap *intern = (html5_dom_object_wrap *) myhtml_node_get_data(node);
	
	if (intern) {
		GC_REFCOUNT(&intern->std)++;
		ZVAL_OBJ(retval, &intern->std);
		return;
	}
	
	// new HTML5\DOM\Node
	object_init_ex(retval, html5_dom_get_node_class(node));
	intern = html5_dom_object_unwrap(Z_OBJ_P(retval));
	
	intern->ptr = node;
	myhtml_node_set_data(node, intern);
	
	// Save reference to parent
	// That would not deleted tree object while at least one node object live
	html5_dom_tree_t *tree_obj = (html5_dom_tree_t *) node->tree->context;
	html5_dom_object_wrap *tree_intern = (html5_dom_object_wrap *) tree_obj->sv;
	
	GC_REFCOUNT(&tree_intern->std)++;
	ZVAL_OBJ(&intern->parent, &tree_intern->std);
}

/*
	PHP methods
*/
PHP_METHOD(Node, __construct) {
	
}
