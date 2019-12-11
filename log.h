#ifndef __LOG_H
#define __LOG_H

#include <stdint.h>

#define ETC_FILE_PATH "./log_conf.json"
#define CONFIG_FILE_SIZE 5000

// #define LOG_FILE_PATH "./test.log"
#define FILE_PATH_STR_SIZE 100

// 日志选项
#define LOG_TIME (1 << 1) // 时间
#define LOG_LINE (1 << 2) // 文件

// 日志等级 值越低,等级越高
#define LOG_LEVEL_0 0
#define LOG_LEVEL_A 1
#define LOG_LEVEL_B 2
#define LOG_LEVEL_C 3
#define LOG_LEVEL_D 4

#define LOG_LEVEL_ALL 4

//编译时期控制日志等级
#define LOG_LEVEL_STATIC_0 0
#define LOG_LEVEL_STATIC_A 1
#define LOG_LEVEL_STATIC_B 2
#define LOG_LEVEL_STATIC_C 3
#define LOG_LEVEL_STATIC_D 4

#define LOG_LEVEL_STATIC_CURRENT 4

//打印选项
#define PRINT_D (1 << 0)    // 程序开始的printf使能
#define PRINT_TIME (1 << 1) // 时间
#define PRINT_LINE (1 << 2) // 文件

// 日志等级 值越低,等级越高
#define PRINT_LEVEL_0 0
#define PRINT_LEVEL_A 1
#define PRINT_LEVEL_B 2
#define PRINT_LEVEL_C 3
#define PRINT_LEVEL_D 4

#define PRINT_LEVEL_ALL 4

// #define LOG_FILE_SIZE 1024 * 1024 * 10
// #define LOG_FILE_PATH_1 "./log/test1.log"
// #define LOG_FILE_PATH_2 "./log/test2.log"

extern char g_etc_file_path[FILE_PATH_STR_SIZE];
extern uint8_t g_log_level;
uint32_t g_print_option;

// void log_write(char *str);          /* 写log */
// void log_loop_init(void);

void log_init(void);
void log_loop_init(void);
void elog(uint8_t level, char *file, int line, const char *format, ...);
void set_option(uint32_t *g_option, int option);


#if LOG_LEVEL_STATIC_CURRENT >= LOG_LEVEL_STATIC_A
#define log_a(...) elog(LOG_LEVEL_A, __FILE__, __LINE__, __VA_ARGS__)
#else
#define log_a(...) ((void)0);
#endif

#if LOG_LEVEL_STATIC_CURRENT >= LOG_LEVEL_STATIC_B
#define log_b(...) elog(LOG_LEVEL_B, __FILE__, __LINE__, __VA_ARGS__)
#else
#define log_b(...) ((void)0);
#endif

#if LOG_LEVEL_STATIC_CURRENT >= LOG_LEVEL_STATIC_C
#define log_c(...) elog(LOG_LEVEL_C, __FILE__, __LINE__, __VA_ARGS__)
#else
#define log_c(...) ((void)0);
#endif

#if LOG_LEVEL_STATIC_CURRENT >= LOG_LEVEL_STATIC_D
#define log_d(...) elog(LOG_LEVEL_D, __FILE__, __LINE__, __VA_ARGS__)
#else
#define log_d(...) ((void)0);
#endif

#endif
