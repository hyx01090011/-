#include"httplib.h"
#include"db.hpp"
#include<signal.h>
#include<jsoncpp/json/json.h>

const char* CONTENT_TYPE="application/json";
MYSQL* mysql=NULL;

int main() 
{
  using namespace httplib;
  mysql=order_system::MySQLInit();
  signal(SIGINT,[](int)
      {
        order_system::MySQLRelease(mysql);
        exit(0);
      });
  order_system::DishTable dish_table(mysql);
  order_system::OrderTable order_table(mysql);

  Server server;
  server.Post("/dish",[&dish_table](const Request& req,
        Response& resp)
      {
        Json::Value req_json;
        Json::Value resp_json;
        Json::Reader reader;
        Json::FastWriter writer;
        printf("新增菜品\n");
        //1.获取到数据并解析成json格式
        bool ret=reader.parse(req.body,req_json);
        if(!ret)
        {
         printf("parse body failed\n!");
         resp_json["ok"]=false;
         resp_json["reason"]="parse body failed!";
         resp.status=400;
         resp.set_content(writer.write(resp_json),CONTENT_TYPE);
         return;
        }
        //2.校验JSON中的信息
        if(req_json["name"].empty()||req_json["price"].empty())
        {
         printf("Request json error format!\n");
         resp_json["ok"]=false;
         resp_json["reason"]="Request json error format!";
         resp.status=400;
         resp.set_content(writer.write(resp_json),CONTENT_TYPE);
         return;
        }
        //3.调用数据库操作来进行插入数据
        ret=dish_table.Insert(req_json); 
        if(!ret)
        {
         printf("dish_table insert failed!\n");
         resp_json["ok"]=false;
         resp_json["reason"]="dish_table insert failed!";
         resp.status=500;
         resp.set_content(writer.write(resp_json),CONTENT_TYPE);
         return;
        }
        //4.构造正确的响应
        resp_json["ok"]=true;
        resp.status=200;
        resp.set_content(writer.write(resp_json),CONTENT_TYPE);
        return;
      });

  server.Get("/dish",[&dish_table](const Request& req,
        Response& resp)
      {
        (void) req;
        Json::Value resp_json;
        Json::FastWriter writer;
        printf("查看所有菜品!");
        //1.不需要处理输入数据，无需校验，直接调用数据库操作
        bool ret=dish_table.SelectAll(&resp_json);
        if(!ret)
        {
          printf("select all dish failed\n");
          resp_json["ok"]=false;
          resp_json["reason"]="select all dish failed!";
          resp.status=500;
          resp.set_content(writer.write(resp_json),CONTENT_TYPE);
          return;
        }
        //2.构造正确的响应
        resp.status=200;
        resp.set_content(writer.write(resp_json),CONTENT_TYPE);
        return;
      });
  //正则表达式
  server.Get(R"(/dish/(\d+))",[&dish_table](const Request& req, //\d(1~9的阿拉伯数字),+代表前面的数字可以重复出现一次或多次
        Response& resp)
      {
        Json::Value resp_json;
        Json::FastWriter writer;
        //std::to_string()
        //1.获取到dish_id
        int32_t dish_id=std::stoi(req.matches[1]);
        printf("获取到编号为%d的菜品\n",dish_id);
        //2.执行数据库操作
        bool ret=dish_table.SelectOne(dish_id,&resp_json);
        if(!ret)
        {
          resp_json["reason"]="dish_table select one failed!";
          resp.status=500;
          resp.set_content(writer.write(resp_json),CONTENT_TYPE);
          return;
        }
        //3.构造一个正确的响应
        resp.status=200;
        resp.set_content(writer.write(resp_json),CONTENT_TYPE);
        return;
      });
    
  server.Put(R"(/dish/(\d+))",[&dish_table](const Request& req,
        Response& resp)
      {
        Json::Value req_json;
        Json::Value resp_json;
        Json::Reader reader;
        Json::FastWriter writer;
        //1.获取到dish_id
        int32_t dish_id=std::stoi(req.matches[1]);
        printf("修改编号为%d的菜品\n",dish_id);
        //2.获取到数据并解析成JSON格式
        bool ret=reader.parse(req.body,req_json);
        if(!ret)
        {
         printf("parse body failed!");
         resp_json["ok"]=false;
         resp_json["reason"]="parse body failed!";
         resp.status=400;
         resp.set_content(writer.write(resp_json),CONTENT_TYPE);
         return;
        }
        //2.校验JSON中的信息
        if(req_json["name"].empty()||req_json["price"].empty())
        {
         printf("Request json error format!");
         resp_json["ok"]=false;
         resp_json["reason"]="Request json error format!";
         resp.status=400;
         resp.set_content(writer.write(resp_json),CONTENT_TYPE);
         return;
        }
        //3.调用数据库操作来进行插入数据
        req_json["dish_id"]=dish_id;//要把dish_id加入到json
        ret=dish_table.Update(req_json); 
        if(!ret)
        {
         printf("dish_table update failed!\n");
         resp_json["ok"]=false;
         resp_json["reason"]="dish_table update failed!";
         resp.status=500;
         resp.set_content(writer.write(resp_json),CONTENT_TYPE);
         return;
        }
        //4.构造正确的响应
        resp_json["ok"]=true;
        resp.status=200;
        resp.set_content(writer.write(resp_json),CONTENT_TYPE);
        return;
      });
  server.Delete(R"(/dish/(\d+))",[&dish_table](const Request& req,
        Response& resp)
      {
        Json::Value resp_json;
        Json::FastWriter writer;
        //1.获取到dish_id
        int32_t dish_id=std::stoi(req.matches[1]);
        printf("删除编号为%d的菜品\n",dish_id);
        //2.执行数据库操作
        bool ret=dish_table.Delete(dish_id);
        if(!ret)
        {
          printf("dish_table delete failed!\n");
          resp_json["ok"]=false;
          resp_json["reason"]="dish_table delete failed!";
          resp.status=500;
          resp.set_content(writer.write(resp_json),CONTENT_TYPE);
          return;
        }
        //3.构造正确的响应
        resp_json["ok"]=true;
        resp.status=200;
        resp.set_content(writer.write(resp_json),CONTENT_TYPE);
        return;
      });
  server.Post("/order",[&order_table](const Request& req,
        Response& resp)
      {
        Json::Value req_json;
        Json::Value resp_json;
        Json::Reader reader;
        Json::FastWriter writer;
        printf("新增一个订单\n");
        //1.获取到body数据并解析 
        bool ret=reader.parse(req.body,req_json);
        if(!ret)
        {
          printf("order_table parse failed!\n");
          resp_json["ok"]=false;
          resp_json["reason"]="order  parse failed!";
          resp.status=400;
          resp.set_content(writer.write(resp_json),CONTENT_TYPE);
          return;
        }
        //2.校验body中的数据是否符合要求
        if(req_json["table_id"].empty()
            ||req_json["time"].empty()
            ||req_json["dish_ids"].empty())
        {
          printf("order_table format error failed!\n");
          resp_json["ok"]=false;
          resp_json["reason"]="order format error failed!";
          resp.status=400;
          resp.set_content(writer.write(resp_json),CONTENT_TYPE);
          return;
        }
        //3.构造一些其他需要的字段
        req_json["status"]=1;// 1 表示订单进行中， 0 表示订单关闭
        //需要把dish_ids(json数组)这个字段转换成dishes(字符串)
        const Json::Value& dish_ids=req_json["dish_ids"];
        req_json["dishes"]=writer.write(dish_ids);
        //4.调用数据库操作，来插入数据
        ret=order_table.Insert(req_json);
        if(!ret)
        {
          printf("order_table insert failed!\n");
          resp_json["ok"]=false;
          resp_json["reason"]="order insert failed!";
          resp.status=500;
          resp.set_content(writer.write(resp_json),CONTENT_TYPE);
          return;
        }
        //5.构造正确的响应
        resp_json["ok"]=true;
        resp.status=200;
        resp.set_content(writer.write(resp_json),CONTENT_TYPE);
        return;
      });

  server.Get("/order",[&order_table](const Request& req,
        Response& resp)
      {
        (void) req;
        printf("获取所有订单\n");
        Json::Value resp_json;
        Json::FastWriter writer;
        //1.执行数据库操作，获取响应信息
        bool ret=order_table.SelectAll(&resp_json);
        if(!ret)
        {
          printf("order_table selectAll failed!\n");
          resp_json["ok"]=false;
          resp_json["reason"]="order selectAll failed!";
          resp.status=500;
          resp.set_content(writer.write(resp_json),CONTENT_TYPE);
          return;
        }
        //2.构造正确的响应
        resp_json["ok"]=true;
        resp.status=200;
        resp.set_content(writer.write(resp_json),CONTENT_TYPE);
        return;
      });
  server.Put(R"(/order/(\d+))",[&order_table](const Request& req,
        Response& resp)
      {
        printf("获取所有订单\n");
        Json::Value req_json;
        Json::Value resp_json;
        Json::Reader reader;
        Json::FastWriter writer;
        //1.获取到order_id
        int32_t order_id=std::stoi(req.matches[1]);
        printf("修改编号为%d的订单状态!\n",order_id);
        //2.解析请求中的status
        bool ret=reader.parse(req.body,req_json);
        if(!ret)
        {
          printf("order_table parse failed!\n");
          resp_json["ok"]=false;
          resp_json["reason"]="order  parse failed!";
          resp.status=400;
          resp.set_content(writer.write(resp_json),CONTENT_TYPE);
          return;
        }
        //3.校验参数内容
        if(req_json["status"].empty())
        {
          printf("order_table format error failed!\n");
          resp_json["ok"]=false;
          resp_json["reason"]="order format error failed!";
          resp.status=400;
          resp.set_content(writer.write(resp_json),CONTENT_TYPE);
          return;
        }
        //4.执行数据库操作
        req_json["order_id"]=order_id;
        ret=order_table.ChangeState(req_json);
        if(!ret)
        {
          printf("order_table ChangeState failed!\n");
          resp_json["ok"]=false;
          resp_json["reason"]="order ChangeState failed!";
          resp.status=500;
          resp.set_content(writer.write(resp_json),CONTENT_TYPE);
          return;
        }
        //5.构造正确的响应
        resp_json["ok"]=true;
        resp.status=200;
        resp.set_content(writer.write(resp_json),CONTENT_TYPE);
        return;
      });
  server.set_base_dir("./wwwroot");
  server.listen("0.0.0.0",9094);
  
  return 0;
}
