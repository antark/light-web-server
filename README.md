###使用 c 实现的一个并发的web服务器。具有基本功能###

<sup><sub>1. 部分实现了 HTTP/1.1，可以提供基本的 web 静态服务</sub></sup><br />
<sup><sub>2. 允许并发</sub></sup><br />

###使用方式###

<sup><sub>$ make   // 编译代码</sub></sup><br />
<sup><sub>$ ./ws 1234   // 启动server进程，端口是 1234</sub></sup><br />

<sup><sub>浏览器中访问：http://localhost:12345/webapp/demo/assets/img/miaoxingren.jpg</sub></sup><br />

###知识###
<sup><sub>1. c 语言基础</sub></sup><br />
<sup><sub>2. 数据结构 —— 循环队列</sub></sup><br />
<sup><sub>3. 操作系统 —— 并发处理</sub></sup><br />
<sup><sub>4. 网络 —— TCP/HTTP 协议</sub></sup><br />
<sup><sub>5. linux socket api</sub></sup><br />
