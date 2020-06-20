# Tinyhttpd

* 主要执行流程图：

![img](https://images2017.cnblogs.com/blog/422590/201711/422590-20171110141226856-793077190.png)

* startup(u_short *port) //先开始启动web服务器的套接字，开始监听对应的端口连接。

  > 返回创建好的套接字

  * 创建套接字

    * 地址，套接字文件描	述符

  * 初始化地址信息

    * IP版本、端口、IP地址

      > Server一般而言将IP地址赋值为INADDR_ANY（该宏值为0），即无论发送到系统中的哪个IP地址（当服务器有多张网卡时会有多个IP地址）的请求都采用该socket来处理，而无需指定固定IP。
      
    * 设置套接字可选项

      *    设置为SO_REUSEADDR是让端口释放后立即就可以被再次使用

  * 分配地址信息（使套接字关联一个相应的地址和端口号 失败返回-1

  * 判断端口号是否为0

    * 如果为0就是动态分配端口号，就要取出分配的端口号
      * getsockname得到套接字绑定的信息，和长度
      * 取出端口，ntohs将网络号转回字符串

  * 准备接受套接字上的连接

    >  在请求拒绝之前N个请求将排队连接

* 循环等待接收套接字的连接 

  > 连接到达时开启一个新的套接字与它通信

  * 要连接的地址和地址长度

* 创建线程去处理请求

  * 传入accept_request()请求处理和要处理的套接字

* 请求处理

  * 设置变量意义：字符串缓存，字符长度，标签头，url，path，是否cgi，文件属性结构，查询字符串

    > struct stat这个结构体是用来描述一个linux系统文件系统中的文件属性的结构。
    >
    > 1、通过路径：
    >
    > int stat(const char *path, struct stat *struct_stat);
    >
    > int lstat(const char *path,struct stat *struct_stat);
    >
    > 两个函数的第一个参数都是文件的路径，第二个参数是struct stat的指针。返回值为0，表示成功执行。
    >
    > 执行失败是，error被自动设置为下面的值：
    >
    > EBADF： 文件描述词无效
    >
    > EFAULT： 地址空间不可访问
    >
    > ELOOP： 遍历路径时遇到太多的符号连接
    >
    > ENAMETOOLONG：文件路径名太长
    >
    > ENOENT：路径名的部分组件不存在，或路径名是空字串
    >
    > ENOMEM：内存不足
    >
    > ENOTDIR：路径名的部分组件不是目录

    * 这两个方法区别在于stat没有处理字符链接(软链接）的能力，如果一个文件是符号链接，stat会直接返回它所指向的文件的属性；而lstat返回的就是这个符号链接的内容。这个文件的内容就是一个字符串，这个字符串就是它所链接的文件的绝对或者相对地址。

  * 取出套接字中的第一行，返回字符串长度

    * 考虑/r/n的情况直接一同吸收

    * > 因为tcp是stream协议，并且_META_DATA_数据不是定长的，
      >
      > 所以没有办法保证一次recv调用不读到_USER_DATA_部分的数据，除非一次读取一个字符。

  * 判断请求方式

    * 都不是错误
    * POST 需要cgi脚本请求数据
    * GET 在URL中有`?`表示要利用cgi脚本请求数据
      * 使用GET方法时，请求参数和对应的值附加在URL后面，利用一个问号（“?”）代表URL的结尾与请求参数的开始，传递参数长度受限制。

  * 取URL地址

    * 配置路径
      * 找不到路径报错
    * 对应找到文件类型
      * 默认index.html
      * 如果是执行文件，执行组就表示利用cgi

  * 如果 执行cgi

    * 执行cgi配置环境
    * 参数：套接字，文件路径，请求方式，`?` 后面的脚本查询
    * if(是GIT) 读取并丢弃请求头 -》
    * if(POST) 找到文本长度 `Content-Length:`记录长度
      * 顺遍把请求头丢弃
      * 如果没有就是400 错误的请求
    * 打印给套接字，状态行
      * 版本 200 OK 状态
    * 创建两个管道[1]
    * 创建子进程执行CGI脚本
      * 子进程
        * 子进程复制管道对应关闭两管道的不用的一端[2]
        * 将请求方式写入环境
        * 如果是GET 将查询字符串写入环境（也就是`?`后面的）[3]
        * 如果是POST写入文本长度写入环境
        * 执行对应位置的脚本
      * 父进程同样关闭不用的两端
      * 请求方式如果是POST 取对应长度把客户端发送的数据拿出，写入管道
      * 读取管道发送给客户端
      * 关闭管道
      * 等待子进程结束

  * 否则 向客户机发送一个常规文件。

    * 使用标头，并在出现错误时向客户报告错误。
    * 参数:一个指向由套接字文件描述符生成的文件结构的指针
    * 读取并丢弃请求头 -》
    * 读取源文件
      * 没有则报没有错误
      * 有源文件
        * 打印给套接字，状态行
          * 版本 200 OK 状态
          * 服务器名字
          * 文本类型 
          * /r/n
        * 读取文件内容输出发送给套接字

  * 关闭客户端套接字





### 失败或错误等状态情况

* 线程创建 如果pthread_create 返回不为0 
  * perror打印线程创建失败

* 请求方式不对时如 第一个字符串不是GET 和POST之一时
  * 发送HTTP/1.0 501 Method Not Implemented
  * 请求方法失效
* start取不到文件结构失败、打开文件失败，未知
  * 找不到文件错误
  * 发送404 NOT FOUND

* 执行cgi脚本时检测到POST消息头中没有Cotent-Length 文本长度的
  * 报错误请求
  * 发送400 BAD REQUEST

* 管道创建、进程创建失败pipe，fork返回-1
  * 属于无法继续执行脚本，服务器内部错误
  * 发送 500 Internal Server Error

* 套接字创建失败，套接字可选项设置失败等
  * perror
    * socket error
    * setsockopt failed
    * bind
    * getsockname
    * listen
    * accept















































### 知识补充

**intptr_t是为了跨平台，其长度总是所在平台的位数，所以用来存放地址**

    read(sockfd, buff, buff_size);       
    write(sockfd, buff, buff_size);
    recv(sockfd, buff, buff_size,MSG_WAITALL); //阻塞模式接收        
    send(scokfd, buff, buff_size,MSG_WAITALL); //阻塞模式发送
    recv(sockfd, buff, buff_size,MSG_DONTWAIT); //非阻塞模式接收        
    send(scokfd, buff, buff_size,MSG_DONTWAIT); //非阻塞模式发送
    recv(sockfd, buff, buff_size,0);        
    send(scokfd, buff, buff_size,0);
read原则

>    数据在不超过指定的长度的时候有多少读多少，没有数据则会一直等待。所以一般情况下：我们读取数据都需要采用循环读的方式读取数据，因为一次read 完毕不能保证读到我们需要长度的数据，read 完一次需要判断读到的数据长度再决定是否还需要再次读取。

recv 原则：

> recv 中有一个MSG_WAITALL 的参数:
> recv(sockfd, buff, buff_size, MSG_WAITALL),
> 正常情况下recv 是会等待直到读取到buff_size 长度的数据，但是这里的WAITALL 也只是尽量读全，在有中断的情况下recv 还是可能会被打断，造成没有读完指定的buff_size的长度。所以即使是采用recv + WAITALL 参数还是要考虑是否需要循环读取的问题，在实验中对于多数情况下recv (使用了MSG_WAITALL)还是可以读完buff_size，所以相应的性能会比直接read 进行循环读要好一些。

* recv的原型是ssize_t recv(int s, void *buf, size_t len, int flags); 通常flags都设置为0，此时recv函数读取tcp buffer中的数据到buf中，并从tcp buffer中移除已读取的数据。把flags设置为MSG_PEEK，仅把tcp buffer中的数据读取到buf中，并不把已读取的数据从tcp buffer中移除，再次调用recv仍然可以读到刚才读到的数据。

* socket经验

      1）尽量使用recv(,,MSG_WAITALL),read必须配合while使用，否则数据量大(240*384)时数据读不完
      2）编程时写入的数据必须尽快读出，否则后面的数据将无法继续写入
      3）最佳搭配如下：
              nbytes = recv(sockfd, buff, buff_size,MSG_WAITALL);
              nbytes = send(scokfd, buff, buff_size,MSG_WAITALL);

```cpp
struct stat {

        mode_t     st_mode;       //文件对应的模式，文件，目录等

        ino_t      st_ino;       //inode节点号

        dev_t      st_dev;        //设备号码

        dev_t      st_rdev;       //特殊设备号码

        nlink_t    st_nlink;      //文件的连接数

        uid_t      st_uid;        //文件所有者

        gid_t      st_gid;        //文件所有者对应的组

        off_t      st_size;       //普通文件，对应的文件字节数

        time_t     st_atime;      //文件最后被访问的时间

        time_t     st_mtime;      //文件内容最后被修改的时间

        time_t     st_ctime;      //文件状态改变时间

        blksize_t st_blksize;    //文件内容对应的块大小

        blkcnt_t   st_blocks;     //伟建内容对应的块数量

      };
```

[1] 管道是不属于进程的资源是操作系统的，所以进程复制的时候不会复制的对象

* 两进程通过操作系统的内存空间进行通信
* 单个管道双向通信会引发错误，向管道传输数据的时，先读的进程会把数据取走，就可能发送自己写自己读了。

[2]复制套接字 dup2(sock, cp1); cp1为指定的文件描述符，  如果指定的数字已经被占用（和某个文件有关联），此函数会自动关闭 close() 断开这个数字和某个文件的关联，再来使用这个合法数字。

* 文件指针关闭一个后会直接关闭对应的文件描述符
* 文件描述符关闭只有全部关完才会关对应的套接字

[3] 1.例子

```json
{
   "query_string":{
      "default_field":"name",
      "query":"(this AND that) OR thu*"
    }
}
```

这里只挑几个常用的参数说一下，其他的一般默认就够了

- query： 需要查询的具体内容
- default_field： 查询的字段

默认是_all，即对所有字段进行查询。

支持多字段——"fields" : ["age", "name"],fields中只要有一个字段满足query的条件即可匹配查询。

支持一些简单的wildcard写法。比如`fields:[“nam*”]`即任何nam开头的字段，注意：如果field与query写在一起比如”query”:”name:obama”，要使用wildcard需要转义---> `”query”:”nam\\*:obama`





## --兼容

\n是换行，就是到下一行\r是回车，就是回到行首其实在Windows里，用\n也就ok了，但是为了兼容，很多地方还都是用\r\n