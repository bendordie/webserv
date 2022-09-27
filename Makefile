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

SRC =	src/webserv.cpp \
		src/EventSelector.cpp \
		src/FdHandler.cpp \
		src/WebServer.cpp \
		src/VirtualServer.cpp \
		src/Location.cpp \
		src/WebSession.cpp \
		src/HttpMessage.cpp \
		src/Request.cpp \
		src/Response.cpp \
		src/Utils.cpp

OBJ = $(SRC:.cpp=.o)

CXX = clang++ -std=c++11

#CXXFLAGS = -Wall -Wextra -Werror

RM = rm -rf

all: $(NAME)

$(NAME): $(SRC) src $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re

