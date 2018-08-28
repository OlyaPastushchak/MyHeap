#include <string.h>	
#include<stdio.h>
unsigned char* system;
unsigned char* data;
unsigned char* free;
size_t heapSize;

size_t* terminals;
size_t countTerminals;

size_t MAX_BLOCKS = 50;

void heapInit(size_t bytes)
{
	if (bytes != 0)
	{
		system = malloc(bytes / 8 + (bytes % 8 == 0 ? 0 : 1));
		memset(system, 0, bytes / 8 + (bytes % 8 == 0 ? 0 : 1));
		data = malloc(bytes);
		memset(data, 0, bytes);
		heapSize = bytes;
		terminals = malloc(MAX_BLOCKS);
		countTerminals = 0;
		free = data;
	}
}

int isByteFree(size_t number)
{
	if (number < heapSize)
	{
		char byteToCheck = *(system + number / 8);
		char mask = 128 >> (number % 8);
		if ((byteToCheck & mask) == 0)
		{
			return 1;
		}
	}
	return 0;
}

void setByteFree(size_t number)
{
	char zero = ~(128 >> (number % 8));
	*(system + number / 8) &= zero;
}

void setByteBusy(size_t number)
{
	char one = 128 >> (number % 8);
	*(system + number / 8) |= one;
}

void* myMalloc(size_t size)
{
	if (size > heapSize || size == 0)
	{
		return NULL;
	}
	size_t number = free - data;
	unsigned char* allocated = free;//pointer to first byte that we will return
	size_t freeFound = 0;//number of free bytes found
	while (freeFound != size)
	{
		if (isByteFree(number))
		{
			++freeFound;
			++number;
		}
		else
		{
			freeFound = 0;
			while (!isByteFree(number))
			{
				++number;
			}
			allocated = data + number;
		}
	}

	size_t counter = number - 1;//use to set bytes as busy

	//set them as busy
	while (freeFound > 0)
	{
		setByteBusy(counter);
		counter--;
		freeFound--;
	}

	//set terminal
	terminals[countTerminals] = number;
	countTerminals++;

	/*find pointer to the first block of free memory*/
	if (allocated == free)
	{
		while (!isByteFree(number))
		{
			++number;
		}
		free = data + number;
	}

	return allocated;
}


int isTerminal(size_t value)
{
	for (int i = 0; i < countTerminals; i++)
	{
		if (terminals[i] == value)
			return 1;
	}
	return 0;
}

size_t indexInArray(size_t value)
{
	for (int i = 0; i < countTerminals; i++)
	{
		if (terminals[i] == value)
			return i;
	}
	return -1;
}



void myFree(void* ptr)
{
	size_t number = (unsigned char*)ptr - data;
	do
	{
		setByteFree(number);
		number++;
	} while (!isTerminal(number));

	/*change pointer to the first block of free memory*/
	if ((unsigned char*)ptr < free)
	{
		free = (unsigned char*)ptr;
	}

	//delete terminal of this block from array
	size_t index = indexInArray(number);
	memcpy(terminals + index, terminals + index + 1, countTerminals - index - 1);
	countTerminals--;
}

size_t calculateSize(void* ptr)
{
	size_t number = (unsigned char*)ptr - data;
	size_t count = 0;
	do
	{
		count++;
		number++;
	} while (!isTerminal(number));

	return count;
}

void* myRealloc(void *ptr, size_t newSize)
{
	if (ptr == NULL)
	{
		return myMalloc(newSize);//створюється новий блок і ф-ція повертає вказівник на нього
	}
	size_t number = (unsigned char*)ptr - data;

	size_t oldSize = calculateSize(ptr);
	if (newSize == oldSize)
	{
		return ptr;
	}

	unsigned char* newPlace = myMalloc(newSize);
	if (newSize < oldSize)
	{
		memcpy(newPlace, (unsigned char*)ptr, newSize);
	}
	else
	{
		memcpy(newPlace, (unsigned char*)ptr, oldSize);
	}
	memset(ptr, 0, oldSize);
	myFree(ptr);

	return newPlace;
}


void printMap()
{
	size_t counter = 0;
	printf("\nfirst: %p\nlast: %p\n", data, data + heapSize -1);
	while (counter != heapSize)
	{
		printf("\t%s", (!isByteFree(counter)) ? "1" : "0");
		++counter;

		if (isTerminal(counter))
		{
			printf(" <-\n");
		}
		else
		{
			printf("\n");
		}
	}
	printf("\nterminals:");
	for (int i = 0; i < countTerminals; i++)
	{
		printf("%d ", terminals[i]);
	}
	printf("\n");
}


void main()
{
	heapInit(20);
	printMap();
	char* a = myMalloc(2);
	printMap();
	char* b = myMalloc(5);
	printMap();
	char*c = myMalloc(3);
	printMap();
	myFree(b);
	printMap();
	char* d = myMalloc(6);
	printMap();
	myRealloc(d, 5);
	printMap();
	


	////0000 0000 0011 1111 1110 0000 0000 0000
	//unsigned char aa = *(system);
	//printf("%d ", aa);// 0000 0000
	//unsigned char aaa = *(system + 1);
	//printf("%d ", aaa);//0011 1111
	//unsigned char aaaa = *(system + 2);
	//printf("%d ", aaaa);//1110 0000
	//unsigned char aaaaa = *(system + 3);
	//printf("%d ", aaaaa);//0000 0000


	getchar();
}
