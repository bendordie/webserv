//
// Created by Carmel Shells on 12/8/21.
//

     #include <sys/types.h>
     #include <sys/uio.h>
     #include <unistd.h>

#include <fcntl.h>
#include <vector>
#include <iostream>
#include <sys/select.h>


int main(){
    char buf[100];

    fcntl(0, F_SETFL , O_NONBLOCK);
    int i = 0;

    while(1){
        i = read(0, buf, 99);
        if (i > -1){
            buf[i] = 0;
            std::cout << buf << std::endl;
        }
//        sleep(1);
        std::cout << "jsdjflka" << std::endl;
    }
}