CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -O2
SANITIZE = -fsanitize=address,undefined -fno-omit-frame-pointer -g

.PHONY: all run test clean

all: contact

contact: src/main.c src/contact.c src/contact.h
	$(CC) $(CFLAGS) -o $@ src/main.c src/contact.c

run: contact
	./contact

# Unit tests, built with AddressSanitizer/UBSan to catch memory errors.
test: tests/test_contact.c src/contact.c src/contact.h
	$(CC) $(CFLAGS) $(SANITIZE) -o test_contact tests/test_contact.c src/contact.c
	./test_contact

clean:
	rm -f contact test_contact test_contacts.dat
