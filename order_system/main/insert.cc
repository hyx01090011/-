// .cc c++源代码文件
// cpp cxx
#include<cstdio>
#include<cstdlib>

//mysql第三方库，在第三方库的时候
//一般只需要包含一个文件就可以了
#include<mysql/mysql.h>
int main()
{
	//1.创建一个数据库的操作“句柄”（遥控器）
	MYSQL* mysql = mysql_init(NULL);
	//2.建立句柄和数据库服务器之间的联系（连接服务器）
	if(mysql_real_connect(mysql,"127.0.0.1","root","123456","order_system2",3306,NULL,0)==NULL)//如何快速给一段代码加上括号：1.可视模式选中一段代码  2.shift+s   3.按括号
  {
    printf("connect failed!%s\n",mysql_error(mysql));
    return 1;
  }
  
  //3.设置编码格式(让mysql服务器的编码方式和客户端的方式匹配)
  //utf_8
  
  mysql_set_character_set(mysql,"utf8");
  
  //4.拼装一个插入数据的SQL
  char sql[1024]={0};
  int price=2000;
  sprintf(sql,"insert into dish_table values(null,'京酱肉丝',%d)",price);
 
  //5.执行SQL语句
  int ret=mysql_query(mysql,sql);
  if(ret!=0)
  {
    printf("sql执行失败！%s\n",mysql_error(mysql));
    return 1;
  }
  
  //6.关闭连接
  mysql_close(mysql);
	return 0;
}
