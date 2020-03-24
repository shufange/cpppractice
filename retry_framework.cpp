/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, PHP, Ruby, 
C#, VB, Perl, Swift, Prolog, Javascript, Pascal, HTML, CSS, JS
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include <iostream>
#include <functional>
#include <string>
#include <map>
#include <unordered_map>
#include <memory>

using namespace std;
class Conn {
public:
    bool stale_;
    std::string host_;
    Conn(string& host)
    { host_ = host; };
};

using ServerFunc = std::function<int()>;
std::unordered_map<string, Conn*> map_;
class ConnWrapper
{
private:
    std::string ctx_;
    Conn* conn_;
    bool set_;
public:
    ConnWrapper(std::string& ctx)
    { ctx_ = ctx;};
    Conn* getConn()
    {
        if (set_) return conn_;
        if (map_.find(ctx_) == map_.end())
        {
            map_.emplace(ctx_, new Conn(ctx_));
        }
        conn_ = map_[ctx_];
        set_ = true;
        return conn_;
        
    };
    void resetConn()
    {
        if (!set_) {
            getConn();
            return;
        }
        set_ = false; //atomic bool
        //lock
        map_.erase(ctx_);
        std::cout << "deleting conn:" << conn_ << std::endl;
        delete conn_;
        getConn();
    };

    int setCtx(string& ctx)
    {
        ctx_ = ctx;
    }
    
    int retryFunc(const ServerFunc func, int retry)
    {
        while (retry-- > 0)
        {
            std::cout << "retry:" << retry << std::endl;
            int ret = func();
            if (ret == 0)
                break;
            else if (getConn()->stale_){
                std::cout << "conn stale, reset and retry" << std::endl;
                resetConn();
            }
        }   
    }
};


 

int func(int& num, int& num1, std::shared_ptr<ConnWrapper>& cw)
{
        Conn* conn = cw->getConn();
        std::cout << "func gets called, num:" << num << std::endl;
        num = 2;
        std::cout << "Conn:" << conn->host_ << ":conn:" << conn << std::endl;
        conn->stale_ = true;
        return 1;
}

std::string empty_string = "";
class A {
public:
    string a_{empty_string};
};

int main()
{   
    printf("Hello World\n");
    string ctx = "a";
    std::shared_ptr<ConnWrapper> cw = std::make_shared<ConnWrapper>(ctx);
    int num = 1;
    int num1 = 1;
    auto f = [&num, &num1, &cw](){
        return func(num, num1, cw);
    };
    std::cout << "[DEBUG]num:" << num << std::endl;
    cw->retryFunc(f, 2);
    std::cout << "[DEBUG]num:" << num << std::endl;
    return 0;
}
