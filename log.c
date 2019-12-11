#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cJSON.h"
#include "log.h"

// log选项
uint32_t g_log_option;
int g_log_file_size = 1 * 1024 * 1024;
char g_log_file_path_1[FILE_PATH_STR_SIZE] = "./test1.log";
char g_log_file_path_2[FILE_PATH_STR_SIZE] = "./test2.log";
char g_etc_file_path[FILE_PATH_STR_SIZE] = ETC_FILE_PATH;

//打印等级，等级越高，打印越少
uint8_t g_log_level = LOG_LEVEL_ALL;
uint8_t g_print_level = PRINT_LEVEL_ALL;

// 打印选项
uint32_t g_print_option;

static int read_log_conf_file(char *file_path);
static int analyze_conf(char *buf);
static void print_cjson_type(int type);

void clear_option(uint32_t *g_option, int option);
int get_option(uint32_t *g_option, int option);

void log_init(void)
{
    read_log_conf_file(g_etc_file_path);
    return;
}

void elog(uint8_t level, char *file, int line, const char *format, ...)
{
    log_init();

    va_list arg;

    if (level <= g_print_level)
    {
        //时间
        if (get_option(&g_print_option, PRINT_TIME) || get_option(&g_log_option, LOG_TIME))
        {
            //打印
            time_t time_log = time(NULL);
            // struct tm *tm_log = localtime(&time_log);
            time_log += 28800;
            struct tm *tm_log = gmtime(&time_log);
            struct timeval tv;
            gettimeofday(&tv, NULL);

            if (get_option(&g_print_option, PRINT_TIME))
            {
                //打印
                printf("[%04d-%02d-%02d %02d:%02d:%02d.%03d]", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec, (int)(tv.tv_usec / 1000));
            }
        }

        //行号
        if (get_option(&g_print_option, PRINT_LINE))
        {
            //打印
            printf("[%s:%d]", file, line);
        }

        if (level <= g_print_level)
        {
            //打印
            va_start(arg, format);
            vprintf(format, arg);
            va_end(arg);
        }
    }

    if (level <= g_log_level)
    {

        FILE *pFile = fopen(g_log_file_path_1, "a+");
        if (pFile == NULL)
        {
            perror("fopen error");
            return;
        }

        if (get_option(&g_log_option, LOG_TIME))
        {
            time_t time_log = time(NULL);
            // struct tm *tm_log = localtime(&time_log);
            time_log += 28800;
            struct tm *tm_log = gmtime(&time_log);
            struct timeval tv;
            gettimeofday(&tv, NULL);
            //写入log文件
            fprintf(pFile, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec, (int)(tv.tv_usec / 1000));
        }

        if (get_option(&g_log_option, LOG_LINE))
        {
            //写入log文件
            fprintf(pFile, "[%s:%d]", file, line);
        }

        if (level <= g_log_level)
        {
            //写入log文件
            va_start(arg, format);
            vfprintf(pFile, format, arg);
            va_end(arg);
        }

        fflush(pFile);

        fclose(pFile);
    }

    return;
}

void set_option(uint32_t *g_option, int option)
{
    *g_option |= option;
}

void clear_option(uint32_t *g_option, int option)
{
    *g_option &= ~option;
}

int get_option(uint32_t *g_option, int option)
{
    return (*g_option & option);
}

/* 备份删除log */
void bkpANDdelet_logFile(void)
{
    struct stat buf;
    stat(g_log_file_path_1, &buf);
    if (get_option(&g_print_option, PRINT_D))
        printf("%s file size = %d\n", g_log_file_path_1, (int)buf.st_size);
    if (buf.st_size > g_log_file_size)
    {
        char cmd[210] = {0};
        sprintf(cmd, "mv %s %s", g_log_file_path_1, g_log_file_path_2);
        system(cmd);
    }
}

void *log_loop_thread(void *arg)
{
    while (1)
    {
        /* 备份删除log */
        bkpANDdelet_logFile();
        sleep(10);
    }
}

void log_loop_init(void)
{
    pthread_t log_loop_thread_t;

    if (pthread_create(&log_loop_thread_t, NULL, log_loop_thread, NULL) == 0)
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("log_loop create success\n");
    }
    else
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("log_loop thread create error\n");
        exit(1);
    }
}

static int read_log_conf_file(char *file_path)
{
    FILE *fd;
    fd = fopen(file_path, "r");
    if (NULL == fd)
    {
        perror(file_path);
        return EXIT_FAILURE;
    }

    char buf[CONFIG_FILE_SIZE] = {0};

    fread(buf, 1, CONFIG_FILE_SIZE - 1, fd);

    if (get_option(&g_print_option, PRINT_D))
        printf("config_file:\n%s\n", buf);

    analyze_conf(buf);

    fclose(fd);
    return EXIT_SUCCESS;
}

