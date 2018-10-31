#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "property_get.h"





int property_get(const char *key, char *value, const char *default_value) 
{
	int len;
	len = __system_property_get(key, value);
	if (len > 0) 
	{
		return len;
	}

	if (default_value) 
	{
		len = strlen(default_value);
		memcpy(value, default_value, len + 1);
	}
	return len;
}




void _get_system_property(const char* name, android_sys_info_item* item)
{
    char value[PROP_VALUE_MAX] = {0};
    int len = __system_property_get(name, value);

    item->len = len;
    strncpy(item->value, value, PROP_VALUE_MAX);
    strncpy(item->name, name, PROP_NAME_MAX);
}