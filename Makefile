# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: piquerue <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/11/02 14:20:07 by piquerue          #+#    #+#              #
#    Updated: 2017/10/10 18:18:02 by piquerue         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = wolf3d

SRC = ./main.c

OBJ = ./main.o

OBJ_2 = ./obj/main.o

INCLUDE = $(shell find . -type f | grep "\.h")

LIB = library_src/lib.a
LIB += ft_ls/lib.a

CFLAGS2 = -g -lpthread -lmlx -framework OpenGL -framework AppKit mlx/libmlxS.a $(LIB)
CFLAGS = -g -lpthread $(LIB)

all: $(NAME)

.PHONY : all clean fclean re lib

obj/%.o: %.c $(INCLUDE)
	@mkdir -p obj/
	gcc -o $@ -c $<  -Wall -Wextra -Werror

lib: $(INCLUDE)
	@make -C library_src
	@make -C ft_ls

$(NAME): lib $(OBJ_2)
	@gcc -c $(SRC) -Wall -Werror -Wextra
	@gcc -o $(NAME) $(OBJ) $(CFLAGS) -Wall -Wextra -Werror
	@mv -f $(OBJ) obj/

clean:
	@make -C library_src clean
	@make -C ft_ls clean
	@rm -rf $(OBJ_2)
fclean: clean
	@make -C library_src fclean
	@make -C ft_ls fclean
	@rm -f $(NAME)

re: fclean all
	@make -C library_src re
	@make -C ft_ls re
