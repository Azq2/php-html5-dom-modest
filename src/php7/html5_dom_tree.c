#include <common/utils.h>
#include <php7/utils.h>

#include "html5_dom_exception.h"
#include "html5_dom_tree.h"
#include "html5_dom_parser.h"
#include "html5_dom_collection.h"

#include "zend_exceptions.h"

#define HTML5_DOM_TREE_FIELD_METHOD_RO(___method, ___field) \
	PHP_METHOD(Tree, ___method) { \
		HTML5_DOM_METHOD_PARAMS(html5_dom_tree_t); \
		html5_dom_tree__##___field(self_object, return_value, 0); \
	}

/*
	Class HTML5\DOM\Tree
*/

static zend_object_handlers php_html5_dom_tree_handlers;
static HashTable php_html5_dom_tree_prop_handlers;

static zend_function_entry php_html5_dom_tree_methods[] = {
	PHP_ME(Tree, __construct,				NULL,	ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(Tree, createElement,				NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, createComment,				NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, createTextNode,			NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, parseFragment,				NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, document,					NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, root,						NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, head,						NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, body,						NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, at,						NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, find,						NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, findId,					NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, findTag,					NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, findClass,					NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, findAttr,					NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, encoding,					NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, encodingId,				NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, tag2id,					NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, id2tag,					NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, namespace2id,				NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, id2namespace,				NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(Tree, parser,					NULL,	ZEND_ACC_PUBLIC)
	
	// User non-friendly and long classic DOM methods
	PHP_MALIAS(Tree,	querySelector,				at,			NULL,		ZEND_ACC_PUBLIC)
	PHP_MALIAS(Tree,	querySelectorAll,			find,		NULL,		ZEND_ACC_PUBLIC)
	PHP_MALIAS(Tree,	getElementById,				findId,		NULL,		ZEND_ACC_PUBLIC)
	PHP_MALIAS(Tree,	getElementsByTagName,		findTag,	NULL,		ZEND_ACC_PUBLIC)
	PHP_MALIAS(Tree,	getElementsByClassName,		findClass,	NULL,		ZEND_ACC_PUBLIC)
	PHP_MALIAS(Tree,	getElementByAttribute,		findAttr,	NULL,		ZEND_ACC_PUBLIC)
	
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
		{"parser",			html5_dom_tree__parser}, 
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
	Finder helpers
*/
/*
static SV *html5_dom_find(CV *cv, html5_dom_parser_t *parser, myhtml_tree_node_t *scope, SV *query, SV *combinator, bool one) {
	mystatus_t status;
	mycss_selectors_entries_list_t *list = NULL;
	size_t list_size = 0;
	mycss_selectors_list_t *selector = NULL;
	modest_finder_selector_combinator_f selector_func = modest_finder_node_combinator_descendant;
	SV *result = &PL_sv_undef;
	
	// Custom combinator as args
	if (combinator) {
		query = sv_stringify(query);
		
		STRLEN combo_len;
		const char *combo = SvPV_const(combinator, combo_len);
		
		if (combo_len > 0)
			selector_func = html5_find_selector_func(combo, combo_len);
	}
	
	if (SvROK(query)) {
		if (sv_derived_from(query, "HTML5::DOM::CSS::Selector")) { // Precompiler selectors
			html5_css_selector_t *selector = INT2PTR(html5_css_selector_t *, SvIV((SV*)SvRV(query)));
			list = selector->list->entries_list;
			list_size = selector->list->entries_list_length;
		} else if (sv_derived_from(query, "HTML5::DOM::CSS::Selector::Entry")) { // One precompiled selector
			html5_css_selector_entry_t *selector = INT2PTR(html5_css_selector_entry_t *, SvIV((SV*)SvRV(query)));
			list = selector->list;
			list_size = 1;
		} else {
			sub_croak(cv, "%s: %s is not of type %s or %s", "HTML5::DOM::Tree::find", "query", "HTML5::DOM::CSS::Selector", "HTML5::DOM::CSS::Selector::Entry");
		}
	} else {
		// String selector, compile it
		query = sv_stringify(query);
		
		STRLEN query_len;
		const char *query_str = SvPV_const(query, query_len);
		
		status = html5_dom_init_css(parser);
		if (status)
			sub_croak(cv, "mycss_init failed: %d (%s)", status, modest_strerror(status));
		
		selector = html5_parse_selector(parser->mycss_entry, query_str, query_len, &status);
		
		if (!selector)
			sub_croak(cv, "bad selector: %s", query_str);
		
		list = selector->entries_list;
		list_size = selector->entries_list_length;
	}
	
	if (one) { // search one element
		myhtml_tree_node_t *node = (myhtml_tree_node_t *) html5_node_finder(parser, selector_func, scope, list, list_size, &status, 1);
		result = node_to_sv(node);
	} else { // search multiple elements
		myhtml_collection_t *collection = (myhtml_collection_t *) html5_node_finder(parser, selector_func, scope, list, list_size, &status, 0);
		result = collection_to_blessed_array(collection);
		if (collection)
			myhtml_collection_destroy(collection);
	}
	
	// destroy parsed selector
	if (selector)
		mycss_selectors_list_destroy(mycss_entry_selectors(parser->mycss_entry), selector, true);
	
	return result;
}
* */

static void html5_dom_simple_find(zval *retval, myhtml_tree_node_t *self, zend_string *key, zend_string *val, zend_string *cmp, zend_bool icase, char type) {
	if (!self) {
		ZVAL_NULL(retval);
		return;
	}
	
	myhtml_collection_t *collection = NULL;
	
	switch (type) {
		case 'T': // tag name
			collection = myhtml_get_nodes_by_name_in_scope(self->tree, NULL, self, key->val, key->len, NULL);
		break;
		
		case 'C': // class
			collection = myhtml_get_nodes_by_attribute_value_whitespace_separated(self->tree, NULL, self, false, "class", 5, key->val, key->len, NULL);
		break;
		
		case 'I': // id (first)
			collection = myhtml_get_nodes_by_attribute_value(self->tree, NULL, self, false, "id", 2, key->val, key->len, NULL);
		break;
		
		case 'A': // attribute
			if (val) {
				char cmp_type = '=';
				if (cmp && cmp->len)
					cmp_type = cmp->val[0];
				
				if (cmp_type == '=') {
					// [key=val]
					collection = myhtml_get_nodes_by_attribute_value(self->tree, NULL, self, icase, key->val, key->len, val->val, val->len, NULL);
				} else if (cmp_type == '~') {
					// [key~=val]
					collection = myhtml_get_nodes_by_attribute_value_whitespace_separated(self->tree, NULL, self, icase, key->val, key->len, val->val, val->len, NULL);
				} else if (cmp_type == '^') {
					// [key^=val]
					collection = myhtml_get_nodes_by_attribute_value_begin(self->tree, NULL, self, icase, key->val, key->len, val->val, val->len, NULL);
				} else if (cmp_type == '$') {
					// [key$=val]
					collection = myhtml_get_nodes_by_attribute_value_end(self->tree, NULL, self, icase, key->val, key->len, val->val, val->len, NULL);
				} else if (cmp_type == '*') {
					// [key*=val]
					collection = myhtml_get_nodes_by_attribute_value_contain(self->tree, NULL, self, icase, key->val, key->len, val->val, val->len, NULL);
				} else if (cmp_type == '|') {
					// [key|=val]
					collection = myhtml_get_nodes_by_attribute_value_hyphen_separated(self->tree, NULL, self, icase, key->val, key->len, val->val, val->len, NULL);
				} else {
					zend_throw_exception_ex(html5_dom_exception_ce, 0, "unknown cmp type: %c", cmp_type);
					ZVAL_NULL(retval);
				}
			} else {
				// [key]
				collection = myhtml_get_nodes_by_attribute_key(self->tree, NULL, self, key->val, key->len, NULL);
			}
		break;
	}
	
	if (collection) {
		if (type == 'I') {
			// Return first search result for getElementById
			if (collection->length) {
				html5_dom_node_to_zval(collection->list[0], retval);
			} else {
				ZVAL_NULL(retval);
			}
		} else {
			// Return collection for others
			html5_dom_collection_to_zval(self->tree, collection, retval);
		}
	} else {
		// Invalid arguments, return NULL
		ZVAL_NULL(retval);
	}
}

/*
	PHP methods
*/
PHP_METHOD(Tree, __construct) {
	
}

PHP_METHOD(Tree, createElement) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_tree_t);
	
	zend_string *tag_name = NULL;
	zend_string *tag_ns = NULL;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|S", &tag_name, &tag_ns) != SUCCESS)
		WRONG_PARAM_COUNT;
	
	// Get namespace id by name
	myhtml_namespace_t ns = MyHTML_NAMESPACE_HTML;
	if (tag_ns) {
		if (!myhtml_namespace_id_by_name(tag_ns->val, tag_ns->len, &ns)) {
			zend_throw_exception_ex(html5_dom_exception_ce, 0, "unknown namespace: %s", tag_ns->val);
			RETURN_NULL();
		}
	}
	
	// Get tag id by name
	myhtml_tag_id_t tag_id = html5_dom_tag_id_by_name(self->tree, tag_name->val, tag_name->len, true);
	
	// create new node
	myhtml_tree_node_t *node = myhtml_node_create(self->tree, tag_id, ns);
	
	// if void - mark self-closed
	if (myhtml_node_is_void_element(node)) {
		if (!node->token) {
			node->token = myhtml_token_node_create(node->tree->token, self->tree->mcasync_rules_token_id);
			if (!node->token) {
				myhtml_tree_node_delete(node);
				zend_throw_exception_ex(html5_dom_exception_ce, 0, "myhtml_token_node_create failed");
				RETURN_NULL();
			}
		}
		node->token->type |= MyHTML_TOKEN_TYPE_CLOSE_SELF | MyHTML_TOKEN_TYPE_DONE;
	}
	
	html5_dom_node_to_zval(node, return_value);
}

PHP_METHOD(Tree, createComment) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_tree_t);
	
	zend_string *text = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &text) != SUCCESS)
		WRONG_PARAM_COUNT;
	
	myhtml_tree_node_t *node = myhtml_node_create(self->tree, MyHTML_TAG__COMMENT, MyHTML_NAMESPACE_HTML);
	myhtml_node_text_set(node, text->val, text->len, MyENCODING_DEFAULT);
	html5_dom_node_to_zval(node, return_value);
}

