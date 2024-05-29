NAME	=	scheduler
SOURCES	=	main.c\
			init.c\
			scheduling.c\
			utils.c\
			free.c\
			cycle.c\
			fcfs.c\
			sjf.c\
			srtf.c\
			rr.c\
			lrrwp.c\

OBJECTS	=	$(SOURCES:.c=.o)

CC		=	gcc
CFLAGS	=	-Wall -Wextra -Werror

.c.o	:
			$(CC) $(CFLAGS) -c $< -o $(<:.c=.o) -I ./

all		:	$(NAME)

$(NAME)	:	$(OBJECTS)
			$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)

clean	:
			rm -f $(OBJECTS)

fclean	:	clean
			rm -f $(NAME)

re		:
			fclean
			all

.PHONY	:	all clean fclean re


