#include "../src/contact.h"

#include <stdio.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond)                                                       \
	do                                                                    \
	{                                                                     \
		if (!(cond))                                                      \
		{                                                                 \
			fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__, #cond); \
			failures++;                                                   \
		}                                                                 \
	} while (0)

static PeoInfo Person(const char* name, int age)
{
	PeoInfo p;
	memset(&p, 0, sizeof(p));
	snprintf(p.name, sizeof(p.name), "%s", name);
	p.age = age;
	snprintf(p.gender, sizeof(p.gender), "%s", "female");
	snprintf(p.phone, sizeof(p.phone), "%s", "+60123456789");
	snprintf(p.addr, sizeof(p.addr), "%s", "Kuching");
	return p;
}

static void TestAddGrowsCapacity(void)
{
	Contact c;
	CHECK(InitContact(&c) == 0);
	CHECK(c.capacity == DEFAULT_CAPACITY);
	char name[MAX_NAME];
	for (int i = 0; i < 10; i++)
	{
		snprintf(name, sizeof(name), "person%d", i);
		PeoInfo p = Person(name, 20 + i);
		CHECK(AddContact(&c, &p) == 0);
	}
	CHECK(c.count == 10);
	CHECK(c.capacity >= 10);
	CHECK(c.data[9].age == 29);
	DestroyContact(&c);
	CHECK(c.data == NULL && c.count == 0);
}

static void TestFindDeleteModify(void)
{
	Contact c;
	InitContact(&c);
	const char* names[] = {"alice", "bob", "carol", "dave"};
	for (int i = 0; i < 4; i++)
	{
		PeoInfo p = Person(names[i], i);
		AddContact(&c, &p);
	}

	CHECK(FindByName(&c, "carol") == 2);
	CHECK(FindByName(&c, "zoe") == -1);

	CHECK(DelContact(&c, "bob") == 0);
	CHECK(c.count == 3);
	CHECK(strcmp(c.data[1].name, "carol") == 0); /* order kept */
	CHECK(DelContact(&c, "dave") == 0);          /* last element */
	CHECK(DelContact(&c, "bob") == -1);
	CHECK(c.count == 2);

	PeoInfo updated = Person("caroline", 40);
	CHECK(ModifyContact(&c, "carol", &updated) == 0);
	CHECK(FindByName(&c, "carol") == -1);
	CHECK(c.data[FindByName(&c, "caroline")].age == 40);
	CHECK(ModifyContact(&c, "nobody", &updated) == -1);
	DestroyContact(&c);
}

static void TestPhoneIsStoredAsText(void)
{
	/* the original version read the phone number with %d into a char array */
	Contact c;
	InitContact(&c);
	PeoInfo p = Person("eve", 30);
	snprintf(p.phone, sizeof(p.phone), "%s", "012-3456 789");
	AddContact(&c, &p);
	CHECK(strcmp(c.data[0].phone, "012-3456 789") == 0);
	DestroyContact(&c);
}

static void TestSort(void)
{
	Contact c;
	InitContact(&c);
	const char* names[] = {"mia", "adam", "zack", "lily"};
	for (int i = 0; i < 4; i++)
	{
		PeoInfo p = Person(names[i], i);
		AddContact(&c, &p);
	}
	SortContact(&c);
	CHECK(strcmp(c.data[0].name, "adam") == 0);
	CHECK(strcmp(c.data[1].name, "lily") == 0);
	CHECK(strcmp(c.data[2].name, "mia") == 0);
	CHECK(strcmp(c.data[3].name, "zack") == 0);
	DestroyContact(&c);
}

static void TestSaveAndLoadRoundTrip(void)
{
	const char* path = "test_contacts.dat";
	remove(path);

	Contact c;
	InitContact(&c);
	CHECK(LoadContact(&c, path) == 0); /* missing file is not an error */
	for (int i = 0; i < 5; i++)
	{
		char name[MAX_NAME];
		snprintf(name, sizeof(name), "user%d", i);
		PeoInfo p = Person(name, i);
		AddContact(&c, &p);
	}
	CHECK(SaveContact(&c, path) == 0);
	DestroyContact(&c);

	Contact loaded;
	InitContact(&loaded);
	CHECK(LoadContact(&loaded, path) == 5);
	CHECK(loaded.count == 5);
	CHECK(strcmp(loaded.data[4].name, "user4") == 0);
	CHECK(strcmp(loaded.data[4].phone, "+60123456789") == 0);
	DestroyContact(&loaded);

	/* saving an empty book produces an empty file */
	Contact empty;
	InitContact(&empty);
	CHECK(SaveContact(&empty, path) == 0);
	CHECK(LoadContact(&empty, path) == 0);
	DestroyContact(&empty);
	remove(path);
}

int main(void)
{
	TestAddGrowsCapacity();
	TestFindDeleteModify();
	TestPhoneIsStoredAsText();
	TestSort();
	TestSaveAndLoadRoundTrip();

	if (failures == 0)
		printf("All tests passed.\n");
	else
		printf("%d check(s) failed.\n", failures);
	return failures == 0 ? 0 : 1;
}
