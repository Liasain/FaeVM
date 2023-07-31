#pragma once

#ifndef unreachable
#define unreachable() (__builtin_unreachable())
#endif

static const char *ANSI_RED       = "\e[1;91m";
static const char *ANSI_GREEN     = "\e[1;92m";
static const char *ANSI_YELLOW    = "\e[1;93m";
static const char *ANSI_BLUE      = "\e[1;94m";
static const char *ANSI_MAGENTA   = "\e[1;95m";
static const char *ANSI_RESET     = "\e[1;39m";
static const char *ANSI_RESET_ALL = "\e[0m";

#define ERR   (0)
#define WARN  (1)
#define INFO  (2)
#define DEBUG (3)
#define TRACE (4)

#define LOG_LEVEL DEBUG

#define err(fmt, args...)   do { fprintf(stderr, "ERROR " "%s:%s:%d: " fmt "\n", __FILE__, __func__, __LINE__, ##args); } while(0)
#define warn(fmt, args...)  do { fprintf(stderr, "WARN  " "%s:%s:%d: " fmt "\n", __FILE__, __func__, __LINE__, ##args); } while(0)
#define info(fmt, args...)  do { fprintf(stderr, "INFO  " "%s:%s:%d: " fmt "\n", __FILE__, __func__, __LINE__, ##args); } while(0)
#define debug(fmt, args...) do { fprintf(stderr, "DEBUG " "%s:%s:%d: " fmt "\n", __FILE__, __func__, __LINE__, ##args); } while(0)
#define trace(fmt, args...) do { fprintf(stderr, "TRACE " "%s:%s:%d: " fmt "\n", __FILE__, __func__, __LINE__, ##args); } while(0)

#if LOG_LEVEL < WARN 
#undef warn
#define warn(...) do{}while(0)
#endif

#if LOG_LEVEL < INFO
#undef info
#define info(...) do{}while(0)
#endif

#if LOG_LEVEL < DEBUG
#undef debug
#define debug(...) do{}while(0)
#endif

#if LOG_LEVEL < TRACE
#undef trace
#define trace(...) do{}while(0)
#endif


#if defined(DEBUG) && DEBUG > 0
 #define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
 #define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif


