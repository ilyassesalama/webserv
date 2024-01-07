NAME     = 	  webserv

SRC      = 	  src/classes/cgi/CGInstance.cpp \
	  	  	  src/classes/cgi/CGIUtils.cpp \
	  	  	  src/classes/config_file/ConfigurationFile.cpp \
	  	  	  src/classes/config_file/DirectivesTools.cpp \
	  	  	  src/classes/config_file/GeneralTools.cpp \
	  	  	  src/classes/config_file/LocationTools.cpp \
	  	  	  src/classes/config_file/SyntaxTools.cpp \
	  	  	  src/classes/request/BoundaryUtils.cpp \
	  	  	  src/classes/request/RequestParser.cpp \
	  	  	  src/classes/request/RequestParserUtils.cpp \
	  	  	  src/classes/request/TransferEncodingUtils.cpp \
	  	  	  src/classes/response/DELETEResponseBuilder.cpp \
	  	  	  src/classes/response/GETResponseBuilder.cpp \
	  	  	  src/classes/response/HTMLGenerators.cpp \
	  	  	  src/classes/response/POSTResponseBuilder.cpp \
	  	  	  src/classes/response/POSTResponseUtils.cpp \
	  	  	  src/classes/response/Response.cpp \
	  	  	  src/classes/response/ResponseGettersSetters.cpp \
	  	  	  src/classes/server/ClientProfile.cpp \
	  	  	  src/classes/server/ConnectionsManager.cpp \
	  	  	  src/classes/server/ServerInstance.cpp \
	  	  	  src/classes/utils/File.cpp \
	  	  	  src/classes/utils/Logger.cpp \
	  	  	  src/classes/utils/String.cpp \
	  	  	  src/classes/utils/Utils.cpp \
	  	      webserv.cpp

HEADERS  =    src/headers/CGInstance.hpp \
		  	  src/headers/ClientProfile.hpp \
		  	  src/headers/ConfigurationFile.hpp \
		  	  src/headers/ConnectionsManager.hpp \
		  	  src/headers/File.hpp \
		  	  src/headers/Logger.hpp \
		  	  src/headers/RequestParser.hpp \
		  	  src/headers/Response.hpp \
		  	  src/headers/ServerInstance.hpp \
		  	  src/headers/String.hpp \
		  	  src/headers/Utils.hpp \
		  	  webserv.hpp

OBJ      =    $(SRC:.cpp=.o)
FLAGS    =    -std=c++98 -Wall -Wextra -Werror
COMPILER =    c++

all: $(NAME)
	@:
	@clear
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