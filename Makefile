.PHONY: all clean
all:myintr
myintr:myintr.o
	g++ -o myintr myintr.o
myintr.o : LISP_Interpreter.cpp
	g++ -o myintr.o -c LISP_Interpreter.cpp
clean:
	rm -f myintr.o myintr 