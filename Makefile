# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cshells <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/12/16 21:53:25 by cshells           #+#    #+#              #
#    Updated: 2021/12/16 21:53:26 by cshells          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = server

SRC =	system/webserv.cpp \
		system/EventSelector.cpp \
		system/FdHandler.cpp \
		system/WebServer.cpp \
		system/VirtualServer.cpp \
		system/ServerOptions.cpp \
		system/LocationOptions.cpp \
		system/WebSession.cpp \
		system/HttpMessage.cpp \
		system/HttpRequest.cpp \
		system/HttpResponse.cpp \
		system/Utils.cpp

OBJ = $(SRC:.cpp=.o)

CXX = clang++ -std=c++11

#CXXFLAGS = -Wall -Wextra -Werror

RM = rm -rf

all: $(NAME)

$(NAME): $(SRC) system/*.hpp $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re

