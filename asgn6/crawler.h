#ifndef CRAWLER_H
#define CRAWLER_H


#include <stddef.h>
#include <stdbool.h>

typedef struct {
	char *url;
	char *html;
	size_t length;
	int depth;
} webpage_t;

struct bag {
    webpage_t **pages;
    size_t size;
    size_t capacity;
};

typedef struct bag bag_t;


bool add_bag(bag_t *bag, webpage_t *page);
void delete_bag(bag_t *bag);



#endif
