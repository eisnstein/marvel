#ifndef __util_h__
#define __util_h__

#define verify_memory(M) { \
    if (M == NULL) { \
        fprintf(stderr, "[ERROR] Could not allocate memory.\n"); \
        exit(-1); \
    } \
}

#endif