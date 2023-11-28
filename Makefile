NAME = webserv
SRC = webserv.cpp network.cpp
HEADERS = webserv.hpp
OBJ = $(SRC:.cpp=.o)
FLAGS = -Wall -Wextra -Werror
COMPILER = c++ -std=c++98

all: $(NAME)

$(NAME): $(OBJ)
	$(COMPILER) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp $(HEADERS)
	$(COMPILER) $(FLAGS) -c $< -o $@
clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all