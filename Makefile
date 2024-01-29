build:
	gcc process_generator.c -o process_generator
	gcc clk.c -o clk
	gcc scheduler.c -o scheduler -lm
	gcc process.c -o process
	gcc test_generator.c -o test_generator

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	./process_generator processes.txt -sch 2 -q 1 -mem 0