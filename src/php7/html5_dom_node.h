#ifndef PHP_HTML5_DOM_NODE_H
#define PHP_HTML5_DOM_NODE_H

#include "php.h"
#include "html5_dom.h"

zend_class_entry *html5_dom_node_ce;
zend_class_entry *html5_dom_node_element_ce;
zend_class_entry *html5_dom_node_document_ce;
zend_class_entry *html5_dom_node_fragment_ce;
zend_class_entry *html5_dom_node_text_ce;
zend_class_entry *html5_dom_node_comment_ce;
zend_class_entry *html5_dom_node_doctype_ce;

void html5_dom_node_class_init();
void html5_dom_node_class_unload();

static void html5_dom_node_free_obj(zend_object *object TSRMLS_DC);
static zend_object *html5_dom_node_create_object(zend_class_entry *ce TSRMLS_DC);

void html5_dom_node_to_zval(myhtml_tree_node_t *node, zval *retval);

// Node
static int html5_dom_node__tag(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__tagName(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__nodeName(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__tagId(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__namespace(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__namespaceId(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__tree(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__nodeType(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__next(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__prev(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__nextNode(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__prevNode(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__first(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__last(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__firstNode(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__lastNode(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__html(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__innerHTML(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__outerHTML(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__text(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__innerText(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__outerText(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__textContent(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__nodeHtml(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__nodeValue(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__data(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__isConnected(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__parent(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__document(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__void(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node__selfClosed(html5_dom_object_wrap *obj, zval *val, int write, int debug);

// Element
static int html5_dom_node_element__children(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node_element__childrenNode(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node_element__className(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node_element__classList(html5_dom_object_wrap *obj, zval *val, int write, int debug);

// DocType
static int html5_dom_node_doctype__name(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node_doctype__systemId(html5_dom_object_wrap *obj, zval *val, int write, int debug);
static int html5_dom_node_doctype__publicId(html5_dom_object_wrap *obj, zval *val, int write, int debug);

// Node
PHP_METHOD(Node, __construct);
PHP_METHOD(Node, tag); // field
PHP_METHOD(Node, nodeName); // field
PHP_METHOD(Node, tagId); // field
PHP_METHOD(Node, namespace); // field
PHP_METHOD(Node, namespaceId); // field
PHP_METHOD(Node, tree); // field
PHP_METHOD(Node, nodeType); // field
PHP_METHOD(Node, next); // field
PHP_METHOD(Node, nextElementSibling); // field
PHP_METHOD(Node, prev); // field
PHP_METHOD(Node, previousElementSibling); // field
PHP_METHOD(Node, nextNode); // field
PHP_METHOD(Node, nextSibling); // field
PHP_METHOD(Node, prevNode); // field
PHP_METHOD(Node, previousSibling); // field
PHP_METHOD(Node, first); // field
PHP_METHOD(Node, firstElementChild); // field
PHP_METHOD(Node, last); // field
PHP_METHOD(Node, lastElementChild); // field
PHP_METHOD(Node, firstNode); // field
PHP_METHOD(Node, firstChild); // field
PHP_METHOD(Node, lastNode); // field
PHP_METHOD(Node, lastChild); // field
PHP_METHOD(Node, html); // field
PHP_METHOD(Node, innerHTML); // field
PHP_METHOD(Node, outerHTML); // field
PHP_METHOD(Node, text); // field
PHP_METHOD(Node, innerText); // field
PHP_METHOD(Node, outerText); // field
PHP_METHOD(Node, textContent); // field
PHP_METHOD(Node, nodeHtml); // field
PHP_METHOD(Node, nodeValue); // field
PHP_METHOD(Node, data); // field
PHP_METHOD(Node, isConnected); // field
PHP_METHOD(Node, parent); // field
PHP_METHOD(Node, parentElement); // field
PHP_METHOD(Node, document); // field
PHP_METHOD(Node, ownerDocument); // field
PHP_METHOD(Node, append);
PHP_METHOD(Node, appendChild);
PHP_METHOD(Node, prepend);
PHP_METHOD(Node, prependChild);
PHP_METHOD(Node, replace);
PHP_METHOD(Node, replaceChild);
PHP_METHOD(Node, replace);
PHP_METHOD(Node, before);
PHP_METHOD(Node, insertBefore);
PHP_METHOD(Node, after);
PHP_METHOD(Node, insertAfter);
PHP_METHOD(Node, remove);
PHP_METHOD(Node, removeChild);
PHP_METHOD(Node, clone);
PHP_METHOD(Node, cloneNode);
PHP_METHOD(Node, void); // field
PHP_METHOD(Node, selfClosed); // field
PHP_METHOD(Node, position);
PHP_METHOD(Node, isSameNode);

// Element
PHP_METHOD(Element, children); // field
PHP_METHOD(Element, childrenNode); // field
PHP_METHOD(Element, childNodes); // field
PHP_METHOD(Element, attr);
PHP_METHOD(Element, removeAttr);
PHP_METHOD(Element, attrArray);
PHP_METHOD(Element, getAttribute);
PHP_METHOD(Element, setAttribute);
PHP_METHOD(Element, removeAttribute);
PHP_METHOD(Element, className); // field
PHP_METHOD(Element, classList); // field
PHP_METHOD(Element, at);
PHP_METHOD(Element, querySelector);
PHP_METHOD(Element, find);
PHP_METHOD(Element, querySelectorAll);
PHP_METHOD(Element, findId);
PHP_METHOD(Element, getElementById);
PHP_METHOD(Element, findTag);
PHP_METHOD(Element, getElementsByTagName);
PHP_METHOD(Element, findClass);
PHP_METHOD(Element, getElementsByClassName);
PHP_METHOD(Element, findAttr);
PHP_METHOD(Element, getElementByAttribute);
PHP_METHOD(Element, getDefaultBoxType);

// DocType
PHP_METHOD(DocType, name); // field
PHP_METHOD(DocType, publicId); // field
PHP_METHOD(DocType, systemId); // field

#endif // PHP_HTML5_DOM_NODE_H
