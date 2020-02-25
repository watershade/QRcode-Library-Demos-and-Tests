CXX ?= g++

CXXFLAGS += -c -Wall -std=c++11 $(shell pkg-config --cflags opencv4 zbar quirc) 
LDFLAGS += $(shell pkg-config --libs --static opencv4 zbar quirc)


TARGET1 = test_qr
OBJECT1 = test_qr.o

TARGET2 = test_zbar
OBJECT2 = test_qr_zbar.o

TARGET3 = test_quirc
OBJECT3 = test_qr_quirc


TARGET = $(TARGET1) $(TARGET2) $(TARGET3)

all: $(TARGET)

$(TARGET1): $(OBJECT1); $(CXX) $< -o $@ $(LDFLAGS)
$(TARGET2): $(OBJECT2); $(CXX) $< -o $@ $(LDFLAGS) 
$(TARGET3): $(OBJECT3); $(CXX) $< -o $@ $(LDFLAGS) 

%.o: %.cpp; $(CXX) $< -o $@ $(CXXFLAGS)

clean: ; rm -f *.o $(TARGET) 

