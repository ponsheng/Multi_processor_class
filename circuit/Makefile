
PROC = 8
EXE= circuit
COMM = 2 #1  SERIAL 2 TREE

$(EXE): main.c
	mpiicc -g -Wall -DCOMM=$(COMM) -o $(EXE) main.c

run: $(EXE) 
	mpiexec -n $(PROC) ./$(EXE)


boot:
	mpdboot -n 4

trace:
	mpdtrace

clean:
	rm $(EXE)
