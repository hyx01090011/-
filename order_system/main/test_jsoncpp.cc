#include<cstdio>
#include<jsoncpp/json/json.h>
#include<cstdlib>
int main()
{
  Json::Value value;
  value["dish_id"]=1;
  value["name"]="红烧肉";
  value["price"]=2000;

  Json::FastWriter writer;
  const std::string&str =writer.write(value);
  //clang另外一个更高端的编译器
  printf("%s\n",str.c_str());
  std::string str2=R"({"name","红烧肉","price":100})";
  Json::Reader reader;
  Json::Value output;
  reader.parse(str2,output);
  printf("%s,%d\n",output["name"].asCString(),
      output["price"].asInt());
  return 0;
}
