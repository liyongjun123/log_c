## linux c 日志系统

1. 配置文件<br>
log_conf.json
```
{
    "print_level": 4,
    "print_time": true,
    "print_line": true,

    "log_level": 4,
    "log_time": true,
    "log_line": true,
    "log_file_size":10,
    "log_file_path_1": "./test1.log",
    "log_file_path_2": "./test2.log"
}

前台打印：
print_level：打印等级 0：禁止打印; 1：开启log_a()打印;2:开启log_a()、log_b()打印 …… 4:开启全部打印
print_time：是否打印时间
print_line：是否打印文件名和行号

日志存储：
log_level：打印等级 0：禁止打印; 1：开启log_a()打印;2:开启log_a()、log_b()打印 …… 4:开启全部打印
log_time：是否打印时间
log_line：是否打印文件名和行号
log_file_size：日志文件大小，单位MB
log_file_path_1：日志文件1，超过log_file_size大小后，覆盖日志文件2，同时删除日志文件1
log_file_path_2：日志文件2
```
2. 命令行选项<br>
-d 开启程序一开始读配置文件的打印

3. 宏<br>
由于本工程就算设置打印等级为0,也会有去读取配置文件的运行消耗，若追求极致效率，可用宏在编译阶段进行关闭;<br>
LOG_LEVEL_STATIC_CURRENT 设置这个宏可选择性编译log_a()、log_b()、log_c()、log_d()