/* 解析配置文件内容 */
static int analyze_conf(char *buf)
{
    cJSON *root;
    cJSON *pJson;

    root = cJSON_Parse(buf);
    if (root == NULL)
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("cjson parse mosquitto message failed\n");
        return EXIT_FAILURE;
    }

    if (get_option(&g_print_option, PRINT_D))
        printf("config file analyze success\n");

    pJson = cJSON_GetObjectItem(root, "log_level");
    if (pJson)
    {
        if (pJson->type == cJSON_Number)
        {
            g_log_level = pJson->valueint;
        }
        else
        {
            if (get_option(&g_print_option, PRINT_D))
            {
                printf("type is error : 0x%02x ", pJson->type);
                print_cjson_type(pJson->type);
                printf("\n");
            }
        }
    }
    else
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("log_level analyze error\n");
    }

    pJson = cJSON_GetObjectItem(root, "log_time");
    if (pJson)
    {
        if (pJson->type == cJSON_True)
        {
            set_option(&g_log_option, PRINT_TIME);
        }
        else
        {
            clear_option(&g_log_option, PRINT_TIME);
            if (get_option(&g_print_option, PRINT_D))
            {
                printf("type is error : 0x%02x ", pJson->type);
                print_cjson_type(pJson->type);
                printf("\n");
            }
        }
    }
    else
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("log_time analyze error\n");
    }

    pJson = cJSON_GetObjectItem(root, "log_line");
    if (pJson)
    {
        if (pJson->type == cJSON_True)
        {
            set_option(&g_log_option, PRINT_LINE);
        }
        else
        {
            clear_option(&g_log_option, PRINT_LINE);
            if (get_option(&g_print_option, PRINT_D))
            {
                printf("type is error : 0x%02x ", pJson->type);
                print_cjson_type(pJson->type);
                printf("\n");
            }
        }
    }
    else
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("log_line analyze error\n");
    }

    pJson = cJSON_GetObjectItem(root, "print_level");
    if (pJson)
    {
        if (pJson->type == cJSON_Number)
        {
            g_print_level = pJson->valueint;
        }
        else
        {
            if (get_option(&g_print_option, PRINT_D))
            {
                printf("type is error : 0x%02x ", pJson->type);
                print_cjson_type(pJson->type);
                printf("\n");
            }
        }
    }
    else
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("print_level analyze error\n");
    }

    pJson = cJSON_GetObjectItem(root, "print_time");
    if (pJson)
    {
        if (pJson->type == cJSON_True)
        {
            set_option(&g_print_option, PRINT_TIME);
        }
        else
        {
            clear_option(&g_print_option, PRINT_TIME);
            if (get_option(&g_print_option, PRINT_D))
            {
                printf("type is error : 0x%02x ", pJson->type);
                print_cjson_type(pJson->type);
                printf("\n");
            }
        }
    }
    else
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("print_time analyze error\n");
    }

    pJson = cJSON_GetObjectItem(root, "print_line");
    if (pJson)
    {
        if (pJson->type == cJSON_True)
        {
            set_option(&g_print_option, PRINT_LINE);
        }
        else
        {
            clear_option(&g_print_option, PRINT_LINE);
            if (get_option(&g_print_option, PRINT_D))
            {
                printf("type is error : 0x%02x ", pJson->type);
                print_cjson_type(pJson->type);
                printf("\n");
            }
        }
    }
    else
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("print_line analyze error\n");
    }

    pJson = cJSON_GetObjectItem(root, "log_file_size");
    if (pJson)
    {
        if (pJson->type == cJSON_Number)
        {
            g_log_file_size = pJson->valueint * 1024 * 1024;
        }
        else
        {
            if (get_option(&g_print_option, PRINT_D))
            {
                printf("type is error : 0x%02x ", pJson->type);
                print_cjson_type(pJson->type);
                printf("\n");
            }
        }
    }
    else
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("log_file_size analyze error\n");
    }

    pJson = cJSON_GetObjectItem(root, "log_file_path_1");
    if (pJson)
    {
        if (pJson->type == cJSON_String)
        {
            memset(g_log_file_path_1, 0, FILE_PATH_STR_SIZE);
            strncpy(g_log_file_path_1, pJson->valuestring, FILE_PATH_STR_SIZE - 1);
        }
        else
        {
            if (get_option(&g_print_option, PRINT_D))
            {
                printf("type is error : 0x%02x ", pJson->type);
                print_cjson_type(pJson->type);
                printf("\n");
            }
        }
    }
    else
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("log_file_path_1 analyze error\n");
    }

    pJson = cJSON_GetObjectItem(root, "log_file_path_2");
    if (pJson)
    {
        if (pJson->type == cJSON_String)
        {
            memset(g_log_file_path_2, 0, FILE_PATH_STR_SIZE);
            strncpy(g_log_file_path_2, pJson->valuestring, FILE_PATH_STR_SIZE - 1);
        }
        else
        {
            if (get_option(&g_print_option, PRINT_D))
            {
                printf("type is error : 0x%02x ", pJson->type);
                print_cjson_type(pJson->type);
                printf("\n");
            }
        }
    }
    else
    {
        if (get_option(&g_print_option, PRINT_D))
            printf("log_file_path_2 analyze error\n");
    }

    if (root)
    {
        cJSON_Delete(root);
        root = NULL;
    }

    return EXIT_SUCCESS;
}

static void print_cjson_type(int type)
{
    switch (type)
    {
    case cJSON_Invalid:
        printf("cJSON_Invalid");
        break;
    case cJSON_False:
        printf("cJSON_False");
        break;
    case cJSON_True:
        printf("cJSON_True");
        break;
    case cJSON_NULL:
        printf("cJSON_NULL");
        break;
    case cJSON_Number:
        printf("cJSON_Number");
        break;
    case cJSON_String:
        printf("cJSON_String");
        break;
    case cJSON_Array:
        printf("cJSON_Array");
        break;
    case cJSON_Object:
        printf("cJSON_Object");
        break;
    case cJSON_Raw:
        printf("cJSON_Raw");
        break;
    default:
        break;
    }
}
