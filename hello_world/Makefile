
PROC = 8 
EXE= hello

$(EXE): main.c
	mpiicc -g -Wall -o $(EXE) main.c

run: $(EXE)
	mpiexec -n $(PROC) ./$(EXE)

boot:
	mpdboot -n 4

trace:
	mpdtrace
