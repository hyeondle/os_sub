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

CONTEXT_SWITCH ?= 0
QUANTUM ?= 20
RQ_SIZE ?= 3

.c.o	:
			$(CC) $(CFLAGS) -DCONTEXT_SWITCH=$(CONTEXT_SWITCH) -DQUANTUM=$(QUANTUM) -DRQ_SIZE=$(RQ_SIZE) -c $< -o $(<:.c=.o) -I ./

all		:	$(NAME)

$(NAME)	:	$(OBJECTS)
			$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)

clean	:
			rm -f $(OBJECTS)

fclean	:	clean
			rm -f $(NAME)

re		:
			make fclean
			make all

.PHONY	:	all clean fclean re


