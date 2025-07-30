CXX = g++
CXXFLAGS = -std=c++17
LDFLAGS = -pthread

SRCS1 = tcpepoll.cpp InetAddress.cpp Socket.cpp
SRCS2 = client.cpp
OBJS1 = $(SRCS1:.cpp=.o)
OBJS2 = $(SRCS2:.cpp=.o)
EXEC1 = tcpepoll
EXEC2 = client


all: $(EXEC1) $(EXEC2)

$(EXEC1): $(OBJS1)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC2): $(OBJS2)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS1) $(EXEC1) $(OBJS2) $(EXEC2)

# 1            $<              表示第一个匹配的依赖
# 2            $@            表示目标
# 3            $^              所有依赖
# 4           %.o: %.c     自动匹配
