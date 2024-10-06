assembler: assembler.o pre_processor.o first_pass.o second_pass.o inst_error_check.o
	gcc -pedantic -ansi -Wall assembler.o pre_processor.o first_pass.o second_pass.o inst_error_check.o -o assembler
assembler.o: assembler.c first_pass.h second_pass.h pre_processor.h
	gcc -c -pedantic -ansi -Wall assembler.c -o assembler.o
pre_processor.o: pre_processor.c pre_processor.h 
	gcc -c -pedantic -ansi -Wall pre_processor.c -o pre_processor.o
first_pass.o: first_pass.c first_pass.h second_pass.h
	gcc -c -pedantic -ansi -Wall first_pass.c -o first_pass.o
second_pass.o: second_pass.c second_pass.h first_pass.h
	gcc -c -pedantic -ansi -Wall second_pass.c -o second_pass.o
inst_error_check.o: inst_error_check.c inst_error_check.h
	gcc -c -pedantic -ansi -Wall inst_error_check.c -o inst_error_check.o
