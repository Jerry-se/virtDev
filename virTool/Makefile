TINYXML2_INC := ../third_party/tinyxml2/include
TINYXML2_LIB := ../third_party/tinyxml2/lib
THIRD_PARTY_LIB_L := $(addprefix -L , $(TINYXML2_LIB))

CC = g++

CFLAGS  := -I $(TINYXML2_INC)
LDFLAGS := -lvirt -lvirt-qemu -lpthread -ltinyxml2 $(THIRD_PARTY_LIB_L)

OBJS=main.o virTool.o vir_helper.o
all: virTool
virTool: $(OBJS)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@
%.o:%.cpp
	$(CC) $^ -c -Wall -g $(CFLAGS)  -o $@
clean:
	-@rm -f *.o
	-@rm -f virTool