PHP_METHOD(Tree, createTextNode) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_tree_t);
	
	zend_string *text = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &text) != SUCCESS)
		WRONG_PARAM_COUNT;
	
	myhtml_tree_node_t *node = myhtml_node_create(self->tree, MyHTML_TAG__TEXT, MyHTML_NAMESPACE_HTML);
	myhtml_node_text_set(node, text->val, text->len, MyENCODING_DEFAULT);
	html5_dom_node_to_zval(node, return_value);
}

PHP_METHOD(Tree, parseFragment) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_tree_t);
	
	zend_string *text = NULL;
	zend_string *tag_name = NULL;
	zend_string *tag_ns = NULL;
	zval *options = NULL;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|SSa", &text, &tag_name, &tag_ns, &options) != SUCCESS)
		WRONG_PARAM_COUNT;
	
	mystatus_t status;
	myhtml_namespace_t ns_id = MyHTML_NAMESPACE_HTML;
	myhtml_tag_id_t tag_id = MyHTML_TAG_DIV;
	html5_dom_options_t opts = {0};
	
	html5_dom_parse_options(&opts, &self->parser->opts, options);
	
	if (!html5_dom_check_options(&opts))
		RETURN_NULL();
	
	if (tag_ns) {
		if (!myhtml_namespace_id_by_name(tag_ns->val, tag_ns->len, &ns_id)) {
			zend_throw_exception_ex(html5_dom_exception_ce, 0, "unknown namespace: %s", tag_ns->val);
			RETURN_NULL();
		}
	}
	
	if (tag_name)
		tag_id = html5_dom_tag_id_by_name(self->tree, tag_name->val, tag_name->len, true);
	
	myhtml_tree_node_t *node = html5_dom_parse_fragment(&opts, self->tree, tag_id, ns_id, text->val, text->len, NULL, &status);
	if (status) {
		zend_throw_exception_ex(html5_dom_exception_ce, status, "myhtml_parse_fragment failed: %d (%s)", status, modest_strerror(status));
		RETURN_NULL();
	}
	
	html5_dom_node_to_zval(node, return_value);
}

