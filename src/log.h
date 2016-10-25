#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "main.h"

#define ALLOC	1
#define SPAM	2
#define DEBUG	3
#define STATUS	4
#define INFO	5
#define WARN	6
#define ERROR	7
#define FAIL	8

extern int loglevel;

//If compiled with -DLOG_LINENO include region and line number in all log messages
#ifdef LOG_LINENO
#define LOG(LEVEL, ALERT, ...) if(LEVEL>=loglevel){\
	printf(LOG_REGION "@%i:    \t", __LINE__);\
	printf("[" ALERT #LEVEL ALERT "]  \t");\
	printf(__VA_ARGS__);}
#else
#define LOG(LEVEL, ALERT, ...) if(LEVEL>=loglevel){\
	printf("[" ALERT #LEVEL ALERT "]  \t");\
	printf(__VA_ARGS__);}
#endif

//If compiled without the flag -DENABLE_SPAM just replace these macros away to nothing
//probably faster
#ifdef ENABLE_SPAM
#define malloc(X) malloc_hook(X, __FILE__, __LINE__)
#define LOG_ALLOC(...) LOG(ALLOC, "", __VA_ARGS__)
#define LOG_SPAM(...) LOG(SPAM, "", __VA_ARGS__)
#else
#define LOG_ALLOC
#define LOG_SPAM
#endif

#define LOG_DEBUG(...) LOG(DEBUG, "", __VA_ARGS__)
#define LOG_STATUS(...) LOG(STATUS, "", __VA_ARGS__)
#define LOG_INFO(...) LOG(INFO, "-", __VA_ARGS__)
#define LOG_WARN(...) LOG(WARN, "*", __VA_ARGS__)
#define LOG_ERROR(...) LOG(ERROR, "**", __VA_ARGS__)
#define LOG_FAIL(...) LOG(FAIL, "!!", __VA_ARGS__)

void *malloc_hook(size_t size, char *file, int line);

typedef void* malloc_sig(size_t);

#define MALLOC_NOLOG(X) ((malloc_sig*)&malloc)(X)

#endif