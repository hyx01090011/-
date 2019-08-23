#include"db.hpp"

//单元测试，一个函数一个函数的去测试
//专业的单元测试框架：Google test，boost::unittest,....
void TestDishTable()
{
  MYSQL* mysql= order_system::MySQLInit();
  order_system::DishTable dish_table(mysql);
  Json::StyledWriter writer;

  //1.插入数据
  //Json::Value dish;
  //dish["name"]="宫保鸡丁";
  //dish["price"]=1000;
  //bool ret = dish_table.Insert(dish);
  //printf("ret = %d\n",ret);

  //2.查找所有数据
  //Json::Value dishes;
  //int ret=dish_table.SelectAll(&dishes);
  //printf("ret= %d\n",ret);
  //Json::StyledWriter writer;
  //printf("%s\n",writer.write(dishes).c_str());

  //3.查找指定数据
  //Json::Value dish;
  //int ret = dish_table.SelectOne(1,&dish);
  //printf("ret=%d\n",ret);
  //printf("%s\n",writer.write(dish).c_str());

  //4.修改指定数据
  //Json::Value dish;
  //dish["dish_id"]=1;
  //dish["name"]="红烧鱼块";
  //dish["price"]=3000;
  //int ret=dish_table.Update(dish);
  //printf("ret=%d\n",ret);

  //5.删除指定数据
  //int ret=dish_table.Delete(1);
  //printf("ret=%d\n",ret);
  
  order_system::MySQLRelease(mysql);
}
void TestOrderTable()
{
  Json::StyledWriter writer;
  MYSQL* mysql = order_system::MySQLInit();
  order_system::OrderTable order_table(mysql);
  
  //1.插入一个订单
  //Json::Value order;
  //order["table_id"]="少林派";
  //order["time"]="2019/08/03 14:30";
  //order["dishes"]="[1, 2, 3]";
  //order["status"]=1;

  //int ret = order_table.Insert(order);
  //printf("ret = %d\n",ret);
  
  //2.查看所有订单
  //Json::Value orders;
  //int ret=order_table.SelectAll(&orders);
  //printf("ret =%d\n",ret);
  //printf("%s\n",writer.write(orders).c_str());
  
  Json::Value order;
  order["order_id"]=6;
  order["status"]=1;
  int ret=order_table.ChangeState(order);
  printf("ret=%d\n",ret);

  order_system::MySQLRelease(mysql);
}
int main()
{
  //TestDishTable();

  TestOrderTable();
  return 0;
}
