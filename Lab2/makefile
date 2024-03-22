# compile code for Lab1
compile : main.c consumer.c producer.c buffer.c buffer.h
	gcc -pthread -o lab2 main.c

# clean up
clean :
	rm lab2

# zip the files
zip :
	zip lab2.zip main.c consumer.c producer.c buffer.c buffer.h makefile

# clean and compile
rebuild : clean compile