PHP_METHOD(Tree, at) {
	
}

PHP_METHOD(Tree, find) {
	
}

PHP_METHOD(Tree, findId) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_tree_t);
	
	zend_string *search = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &search) != SUCCESS)
		WRONG_PARAM_COUNT;
	html5_dom_simple_find(return_value, myhtml_tree_get_document(self->tree), search, NULL, NULL, 0, 'I');
}

PHP_METHOD(Tree, findTag) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_tree_t);
	
	zend_string *search = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &search) != SUCCESS)
		WRONG_PARAM_COUNT;
	html5_dom_simple_find(return_value, myhtml_tree_get_document(self->tree), search, NULL, NULL, 0, 'T');
}

PHP_METHOD(Tree, findClass) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_tree_t);
	
	zend_string *search = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &search) != SUCCESS)
		WRONG_PARAM_COUNT;
	html5_dom_simple_find(return_value, myhtml_tree_get_document(self->tree), search, NULL, NULL, 0, 'C');
}

PHP_METHOD(Tree, findAttr) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_tree_t);
	
	zend_string *search = NULL;
	zend_string *value = NULL;
	zend_bool icase = 0;
	zend_string *cmp = NULL;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|S", &search, &value, &icase) != SUCCESS)
		WRONG_PARAM_COUNT;
	
	html5_dom_simple_find(return_value, myhtml_tree_get_document(self->tree), search, value, cmp, icase, 'A');
}

PHP_METHOD(Tree, tag2id) {
	
}

PHP_METHOD(Tree, id2tag) {
	
}

PHP_METHOD(Tree, namespace2id) {
	
}

PHP_METHOD(Tree, id2namespace) {
	
}

HTML5_DOM_TREE_FIELD_METHOD_RO(document, document);
HTML5_DOM_TREE_FIELD_METHOD_RO(root, root);
HTML5_DOM_TREE_FIELD_METHOD_RO(head, head);
HTML5_DOM_TREE_FIELD_METHOD_RO(body, body);
HTML5_DOM_TREE_FIELD_METHOD_RO(encoding, encoding);
HTML5_DOM_TREE_FIELD_METHOD_RO(encodingId, encodingId);
HTML5_DOM_TREE_FIELD_METHOD_RO(parser, parser);

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

static int html5_dom_tree__parser(html5_dom_object_wrap *obj, zval *val, int write) {
	html5_dom_tree_t *self = (html5_dom_tree_t *) obj->ptr;
	
	if (!write) {
		zval *parent = self->parent;
		ZVAL_COPY(val, parent);
		return 1;
	}
	
	return 0;
}
