## linux c 日志系统

1. 配置文件<br>
./etc/log_conf.json
```
{
    "log_level": 4,
    "print_time": true,
    "print_line": true,
    "log_file_size":100,
    "log_file_path_1": "./log/test_1.log",
    "log_file_path_2": "./log/test_2.log"
}

log_level: 打印等级 0：禁止打印; 1：打印log_a();2:打印log_a()、log_b() …… 4:全部打印
print_time：是否打印时间
print_line： 是否打印文件名和行号
log_file_size: 日志文件大小，单位MB
log_file_path_1：日志文件1
log_file_path_2：日志文件2，日志文件1超过log_file_size大小后，覆盖掉日志文件2
```
2. 命令行选项
-d 开启程序一开始读配置文件的打印

3. 宏
由于本工程就算设置打印等级为0,也会有去读取配置文件的运行消耗，
若追求极致效率，可用宏在编译阶段进行关闭;
LOG_LEVEL_STATIC_CURRENT 设置这个宏的值可选择性编译log_a()、log_b()、log_c()、log_d()