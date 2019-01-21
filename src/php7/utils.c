#include "utils.h"

void *html5_dom_zend_object_alloc(size_t obj_size, zend_class_entry *ce) {
	void *obj = emalloc(obj_size + zend_object_properties_size(ce));
	/* Subtraction of sizeof(zval) is necessary, because zend_object_properties_size() may be
	 * -sizeof(zval), if the object has no properties. */
	memset(obj, 0, obj_size - sizeof(zval));
	return obj;
}

html5_dom_object_wrap *html5_dom_object_wrap_create(zend_class_entry *ce, zend_object_handlers *handlers) {
	html5_dom_object_wrap *intern = html5_dom_zend_object_alloc(sizeof(html5_dom_object_wrap), ce);
	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->std.handlers = handlers;
	return intern;
}

void html5_dom_object_wrap_free(html5_dom_object_wrap *object) {
	zend_object_std_dtor(&object->std);
}

void html5_dom_prop_handler_init(HashTable *hash, html5_dom_prop_handler_list *handlers) {
	zend_hash_init(hash, 0, NULL, NULL, 1);
	
	int i = 0;
	while (handlers[i].func) {
		zend_string *str = zend_string_init_interned(handlers[i].name, strlen(handlers[i].name), 1);
		zend_hash_add_ptr(hash, str, handlers[i].func);
		zend_string_release(str);
		++i;
	}
}

void html5_dom_prop_handler_free(HashTable *hash) {
	zend_hash_destroy(hash);
}

/*
	Generic object handlers
*/
zval *html5_dom_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot) {
	html5_dom_object_wrap *obj = html5_dom_object_unwrap(Z_OBJ_P(object))
	
	zend_string *member_str = zval_get_string(member);
	zval *retval = NULL;
	
	if (!obj->prop_handler || !zend_hash_exists(obj->prop_handler, member_str))
		retval = zend_get_std_object_handlers()->get_property_ptr_ptr(object, member, type, cache_slot);
	
	zend_string_release(member_str);
	
	return retval;
}

zval *html5_dom_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv) {
	html5_dom_object_wrap *obj = html5_dom_object_unwrap(Z_OBJ_P(object));
	zend_string *member_str = zval_get_string(member);
	zval *retval;
	html5_dom_prop_handler handler = NULL;
	
	if (obj->prop_handler != NULL)
		handler = zend_hash_find_ptr(obj->prop_handler, member_str);
	
	if (handler) {
		if (handler(obj, rv, 0)) {
			retval = rv;
		} else {
			retval = &EG(uninitialized_zval);
		}
	} else {
		retval = zend_get_std_object_handlers()->read_property(object, member, type, cache_slot, rv);
	}
	
	zend_string_release(member_str);
	
	return retval;
}

// PHP internal API very unstable.
// Why if i write extension in perl - it works  without any changes after each new version?! :(
#if PHP_VERSION_ID > 70400
zval *html5_dom_write_property(zval *object, zval *member, zval *value, void **cache_slot) {
	html5_dom_object_wrap *obj = html5_dom_object_unwrap(Z_OBJ_P(object));
	zend_string *member_str = zval_get_string(member);
	html5_dom_prop_handler handler = NULL;
	
	if (obj->prop_handler != NULL)
		handler = zend_hash_find_ptr(obj->prop_handler, member_str);
	
	if (handler) {
		handler(obj, value, 1);
	} else {
		value = zend_get_std_object_handlers()->write_property(object, member, value, cache_slot);
	}
	
	zend_string_release(member_str);
	
	return value;
}
#else
void html5_dom_write_property(zval *object, zval *member, zval *value, void **cache_slot) {
	html5_dom_object_wrap *obj = html5_dom_object_unwrap(Z_OBJ_P(object));
	zend_string *member_str = zval_get_string(member);
	html5_dom_prop_handler handler = NULL;
	
	if (obj->prop_handler != NULL)
		handler = zend_hash_find_ptr(obj->prop_handler, member_str);
	
	if (handler) {
		handler(obj, value, 1);
	} else {
		zend_get_std_object_handlers()->write_property(object, member, value, cache_slot);
	}
	
	zend_string_release(member_str);
}
#endif

int html5_dom_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot) {
	html5_dom_object_wrap *obj = html5_dom_object_unwrap(Z_OBJ_P(object));
	zend_string *member_str = zval_get_string(member);
	html5_dom_prop_handler handler = NULL;
	
	int retval = 0;

	if (obj->prop_handler != NULL)
		handler = zend_hash_find_ptr(obj->prop_handler, member_str);
	
	if (handler) {
		zval tmp;
		
		switch (has_set_exists) {
			default:
				retval = 1;
			break;
			
			case ZEND_ISEMPTY:
				if (handler(obj, &tmp, 0)) {
					retval = zend_is_true(&tmp);
					zval_ptr_dtor(&tmp);
				}
			break;
			
			case 0:
				if (handler(obj, &tmp, 0)) {
					retval = (Z_TYPE(tmp) != IS_NULL);
					zval_ptr_dtor(&tmp);
				}
			break;
		}
	} else {
		retval = zend_get_std_object_handlers()->has_property(object, member, has_set_exists, cache_slot);
	}
	
	zend_string_release(member_str);
	
	return retval;
}

HashTable *html5_dom_get_debug_info(zval *object, int *is_temp) {
	html5_dom_object_wrap *obj = html5_dom_object_unwrap(Z_OBJ_P(object));
	
	*is_temp = 1;
	
	HashTable *debug_info, *std_props;
	zend_string *string_key;
	html5_dom_prop_handler handler;
	
	std_props = zend_std_get_properties(object);
	debug_info = zend_array_dup(std_props);
	
	if (!obj->prop_handler)
		return debug_info;
	
	ZEND_HASH_FOREACH_STR_KEY_PTR(obj->prop_handler, string_key, handler) {
		zval value;
		
		if (!handler(obj, &value, 0) || !string_key)
			continue;
		
		zend_hash_add(debug_info, string_key, &value);
	} ZEND_HASH_FOREACH_END();
	
	return debug_info;
}
