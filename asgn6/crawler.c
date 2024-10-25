#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "url.h"
#include "hashtable.h"
#include "pagedir.h"
#include "crawler.h"
#include "curl.h"

char* get_URL(const char* content, size_t* pos) {
    const char *anchorStart = content + *pos;
    while ((anchorStart = strstr(anchorStart, "<a")) != NULL) {
        const char *start = strstr(anchorStart, "href=\"");
        if (start != NULL) {
            start += strlen("href=\"");
            const char *end = start;
            while (*end != '\0' && *end != '\"') {
                end++;
            }

            if (*end == '\"') {
                size_t urlLength = end - start;

                char* extractedURL = malloc(urlLength + 1);
                if (extractedURL == NULL) {
                    fprintf(stderr, "Memory allocation failed\n");
                    exit(EXIT_FAILURE);
                }

                strncpy(extractedURL, start, urlLength);
                extractedURL[urlLength] = '\0';

                *pos = end - content;
                return extractedURL;
            } else {
                printf("Invalid HTML format \n");
                return NULL;
            }
        } else {
            printf("No URL found in the anchon");
            return NULL;
        }
        
        anchorStart++;
    }

    printf("No anchor tag found in the HTML\n");
    return NULL;
}

// Define my_my_strdup function
char *my_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup) {
        memcpy(dup, s, len);
    }
    return dup;
}

/**
 * Parses command-line arguments, placing the corresponding values into the pointer arguments seedURL,
 * pageDirectory and maxDepth. argc and argv should be passed in from the main function.
 */
static void parseArgs(const int argc, char *argv[], char **seedURL, char **pageDirectory, int *maxDepth) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s seedURL pageDirectory maxDepth\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    *seedURL = argv[1];
    *pageDirectory = argv[2];
    *maxDepth = atoi(argv[3]);

    if (*maxDepth < 0 || *maxDepth > 10) {
        fprintf(stderr, "maxDepth not in range\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Scans a webpage for URLs.
 */
static void pageScan(webpage_t *page, bag_t *pagesToCrawl, hashtable_t *pagesSeen) {
    char *content = page->html;
    size_t pos = 0;

    while (1) {
        char *url = get_URL(content, &pos);
        printf("THE URL: %s\n", url);

    	if (url == NULL) {
        	break; 
    	} else {
        	if (strstr(url, "#") != NULL) {
            		printf("URL contains '#': %s\n", url);
            		free(url); 
            		continue; 
        	}
    	}

        char *normalized_url = normalizeURL(page->url, url);
        
        
        
        printf("%d    Found: %s\n", page->depth, normalized_url);

        if (normalized_url != NULL && isInternalURL(page->url, normalized_url)) {
            int marker = 1; 
            if (hashtable_insert(pagesSeen, normalized_url, &marker)) {
                webpage_t *newPage = (webpage_t *)malloc(sizeof(webpage_t));
                newPage->url = normalized_url;
                newPage->depth = page->depth + 1;
                

                newPage->html = NULL; 
                

                add_bag(pagesToCrawl, newPage);
                

                printf("%d    Added: %s\n", newPage->depth, newPage->url);
            } else {
                printf("%d   IgnDupl: %s\n", page->depth, normalized_url);
                free(normalized_url);
            }
        } else if (normalized_url != NULL) {
            printf("%d  IgnExtrn: %s\n", page->depth, normalized_url);
            free(normalized_url);
        }

        free(url);
    }
	
}


/**
 * Crawls webpages given a seed URL, a page directory and a max depth.
 */
static void crawl(char *seedURL, char *pageDirectory, const int maxDepth) {
    hashtable_t *pagesSeen = hashtable_new(10000);
    
    int mark = 1; 
    hashtable_insert(pagesSeen, seedURL, &mark);

    bag_t *bag = malloc(sizeof(bag_t));
    if (bag == NULL) {
        return NULL;
    }
    bag->pages = malloc(10000 * sizeof(webpage_t *));
    if (bag->pages == NULL) {
        free(bag);
    }
    bag->capacity = 10000;
    bag->size = 0;
    bag_t *pagesToCrawl = bag;


    webpage_t *seedPage = (webpage_t *)malloc(sizeof(webpage_t));
    seedPage->url = my_strdup(seedURL);
    seedPage->depth = 0;
    seedPage->html = NULL; 

   
    add_bag(pagesToCrawl, seedPage);

    
    if (!pagedir_init(pageDirectory)) {
        fprintf(stderr, "Unable to mark %s\n", pageDirectory);
        exit(EXIT_FAILURE);
    }

    int documentID = 1; 
    int depth = 0;      


    while (!(pagesToCrawl == NULL || pagesToCrawl->size == 0)) {
    	if (pagesToCrawl == NULL || pagesToCrawl->size == 0) {
        	return NULL;
   	 }

   	webpage_t *page = pagesToCrawl->pages[0];

    	for (size_t i = 0; i < pagesToCrawl->size - 1; ++i) {
        	pagesToCrawl->pages[i] = pagesToCrawl->pages[i + 1];
   	 }

    	pagesToCrawl->size--; 	
        webpage_t *current = page;

        depth = current->depth;
	if (depth >  maxDepth){
		break;
	}
        if (current->html == NULL) {

            
            printf("herey:\n\n\n");
            
            size_t content_size;
            char *content = download(current->url, &content_size);
            
            printf("The URL: %s\n", current->url);
            if (content == NULL) {
    		printf("Failed to download: %s\n", current->url);
    		free(current->url);
    		free(current);
    		continue;  
		}

            if (content != NULL) {
                
                current->html = content; 
    
    
                
                
                printf("%d   Fetched: %s\n", current->depth, current->url);
                printf("%d   Scanning: %s\n\n\n", current->depth, current->url);
                pageScan(current, pagesToCrawl, pagesSeen); 
                
                
            } 
        } else {
        
            printf("%d   Fetched: %s\n", current->depth, current->url);
            printf("%d   Scanning: %s\n", current->depth, current->url);
            pageScan(current, pagesToCrawl, pagesSeen); 
        }

      
        pagedir_save(current, pageDirectory, documentID);
        
        documentID++;

        free(current->url);
        free(current);
    }
    hashtable_delete(pagesSeen, NULL);
    delete_bag(pagesToCrawl);
}


int main(const int argc, char *argv[]) {
	
    char *seedURL;
    char *pageDirectory;
    int maxDepth;

    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);

    crawl(seedURL, pageDirectory, maxDepth);

    return EXIT_SUCCESS;

}
