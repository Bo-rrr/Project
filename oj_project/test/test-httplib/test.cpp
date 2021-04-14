#include <stdio.h>
#include "httplib.h"

void func(const httplib::Request& req, httplib::Response& resp)
{
    resp.set_content("<html>57</html>", 15, "text/html");
    printf("recv abc\n");
}

int main()
{
    //创建httplib当中的server类对象， 使用该类对象， 模拟创建一个http服务器
    httplib::Server svr;

    svr.Get("/abc", func);

    svr.listen("0.0.0.0", 18989);
    return 0;
}
