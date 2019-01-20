#ifndef PHP_HTML5_DOM_H
#define PHP_HTML5_DOM_H

#define PHP_HTML5_DOM_VERSION "1.0.0"

ZEND_BEGIN_MODULE_GLOBALS(html5_dom)
	
ZEND_END_MODULE_GLOBALS(html5_dom)

extern zend_module_entry html5_dom_module_entry;
#define phpext_html5_dom_ptr &html5_dom_module_entry

#if defined(ZTS) && defined(COMPILE_DL_HTML5_DOM)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

zend_object_handlers *html5_dom_get_obj_handlers();

#endif // PHP_HTML5_DOM_H
