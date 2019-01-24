#ifndef PHP_HTML5_DOM_CSS_SELECTOR_H
#define PHP_HTML5_DOM_CSS_SELECTOR_H

#include "php.h"
#include "html5_dom.h"

zend_class_entry *html5_dom_css_selector_ce;

void html5_dom_css_selector_class_init();
void html5_dom_css_selector_class_unload();

static void html5_dom_css_selector_free_obj(zend_object *object TSRMLS_DC);
static zend_object *html5_dom_css_selector_create_object(zend_class_entry *ce TSRMLS_DC);

static int html5_dom_css_selector__text(html5_dom_object_wrap *obj, zval *val, int write);
static int html5_dom_css_selector__valid(html5_dom_object_wrap *obj, zval *val, int write);
static int html5_dom_css_selector__length(html5_dom_object_wrap *obj, zval *val, int write);
static int html5_dom_css_selector__entries(html5_dom_object_wrap *obj, zval *val, int write);

void html5_dom_css_serialize_selector(html5_css_selector_t *self, mycss_selectors_list_t *selector, zval *result);
void html5_dom_css_serialize_entry(html5_css_selector_t *self, mycss_selectors_list_t *selector, mycss_selectors_entry_t *entry, zval *result);

PHP_METHOD(Selector, __construct);
PHP_METHOD(Selector, text); // field
PHP_METHOD(Selector, valid); // field
PHP_METHOD(Selector, length);
PHP_METHOD(Selector, ast);
PHP_METHOD(Selector, entry);

// Countable
PHP_METHOD(Selector, count);

#endif // PHP_HTML5_DOM_CSS_SELECTOR_H
