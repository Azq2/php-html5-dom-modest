#include <common/utils.h>
#include <php7/utils.h>

#include "html5_dom_exception.h"
#include "html5_dom_parser.h"
#include "html5_dom_tree.h"

#include "zend_exceptions.h"

/*
	Class HTML5\DOM
*/

static zend_object_handlers php_html5_dom_handlers;

static zend_function_entry php_html5_dom_methods[] = {
	PHP_ME(DOM, __construct,			NULL,	ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(DOM, parse,					NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(DOM, parseChunkStart,		NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(DOM, parseChunk,				NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(DOM, parseChunkTree,			NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(DOM, parseChunkEnd,			NULL,	ZEND_ACC_PUBLIC)
	PHP_ME(DOM, parseAsync,				NULL,	ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void html5_dom_class_init() {
	memcpy(&php_html5_dom_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_html5_dom_handlers.offset		= XtOffsetOf(html5_dom_object_wrap, std);
	php_html5_dom_handlers.free_obj		= html5_dom_free_obj;
	php_html5_dom_handlers.clone_obj	= NULL;
	
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "HTML5\\DOM", php_html5_dom_methods);
	ce.create_object = html5_dom_create_object;
	
	html5_dom_ce = zend_register_internal_class(&ce);
}

static zend_object *html5_dom_create_object(zend_class_entry *ce TSRMLS_DC) {
	html5_dom_object_wrap *intern = html5_dom_object_wrap_create(ce, &php_html5_dom_handlers);
	
	DOM_GC_TRACE("DOM::new (refs=%d)", GC_REFCOUNT(&intern->std));
	
	return &intern->std;
}

static void html5_dom_free_obj(zend_object *object TSRMLS_DC) {
	fprintf(stderr, "html5_dom_free_obj!\n");
	
	html5_dom_object_wrap *obj = html5_dom_object_unwrap(object);
	
	DOM_GC_TRACE("DOM::DESTROY (refs=%d)", GC_REFCOUNT(&obj->std));
	
	if (obj->ptr)
		obj->ptr = html5_dom_parser_free((html5_dom_parser_t *) obj->ptr);
	
	html5_dom_object_wrap_free(obj);
}

void html5_dom_class_unload() {
	// Nothing todo...
}

static long hv_get_int_value(zval *val, const char *key, int length, long def) {
	if (val) {
		zval *tmp = zend_hash_str_find(Z_ARRVAL_P(val), key, length);
		if (tmp) {
			convert_to_long(tmp);
			if (Z_TYPE_P(tmp) == IS_LONG)
				return Z_LVAL_P(tmp);
			
		}
	}
	return def;
}

static myencoding_t hv_get_encoding_value(zval *val, const char *key, int length, myencoding_t def) {
	if (val) {
		zval *tmp = zend_hash_str_find(Z_ARRVAL_P(val), key, length);
		if (tmp) {
			convert_to_string(tmp);
			
			if (Z_TYPE_P(tmp) == IS_STRING) {
				size_t enc_length = Z_STRLEN_P(tmp);
				const char *enc_str = Z_STRVAL_P(tmp);
				
				if (enc_length > 0) {
					myencoding_t enc_id;
					if (isdigit(enc_str[0])) { // May be encoding id
						convert_to_long(tmp);
						if (Z_TYPE_P(tmp) == IS_LONG) {
							if (enc_id == MyENCODING_AUTO || enc_id == MyENCODING_DEFAULT || enc_id == MyENCODING_NOT_DETERMINED)
								return enc_id;
							if (!myencoding_name_by_id(enc_id, NULL))
								return MyENCODING_NOT_DETERMINED;
						}
						return MyENCODING_NOT_DETERMINED;
					} else { // May be encoding name
						if (!myencoding_by_name(enc_str, enc_length, &enc_id)) {
							if (enc_length == 4 && strcasecmp(enc_str, "auto") == 0)
								return MyENCODING_AUTO;
							if (enc_length == 7 && strcasecmp(enc_str, "default") == 0)
								return MyENCODING_DEFAULT;
							return MyENCODING_NOT_DETERMINED;
						}
					}
					return enc_id;
				} else {
					return MyENCODING_NOT_DETERMINED;
				}
			} else if (Z_TYPE_P(tmp) != IS_NULL) {
				return MyENCODING_NOT_DETERMINED;
			}
		}
	}
	return def;
}

static void html5_dom_parse_options(html5_dom_options_t *opts, html5_dom_options_t *extend, zval *options) {
	opts->threads					= hv_get_int_value(options, "threads", 7, extend ? extend->threads : 0);
	opts->ignore_whitespace			= hv_get_int_value(options, "ignore_whitespace", 17, extend ? extend->ignore_whitespace : 0) > 0;
	opts->ignore_doctype			= hv_get_int_value(options, "ignore_doctype", 14, extend ? extend->ignore_doctype : 0) > 0;
	opts->scripts					= hv_get_int_value(options, "scripts", 7, extend ? extend->scripts : 0) > 0;
	opts->encoding					= hv_get_encoding_value(options, "encoding", 8, extend ? extend->encoding : MyENCODING_AUTO);
	opts->default_encoding			= hv_get_encoding_value(options, "default_encoding", 16, extend ? extend->default_encoding : MyENCODING_DEFAULT);
	opts->encoding_use_meta			= hv_get_int_value(options, "encoding_use_meta", 17, extend ? extend->encoding_use_meta : 1) > 0;
	opts->encoding_use_bom			= hv_get_int_value(options, "encoding_use_bom", 16, extend ? extend->encoding_use_bom : 1) > 0;
	opts->encoding_prescan_limit	= hv_get_int_value(options, "encoding_prescan_limit", 22, extend ? extend->encoding_prescan_limit : 1024);
	
	#ifdef MyCORE_BUILD_WITHOUT_THREADS
		opts->threads = 0;
	#endif
}

static bool html5_dom_check_options(html5_dom_options_t *opts) {
	if (opts->encoding == MyENCODING_NOT_DETERMINED) {
		zend_throw_exception_ex(html5_dom_exception_ce, 0, "invalid encoding value");
		return 0;
	} else if (opts->default_encoding == MyENCODING_NOT_DETERMINED || opts->default_encoding == MyENCODING_AUTO) {
		zend_throw_exception_ex(html5_dom_exception_ce, 0, "invalid default_encoding value");
		return 0;
	} else if (opts->threads < 0) {
		zend_throw_exception_ex(html5_dom_exception_ce, 0, "invalid threads count");
		return 0;
	} else if (opts->encoding_prescan_limit < 0) {
		zend_throw_exception_ex(html5_dom_exception_ce, 0, "invalid encoding_prescan_limit value");
		return 0;
	}
	return 1;
}

static void html5_dom_create_tree_object(zval *retval, myhtml_tree_t *tree, zval *parent, bool used) {
	html5_dom_tree_t *tree_obj = (html5_dom_tree_t *) tree->context;
	
	if (tree_obj) {
		html5_dom_object_wrap *intern = (html5_dom_object_wrap *) tree_obj->sv;
		GC_REFCOUNT(&intern->std)++;
		ZVAL_OBJ(retval, &intern->std);
		return;
	}
	
	object_init_ex(retval, html5_dom_tree_ce);
	
	html5_dom_object_wrap *intern = html5_dom_object_unwrap(Z_OBJ_P(retval));
	html5_dom_object_wrap *parent_intern = html5_dom_object_unwrap(Z_OBJ_P(parent));
	
	tree->context = emalloc(sizeof(html5_dom_tree_t));
	tree_obj = (html5_dom_tree_t *) tree->context;
	
	tree_obj->tree = tree;
	tree_obj->parent = emalloc(sizeof(zval));
	tree_obj->parser = (html5_dom_parser_t *) parent_intern->ptr;
	tree_obj->fragment_tag_id = MyHTML_TAG__UNDEF;
	tree_obj->used = used;
	
	intern->ptr = tree_obj;
	tree_obj->sv = intern;
	
	ZVAL_COPY(tree_obj->parent, parent);
}

PHP_METHOD(DOM, __construct) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_parser_t);
	
	zval *options = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a", &options) != SUCCESS)
		WRONG_PARAM_COUNT; 
	
	mystatus_t status;
	
	html5_dom_options_t opts = {0};
	html5_dom_parse_options(&opts, NULL, options);
	
	if (html5_dom_check_options(&opts)) {
		self = html5_dom_parser_new(&opts);
		self->myhtml = myhtml_create();
		
		if (self->opts.threads <= 1) {
			status = myhtml_init(self->myhtml, MyHTML_OPTIONS_PARSE_MODE_SINGLE, 1, 0);
		} else {
			status = myhtml_init(self->myhtml, MyHTML_OPTIONS_DEFAULT, self->opts.threads, 0);
		}
		
		if (status) {
			self = html5_dom_parser_free(self);
			zend_throw_exception_ex(html5_dom_exception_ce, status, "myhtml_init failed: %d (%s)", status, modest_strerror(status));
		}
		
		self_object->ptr = self;
	}
}

PHP_METHOD(DOM, parse) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_parser_t);
	
	zval *options = NULL;
	zend_string *html;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|a", &html, &options) != SUCCESS)
		WRONG_PARAM_COUNT; 
	
	mystatus_t status;
	html5_dom_options_t opts = {0};
	
	html5_dom_parse_options(&opts, &self->opts, options);
	
	if (!html5_dom_check_options(&opts))
		RETURN_NULL();
	
	myhtml_tree_t *tree = myhtml_tree_create();
	status = myhtml_tree_init(tree, self->myhtml);
	
	if (status) {
		myhtml_tree_destroy(tree);
		zend_throw_exception_ex(html5_dom_exception_ce, status, "myhtml_tree_init failed: %d (%s)", status, modest_strerror(status));
		RETURN_NULL();
	}
	
	size_t html_length = html->len;
	const char *html_str = html->val;
	
	myencoding_t encoding = html5_dom_auto_encoding(&opts, &html_str, &html_length);
	html5_dom_apply_tree_options(tree, &opts);
	
	status = myhtml_parse(tree, encoding, html_str, html_length);
	if (status) {
		myhtml_tree_destroy(tree);
		zend_throw_exception_ex(html5_dom_exception_ce, status, "myhtml_parse failed: %d (%s)", status, modest_strerror(status));
		RETURN_NULL();
	}
	
	html5_dom_create_tree_object(return_value, tree, self_php_object, false);
}

PHP_METHOD(DOM, parseChunkStart) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_parser_t);
	
	zval *options = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a", &options) != SUCCESS)
		WRONG_PARAM_COUNT; 
	
	mystatus_t status;
	
	html5_dom_options_t opts = {0};
	html5_dom_parse_options(&opts, NULL, options);
	
	if (html5_dom_check_options(&opts)) {
		if (self->tree) {
			if (self->tree->context) {
				html5_dom_tree_t *tree_context = (html5_dom_tree_t *) self->tree;
				tree_context->used = false;
			} else {
				myhtml_tree_destroy(self->tree);
			}
			
			self->tree = NULL;
		}
		
		self->tree = myhtml_tree_create();
		status = myhtml_tree_init(self->tree, self->myhtml);
		
		if (status) {
			self->tree = myhtml_tree_destroy(self->tree);
			zend_throw_exception_ex(html5_dom_exception_ce, status, "myhtml_tree_init failed: %d (%s)", status, modest_strerror(status));
		} else {
			self->chunks = 0;
			myhtml_encoding_set(self->tree, self->chunk_opts.encoding == MyENCODING_AUTO ? self->chunk_opts.default_encoding : self->chunk_opts.encoding);
		}
	}
	
	RETURN_ZVAL(self_php_object, true, false);
}

PHP_METHOD(DOM, parseChunk) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_parser_t);
	mystatus_t status;
	
	zend_string *html;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &html) != SUCCESS)
		WRONG_PARAM_COUNT;
	
	if (!self->tree) {
		self->tree = myhtml_tree_create();
		status = myhtml_tree_init(self->tree, self->myhtml);
		
		if (status) {
			myhtml_tree_destroy(self->tree);
			zend_throw_exception_ex(html5_dom_exception_ce, status, "myhtml_tree_init failed: %d (%s)", status, modest_strerror(status));
			RETURN_NULL();
		}
		
		memcpy(&self->opts, &self->chunk_opts, sizeof(html5_dom_options_t));
		myhtml_encoding_set(self->tree, self->chunk_opts.encoding == MyENCODING_AUTO ? self->chunk_opts.default_encoding : self->chunk_opts.encoding);
		self->chunks = 0;
	}
	
	size_t html_length = html->len;
	const char *html_str = html->val;
	
	// Try detect encoding only in first chunk
	if (!self->chunks) {
		myhtml_encoding_set(self->tree, html5_dom_auto_encoding(&self->chunk_opts, &html_str, &html_length));
		html5_dom_apply_tree_options(self->tree, &self->chunk_opts);
	}
	
	++self->chunks;
	
	status = myhtml_parse_chunk(self->tree, html_str, html_length);
	if (status) {
		if (!self->tree->context)
			myhtml_tree_destroy(self->tree);
		
		zend_throw_exception_ex(html5_dom_exception_ce, status, "myhtml_parse_chunk failed: %d (%s)", status, modest_strerror(status));
		RETURN_NULL();
	}
	
	RETURN_ZVAL(self_php_object, true, false);
}

