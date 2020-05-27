# Integration Server
个人整合服务端  
集合了工作时所接触的工具层,网络层,及测试例子  
  
## 总览
编码主体采用c++11  
目前支持主体模块: proto protocpp  
  
## Proto
通讯协议序列化方案采用protobuf  
执行脚本 bash gen_proto.sh 生成协议枚举,协议代码文件  
* protos 目录内为.proto协议  
* op_cmd 目录内为数据源及协议枚举文件模板  
  
## Protocpp
协议枚举,协议代码文件的存放目录  
使用Makefile 编译成libprotocpp.a  
  
## Utils
基础工具模块  

## Boost network
基于boost::asio实现的网络层

