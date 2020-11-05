#include <map>
#include <string>
#include <fstream>
#include <iostream>

#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#include "json/json.h"

std::string image;
bool benchmark = true;

std::string GetFile();
void OnRequest(const CHttpRequest& req, CHttpResponse& resp) {
    //std::cout << "Headers " << req.GetMethodString() << " " << req.GetPath() << std::endl;
    if (!benchmark) {
        const std::map<std::string, std::string>& headers = req.GetHeaders();
        for (const auto& header : headers) {
            std::cout << header.first << ": " << header.second << std::endl;
        }
    }

    if (req.GetPath() == "/") {
        resp.SetStatusCode(k200Ok);
        resp.SetStatusMessage("OK");
        resp.SetContentType("text/html");
        resp.AddHeader("Server", "CppNet");
        CHttpServer::_time_tool.Now();
        std::string now = std::to_string(CHttpServer::_time_tool.GetMsec());
        resp.SetBody("<html><head><title>This is title</title></head>"
            "<body><h1>Hello</h1>Now is " + now +
            "</body></html>");

    } else if (req.GetPath() == "/favicon.ico") {
        resp.SetStatusCode(k200Ok);
        resp.SetStatusMessage("OK");
        resp.SetContentType("image/png");
        if (image.empty()) {
            image = GetFile();
        }
        resp.SetBody(image);

    } else if (req.GetPath() == "/hello") {
        resp.SetStatusCode(k200Ok);
        resp.SetStatusMessage("OK");
        resp.SetContentType("text/plain");
        resp.AddHeader("Server", "CppNet");
        resp.SetBody("hello, world!\n");

    } 
    else if(req.GetPath() =="/test")
    {
        resp.SetStatusCode(k200Ok);
        resp.SetStatusMessage("OK");
        resp.SetContentType("text/plain");
        resp.AddHeader("Server", "CppNet");
        
        Json::Value writeObject;
	    Json::Value array;  
	    
	    for(int i=0; i<86400;i++){
        array["T"] = "2020-10-12 10:10:10.12";
		array["x"] = 5.6264354376048686;
		array["y"] = -760.01067774477781;
		array["z"] = 7307.5364996909384;
		array["vx"] = -6490536.5807291884;
		array["vy"] = 3616285.5418396913;
		array["vz"] = 376117.60900893307;
		writeObject["J2000"].append(array);
	   }
		
		std::string res = writeObject.toStyledString();
        
        resp.SetBody(res);
     
    }
    else {
        resp.SetStatusCode(k404NotFound);
        resp.SetStatusMessage("Not Found");
        resp.SetCloseConnection(true);
    }
}

std::string GetFile() {
    std::ifstream t("logo.png");
    std::string str((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());
    return std::move(str);
}

void DisConnectionFunc(const cppnet::Handle& , uint32_t ) {
    std::cout << "[DisConnectionFunc]" << std::endl;
}

int main() {
    cppnet::CCppNet net;
    net.Init(2);

    CHttpServer server;
    server.SetHttpCallback(OnRequest);

    net.SetAcceptCallback(std::bind(&CHttpServer::OnConnection, &server, std::placeholders::_1, std::placeholders::_2));
    net.SetWriteCallback(std::bind(&CHttpServer::OnMessageSend, &server, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    net.SetReadCallback(std::bind(&CHttpServer::OnMessage, &server, std::placeholders::_1, std::placeholders::_2, 
                                              std::placeholders::_3, std::placeholders::_4));
    net.SetDisconnectionCallback(DisConnectionFunc);

    net.ListenAndAccept("192.168.1.56", 8921);

    net.Join();
}
