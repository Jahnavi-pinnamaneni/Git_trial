#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "llfifo.h"

typedef struct node
{
    void * data;
    struct node* next;
}fifo_node;

typedef struct llfifo_s
{
    fifo_node* unuse;
    fifo_node* inuse;
    fifo_node* inuse_rear;
    int q_capacity;
    int q_len;
}llfifo_t;

llfifo_t *llfifo_create(int capacity)
{
    if((capacity > ((1ULL<<31)-1)) || (capacity < 0))
        return NULL;
    llfifo_t *fifo = NULL;
    fifo = (llfifo_t *)malloc(sizeof(llfifo_t));
    if(fifo == NULL)
        return NULL;
    fifo->unuse = NULL;
    fifo->inuse = NULL;
    fifo->inuse_rear = NULL;
    fifo->q_len = 0;
    fifo_node *temp = NULL;
    fifo_node *temp1 = NULL;
    for(int i = 0; i < capacity; i++)
    {
        temp = (fifo_node *)malloc(sizeof(fifo_node));
        if(temp == NULL)
            return 0;
        temp->next = NULL;
        if(temp1 == NULL)
            fifo->unuse = temp1 = temp;
        else
        {
            temp1->next = temp;
            temp1 = temp;
        }

    }
    //printf("%p\n",fifo->unuse);

    fifo->q_capacity = capacity;
    return fifo;

}

int llfifo_enqueue(llfifo_t *fifo, void * element)
{
    if((fifo->unuse != NULL))
    {
        if((fifo->inuse == NULL) && (fifo->q_len == 0))
        {
            printf("adding first element\n");
            fifo->inuse = fifo->unuse;
            fifo->unuse = fifo->unuse->next;
            fifo->inuse->data = element;
            fifo->inuse->next = NULL;
            fifo->inuse_rear = fifo->inuse;
            fifo->q_len++;
        }
        else if(fifo->inuse != NULL)
        {
            printf("adding elements from existing capacity\n");
            fifo->inuse->next = fifo->unuse;
            fifo->inuse = fifo->unuse;
            fifo->unuse = fifo->unuse->next;
            fifo->q_len++;
        }
    }
    else
    {
        printf("mallocing new element\n");
        fifo_node *temp = (fifo_node *)malloc(sizeof(fifo_node));
        temp->data = element;
        temp->next = NULL;
        if(fifo->q_len == 0)
        {
            fifo->inuse = temp;
            fifo->inuse_rear = fifo->inuse;
        }
        else
        {
          fifo->inuse->next = temp;
          fifo->inuse = temp;
        }

        fifo->q_capacity++;
        fifo->q_len++;
    }
    return fifo->q_len;
}

void *llfifo_dequeue(llfifo_t *fifo)
{
    if((fifo == NULL) | (fifo->inuse_rear == NULL))
        return NULL;

    printf("fifo unuse %p\n", fifo->unuse);
    void * data_ele = fifo->inuse_rear->data;
    if(fifo->unuse == NULL)
    {
        printf("inside first dequeue\n");
        fifo->unuse = fifo->inuse_rear;
        fifo->inuse_rear = fifo->inuse_rear->next;
        printf("inuse rear %p\n", fifo->inuse_rear);
        fifo->unuse->next = NULL;
    }
    else if((fifo->unuse->next == NULL) && (fifo->unuse != NULL))
    {
        printf("inside next dequeue\n");
       fifo->unuse->next = fifo->inuse_rear;
       fifo->unuse = fifo->inuse_rear;
       fifo->inuse_rear = fifo->inuse_rear->next;
       fifo->unuse->next = NULL;
    }
    fifo->q_len--;
    return data_ele;
}

int llfifo_length(llfifo_t *fifo)
{
    return fifo->q_len;
}

int llfifo_capacity(llfifo_t *fifo)
{
    return fifo->q_capacity;
}

void llfifo_destroy(llfifo_t *fifo)
{
    fifo_node * temp;
    while(fifo->inuse_rear != NULL)
    {
        temp = fifo->inuse_rear;
        fifo->inuse_rear = fifo->inuse_rear->next;
        free(temp);
    }
    while(fifo->unuse != NULL)
    {
        temp = fifo->unuse;
        fifo->unuse = fifo->unuse->next;
        free(temp);
    }
    free(fifo);

}


