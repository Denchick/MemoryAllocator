#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

struct header_t {
	size_t size;
	unsigned is_free;
	struct header_t *next;
};

struct header_t *head = NULL, *tail = NULL;
pthread_mutex_t global_malloc_lock;

struct header_t *get_free_block(size_t size)
{
	struct header_t *current = head;
	while(current) 
	{
		if (current->is_free && current->size >= size)
			return current;
		current = current->next;
	}
	return NULL;
}

void free(void *block)
{
	//printf("free");
	struct header_t *header, *tmp;
	void *programbreak;

	if (!block)
		return;
	pthread_mutex_lock(&global_malloc_lock);
	header = (struct header_t*)block - 1;
	programbreak = sbrk(0);


	if ((char*)block + header->size != programbreak) 
	{
		header->is_free = 1;
		pthread_mutex_unlock(&global_malloc_lock);
		return;
	}

	if (head == tail) 
		head = tail = NULL;
	else 
	{
		tmp = head;
		while (tmp) 
		{
			if(tmp->next == tail) 
			{
				tmp->next = NULL;
				tail = tmp;
			}
			tmp = tmp->next;
		}
	}
	sbrk(0 - header->size - sizeof(struct header_t));
	pthread_mutex_unlock(&global_malloc_lock);
}

void *malloc(size_t size)
{
	//printf("malloc");
	size_t total_size;
	void *block;
	struct header_t *header;
	if (!size)
		return NULL;
	pthread_mutex_lock(&global_malloc_lock);
	header = get_free_block(size);
	if (header) 
	{
		header->is_free = 0;
		pthread_mutex_unlock(&global_malloc_lock);
		return (void*)(header + 1);
	}

	total_size = sizeof(struct header_t) + size;
	block = sbrk(total_size);
	if (block == (void*) -1) 
	{
		pthread_mutex_unlock(&global_malloc_lock);
		return NULL;
	}
	header = block;
	header->size = size;
	header->is_free = 0;
	header->next = NULL;
	if (!head)
		head = header;
	if (tail)
		tail->next = header;
	tail = header;
	pthread_mutex_unlock(&global_malloc_lock);
	return (void*)(header + 1);
}

void *calloc(size_t num, size_t nsize)
{
	size_t size;
	void *block;
	if (!num || !nsize)
		return NULL;
	size = num * nsize;
	if (nsize != size / num)
		return NULL;
	block = malloc(size);
	if (!block)
		return NULL;
	memset(block, 0, size);
	return block;
}

void *realloc(void *block, size_t size)
{
	struct header_t *header;
	void *new_block;
	if (!block || !size)
		return malloc(size);
	header = (struct header_t*)block - 1;
	if (header->size >= size)
		return block;
	new_block = malloc(size);
	if (new_block) {
		memcpy(new_block, block, header->size);
		free(block);
	}
	return new_block;
}

void dump_memory()
{
	struct header_t *current = head;
	printf("head = %p, tail = %p \n", (void*)head, (void*)tail);
	while(current) 
	{
		printf("address = %p, size = %zu, is_free=%u, next=%p\n",
			(void*)current, current->size, current->is_free, (void*)current->next);
		current = current->next;
	}
}
