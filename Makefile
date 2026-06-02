BUILD_DIR = build
CC = g++
#VCPKG_ROOT = /vcpkg
#INCLUDE_DIRS = -I$(VCPKG_ROOT)/installed/x64-osx/include
#LIBRARY_DIRS = -L$(VCPKG_ROOT)/installed/x64-osx/lib
CFLAGS = -Wall -std=c++11 -g $(INCLUDE_DIRS)
LIBS = -lcurl
TARGET = TeamAProgram
#SRCS = main.cpp Retrieve.cpp portfolio.cpp DateRange.cpp Bootstrapping.cpp helper.cpp plot_helper.cpp PortfolioClassifier.cpp vector_math.cpp
SRCS := $(wildcard *.cpp)
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(OBJS) $(LIBS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)