#include "contact.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int InitContact(Contact* pc)
{
	pc->count = 0;
	pc->data = calloc(DEFAULT_CAPACITY, sizeof(PeoInfo));
	if (pc->data == NULL)
	{
		pc->capacity = 0;
		return -1;
	}
	pc->capacity = DEFAULT_CAPACITY;
	return 0;
}

void DestroyContact(Contact* pc)
{
	free(pc->data);
	pc->data = NULL;
	pc->count = 0;
	pc->capacity = 0;
}

/* Makes room for at least one more contact. */
static int CheckCapacity(Contact* pc)
{
	if (pc->count < pc->capacity)
		return 0;

	/* realloc into a temporary so the old block is not leaked if it fails */
	PeoInfo* ptr = realloc(pc->data, (pc->capacity + GROWTH_STEP) * sizeof(PeoInfo));
	if (ptr == NULL)
		return -1;
	pc->data = ptr;
	pc->capacity += GROWTH_STEP;
	return 0;
}

int AddContact(Contact* pc, const PeoInfo* person)
{
	if (CheckCapacity(pc) != 0)
		return -1;
	pc->data[pc->count] = *person;
	pc->count++;
	return 0;
}

int FindByName(const Contact* pc, const char* name)
{
	for (size_t i = 0; i < pc->count; i++)
	{
		if (strcmp(pc->data[i].name, name) == 0)
			return (int)i;
	}
	return -1;
}

int DelContact(Contact* pc, const char* name)
{
	int pos = FindByName(pc, name);
	if (pos == -1)
		return -1;

	/* shift the later contacts left over the deleted one */
	memmove(pc->data + pos, pc->data + pos + 1, (pc->count - pos - 1) * sizeof(PeoInfo));
	pc->count--;
	return 0;
}

int ModifyContact(Contact* pc, const char* name, const PeoInfo* person)
{
	int pos = FindByName(pc, name);
	if (pos == -1)
		return -1;
	pc->data[pos] = *person;
	return 0;
}

static int CmpPeoByName(const void* e1, const void* e2)
{
	return strcmp(((const PeoInfo*)e1)->name, ((const PeoInfo*)e2)->name);
}

void SortContact(Contact* pc)
{
	if (pc->count > 1)
		qsort(pc->data, pc->count, sizeof(PeoInfo), CmpPeoByName);
}

int SaveContact(const Contact* pc, const char* path)
{
	FILE* pfWrite = fopen(path, "wb");
	if (pfWrite == NULL)
		return -1;

	size_t written = fwrite(pc->data, sizeof(PeoInfo), pc->count, pfWrite);
	int closed = fclose(pfWrite);
	return (written == pc->count && closed == 0) ? 0 : -1;
}

int LoadContact(Contact* pc, const char* path)
{
	FILE* pfRead = fopen(path, "rb");
	if (pfRead == NULL)
		return errno == ENOENT ? 0 : -1; /* no file yet: start with an empty book */

	PeoInfo tmp;
	int loaded = 0;
	while (fread(&tmp, sizeof(PeoInfo), 1, pfRead) == 1)
	{
		/* never trust the file: make sure every string is terminated */
		tmp.name[MAX_NAME - 1] = '\0';
		tmp.gender[MAX_GENDER - 1] = '\0';
		tmp.phone[MAX_PHONE - 1] = '\0';
		tmp.addr[MAX_ADDR - 1] = '\0';
		if (AddContact(pc, &tmp) != 0)
		{
			fclose(pfRead);
			return -1;
		}
		loaded++;
	}
	int failed = ferror(pfRead);
	fclose(pfRead);
	return failed ? -1 : loaded;
}
