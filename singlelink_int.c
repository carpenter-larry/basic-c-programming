/*
 * realization of in linklist
 * create at 2016-12-18 by liuzhangbin
 * email : larry.liu2011@hotmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// int_node data struct
struct int_node {
	int data;				// data
	struct int_node *next;	// pointer to next int_node
};

/*
 * create new int_node for linklist
 * return NULL if failed
 */
struct int_node *create_node(int value)
{
	// malloc memory for new int_node
	struct int_node *new_node = (struct int_node *)malloc(sizeof(struct int_node));
	if (new_node) {
		// initialize
		memset(new_node, 0, sizeof(struct int_node));
		new_node->data = value;
	} else {
		printf("[%s]error : malloc memory for new int_node failed!\n", __func__);
	}

	return new_node;
};

/*
 * add int_node to linklist
 * 1. p_head : pointer to linklist's head int_node
 * 2. p_node : new int node
 * return 0 : normal, 1 : abnormal
 */
int add_node_to_linklist(struct int_node **p_head, struct int_node *p_node)
{
	if (p_head == NULL) {
		printf("[%s]error : p_head is NULL!\n", __func__);
		return 1;
	}
	if (p_node == NULL) {
		printf("[%s]error : p_node is NULL!\n", __func__);
		return 1;
	}
	if (*p_head == NULL) {
		*p_head = p_node;
		return 0;
	}

	struct int_node **itr = p_head;
	while ((*itr)->next) {
		itr = &(*itr)->next;
	}
	(*itr)->next = p_node;

	return 0;
}

/*
 * delete int_node from linklist
 * 1. p_head  : pointer to linklist's head int_node
 * 2. p_value : int node value to delete
 * return 0 : normal, 1 : abnormal
 */
int del_node_from_linklist(struct int_node **p_head, int p_value)
{
	struct int_node **itr = p_head;
	while (*itr) {
		if ((*itr)->data == p_value) {
			struct int_node * tmp_node = *itr;
			*itr = (*itr)->next;
			free(tmp_node);
			tmp_node = NULL;
			return 0;
		}
		itr = &(*itr)->next;
	}

	return 1;
}

/*
 * check if p_value in linklist
 * 1. p_head  : pointer to linklist's head int_node
 * 2. p_value : int node value to check
 * return 0 : normal, 1 : abnormal
 */
int is_node_in_linklist(struct int_node **p_head, int p_value)
{
	struct int_node **itr = p_head;
	while (*itr) {
		if ((*itr)->data == p_value) {
			return 0;
		}
		itr = &(*itr)->next;
	}

	return 1;
}

/*
 * print linklist
 */
void dump_int_linklist(struct int_node **p_head)
{
	struct int_node **itr = p_head;
	int index = 0;
	while (*itr) {
		printf("index(%d), value(%d)\n", index++, (*itr)->data);
		itr = &(*itr)->next;
	}
}

// test
int main()
{
	struct int_node *head_node = NULL;
	struct int_node *new_node = create_node(1);
	add_node_to_linklist(&head_node, new_node);
	new_node = create_node(2);
	add_node_to_linklist(&head_node, new_node);
	dump_int_linklist(&head_node);
	printf("%d is %s linklist\n", 1, is_node_in_linklist(&head_node, 1) == 0 ? "in" : "not in");

	del_node_from_linklist(&head_node, 1);
	dump_int_linklist(&head_node);
	printf("%d is %s linklist\n", 1, is_node_in_linklist(&head_node, 1) == 0 ? "in" : "not in");

	return 0;
}

