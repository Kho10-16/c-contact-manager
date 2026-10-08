#ifndef CONTACT_H
#define CONTACT_H

#include <stddef.h>

#define MAX_NAME 20
#define MAX_GENDER 10
#define MAX_PHONE 16
#define MAX_ADDR 30

#define DEFAULT_CAPACITY 3
#define GROWTH_STEP 2

typedef struct PeoInfo
{
	char name[MAX_NAME];
	int age;
	char gender[MAX_GENDER];
	char phone[MAX_PHONE];
	char addr[MAX_ADDR];
} PeoInfo;

/* Dynamic contact book: data grows with realloc as contacts are added. */
typedef struct Contact
{
	PeoInfo* data;
	size_t count;    /* number of contacts stored */
	size_t capacity; /* number of slots allocated */
} Contact;

/* Allocates the initial storage. Returns 0 on success, -1 on allocation failure. */
int InitContact(Contact* pc);

/* Frees the storage; the book can be re-initialised afterwards. */
void DestroyContact(Contact* pc);

/* Copies *person into the book, growing it if needed. Returns 0 on success, -1 if out of memory. */
int AddContact(Contact* pc, const PeoInfo* person);

/* Returns the index of the first contact called name, or -1 if there is none. */
int FindByName(const Contact* pc, const char* name);

/* Removes the contact called name, keeping the others in order. Returns 0 if removed, -1 if not found. */
int DelContact(Contact* pc, const char* name);

/* Replaces the contact called name with *person. Returns 0 on success, -1 if not found. */
int ModifyContact(Contact* pc, const char* name, const PeoInfo* person);

/* Sorts the contacts by name (ascending). */
void SortContact(Contact* pc);

/* Writes all contacts to a binary file. Returns 0 on success, -1 on I/O error. */
int SaveContact(const Contact* pc, const char* path);

/*
 * Appends the contacts stored in a binary file to the book.
 * Returns the number loaded (0 if the file does not exist yet), or -1 on error.
 */
int LoadContact(Contact* pc, const char* path);

#endif
