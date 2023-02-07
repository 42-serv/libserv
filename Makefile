.SUFFIXES: .cpp .o .hpp .h .tpp
.PHONY: all clean fclean re bonus

CXX = c++
RM = rm -f
AR = ar
ARFLAGS = crs

CXXFLAGS += -MMD -MF $(@:.o=.d) -MT $@ -MP
CXXFLAGS += --std=c++98 -Wall -Wextra -Werror
CXXFLAGS += -Iincludes

TARGET = libserv.a
OBJECTS_DIR = objs/

SOURCES += \
	event_channel_base.cpp \
	event_handler_base.cpp \
	event_layer.cpp \
	event_worker.cpp \
	event_worker_group.cpp \
	server_bootstrap.cpp \
	server_channel.cpp \
	stream_channel.cpp \
	task_base.cpp \

ifeq ($(shell uname), Linux)
	SOURCES += \
		event_worker.epoll.cpp \

else
	SOURCES += \
		event_worker.kqueue.cpp \

endif

OBJECTS = $(addprefix $(OBJECTS_DIR), $(SOURCES:.cpp=.o))

ifdef ASAN
	CXXFLAGS += -fsanitize=address
	LDFLAGS += -fsanitize=address
	DEBUG = 1
endif

ifdef MSAN
	CXXFLAGS += -fsanitize=memory
	LDFLAGS += -fsanitize=memory
	DEBUG = 1
endif

ifdef TSAN
	CXXFLAGS += -fsanitize=thread
	LDFLAGS += -fsanitize=thread
	DEBUG = 1
endif

ifdef UBSAN
	CXXFLAGS += -fsanitize=undefined
	LDFLAGS += -fsanitize=undefined
	DEBUG = 1
endif

ifdef DEBUG
	CXXFLAGS += -g3 -O0
endif

all: $(TARGET)
clean: cleanobj cleanbin
cleanobj:	;	$(RM) -r $(OBJECTS_DIR)
cleanbin:	;	$(RM) $(TARGET)
re: clean	;	$(MAKE)

$(OBJECTS_DIR):
	mkdir $(OBJECTS_DIR)

$(addprefix $(OBJECTS_DIR), %.o): %.cpp | $(OBJECTS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

$(TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $?

-include $(OBJECTS:.o=.d)
