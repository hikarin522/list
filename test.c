
#include <stdio.h>
#include "list.h"
// list.c も一緒にコンパイルしてね

bool data_less(void *a, void *b)
{
	return *(int *)a < *(int *)b;
}

int main(void)
{
	FILE *ifs;
	char ifname[100] = {0};

	int i, tmp;
	
	list data;
	list_iterator it;
	list_iterator itbegin, itend;

	data = list_new(sizeof(int));
	if (data == NULL)
		return 0;

	list_set_less(data, data_less);
	
	printf("Input filename : ");
	scanf("%s", ifname);

	ifs = fopen(ifname, "r");
	if (ifs == NULL)
		return 0;

	while (!feof(ifs) && fscanf(ifs, "%d", &tmp) != EOF)
	{
		int *temp = (int *)list_push_back(data);
		if (temp == NULL)
			return 0;

		*temp = tmp;
	}

	fclose(ifs);

	printf("Pre: ");
	for (it = list_begin(data); !list_match(it, list_end(data)); list_increment(&it))
		printf(" %d", *(int *)list_at(it));
	printf("\n");
	
	list_quick_sort(data, LESS);
	printf("Post:");
	for (it = list_begin(data); !list_match(it, list_end(data)); list_increment(&it))
		printf(" %d", *(int *)list_at(it));
	printf("\n");

	list_delete(data);

	return 0;
}

