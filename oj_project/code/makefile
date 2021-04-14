INCLUDE_PATH=-I /home/wudu/third_part/include -I /usr/include/jsoncpp
LIB_PATH=-L /home/wudu/third_part/lib -lctemplate -ljsoncpp -lpthread

BIN=../bin/oj_svr

$(BIN):oj_server.cpp
	g++ $^ -o $@ -g $(INCLUDE_PATH) $(LIB_PATH)

.PHONY:clean
clean:
	rm $(BIN)
