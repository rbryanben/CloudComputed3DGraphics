#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <map>
#include <vector>
#include "helpers/string_helper.hpp"
#include "utils/server_render.h"

#define PORT 8085

using namespace std;

// Sessions
ServerRenderer renderer = ServerRenderer(800.0f,800.0f,"Python Pygame",true);

// Process Event
string proccessEvent(char buffer[1024],int clientPort){
    //get angle
    vector<string> data = splitDelimeter(buffer,' ');
    
    //render
    return renderer.onRender(stof(data.at(1)));
}

// main function
int main(int argc, char* argv[]){
    // Test Render
    renderer.test();
    renderer.onRender(0.0f);

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1 ;
    int addrlen = sizeof(address);
    
 
    // Socket file descriptor
    if ((server_fd = socket(AF_INET,SOCK_STREAM,0)) == 0){
        cout << "Socket failed" << endl;
        exit(EXIT_FAILURE);
    }

    // Attach the socket to port 8080
    if (setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        cout << "Failed to attach to port" << endl;
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attach the socket to port 8080
    if (bind(server_fd,(struct sockaddr*)&address,sizeof(address)) < 0){
        cout << "Failed to bind to port" << endl;
        exit(EXIT_FAILURE);
    }
    
    // Listen
    if (listen(server_fd,3)<0){
        cout << "Failed to listen" << endl;
        exit(EXIT_FAILURE);
    }
    

    cout << "Listening" << endl;
    
    // Accept connnection
    if ((new_socket = accept(server_fd,(struct sockaddr*)&address,(socklen_t*)&addrlen)) < 0){
        cout << "Failed to Accept" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Client connected on port " << address.sin_port << endl;
    char buffer[1024] = {0};
    while (true){
        //read 
        valread = read(new_socket,buffer,1024);
        
        //Compute result
        string res = proccessEvent(buffer,address.sin_port);
        char responseBuffer[res.length()+10];
        for (int i=0; i != res.length(); i++){
            responseBuffer[i] = res[i];
        }
        //Send Response
        send(new_socket,responseBuffer,res.length(),0);
    }
    
    // Teardown
    close(new_socket);
    shutdown(server_fd,SHUT_RDWR);
    return 0;
}