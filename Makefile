NAME = webserv

# Find all .cpp files in the current directory and subdirectories
SRC = $(shell find . -name '*.cpp')

HEADERS = $(shell find . -name '*.hpp')
OBJ = $(SRC:.cpp=.o)
FLAGS = #-Wall -Wextra -Werror
COMPILER = c++ -std=c++98

all: $(NAME)

$(NAME): $(OBJ)
	$(COMPILER) $(FLAGS) $(OBJ) -o $(NAME)
	@clear
%.o: %.cpp $(HEADERS)
	$(COMPILER) $(FLAGS) -c $< -o $@
	@clear
clean:
	rm -rf $(OBJ)
	@clear
fclean: clean
	rm -rf $(NAME)
	@clear
re: fclean all