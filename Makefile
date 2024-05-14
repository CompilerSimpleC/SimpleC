CC= g++
OBJS= syntax_analyzer.cpp
OUT = main.out
INPUT = test.txt

all: $(OUT)

$(OUT) : $(OBJS)
	$(CC) -o $(OUT) $(OBJS) -std=c++17

test:
	./$(OUT) $(INPUT)
	rm $(OUT)

clean:
	rm -f *.out
	rm -f *.o
