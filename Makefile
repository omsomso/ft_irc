NAME		:= ircserv
CC			:= c++
FLAGS		:= -std=c++98 -Wall -Wextra -Werror #-fsanitize=address -g3
RM			:= rm -rf

SRCDIR		:= src
INCDIR		:= inc
OBJDIR		:= obj
SRC 		:= $(shell find $(SRCDIR) -name '*.cpp' ! -name '*_bonus.cpp')
OBJ 		:= $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))
BONUS_OBJ 	:= $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(BONUS_SRC))



define HEADER
  _____  __     .__                
_/ ____\/  |_   |__|______   ____  
\   __\ \   __\ |  \_  __ \_/ ___\ 
 |  |   |  |    |  ||  | \/\  \___ 
 |__|   |__|____|__||__|    \___  >
          /_____/               \/ 
endef
export HEADER

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
			@mkdir -p '$(@D)'
			@$(CC) -c $(FLAGS) $< -o $@

all:		$(NAME)

$(NAME): 	$(OBJ)
			@$(CC) $(FLAGS) -o $@ $^ -I$(INCDIR)
			@echo "$$HEADER"

run:		re
			./ircserv 6667 supermdp

clean:
			@$(RM) $(OBJDIR)

fclean:		clean
			@$(RM) $(NAME)

re:			fclean $(NAME)

.PHONY:		all clean fclean re
