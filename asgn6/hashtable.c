#include <stdio.h>
#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

hashtable_t *hashtable_new(const int num_slots) {
    if (num_slots <= 0) {
        return NULL;  
    }
    hashtable_t *ht = (hashtable_t *)malloc(sizeof(hashtable_t));
    if (ht == NULL) {
        return NULL;  
    }
    ht->entries = (ht_entry *)calloc(num_slots, sizeof(ht_entry));
    if (ht->entries == NULL) {
        free(ht);
        return NULL;  
    }
    ht->num_slots = num_slots;
    ht->length = 0;
    return ht;
}

bool hashtable_insert(hashtable_t *ht, const char *key, void *item) {
    if (ht == NULL || key == NULL || item == NULL) {
        return false;  
    }

    for (int i = 0; i < ht->num_slots; i++) {
        if (ht->entries[i].key != NULL && strcmp(ht->entries[i].key, key) == 0) {
            return false;  
        }
    }

    int index = 0;
    while (index < ht->num_slots && ht->entries[index].key != NULL) {
        index++;
    }

    if (index == ht->num_slots) {
        return false;  
    }

    ht->entries[index].key = strdup(key);
    ht->entries[index].value = item;
    ht->length++;

    return true;
}


void *hashtable_find(hashtable_t *ht, const char *key) {
    if (ht == NULL || key == NULL) {
        return NULL;  
    }

    for (int i = 0; i < ht->num_slots; i++) {
        if (ht->entries[i].key != NULL && strcmp(ht->entries[i].key, key) == 0) {
            return ht->entries[i].value;  
        }
    }

    return NULL;  
}

void hashtable_print(hashtable_t *ht, FILE *fp, void (*itemprint)(FILE *fp, const char *key, void *item)) {
    if (fp == NULL) {
        return;  
    }
    if (ht == NULL) {
        fprintf(fp, "NULL\n");
        return;
    }
    fprintf(fp, "Hashtable:\n");
    for (int i = 0; i < ht->num_slots; i++) {
        if (ht->entries[i].key != NULL && itemprint != NULL) {
            itemprint(fp, ht->entries[i].key, ht->entries[i].value);
        }
    }
}

void hashtable_iterate(hashtable_t *ht, void *arg, void (*itemfunc)(void *arg, const char *key, void *item) ) {
    if (ht == NULL || itemfunc == NULL) {
        return;
    }
    for (int i = 0;  i < ht->num_slots; i++) {
    	ht_entry* entry = &ht->entries[i];
    	if (entry->key != NULL) {
    		itemfunc(arg, entry->key, entry->value);
    	}
    }
    return;
}

void hashtable_delete(hashtable_t *ht, void (*itemdelete)(void *item)) {
    if (ht == NULL) {
        return;
    }
    for (int i = 0; i < ht->num_slots; i++) {
        ht_entry *entry = &ht->entries[i];
        if (entry->key != NULL) {
            if (itemdelete != NULL) {
                itemdelete(entry->value);
            }
            free(entry->key);
        }
    }
    free(ht->entries);
    free(ht);
}


