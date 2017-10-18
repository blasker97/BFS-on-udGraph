/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Brian L
 *
 * Created on October 14, 2017, 12:00 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "input_error.h"
#include <limits.h>


struct node
{
    int vertex;
    int distance;
    struct node* next;
};

struct Graph
{
    int numVertices;
    struct node** adjLists;
};

struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
};

struct Graph* parse_getline(FILE* fp, struct Graph* unGraph);
struct Graph* createGraph(int vertices);
void addEdge(struct Graph* graph, int src, int dest);
void printGraph(struct Graph* graph);
void bfs(struct Graph* graph, int distance[]);
int dequeue(struct Queue* queue);
void enqueue(struct Queue* queue, int item);
int isEmpty(struct Queue* queue);


int main(int argc, char** argv) {
    
    if (argc!=3)
        exit(INCORRECT_NUMBER_OF_COMMAND_LINE_ARGUMENTS);
    
    FILE* fp = fopen(argv[1],"r");
    
    if(!fp)
        exit(INPUT_FILE_FAILED_TO_OPEN);
    
    if(fgetc(fp) == EOF)
        exit(PARSING_ERROR_EMPTY_INPUT_FILE);
    fseek(fp, 0, SEEK_SET);
    
    struct Graph* unGraph;
    
    unGraph = parse_getline(fp, unGraph);
    
    int* distance = malloc(sizeof(int) * unGraph->numVertices);
    int i;
    
    //printGraph(unGraph);
    bfs(unGraph, distance);
  
//    for(i=1;i<unGraph->numVertices+1; i++){
//        printf("%d\n", distance[i]);
//    }
  
    
    if (fclose(fp) == EOF)
	exit(INPUT_FILE_FAILED_TO_CLOSE);
    
   
    
    fp = fopen(argv[2],"w");
    
    if(!fp)
        exit(OUTPUT_FILE_FAILED_TO_OPEN);
    
    for(i=0;i<unGraph->numVertices; i++){
        fprintf(fp,"%d\n", distance[i]);
   }
    
    if (fclose(fp) == EOF)
        exit(OUTPUT_FILE_FAILED_TO_CLOSE);
    
    free(distance);
    
    for(i = 0; i<unGraph->numVertices; i++){
        free(unGraph->adjLists[i]);
    }
    

    return (EXIT_SUCCESS);
}

struct Graph* parse_getline(FILE* fp, struct Graph* unGraph) {
	char* line = NULL;
        int numOfSrc;
        int numOfDest;
	size_t nbytes = 0;
        int i=0, linelen=0, j=0, numOfVertex = 0 , k =0;
	while ((linelen=getline(&line, &nbytes, fp)) != -1) {
		line[linelen-1] = '\0'; //removing the newline and adding the NULL character
                      
                if(j == 0){
                    for(i=0; i<linelen-1; i++){
                        if(isdigit(line[i])==0){
                            exit(PARSING_ERROR_INVALID_FORMAT);
                        }
                        else{
                            if(k==0){
                            numOfVertex += line[i] - '0';
                            k++;
                            }
                            else{
                                numOfVertex *= 10;
                                numOfVertex+= line[i] - '0';
                            }
                        }
                    }	
                    unGraph = createGraph(numOfVertex);  
                    j++;
                }
                else{
                    
                    numOfSrc = 0;
                    numOfDest = 0;
                    
                    if(line[0] != '('  || line[linelen-2] != ')'){
                        exit(PARSING_ERROR_INVALID_FORMAT);
                    }
                    i = 1;
                    k = 0;
                    while(line[i]!=','){
                        if(k==0){
                            numOfSrc += line[i] - '0';
                            k++;
                        }
                        else{
                            numOfSrc *= 10;
                            numOfSrc += line[i] - '0';
                        }
                        i++;
                    }
                    i++;
                    k = 0;
                    while(line[i]!=')'){
                        if(k==0){
                            numOfDest += line[i] - '0';
                            k++;
                        }
                        else{
                            numOfDest *= 10;
                            numOfDest += line[i] - '0';    
                        }
                        i++;
                    }
                    
                    if(numOfSrc == 0 || numOfDest == 0)
                        exit(PARSING_ERROR_INVALID_FORMAT);
                    
                    if(numOfSrc > numOfVertex || numOfDest > numOfVertex)
                        exit(INTEGER_IS_NOT_A_VERTEX);
                   
                    addEdge(unGraph,numOfSrc,numOfDest);
                }
        
       	}

	free(line);
        return unGraph;
}

// creates node and int
struct node* createNode(int v)
{
    struct node* newNode = malloc(sizeof(struct node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}
 

struct Graph* createGraph(int vertices)
{
    struct Graph* graph = malloc(sizeof(struct Graph));
    graph->numVertices = vertices;
 
    graph->adjLists = malloc(vertices * sizeof(struct node*));
 
    int i;
    for (i = 0; i < vertices; i++)
        graph->adjLists[i] = NULL;
 
    return graph;
}
 
void addEdge(struct Graph* graph, int src, int dest)
{
    //takes adjlist location and subtracts by 1 so the 0 position is not skipped
    // Add edge from src to dest
    struct node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src-1];
    graph->adjLists[src-1] = newNode;
 
    // Add edge from dest to src
    newNode = createNode(src);
    newNode->next = graph->adjLists[dest-1];
    graph->adjLists[dest-1] = newNode;
}
 
void printGraph(struct Graph* graph)
{
    int v;
    for (v = 0; v < graph->numVertices; v++)
    {
        struct node* temp = graph->adjLists[v];
        printf("\n Adjacency list of vertex %d\n ", v+1);
        while (temp)
        {
            printf("%d -> ", temp->vertex);
            temp = temp->next;
        }
        printf("\n");
    }
}

//queue referenced from Queues in C geeksforgeeks
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0; 
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}
// determines if queue is empty
int isEmpty(struct Queue* queue)
{  
    return (queue->size == 0); 
}

// Function to add an vertex to the queue.  
void enqueue(struct Queue* queue, int item)
{
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}
 
// Function to remove a vertex from queue so that distance can be gathered. 
int dequeue(struct Queue* queue)
{
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}
void bfs(struct Graph* graph, int distance[]){
    
    struct Queue* queue;
    int i;
    int u;
  
    
    for(i=0; i<graph->numVertices; i++){
        distance[i] = -1;
    }
    
    distance[0] = 0;
    
    queue = createQueue(graph->numVertices);
    enqueue(queue, 1);
    
    while(!isEmpty(queue)){
        u = dequeue(queue);
        
        
        struct node* temp = graph->adjLists[u-1];
        while(temp){
            if(distance[(temp->vertex) - 1] == -1){
                distance[(temp->vertex) - 1]=distance[u - 1]+1;
                enqueue(queue, temp->vertex);
            }
            temp = temp->next;
        }
    }
}
    





