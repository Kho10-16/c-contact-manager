#include "contact.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "contacts.dat"

/* Reads one line into buf without the trailing newline. Returns 0, or -1 at end of input. */
static int ReadLine(const char* prompt, char* buf, size_t size)
{
	printf("%s", prompt);
	fflush(stdout);
	if (fgets(buf, (int)size, stdin) == NULL)
		return -1;

	size_t len = strcspn(buf, "\n");
	if (buf[len] == '\n')
	{
		buf[len] = '\0';
	}
	else
	{
		/* line was longer than the buffer: discard the rest */
		int ch;
		while ((ch = getchar()) != '\n' && ch != EOF)
			;
	}
	return 0;
}

/* Prompts until a whole number in [min, max] is entered. Returns 0, or -1 at end of input. */
static int ReadInt(const char* prompt, int min, int max, int* out)
{
	char buf[32];
	for (;;)
	{
		if (ReadLine(prompt, buf, sizeof(buf)) != 0)
			return -1;
		char* end;
		long value = strtol(buf, &end, 10);
		if (end != buf && *end == '\0' && value >= min && value <= max)
		{
			*out = (int)value;
			return 0;
		}
		printf("Please enter a number from %d to %d.\n", min, max);
	}
}

/* Prompts until a non-empty value is entered. */
static int ReadText(const char* prompt, char* buf, size_t size)
{
	for (;;)
	{
		if (ReadLine(prompt, buf, size) != 0)
			return -1;
		if (buf[0] != '\0')
			return 0;
		printf("This field cannot be empty.\n");
	}
}

static int ReadPerson(PeoInfo* p)
{
	memset(p, 0, sizeof(*p));
	if (ReadText("Name: ", p->name, sizeof(p->name)) != 0)
		return -1;
	if (ReadInt("Age: ", 0, 150, &p->age) != 0)
		return -1;
	if (ReadText("Gender: ", p->gender, sizeof(p->gender)) != 0)
		return -1;
	if (ReadText("Phone: ", p->phone, sizeof(p->phone)) != 0)
		return -1;
	if (ReadText("Address: ", p->addr, sizeof(p->addr)) != 0)
		return -1;
	return 0;
}

static void PrintHeader(void)
{
	printf("%-20s %-5s %-10s %-16s %-30s\n", "Name", "Age", "Gender", "Phone", "Address");
}

static void PrintPerson(const PeoInfo* p)
{
	printf("%-20s %-5d %-10s %-16s %-30s\n", p->name, p->age, p->gender, p->phone, p->addr);
}

static void ShowContact(const Contact* pc)
{
	if (pc->count == 0)
	{
		printf("The contact book is empty.\n");
		return;
	}
	PrintHeader();
	for (size_t i = 0; i < pc->count; i++)
		PrintPerson(&pc->data[i]);
}

static void Menu(void)
{
	printf("\n==== Contact Manager ====\n");
	printf("1. Add       2. Delete\n");
	printf("3. Search    4. Modify\n");
	printf("5. Show all  6. Sort by name\n");
	printf("0. Save and exit\n");
}

int main(void)
{
	Contact con;
	if (InitContact(&con) != 0)
	{
		perror("InitContact");
		return EXIT_FAILURE;
	}
	int loaded = LoadContact(&con, DATA_FILE);
	if (loaded < 0)
	{
		perror("LoadContact");
		DestroyContact(&con);
		return EXIT_FAILURE;
	}
	printf("Loaded %d contact(s) from %s\n", loaded, DATA_FILE);

	int input = -1;
	while (input != 0)
	{
		Menu();
		if (ReadInt("Select: ", 0, 6, &input) != 0)
			input = 0; /* end of input: save and quit */

		PeoInfo person;
		char name[MAX_NAME];
		int pos;
		switch (input)
		{
		case 1:
			if (ReadPerson(&person) != 0)
				break;
			if (FindByName(&con, person.name) != -1)
				printf("%s is already in the book.\n", person.name);
			else if (AddContact(&con, &person) != 0)
				printf("Out of memory, contact not added.\n");
			else
				printf("Added.\n");
			break;
		case 2:
			if (ReadText("Name to delete: ", name, sizeof(name)) != 0)
				break;
			printf(DelContact(&con, name) == 0 ? "Deleted.\n" : "Cannot find that contact.\n");
			break;
		case 3:
			if (ReadText("Name to search: ", name, sizeof(name)) != 0)
				break;
			pos = FindByName(&con, name);
			if (pos == -1)
			{
				printf("Cannot find that contact.\n");
				break;
			}
			PrintHeader();
			PrintPerson(&con.data[pos]);
			break;
		case 4:
			if (ReadText("Name to modify: ", name, sizeof(name)) != 0)
				break;
			if (FindByName(&con, name) == -1)
			{
				printf("Cannot find that contact.\n");
				break;
			}
			printf("Enter the new details:\n");
			if (ReadPerson(&person) != 0)
				break;
			pos = FindByName(&con, person.name);
			if (strcmp(person.name, name) != 0 && pos != -1)
			{
				printf("%s is already in the book.\n", person.name);
				break;
			}
			ModifyContact(&con, name, &person);
			printf("Updated.\n");
			break;
		case 5:
			ShowContact(&con);
			break;
		case 6:
			SortContact(&con);
			printf("Sorted by name.\n");
			break;
		case 0:
			break;
		}
	}

	int status = EXIT_SUCCESS;
	if (SaveContact(&con, DATA_FILE) != 0)
	{
		perror("SaveContact");
		status = EXIT_FAILURE;
	}
	else
	{
		printf("Saved %zu contact(s) to %s. Bye!\n", con.count, DATA_FILE);
	}
	DestroyContact(&con);
	return status;
}
