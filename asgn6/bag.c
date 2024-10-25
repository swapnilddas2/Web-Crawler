#include <stdio.h>
#include <stdlib.h>
#include "crawler.h"

bool add_bag(bag_t *bag, webpage_t *page) {
    if (bag == NULL || page == NULL || bag->size >= bag->capacity) {
        return false;
    }

    bag->pages[bag->size] = page; 
    bag->size++; 

    return true;
}

void delete_bag(bag_t *bag) {
    if (bag == NULL) {
        return;
    }

    free(bag->pages);
    free(bag);
}

