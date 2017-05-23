#ifndef	__J2735_MEM_H__
#define	__J2735_MEM_H__

#include <tx_api.h>

void j2735_init(void);
void j2735_exit(void);
void *j2735_calloc(unsigned int nmemb, unsigned int size);
void *j2735_malloc(unsigned int size);
void *j2735_realloc(void *ptr, unsigned int size);
void j2735_free(void *ptr);

#endif
