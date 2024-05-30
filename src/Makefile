JAVA	=	java
JAVAC	=	javac

SOURCE	=	ProcessSchedulerGUI.java\
			ProcessInfo.java\
			GanttChart.java\
			InitInfo.java\
			ResultInfo.java\

CLASSES	=	$(SOURCE:.java=.class)

MAIN	=	ProcessSchedulerGUI

CONTEXT_SWITCH ?= 0
QUANTUM ?= 20
RQ_SIZE ?= 3

all: $(CLASSES)

%.class	:	%.java
			$(JAVAC) $<

run	:	all
		$(JAVA) $(MAIN)

scheduler :
		$(MAKE) -C bins CONTEXT_SWITCH=$(CONTEXT_SWITCH) QUANTUM=$(QUANTUM) RQ_SIZE=$(RQ_SIZE)

clean_scheduler :
		$(MAKE) -C bins clean

clean	:
		rm -f *.class
		$(MAKE) -C bins fclean

re	:
		make clean
		make all

.PHONY	: all run clean
