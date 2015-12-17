<meta http-equiv="Content-Type" content="text/html; charset=utf-8">

# AtomWeb -- 嵌入式Web服务器生成器

## 简介
AtomWeb是一个嵌入式Web服务器生成工具。使用AtomWeb，你可以在嵌入式设备上生成
一个动态Web服务器。整个服务器非常之小，不需要依赖任何脚本语言，甚至也不需要文件
系统。你可以像使用PHP一样用C语言开发动态页面，把设备变为iot设备。

## 配置
编辑"config.mk"文件进行配置。其中"ARCH"指定服务器的运行平台。  
下面的配置表示在X86 32bits linux系统下运行：

	ARCH:=i686-linux

下面的配置表示在X86 64bits linux系统下运行：

	ARCH:=x86_64-linux

下面的配置表示在Windows 32 bits MinGW下运行：

	ARCH:=mingw32

## 编译
运行下面的命令进行编译：
	make

## 安装
运行下面的命令进行安装：

	make install

缺省安装路径为"/usr"。  
AtomWeb主要包括：

1. 运行在开发环境下的工具：
	* aw-scanner: Web 服务器文件扫描工具。扫描服务器源目录，生成Web文件查找表。
	* aw-converter: Web 页面转换工具。将源文件转换为C文件。
2. 目标平台链接库libatomweb.a: 提供基本HTTP功能。

## 使用方法

1. 创建一个目录用于存放源文件。如：

		mkdir www

2. 将服务器需要的文件拷贝到源目录下。如：

		cp SRC/logo.png www

3. 添加动态页面文件
	"js","json","html","xml"等格式的文件都可以变为动态文件。动态页面文件后
	缀为".aw"。如：

		* 1.html.aw 对应1.html。
		* foo.js.aw 对应foo.js。  
	在"www"目录下编辑"index.html.aw"文件如下：

			<html>hello, <?= "%s",aw_session_get_param(sess,"male")?"boy":"girl" ?><img src="logo.png"/></html>

4. 扫描源目录。

		aw-scanner -m myweb.mk -c myweb.c www

	命令扫描“www"目录下的所有文件，生成Makefile文件"myweb.mk"和C源文件"myweb.c"。

5. 创建服务器入口源文件"main.c"
	示例代码如下：

		#include <sys/types.h>
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <errno.h>
		#include <stdlib.h>
		#include <atomweb.h> /*包含AtomWeb头文件*/

		int main(int argc, char **argv)
		{
			struct sockaddr_in addr;
			int serv_sock = -1;
			int port = 80;
			int r;
			AW_Server *serv;
			extern const AW_Map *aw_map;

			/*创建Server*/
			serv = aw_server_create();

			/*指定Server的文件查找表*/
			aw_server_set_map(serv, aw_map);

			serv_sock = socket(AF_INET, SOCK_STREAM, 0);

			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htons(INADDR_ANY);
			addr.sin_port = htons(port);

			r = bind(serv_sock, (struct sockaddr*)&addr, sizeof(addr));
			if (r == -1) {
				fprintf(stderr, "bind to port %d failed\n", port);
				exit(1);
			}

			fprintf(stderr, "bind to port %d\n", port);

			r = listen(serv_sock, 5);
			if (r == -1) {
				fprintf(stderr, "listen failed\n");
				exit(1);
			}

			while (1) {
				struct sockaddr_in client_addr;
				socklen_t len = sizeof(client_addr);
				int sess_sock;
				AW_Session *sess;

				sess_sock = accept(serv_sock, (struct sockaddr*)&client_addr,
							&len);
				if (sess_sock < 0) {
					fprintf(stderr, "accept failed\n");
					continue;
				}

				fprintf(stderr, "accept a client\n");

				/*创建HTTP对话*/
				sess = aw_session_create(serv, (AW_Ptr)(AW_IntPtr)sess_sock);

				/*运行对话*/
				aw_session_run(sess);

				/*释放*/
				aw_session_destroy(sess);

				fprintf(stderr, "disconnect\n");
				close(sess_sock);
			}

			fprintf(stderr, "close the server\n");
			close(serv_sock);

			/*释放Server资源*/
			aw_server_destroy(serv);

			return 0;
		}

6. 创建"Makefile",代码如下：

		all: myserver

		include myweb.mk #包含aw-scanner生成的makefile

		myserver: main.c $(AW_SRCS) myweb.c
			gcc -o $@ $^ -I/usr/include/atomweb -latomweb

7. 运行“make”生成"myserver"程序

8. 启动服务器"myserver"
	运行浏览器访问"http://127.0.0.1/"，可以看到页面显示：

		hello, girl

	给页面家一个参数"http://127.0.0.1/?male=1"，页面显示变为：

		hello, boy

## 命令

从上面的例子中，我们在页面中插入了一段C代码。AtomWeb动态页面中可以加入以下几种代码块：

* &lt;?h CODE ?&gt; : 代码被插入生成C文件的头部  
	如：
	
		<?h
		#include <stdio.h>
		#include <string.h>
		?>

* &lt;?t CODE ?&gt; : 代码被插入生成C文件的尾部
* &lt;?m CODE ?&gt; : 代码声明用户数据的结构成员  
	每个页面都可以定义一个用户数据结构，如下代码：
	
		<?m
			int a;
			int b;
		?>
	
	被转换为:
	
		typedef struct {
			int a;
			int b;
		} AW_UserData;

* &lt;?i CODE ?&gt; : 代码在session初始化时被执行。  
	此部分代码可以访问表示session的变量"sess"和用户数据结构"udata"。
	这部分代码主要用来获取session参数，初始化用户数据结构。如：

		<?i
			/*获取参数min并转化为整数*/
			udata->a = strtol(aw_session_get_param(sess, "min"), NULL, 0);

			/*获取参数max并转化为整数*/
			udata->b = strtol(aw_session_get_param(sess, "max"), NULL, 0);
		?>

* &lt;?c CODE ?&gt; : 代码在session运行时被执行。 
	此部分代码可以访问表示session的变量"sess"和用户数据结构"udata"。
	看下面的代码：

		<?c
			int i;
			for (i = 0; i < 10; i++) {
				?>
				atom
				<?c
			}
		?>
	
	运行时会生成10个"atom"。

* &lt;?= FORMAT,EXPR... ?&gt; : 在session运行时按FORMAT格式输出一组表达式值。 
	类似C语言中的"printf"，FORMAT为输出格式描述字符串，后面跟需要输出的一组表达式。
	此部分代码可以访问表示session的变量"sess"和用户数据结构"udata"。
	看下面的代码：

		<?= "integer:%d double:%f string:%s", 100, 3.1415926, "hello" ?>

	运行结果为：

		integer:100 double:3.1415926 string: hello

* &lt;?r CODE ?&gt; : 代码在用户数据结构释放时被调用。 
	此部分代码主要用于释放用户数据结构中分配的资源。
	如：

		<?m
			char *buf;
		?>
		<?i
			udata->buf = (char*)malloc(sizeof(char) * 1024);
		?>
		<?c
			fread(udata->buf, 1, 1024, fp);	
		?>
		<?r
			free(udata->buf);
		?>
