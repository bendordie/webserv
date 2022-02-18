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

SRC =	main_new.cpp \
		EventSelector.cpp \
		FdHandler.cpp \
		WebServer.cpp \
		WebSession.cpp \
		HttpMessage.cpp \
		HttpRequest.cpp \
		HttpResponse.cpp \
		Utils.cpp

OBJ = $(SRC:.cpp=.o)

CXX = clang++

#CXXFLAGS = -Wall -Wextra -Werror

RM = rm -rf

all: $(NAME)

$(NAME): $(SRC) *.hpp $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re

