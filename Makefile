nvrtime: nvrtime.o
	$(CC) -o $@ $^ $(LDFLAGS)
nvrtime.o: nvrtime.h
