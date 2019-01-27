#include <common/utils.h>
#include <php7/utils.h>

#include "html5_dom_exception.h"
#include "html5_dom_node.h"
#include "html5_dom_parser.h"
#include "html5_dom_collection.h"

#include "zend_exceptions.h"
#include "zend_smart_str.h"

/*
	Class HTML5\DOM\Node
*/

static zend_object_handlers php_html5_dom_node_handlers;
static HashTable php_html5_dom_node_prop_handlers;
static HashTable php_html5_dom_node_element_prop_handlers;
static HashTable php_html5_dom_node_doctype_prop_handlers;

static zend_function_entry php_html5_dom_node_methods[] = {
	PHP_ME(Node, __construct,			NULL,	ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_FE_END
};

void html5_dom_node_class_init() {
	html5_dom_prop_handler_list handlers_node[] = {
		{"tag",							html5_dom_node__tag}, 
		{"tagName",						html5_dom_node__tagName}, 
		{"nodeName",					html5_dom_node__nodeName}, 
		{"tagId",						html5_dom_node__tagId}, 
		{"namespace",					html5_dom_node__namespace}, 
		{"namespaceId",					html5_dom_node__namespaceId}, 
		{"tree",						html5_dom_node__tree}, 
		{"nodeType",					html5_dom_node__nodeType}, 
		{"next",						html5_dom_node__next}, 
		{"nextElementSibling",			html5_dom_node__next}, 
		{"prev",						html5_dom_node__prev}, 
		{"previousElementSibling",		html5_dom_node__prev}, 
		{"nextNode",					html5_dom_node__nextNode}, 
		{"nextSibling",					html5_dom_node__nextNode}, 
		{"prevNode",					html5_dom_node__prevNode}, 
		{"previousSibling",				html5_dom_node__prevNode}, 
		{"first",						html5_dom_node__first}, 
		{"firstElementChild",			html5_dom_node__first}, 
		{"last",						html5_dom_node__last}, 
		{"lastElementChild",			html5_dom_node__last}, 
		{"firstNode",					html5_dom_node__firstNode}, 
		{"firstChild",					html5_dom_node__firstNode}, 
		{"lastNode",					html5_dom_node__lastNode}, 
		{"lastChild",					html5_dom_node__lastNode}, 
		{"html",						html5_dom_node__html}, 
		{"innerHTML",					html5_dom_node__innerHTML}, 
		{"outerHTML",					html5_dom_node__outerHTML}, 
		{"text",						html5_dom_node__text}, 
		{"innerText",					html5_dom_node__innerText}, 
		{"outerText",					html5_dom_node__outerText}, 
		{"textContent",					html5_dom_node__textContent}, 
		{"nodeHtml",					html5_dom_node__nodeHtml}, 
		{"nodeValue",					html5_dom_node__nodeValue}, 
		{"data",						html5_dom_node__data}, 
		{"isConnected",					html5_dom_node__isConnected}, 
		{"parent",						html5_dom_node__parent}, 
		{"parentElement",				html5_dom_node__parent}, 
		{"document",					html5_dom_node__document}, 
		{"ownerDocument",				html5_dom_node__document}, 
		{"void",						html5_dom_node__void}, 
		{"selfClosed",					html5_dom_node__selfClosed}, 
		{"",							NULL}, 
	};
	html5_dom_prop_handler_list handlers_element[] = {
		{"children",					html5_dom_node_element__children}, 
		{"childrenNode",				html5_dom_node_element__childrenNode}, 
		{"childNodes",					html5_dom_node_element__childrenNode}, 
		{"",							NULL}, 
	};
	html5_dom_prop_handler_list handlers_doctype[] = {
		{"name",						html5_dom_node_doctype__name}, 
		{"systemId",					html5_dom_node_doctype__systemId}, 
		{"publicId",					html5_dom_node_doctype__publicId}, 
		{"",							NULL}, 
	};
	
	zend_class_entry ce;
	memcpy(&php_html5_dom_node_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_html5_dom_node_handlers.offset					= XtOffsetOf(html5_dom_object_wrap, std);
	php_html5_dom_node_handlers.free_obj				= html5_dom_node_free_obj;
	php_html5_dom_node_handlers.clone_obj				= NULL;
	
	php_html5_dom_node_handlers.read_property			= html5_dom_read_property;
	php_html5_dom_node_handlers.write_property			= html5_dom_write_property;
	php_html5_dom_node_handlers.get_property_ptr_ptr	= html5_dom_get_property_ptr_ptr;
	php_html5_dom_node_handlers.has_property			= html5_dom_has_property;
	php_html5_dom_node_handlers.get_debug_info			= html5_dom_get_debug_info;
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Node", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_ce = zend_register_internal_class(&ce);
	html5_dom_prop_handler_init(&php_html5_dom_node_prop_handlers, handlers_node);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Element", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_element_ce = zend_register_internal_class_ex(&ce, html5_dom_node_ce);
	html5_dom_prop_handler_init(&php_html5_dom_node_element_prop_handlers, handlers_node);
	html5_dom_prop_handler_add(&php_html5_dom_node_element_prop_handlers, handlers_element);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Document", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_document_ce = zend_register_internal_class_ex(&ce, html5_dom_node_element_ce);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Fragment", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_fragment_ce = zend_register_internal_class_ex(&ce, html5_dom_node_element_ce);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Text", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_text_ce = zend_register_internal_class_ex(&ce, html5_dom_node_ce);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\Comment", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_comment_ce = zend_register_internal_class_ex(&ce, html5_dom_node_ce);
	
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM\\DocType", php_html5_dom_node_methods);
	ce.create_object = html5_dom_node_create_object;
	html5_dom_node_doctype_ce = zend_register_internal_class_ex(&ce, html5_dom_node_ce);
	html5_dom_prop_handler_init(&php_html5_dom_node_doctype_prop_handlers, handlers_node);
	html5_dom_prop_handler_add(&php_html5_dom_node_doctype_prop_handlers, handlers_doctype);
}

static zend_object *html5_dom_node_create_object(zend_class_entry *ce TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_wrap_create(ce, &php_html5_dom_node_handlers);
	
	if (ce == html5_dom_node_element_ce) {
		intern->prop_handler = &php_html5_dom_node_element_prop_handlers;
	} else if (ce == html5_dom_node_doctype_ce) {
		intern->prop_handler = &php_html5_dom_node_doctype_prop_handlers;
	} else {
		intern->prop_handler = &php_html5_dom_node_prop_handlers;
	}
	
	DOM_GC_TRACE("DOM::Node::new (refs=%d)", GC_REFCOUNT(&intern->std));
	
	return &intern->std;
}

static void html5_dom_node_free_obj(zend_object *object TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_unwrap(object);
	
	DOM_GC_TRACE("DOM::Node::DESTROY (refs=%d)", GC_REFCOUNT(&intern->std));
	
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) intern->ptr;
	
	if (self) {
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
	
	html5_dom_object_wrap_free(intern);
}

void html5_dom_node_class_unload() {
	zend_hash_destroy(&php_html5_dom_node_prop_handlers);
	zend_hash_destroy(&php_html5_dom_node_element_prop_handlers);
	zend_hash_destroy(&php_html5_dom_node_doctype_prop_handlers);
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
	if (!node) {
		ZVAL_NULL(retval);
		return;
	}
	
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

/*
	Acessors : NODE
*/
static int _node_tag_name(html5_dom_object_wrap *obj, zval *val, int write, int debug, int uppercase) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	myhtml_tree_t *tree = self->tree;
	
	// Set new tag name
	if (write) {
		convert_to_string(val);
		
		const char *new_tag_name = Z_STRVAL_P(val);
		size_t new_tag_name_len = Z_STRLEN_P(val);
		
		if (!new_tag_name_len) {
			zend_throw_exception_ex(html5_dom_exception_ce, 0, "empty tag name not allowed.");
			return 0;
		}
		
		self->tag_id = html5_dom_tag_id_by_name(self->tree, new_tag_name, new_tag_name_len, true);
		
		return 1;
	}
	
	// Get tag name
	else {
		if (tree && tree->tags) {
			const myhtml_tag_context_t *tag_ctx = myhtml_tag_get_by_id(tree->tags, self->tag_id);
			
			if (uppercase) {
				char *new_name = estrndup(tag_ctx->name ? tag_ctx->name : "", tag_ctx->name_length);
				for (size_t i = 0; i < tag_ctx->name_length; ++i)
					new_name[i] = toupper(new_name[i]);
				ZVAL_STRINGL(val, new_name, tag_ctx->name_length);
				efree(new_name);
			} else {
				ZVAL_STRINGL(val, tag_ctx->name ? tag_ctx->name : "", tag_ctx->name_length);
			}
		} else {
			ZVAL_NULL(val);
		}
		
		return 1;
	}
}

static int html5_dom_node__tag(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node_tag_name(obj, val, write, debug, 0);
}

static int html5_dom_node__tagName(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node_tag_name(obj, val, write, debug, 1);
}

static int html5_dom_node__nodeName(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node_tag_name(obj, val, write, debug, 1);
}

static int html5_dom_node__tagId(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	// Set new tag id
	if (write) {
		convert_to_long(val);
		
		zend_long new_tag_id = Z_LVAL_P(val);
		const myhtml_tag_context_t *tag_ctx = myhtml_tag_get_by_id(self->tree->tags, new_tag_id);
		
		if (tag_ctx) {
			self->tag_id = new_tag_id;
		} else {
			zend_throw_exception_ex(html5_dom_exception_ce, 0, "unknown tag id %ld", new_tag_id);
			return 0;
		}
		
		return 1;
	}
	
	// Get tag id
	else {
		ZVAL_LONG(val, self->tag_id);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__namespace(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	myhtml_tree_t *tree = self->tree;
	
	// Set new ns name
	if (write) {
		convert_to_string(val);
		
		const char *new_ns = Z_STRVAL_P(val);
		size_t new_ns_len = Z_STRLEN_P(val);
		
		myhtml_namespace_t ns;
		if (!myhtml_namespace_id_by_name(new_ns, new_ns_len, &ns)) {
			zend_throw_exception_ex(html5_dom_exception_ce, 0, "unknown namespace: %s", new_ns);
			return 0;
		}
		
		myhtml_node_namespace_set(self, ns);
		
		return 1;
	}
	
	// Get ns name
	else {
		size_t ns_name_len;
		const char *ns_name = myhtml_namespace_name_by_id(myhtml_node_namespace(self), &ns_name_len);
		ZVAL_STRINGL(val, ns_name ? ns_name : "", ns_name_len);
		return 1;
	}
}

static int html5_dom_node__namespaceId(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	// Set new ns id
	if (write) {
		convert_to_long(val);
		
		zend_long new_ns_id = Z_LVAL_P(val);
		
		if (myhtml_namespace_name_by_id(new_ns_id, NULL)) {
			myhtml_node_namespace_set(self, new_ns_id);
		} else {
			zend_throw_exception_ex(html5_dom_exception_ce, 0, "unknown namespace id %ld", new_ns_id);
			return 0;
		}
		
		return 1;
	}
	
	// Get ns id
	else {
		ZVAL_LONG(val, myhtml_node_namespace(self));
		return 1;
	}
}

static int html5_dom_node__tree(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	// We need to go deeper
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	myhtml_tree_t *tree = (myhtml_tree_t *) self->tree;
	html5_dom_tree_t *tree_obj = (html5_dom_tree_t *) tree->context;
	
	if (!write) {
		html5_dom_object_wrap *tree_intern = (html5_dom_object_wrap *) tree_obj->sv;
		GC_REFCOUNT(&tree_intern->std)++;
		ZVAL_OBJ(val, &tree_intern->std);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__nodeType(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	html5_dom_tree_t *context = (html5_dom_tree_t *) self->tree->context;
	
	if (!write) {
		ZVAL_LONG(val, 0);
		
		if (self->tag_id != MyHTML_TAG__UNDEF) {
			if (self->tag_id == MyHTML_TAG__TEXT) {
				ZVAL_LONG(val, TEXT_NODE);
			} else if (self->tag_id == MyHTML_TAG__COMMENT) {
				ZVAL_LONG(val, COMMENT_NODE);
			} else if (self->tag_id == MyHTML_TAG__DOCTYPE) {
				ZVAL_LONG(val, DOCUMENT_TYPE_NODE);
			} else if (context->fragment_tag_id && self->tag_id == context->fragment_tag_id) {
				ZVAL_LONG(val, DOCUMENT_FRAGMENT_NODE);
			} else {
				ZVAL_LONG(val, ELEMENT_NODE);
			}
		} else {
			// Modest myhtml bug - document node has tag_id == MyHTML_TAG__UNDEF
			if (node_is_document(self))
				ZVAL_LONG(val, DOCUMENT_NODE);
		}
		
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__next(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		myhtml_tree_node_t *node = myhtml_node_next(self);
		while (node && !node_is_element(node))
			node = myhtml_node_next(node);
		html5_dom_node_to_zval(node, val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__prev(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		myhtml_tree_node_t *node = myhtml_node_prev(self);
		while (node && !node_is_element(node))
			node = myhtml_node_prev(node);
		html5_dom_node_to_zval(node, val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__nextNode(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		html5_dom_node_to_zval(myhtml_node_next(self), val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__prevNode(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		html5_dom_node_to_zval(myhtml_node_prev(self), val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__first(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		myhtml_tree_node_t *node = myhtml_node_child(self);
		while (node && !node_is_element(node))
			node = myhtml_node_next(node);
		html5_dom_node_to_zval(node, val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__last(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		myhtml_tree_node_t *node = myhtml_node_last_child(self);
		while (node && !node_is_element(node))
			node = myhtml_node_prev(node);
		html5_dom_node_to_zval(node, val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__firstNode(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		html5_dom_node_to_zval(myhtml_node_child(self), val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__lastNode(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		html5_dom_node_to_zval(myhtml_node_last_child(self), val);
		return 1;
	}
	
	return 0;
}

static int _node__html(html5_dom_object_wrap *obj, zval *val, int write, int debug, int mode) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (write) {
		// outerHTML
		if (mode == 2 && !myhtml_node_parent(self)) {
			zend_throw_exception_ex(html5_dom_exception_ce, 0, "This element has no parent node.");
			return 0;
		}
		
		convert_to_string(val);
		
		size_t text_len = Z_STRLEN_P(val);
		const char *text_str = Z_STRVAL_P(val);
		
		if (node_is_element(self) || node_is_document(self)) { // parse fragment and replace all node childrens with it
			// parse fragment
			mystatus_t status;
			html5_fragment_parts_t parts = {0};
			myhtml_tree_node_t *context_node = mode == 2 ? myhtml_node_parent(self) : self;
			myhtml_tag_id_t context_tag_id = context_node->tag_id;
			
			// hack for document node
			if (node_is_document(context_node))
				context_tag_id = MyHTML_TAG_HTML;
			
			html5_dom_tree_t *tree_context = (html5_dom_tree_t *) self->tree->context;
			html5_dom_options_t opts = {0};
			html5_dom_parse_options(&opts, &tree_context->parser->opts, NULL);
			
			// force set encoding to UTF-8
			opts.encoding			= MyENCODING_DEFAULT;
			opts.default_encoding	= MyENCODING_DEFAULT;
			
			myhtml_tree_node_t *fragment = html5_dom_parse_fragment(&opts, self->tree, context_tag_id, myhtml_node_namespace(context_node), text_str, text_len, &parts, &status);
			if (status) {
				zend_throw_exception_ex(html5_dom_exception_ce, status, "myhtml_parse_fragment failed: %d (%s)", status, modest_strerror(status));
				return 0;
			}
			
			// remove all child nodes
			myhtml_tree_node_t *node = myhtml_node_child(self);
			while (node) {
				myhtml_tree_node_t *next = myhtml_node_next(node);
				myhtml_tree_node_remove(node);
				html5_tree_node_delete_recursive(node);
				node = next;
			}
			
			// cleanup references in tree
			if (node_is_root(self)) {
				self->tree->node_body = parts.node_body;
				self->tree->node_head = parts.node_head;
			} else if (node_is_document(self)) {
				self->tree->node_html = parts.node_html;
				self->tree->node_body = parts.node_body;
				self->tree->node_head = parts.node_head;
			}
			
			if (fragment != self->tree->node_html) {
				// add fragment
				node = myhtml_node_child(fragment);
				while (node) {
					myhtml_tree_node_t *next = myhtml_node_next(node);
					myhtml_tree_node_remove(node);
					if (mode == 2) { // outerHTML
						myhtml_tree_node_insert_before(self, node);
					} else { // innerHTML
						myhtml_tree_node_add_child(self, node);
					}
					node = next;
				}
				
				// remove self if outerHTML
				if (mode == 2)
					myhtml_tree_node_remove(self);
				
				// free fragment
				html5_tree_node_delete_recursive(fragment);
			} else {
				// fragment now is html node, why not?
				fragment->tag_id = MyHTML_TAG_HTML;
				myhtml_tree_node_remove(fragment);
				myhtml_tree_node_add_child(self, fragment);
			}
		} else { // same as nodeValue, for user friendly API
			myhtml_node_text_set(self, text_str, text_len, MyENCODING_DEFAULT);
		}
		
		return 1;
	} else {
		smart_str buf = {0};
		
		if (self->tag_id == MyHTML_TAG__UNDEF || mode == 1 || html5_dom_is_fragment(self)) { // innerHTML
			myhtml_tree_node_t *node = myhtml_node_child(self);
			while (node) {
				myhtml_serialization_tree_callback(node, html5_dom_serialization_callback, &buf);
				node = myhtml_node_next(node);
			}
		} else { // outerHTML
			myhtml_serialization_tree_callback(self, html5_dom_serialization_callback, &buf);
		}
		
		if (buf.s) {
			ZSTR_VAL(buf.s)[ZSTR_LEN(buf.s)] = '\0';
			ZVAL_NEW_STR(val, buf.s);
		} else {
			ZVAL_EMPTY_STRING(val);
		}
		
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__html(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node__html(obj, val, write, debug, 0);
}

static int html5_dom_node__innerHTML(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node__html(obj, val, write, debug, 1);
}

static int html5_dom_node__outerHTML(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node__html(obj, val, write, debug, 2);
}

static void _recursive_node_text(myhtml_tree_node_t *node, smart_str *str) {
	node = myhtml_node_child(node);
	while (node) {
		if (node->tag_id == MyHTML_TAG__TEXT) {
			size_t text_len = 0;
			const char *text = myhtml_node_text(node, &text_len);
			if (text_len)
				smart_str_appendl(str, text, text_len);
		} else if (node_is_element(node)) {
			_recursive_node_text(node, str);
		}
		node = myhtml_node_next(node);
	}
}

static int _node__text(html5_dom_object_wrap *obj, zval *val, int write, int debug, int mode) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	myhtml_tree_t *tree = self->tree;
	
	if (!node_is_element(self)) {
		if (mode == 2 || mode == 3 || mode == 5) {
			// Unsupported!
			return 0;
		} else if (write) { // set node value
			convert_to_string(val);
			
			size_t text_len = Z_STRLEN_P(val);
			const char *text_str = Z_STRVAL_P(val);
			
			myhtml_node_text_set(self, text_str, text_len, MyENCODING_DEFAULT);
		} else { // get node value
			size_t text_len = 0;
			const char *text = myhtml_node_text(self, &text_len);
			ZVAL_STRINGL(val, text ? text : "", text_len);
		}
	} else {
		if (mode == 1 || mode == 4) {
			// Unsupported!
			return 0;
		} else if (write) { // remove all childrens and add text node
			convert_to_string(val);
			
			size_t text_len = Z_STRLEN_P(val);
			const char *text_str = Z_STRVAL_P(val);
			
			// remove all children nodes
			myhtml_tree_node_t *node = myhtml_node_child(self);
			while (node) {
				myhtml_tree_node_t *next = myhtml_node_next(node);
				myhtml_tree_node_remove(node);
				html5_tree_node_delete_recursive(node);
				node = next;
			}
			
			// cleanup references in tree
			if (node_is_root(self)) {
				self->tree->node_body = NULL;
				self->tree->node_head = NULL;
			} else if (node_is_document(self)) {
				self->tree->node_html = NULL;
				self->tree->node_body = NULL;
				self->tree->node_head = NULL;
			}
			
			// innerText, outerText
			if (mode == 2 || mode == 5) {
				size_t last_pos = 0;
				for (size_t i = 0; i < text_len; ++i) {
					bool is_end = (i >= text_len - 1);
					bool is_new_line = (text_str[i] == '\n' || text_str[i] == '\r');
					if (is_end || is_new_line) {
						if (is_end && !is_new_line)
							++i;
						
						// insert new text node
						if (i - last_pos) {
							myhtml_tree_node_t *text_node = myhtml_node_create(self->tree, MyHTML_TAG__TEXT, myhtml_node_namespace(self));
							myhtml_node_text_set(text_node, &text_str[last_pos], i - last_pos, MyENCODING_DEFAULT);
							if (mode == 5) { // outerText
								myhtml_tree_node_insert_before(self, text_node);
							} else { // innerText
								myhtml_tree_node_add_child(self, text_node);
							}
						}
						
						// insert new br
						if (is_new_line) {
							myhtml_tree_node_t *text_node = myhtml_node_create(self->tree, MyHTML_TAG_BR, myhtml_node_namespace(self));
							if (!text_node->token) {
								text_node->token = myhtml_token_node_create(self->tree->token, self->tree->mcasync_rules_token_id);
								if (!text_node->token) {
									myhtml_tree_node_delete(text_node);
									zend_throw_exception_ex(html5_dom_exception_ce, 0, "myhtml_token_node_create failed");
									return 0;
								}
								text_node->token->type |= MyHTML_TOKEN_TYPE_CLOSE_SELF | MyHTML_TOKEN_TYPE_DONE;
							}
							
							if (mode == 5) { // outerText
								myhtml_tree_node_insert_before(self, text_node);
							} else { // innerText
								myhtml_tree_node_add_child(self, text_node);
							}
						}
						
						if (!is_end) {
							if (text_str[i] == '\r' && text_str[i + 1] == '\n')
								++i;
							last_pos = i + 1;
						}
					}
				}
			}
			// text, textContent
			else {
				myhtml_tree_node_t *text_node = myhtml_node_create(self->tree, MyHTML_TAG__TEXT, myhtml_node_namespace(self));
				myhtml_node_text_set(text_node, text_str, text_len, MyENCODING_DEFAULT);
				myhtml_tree_node_add_child(self, text_node);
			}
			
			if (mode == 5) {
				// remove self, if outerText
				myhtml_tree_node_remove(self);
			}
		} else { // recursive serialize node to text
			// innerText, outerText
			if (mode == 2 || mode == 5) {
				html5_dom_inner_text_state_t state = {0};
				state.last_br = true;
				state.new_line = true;
				
				mycore_string_init(self->tree->mchar, self->tree->mchar_node_id, &state.value, 1);
				
				myhtml_tree_node_t *next = myhtml_node_child(self);
				while (next) {
					html5_dom_recursive_node_inner_text(next, &state);
					next = myhtml_node_next(next);
				}
				html5_dom_rtrim_mystring(&state.value, ' ');
				
				ZVAL_STRINGL(val, state.value.data ? state.value.data : "", state.value.length);
				mycore_string_destroy(&state.value, 0);
			}
			// text, textContent
			else {
				smart_str buf = {0};
				_recursive_node_text(self, &buf);
				
				if (buf.s) {
					ZSTR_VAL(buf.s)[ZSTR_LEN(buf.s)] = '\0';
					ZVAL_NEW_STR(val, buf.s);
				} else {
					ZVAL_EMPTY_STRING(val);
				}
			}
		}
	}
	
	return 1;
}

static int html5_dom_node__text(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node__text(obj, val, write, debug, 0);
}

static int html5_dom_node__nodeValue(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node__text(obj, val, write, debug, 1);
}

static int html5_dom_node__innerText(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node__text(obj, val, write, debug, 2);
}

static int html5_dom_node__textContent(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node__text(obj, val, write, debug, 3);
}

static int html5_dom_node__data(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node__text(obj, val, write, debug, 4);
}

static int html5_dom_node__outerText(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node__text(obj, val, write, debug, 5);
}

static int html5_dom_node__nodeHtml(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		smart_str buf = {0};
		
		myhtml_serialization_node_callback(self, html5_dom_serialization_callback, &buf);
		
		if (buf.s) {
			ZSTR_VAL(buf.s)[ZSTR_LEN(buf.s)] = '\0';
			ZVAL_NEW_STR(val, buf.s);
		} else {
			ZVAL_EMPTY_STRING(val);
		}
		
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__isConnected(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		ZVAL_BOOL(val, myhtml_node_parent(self) != NULL);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__parent(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		html5_dom_node_to_zval(myhtml_node_parent(self), val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__document(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		html5_dom_node_to_zval(myhtml_tree_get_document(self->tree), val);
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__void(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		ZVAL_BOOL(val, myhtml_node_is_void_element(self));
		return 1;
	}
	
	return 0;
}

static int html5_dom_node__selfClosed(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		ZVAL_BOOL(val, myhtml_node_is_close_self(self));
		return 1;
	}
	
	return 0;
}

/*
	Acessors : ELEMENT
*/

static int _node_element__children(html5_dom_object_wrap *obj, zval *val, int write, int debug, int mode) {
	myhtml_tree_node_t *self = (myhtml_tree_node_t *) obj->ptr;
	
	if (!write) {
		mystatus_t status;
		myhtml_collection_t *collection = myhtml_collection_create(1024, &status);
		
		if (status) {
			zend_throw_exception_ex(html5_dom_exception_ce, status, "myhtml_collection_create failed: %d", status);
			return 0;
		}
		
		myhtml_tree_node_t *child = myhtml_node_child(self);
		while (child) {
			if (mode || node_is_element(child)) {
				collection->list[collection->length] = child;
				collection->length++;
				
				if (collection->length >= collection->size) {
					mystatus_t mystatus = myhtml_collection_check_size(collection, 1024, 0);
					if (mystatus) {
						myhtml_collection_destroy(collection);
						zend_throw_exception_ex(html5_dom_exception_ce, status, "myhtml_collection_check_size failed: %d", status);
						return 0;
					}
				}
			}
			child = myhtml_node_next(child);
		}
		
		html5_dom_collection_to_zval(self->tree, collection, val);
		
		return 1;
	}
	
	return 0;
}

static int html5_dom_node_element__children(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node_element__children(obj, val, write, debug, 0);
}

static int html5_dom_node_element__childrenNode(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return _node_element__children(obj, val, write, debug, 1);
}

static int html5_dom_node_element__className(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return 0;
}

static int html5_dom_node_element__classList(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return 0;
}

// DocType
static int html5_dom_node_doctype__name(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return 0;
}

static int html5_dom_node_doctype__systemId(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return 0;
}

static int html5_dom_node_doctype__publicId(html5_dom_object_wrap *obj, zval *val, int write, int debug) {
	return 0;
}

