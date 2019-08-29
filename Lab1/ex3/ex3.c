#include <stdio.h>
#include <stdlib.h>
// stdlib includes malloc() and free()

// define instructions
#define PREVIOUS -1
#define NEXT 1
#define DELETE 0
#define INSERTSUBNODE 2
#define COLLAPSE 3 // New instruction compared to ex2.

// Node declarations

typedef struct SUBNODE
{
	int data;
	struct SUBNODE* nextSubNode;
} subNode;

typedef struct NODE
{
	int data;
	struct NODE* previousNode;
	struct NODE* nextNode;
	subNode* subNodeHead;
} node;

// Function prototypes 

void insertNodeNext(int,int,node*); 
void insertNodePrevious(int,int,node*);
void deleteNode(int,node*);
void printList(node*);
void printSubNodes(node*);
void deleteList(node*);
void insertSubNode(int,int,int,node*);
void deleteAllSubNodes(node*);

void collapseSubNodes(int, node*); //TODO 


// Start of main
int main()
{
	int position;
	int instruction;
	int subPosition;
	int value;

	void (*fptr[])() = {insertNodePrevious, deleteNode, insertNodeNext, insertSubNode, collapseSubNodes};
	
	// Declaration of the origin Node
	node* originNode = (node*)malloc(sizeof(node));
	originNode->previousNode = originNode;
	originNode->nextNode = originNode;
	originNode->data = 0;
	originNode->subNodeHead = NULL;
	
	
	
	while(scanf("%i%i",&instruction,&position) == 2)
		//scanf() returns an int that corresponds to the number of values scanned. 
	{
		// TODO: You would have to implement input and processing yourself. 
		if (instruction == DELETE)
		{
			(*fptr[instruction+1])(position, originNode);
		}
		else if (instruction == INSERTSUBNODE)
		{
			scanf("%i%i", &subPosition, &value);
			(*fptr[instruction+1])(position, subPosition, value, originNode);
		}
		else if (instruction == NEXT)
		{
			scanf("%i", &value);
			(*fptr[instruction+1])(position, value, originNode);
		}
		else if (instruction == PREVIOUS)
		{
			scanf("%i", &value);
			(*fptr[instruction+1])(position, value, originNode);
		}
		else if (instruction == COLLAPSE)
		{
			(*fptr[instruction+1])(position, originNode);
		}
	}
	printList(originNode);
	deleteList(originNode);
	
	printf("Circular List after delete\n");
	printList(originNode);
	
	free(originNode);
	
	return 0;
}


//Function Implementations 

void collapseSubNodes(int position,node* targetNode)
{
	// TODO
	int i;
	for(i=0;i<position;i++) {
		targetNode = targetNode->nextNode;
	}
	subNode *subPtr = targetNode->subNodeHead;
	int sum = 0;
	while(subPtr != NULL) {
		sum += subPtr->data;
		subPtr = subPtr->nextSubNode;
	}
	targetNode->data += sum;
	(*deleteAllSubNodes)(targetNode);
}

void insertSubNode(int position, int subPosition, int value, node *originNode)
{
	// TODO
	int i;
	for(i=0;i<position;i++) {
		originNode = originNode->nextNode;
	}
	subNode *newNode = (subNode *) malloc(sizeof(subNode));
	newNode->data = value;
	if(originNode->subNodeHead == NULL) {
		originNode->subNodeHead = newNode;
		newNode->nextSubNode = NULL;
	} else {
		if(subPosition == 0) {
			newNode->nextSubNode = originNode->subNodeHead;
			originNode->subNodeHead = newNode;
		} else {
			subNode *ptr = originNode->subNodeHead;
			for(i=0;i<subPosition-1;i++) {
				ptr = ptr->nextSubNode;
			}
			newNode->nextSubNode = ptr->nextSubNode;
			ptr->nextSubNode = newNode;
		}
	}
}

