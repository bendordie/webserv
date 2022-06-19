# webserv

Simple Web-Server based on HTTP/1.1.

In addition to mastering the client-server interactions and the principles of event-driven programming, the goal of the project was to learn and apply the basics of OOP.

# About

This project is an implementation of a simple web server capable of handling a number of requests, such as GET, POST, DELETE. It is also possible to process POST requests using built-in CGI scripts. The web server has its own configuration file where you can configure the server name, ip address and listening port, define different URLs, configure redirects and set permissible request types (all same as in Nginx).

# Getting started

Write <code>make</code> in the console in the root directory of the project. Then <code>./server config.cfg</code>

After that you can already send requests to the server (e.g. by writing <code>localhost</code> in the browser).
