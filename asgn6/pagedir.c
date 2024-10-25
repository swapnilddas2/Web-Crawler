#include <stdio.h>
#include <stdlib.h>
#include "pagedir.h"
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

bool pagedir_init(const char *pageDirectory) {
    struct stat directoryStat;
    if (pageDirectory == NULL || *pageDirectory == '\0') {
        fprintf(stderr, "Invalid page director\n");
        return false;
    }

    if (stat(pageDirectory, &directoryStat) == -1) {
        fprintf(stderr, "Page directory does not exist\n");
        return false;
    }

    if (!S_ISDIR(directoryStat.st_mode)) {
        fprintf(stderr, "Path is not a directory\n");
        return false;
    }

    if (access(pageDirectory, W_OK) == -1) {
        fprintf(stderr, "Directory is not writable\n");
        return false;
    }
    return true;
}


void pagedir_save(const webpage_t *page, const char *pageDirectory, const int documentID) {
    
    if (documentID == 1){
    	char file[100];
    	strcpy(file,pageDirectory);
    	strcat(file,"/");
    	strcat(file,pageDirectory);
    	strcat(file,".crawler");
    	fopen(file,"w");
    }
    if (page == NULL || pageDirectory == NULL || *pageDirectory == '\0') {
        fprintf(stderr, "Invalid page or page directory\n");
        return;
    }

    char filename[100];
    snprintf(filename, sizeof(filename), "%s/%d", pageDirectory, documentID);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to create file\n");
        return;
    }

    fprintf(file, "%s\n%d\n%s", page->url, page->depth, page->html);

    fclose(file);
}
