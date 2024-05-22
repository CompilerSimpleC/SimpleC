CC= g++
OBJS= syntax_analyzer.cpp
OUT = main.out
INPUT1 = test1.txt
INPUT2 = test2.txt
INPUT3 = test3.txt
INPUT4 = test4.txt
INPUT5 = test5.txt
INPUT6 = test6.txt
INPUT7 = test7.txt
INPUT8 = test8.txt
INPUT9 = test9.txt
INPUT10 = test10.txt

all: $(OUT)

$(OUT) : $(OBJS)
	$(CC) -o $(OUT) $(OBJS) -std=c++17

test1:
	./$(OUT) $(INPUT1)

test2:
	./$(OUT) $(INPUT2)

test3:
	./$(OUT) $(INPUT3)

test4:
	./$(OUT) $(INPUT4)

test5:
	./$(OUT) $(INPUT5)

test6:
	./$(OUT) $(INPUT6)

test7:
	./$(OUT) $(INPUT7)

test8:
	./$(OUT) $(INPUT8)

test9:
	./$(OUT) $(INPUT9)

test10:
	./$(OUT) $(INPUT10)

clean:
	rm -f *.out
	rm -f *.o