PHP_METHOD(DOM, parseChunkTree) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_parser_t);
	
	if (!self->tree) {
		zend_throw_exception_ex(html5_dom_exception_ce, 0, "call parseChunkStart or parseChunk first");
	} else {
		html5_dom_create_tree_object(return_value, self->tree, self_php_object, true);
	}
}

PHP_METHOD(DOM, parseChunkEnd) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_parser_t);
	
	mystatus_t status;
	
	if (!self->tree) {
		zend_throw_exception_ex(html5_dom_exception_ce, 0, "call parseChunkStart or parseChunk first");
		RETURN_NULL();
	}
	
	status = myhtml_parse_chunk_end(self->tree);
	if (status) {
		if (!self->tree->context)
			myhtml_tree_destroy(self->tree);
		zend_throw_exception_ex(html5_dom_exception_ce, status, "myhtml_parse_chunk failed: %d (%s)", status, modest_strerror(status));
		RETURN_NULL();
	}
	
	if (self->tree) {
		html5_dom_tree_t *tree_context = (html5_dom_tree_t *) self->tree;
		tree_context->used = false;
	}
	
	html5_dom_create_tree_object(return_value, self->tree, self_php_object, false);
	self->tree = NULL;
}

PHP_METHOD(DOM, parseAsync) {
	HTML5_DOM_METHOD_PARAMS(html5_dom_parser_t);
	
}
