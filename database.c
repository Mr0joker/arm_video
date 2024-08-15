#include "database.h"
#include <stdio.h>
#include <string.h>


// 定义全局变量来存储获取到的密码
static char *password = NULL;

int callback(void *data, int argc, char **argv, char **azColName) 
{
    if (argc > 0 && argv[0]!= NULL) 
    {
        password = strdup(argv[0]);
    }
    return 0;
}

char* get_usr_passwd(sqlite3 *updb,char *USRNAME)
{
    //打开一个数据库连接对象
    int r = sqlite3_open("./pswd.db",&updb);
    if(r != SQLITE_OK)
    {
        perror("sqlite3_open failed");
        return NULL;
    }
    char *zErrMsg = 0;
    //准备一个SQL语句对象
    char sql[256];
    sprintf(sql,"SELECT PASSWORD FROM USRLOGIN WHERE USRNAME= %s ",USRNAME);
    
    //执行准备好的SQL语句
    r = sqlite3_exec(updb, sql, callback, 0, &zErrMsg);
    if (r!= SQLITE_OK) {
        fprintf(stderr, "SQL 错误: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    sqlite3_close(updb);
    return password;
}