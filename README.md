# Contact Manager (C)

![CI](https://github.com/Kho10-16/c-contact-manager/actions/workflows/ci.yml/badge.svg)

A command-line contact book written in C11. Contacts live in a dynamically growing array and are
saved to a binary file, so they are still there the next time you run the program.

## Features

- Add, delete, search, modify and list contacts (name, age, gender, phone, address)
- Sort contacts by name with `qsort`
- **Dynamic memory**: storage starts at 3 slots and grows with `realloc`; a failed allocation
  leaves existing data intact instead of leaking or corrupting it
- **Persistence**: contacts are loaded from `contacts.dat` on start-up and saved on exit
- **Safe input**: every field is read with bounded `fgets`, numbers are validated with `strtol`,
  and invalid or over-long input never overflows a buffer or loops forever
- **Tested**: unit tests for the core logic run under AddressSanitizer and UndefinedBehaviorSanitizer,
  and GitHub Actions builds with `-Wall -Wextra -Wpedantic -Werror` on every push

## Getting started

Requires a C compiler (gcc or clang) and `make`.

```bash
make        # build ./contact
make run    # build and start the program
make test   # run the unit tests with sanitizers
```

```
==== Contact Manager ====
1. Add       2. Delete
3. Search    4. Modify
5. Show all  6. Sort by name
0. Save and exit
Select: 5
Name                 Age   Gender     Phone            Address
Alice Tan            22    Female     012-345 6789     Kuching
Bob                  30    Male       0198765432       Miri
```

## Design

The program is split so the core logic can be tested without a terminal:

| File | Responsibility |
|---|---|
| `src/contact.h` / `src/contact.c` | The contact book: memory management, add/find/delete/modify/sort, binary file I/O. No user interaction. |
| `src/main.c` | Menu, prompts and input validation. |
| `tests/test_contact.c` | Unit tests for `contact.c`. |

Deleting a contact shifts the later ones down with `memmove`, so the order is kept. The save file
is a raw array of `PeoInfo` records; on load each string field is re-terminated so a corrupted
file cannot cause an out-of-bounds read.
