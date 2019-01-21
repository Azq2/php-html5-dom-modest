#ifndef PHP_HTML5_DOM_PARSER_H
#define PHP_HTML5_DOM_PARSER_H

#include "php.h"
#include "html5_dom.h"

zend_class_entry *html5_dom_ce;

void html5_dom_class_init();
void html5_dom_class_unload();

int html5_dom_check_options(html5_dom_options_t *opts);
void html5_dom_parse_options(html5_dom_options_t *opts, html5_dom_options_t *extend, zval *options);

static void html5_dom_free_obj(zend_object *object TSRMLS_DC);
static zend_object *html5_dom_create_object(zend_class_entry *ce TSRMLS_DC);

PHP_METHOD(DOM, __construct);
PHP_METHOD(DOM, parse);
PHP_METHOD(DOM, parseChunkStart);
PHP_METHOD(DOM, parseChunk);
PHP_METHOD(DOM, parseChunkTree);
PHP_METHOD(DOM, parseChunkEnd);
PHP_METHOD(DOM, parseAsync);

#endif // PHP_HTML5_DOM_PARSER_H
