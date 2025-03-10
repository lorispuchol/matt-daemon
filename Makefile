CC          =   c++
CFLAGS      =   -Wall -Werror -Wextra # -std=c++11
NAME        =   Matt_daemon
RM          =   rm -rf

INCLDS_DIR  =   includes/

INCLUDES    =   Matt_daemon.hpp \
                Tintin_reporter.hpp

SRCS_DIR    =   srcs/

SRCS        =   Matt_daemon.cpp \
                Tintin_reporter.cpp

OBJS        :=  ${addprefix objs/, ${SRCS:.cpp=.o}}


# Do not use -I ${INCLDS_DIR}, this relink all if a file is modified
objs/%.o:   ${SRCS_DIR}%.cpp $(addprefix ${INCLDS_DIR}, ${INCLUDES}) Makefile
			@mkdir -p $(dir $@)
			$(CC) ${CFLAGS} -c $< -o $@ 

all:        ${NAME}

${NAME}:    ${OBJS}
			$(CC) ${CFLAGS} -o $(NAME) ${OBJS}

clean:
			${RM} objs/

fclean:     clean
			${RM} ${NAME}

re:         fclean all

.PHONY:     all clean fclean re