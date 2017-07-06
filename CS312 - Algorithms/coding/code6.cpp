#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<iomanip>
#include<cstring>

using namespace std;

#define GridSizeX 20
#define GridSizeY 20
#define QueueSize 400
#define maxNum 99
#define rootTwo 1.4142135

struct Node {
    int visited;
    int parent[2];
    char symbol;
    int x, y;
    float weight;
    Node() {
        symbol = '.';
        visited = 0;
        parent[0] = -1;
        parent[1] = -1;
        weight = maxNum;
    }
} nodes[GridSizeY][GridSizeX];

Node * queue[QueueSize];
int front = 0, back=0;
int offset[8][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {-1,1}, {-1,-1}, {1,-1}};

void showGrid();
void showGridWeights();
int inbounds(int, int);
void enqueue(int, int);
void dequeue();
void parentPath(int *);

int main(int argc, char **argv) {
    float timeMult = 1;
    if(argc > 1)
        timeMult = atoi(argv[1]);
    for(int i=0; i < GridSizeY; i++) {
        for(int j=0; j < GridSizeX; j++) {
            nodes[i][j].x = j;
            nodes[i][j].y = i;
        }
    }

    nodes[17][2].symbol = '$';
    nodes[2][16].symbol = 'G';
    int found=0;
    int start[2] = {17, 2};
    int goal[2] = {2, 16};
    queue[back] = &nodes[start[0]][start[1]];
    back = (back+1) % QueueSize;
    nodes[start[0]][start[1]].visited = 1;
    nodes[start[0]][start[1]].weight = 0;

    char oldSymbol;
    do {
        Node *curr = queue[front];
        dequeue();
        oldSymbol = curr->symbol;
        curr->symbol = 'C';
        for(int k=0; k<8; k++) {
            int x = curr->x + offset[k][1];
            int y = curr->y + offset[k][0];
            if(inbounds(x,y)) {
                if(nodes[y][x].visited != 1) {
                    enqueue(x, y);
                    nodes[y][x].visited = 1;
                    nodes[y][x].symbol = 'v';
                    if((nodes[y][x].weight > curr->weight + 1) && k < 4) {
                        nodes[y][x].weight = curr->weight + 1;
                    }
                    if((nodes[y][x].weight > curr->weight + 1) && k >= 4) {
                        nodes[y][x].weight = curr->weight + rootTwo;
                    }
                    nodes[y][x].parent[0] = curr->y;
                    nodes[y][x].parent[1] = curr->x;
                    if( x == goal[1] && y == goal[0]) {
                        found = 1;
                        break;
                    }
                }
            }
        }
        showGrid();
        usleep(100000/timeMult);
        curr->symbol = oldSymbol;
    } while(!found);

    if(argc > 2) {
        if(strcmp("parents", argv[2]) == 0) {
            parentPath(start);
        }
        else if(strcmp("weights", argv[2]) == 0) {
            showGridWeights();
        }
    }

    return 0;
}



void parentPath(int * start) {
    Node * temp = queue[back-1];
    int x, y;
    while((temp->x != start[1] || temp->y != start[0])) {
        y = temp->parent[0];
        x = temp->parent[1];
        temp->symbol = 'P';
        temp = &nodes[y][x];
        showGrid();
    }
}

void enqueue(int x, int y) {
    queue[back] = &nodes[y][x];
    back = (back + 1) % QueueSize;
}

void dequeue() {
    front = (front + 1) % QueueSize;
}

void showGrid() {
    system("clear");
    for(int i=0; i < GridSizeY; i++) {
        for(int j=0; j < GridSizeX; j++) {
            cout << nodes[i][j].symbol << " ";
        }
        cout << endl;
    }
}

void showGridWeights() {
    for(int i=0; i < GridSizeY; i++) {
        for(int j=0; j < GridSizeX; j++) {
            cout << setw(3) << setprecision(2) << right << nodes[i][j].weight << "  ";
        }
        cout << endl;
    }
}

int inbounds(int x, int y) {
    return (x >= 0 && x < GridSizeX && y >= 0 && y < GridSizeY);
}
