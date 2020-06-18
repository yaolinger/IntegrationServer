# Integration Server
个人整合服务端  

## 总览
编码主体采用c++11  
目前支持主体模块: proto protocpp utils boost_network test single_server robot  
编译运行环境搭建: git clone https://github.com/yaolinger/env_script.git

## Proto
通讯协议序列化方案采用protobuf  
执行脚本 bash gen_proto.sh 生成协议枚举,协议代码文件  
* protos 目录内为.proto协议  
* op_cmd 目录内为数据源及协议枚举文件模板  
  
## Protocpp
协议枚举,协议代码文件的存放目录  

## Utils
基础工具模块,常用工具类  

## Boost_network
基于boost::asio实现的网络层  

## Service_core
基于不同的网络方案实现的服务核心模块  

## Test
测试模块  
用于测试utils boost_network  

## Single_server
boost服务器示例  

## Robot
机器人, 用于协议测试  
