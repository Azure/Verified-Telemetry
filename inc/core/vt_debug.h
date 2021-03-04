#ifndef _VT_DEBUG_H
#define _VT_DEBUG_H

#define PRINT_DEBUG 0

typedef enum DEBUG_STATE
{
    DEBUG = 0,
    USER  = 1
} DEBUG_STATE;


#define VT_DEBUG_PRINT(MODE, ...)  do { if (MODE || PRINT_DEBUG) printf(__VA_ARGS__);} while (0);


#endif