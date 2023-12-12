NAME = webserv

# Find all .cpp files in the current directory and subdirectories
SRC = $(shell find . -name '*.cpp')

HEADERS = $(shell find . -name '*.hpp')
OBJ = $(SRC:.cpp=.o)
FLAGS = #-std=c++98 -Wall -Wextra -Werror #-fsanitize=address
COMPILER = c++

all: $(NAME)
	@:
#@clear

$(NAME): $(OBJ)
	$(COMPILER) $(FLAGS) $(OBJ) -o $(NAME)
#@clear
%.o: %.cpp $(HEADERS)
	$(COMPILER) $(FLAGS) -c $< -o $@
#@clear
clean:
	rm -rf $(OBJ)
#@clear
fclean: clean
	rm -rf $(NAME)
#@clear
re: fclean all