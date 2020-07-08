# Integration Server
写着玩的,未经历线上考验,不要偷看哦~~~  
集合服务端,把日常的工具集,网络模型,线程模型等集合到一起~  

## 总览
编码主体采用: c++11  
编译器版本: gcc version 5.4.0  
python版本: 2.7.12  
python第三方: mako  
目前支持主体模块: proto protocpp utils boost_network test single_server robot  
项目部署:  
1. 代码拉取: git clone https://github.com/yaolinger/IntegrationServer.git  
2. 编译运行环境脚本拉取: git clone https://github.com/yaolinger/env_script.git  
3. 编译前保证gcc版本  
4. 第三方环境搭建: https://github.com/yaolinger/env_script/blob/master/README.md  
5. 进入工作目录编译: make -j  

## Proto
通讯协议序列化方案采用protobuf  
执行脚本 bash gen_proto.sh 生成协议枚举,协议代码文件  
代码生成依赖于python, 需要搭建python环境及其所需的依赖库  
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
