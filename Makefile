CC= g++
OBJS= syntax_analyzer.cpp
OUT = main.out
INPUT1 = input1
INPUT2 = input2 
INPUT3 = input3
INPUT4 = input4
INPUT5 = input5
INPUT6 = input6
INPUT7 = input7
INPUT8 = input8
INPUT9 = input9
INPUT10 = input10

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