void insertNodePrevious(int position, int value, node *originNode)
{
	// TODO
	// Note: you can use a similar implementation as in ex1.
	int i;
	for (i = 0; i < position; ++i)
		originNode = originNode->nextNode;
	node *newNode = (node *)malloc(sizeof(node));
	newNode->data = value;
	newNode->nextNode = originNode;
	newNode->previousNode = originNode->previousNode;
	newNode->subNodeHead = NULL;
	originNode->previousNode->nextNode = newNode;
	originNode->previousNode = newNode;
	return;
}

void insertNodeNext(int position, int value, node *originNode)
{
	// TODO
	// Note: you can use a similar implementation as in ex1.
	int i;
	for (i = 0; i < position; i++)
		originNode = originNode->nextNode;
	node *newNode = (node *)malloc(sizeof(node));
	newNode->data = value;
	newNode->nextNode = originNode->nextNode;
	newNode->previousNode = originNode;
	newNode->subNodeHead = NULL;
	originNode->nextNode->previousNode = newNode;
	originNode->nextNode = newNode;
	return;
}

void deleteNode(int position, node *originNode)
{
	// TODO
	// Note: you can use a similar implementation as in ex1.
	int i;
	for (i = 0; i < position; i++)
		originNode = originNode->nextNode;
	originNode->previousNode->nextNode = originNode->nextNode;
	originNode->nextNode->previousNode = originNode->previousNode;
	deleteAllSubNodes(originNode);
	free(originNode);
	return;
}

void deleteAllSubNodes(node *targetNode)
{
	// Optional TODO
	// it is advised to use a helper function in conjunction with deleteNode().
	subNode *ptr = targetNode->subNodeHead;
	while(ptr != NULL) {
		subNode *temp = ptr;
		ptr = ptr->nextSubNode;
		free(temp);
	}
	targetNode->subNodeHead = NULL;
}

void deleteList(node *originNode)
{
	// TODO
	// Note: you can use a similar implementation as in ex1.
	deleteAllSubNodes(originNode);
	node *nodePtr = originNode->nextNode;
	originNode->nextNode = originNode;
	originNode->previousNode = originNode;
	while (nodePtr != originNode)
	{
		node *nextPtr = nodePtr->nextNode;
		deleteAllSubNodes(nodePtr);
		free(nodePtr);
		nodePtr = nextPtr;
	}
	return;
}



//Print list has been implemented for you. 
void printList(node* originNode)
{
	int count = 0;
	printf("Printing clockwise:\n");
	node* iterator = originNode->nextNode;
	printf("[Pos %i:%i]",0,originNode->data);
	// printing subNodes
	printSubNodes(originNode);
	printf("\n   |\n   v\n");
	while(iterator != originNode)
	{
		count++;
		printf("[Pos %i:%i]",count,iterator->data);
		// printing subNodes
		printSubNodes(iterator);
		printf("\n   |\n   v\n");
		iterator = iterator->nextNode;
	}
	printf("[Pos %i:%i]",0,originNode->data);
	// printing subNodes
	printSubNodes(originNode);		
	
	printf("\nPrinting counter-clockwise:\n");
	iterator = originNode->previousNode;
	printf("[Pos %i:%i]",0,originNode->data);
	// printing subNodes
	printSubNodes(originNode);
	printf("\n   |\n   v\n");
	while(iterator!= originNode)
	{
		printf("[Pos %i:%i]",count,iterator->data);
		// printing subNodes
		printSubNodes(iterator);
		printf("\n   |\n   v\n");
		iterator = iterator->previousNode;
		count--;
	}
	printf("[Pos %i:%i]",0,originNode->data);	
	// printing subNodes
	printSubNodes(originNode);
	printf("\n");

	return;
}

void printSubNodes(node* mainNode)
{
	int count = 0;
	subNode* iterator = mainNode->subNodeHead;
	while(iterator != NULL)
	{
		printf("->[subNode pos %i:%i]",count,iterator->data);
		iterator = iterator->nextSubNode;
		count++;
	}
}
