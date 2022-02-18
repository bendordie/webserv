//
// Created by Carmel Shells on 12/14/21.
//

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>

#define SERVER_PORT 80
#define MAXLINE 4096


int main(int argc, char* argv[]) {

    if (argc != 3 && argc != 4) {
        write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
        exit(1);
    }

    int                 socketfd, n;
    int                 bytes_sent;
    char                sendline[MAXLINE];
    char                recvline[MAXLINE];
    struct sockaddr_in  server_addr;

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        return 1;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid address format");
        return 1;
    }
    if (connect(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect error");
        return 1;
    }

    if (argc != 4) {
//        sprintf(sendline, "GET /rfer HTTP/1.1\n"
//                          "Host: 127.0.0.1\n"
//                          "Connection: keep-alive\n"
//                          "Cache-Control: max-age=0\n"
//                          "sec-ch-ua: \"Chromium\";v=\"94\", \"Google Chrome\";v=\"94\", \";Not A Brand\";v=\"99\"\n"
//                          "sec-ch-ua-mobile: ?0\n"
//                          "sec-ch-ua-platform: \"macOS\"\n"
//                          "DNT: 1\n"
//                          "Upgrade-Insecure-Requests: 1\n"
//                          "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.81 Safari/537.36\n"
//                          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b\r\n\r\n");

        sprintf(sendline, "GET / HTTP/1.1\r\n"
                          "Host: 127.0.0.1\r\n"
                          "Connection: keep-alive\r\n"
                          "Cache-Control: max-age=0\r\n"
                          "sec-ch-ua: \"Chromium\";v=\"94\", \"Google Chrome\";v=\"94\", \";Not A Brand\";v=\"99\"\r\n"
                          "sec-ch-ua-mobile: ?0\r\n"
                          "sec-ch-ua-platform: \"macOS\"\r\n"
                          "DNT: 1\r\n"
                          "Upgrade-Insecure-Requests: 1\r\n"
                          "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.81 Safari/537.36\r\n"
                          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
                          "Sec-Fetch-Site: none\r\n"
                          "Sec-Fetch-Mode: navigate\r\n"
                          "Sec-Fetch-User: ?1\r\n"
                          "Sec-Fetch-Dest: document\r\n"
                          "Accept-Encoding: gzip, deflate, br\r\n"
                          "Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7\r\n\r\n");
//                          "GET /favicon.ico HTTP/1.1\n"
//                          "Host: localhost\n"
//                          "Connection: keep-alive\n"
//                          "sec-ch-ua: \"Chromium\";v=\"94\", \"Google Chrome\";v=\"94\", \";Not A Brand\";v=\"99\"\n"
//                          "DNT: 1\n"
//                          "sec-ch-ua-mobile: ?0\n"
//                          "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.81 Safari/537.36\n"
//                          "sec-ch-ua-platform: \"macOS\"\n"
//                          "Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\n"
//                          "Sec-Fetch-Site: same-origin\n"
//                          "Sec-Fetch-Mode: no-cors\n"
//                          "Sec-Fetch-Dest: image\n"
//                          "Referer: http://localhost/\n"
//                          "Accept-Encoding: gzip, deflate, br\n"
//                          "Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7\r\n\r\n");
    }
    else {
        std::string line(argv[3]);

        line += "\r\n\r\n";
        sprintf(sendline, "%s", line.c_str());
    }
//    sprintf(sendline, "GET / HTTP/1.1 Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis congue purus quis ante consectetur tempor. Integer tincidunt, nulla at pulvinar blandit, quam ex maximus lectus, eu ullamcorper metus purus et lectus. Nunc vitae porttitor dolor. In euismod urna eu fermentum tempus. Suspendisse nec augue.\r\n\r\n");
    bytes_sent = strlen(sendline);

    if (write(socketfd, sendline, bytes_sent) != bytes_sent) {
        perror("Write error");
        return 1;
    }
    memset(recvline, 0, MAXLINE);

//    FILE *file = fopen("filerecv.png", "w");
//
//    while ((n = read(socketfd, recvline, MAXLINE-1)) > 0) {
//        fprintf(file, "%s", recvline);
//    }
//    fclose(file);

    std::ofstream ofs;

    ofs.open("filerecv.png", std::ofstream::binary);

    while ((n = read(socketfd, recvline, MAXLINE-1)) > 0) {
        ofs.write(recvline, n);
    }
    ofs.close();

//    while ((n = read(socketfd, recvline, MAXLINE-1)) > 0) {
//        printf("%s", recvline);
//    }

    if (n < 0) {
        perror("Read error");
    }

    return 0;
}