# Integration Server
个人整合服务端  

## 总览
编码主体采用c++11  
编译器版本: gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.10)  
目前支持主体模块: proto protocpp utils boost_network test single_server robot  
项目部署顺序:  
1. 代码拉取: git clone https://github.com/yaolinger/IntegrationServer.git  
2. 编译运行环境脚本拉取: git clone https://github.com/yaolinger/env_script.git  
3. 阅读并生成代码环境: https://github.com/yaolinger/env_script/blob/master/README.md  
4. 进入工作目录编译: make -j  

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
