#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<iomanip>
#include<cstring>

using namespace std;

#define GridSizeX 20
#define GridSizeY 20
#define QueueSize 400
#define MaxNum 99
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
        weight = MaxNum;
    }
} nodes[GridSizeY][GridSizeX];

Node * queue[QueueSize];
int front = 0, back=0;
int offset[8][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {-1,1}, {-1,-1}, {1,-1}};

Node * getSmallest(Node *);
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
    Node * curr = &nodes[start[0]][start[1]];
    curr->weight = 0;
    curr->visited = 1;
    do {
        curr = getSmallest(curr);
        curr->visited = 1;
        curr->symbol = 'v';
        for(int k=0; k<8; k++) {
            int x = curr->x + offset[k][1];
            int y = curr->y + offset[k][0];
            if(inbounds(x,y)) {
                if(nodes[y][x].visited != 1) {
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

Node * getSmallest(Node * curr) {
    int minWeight = MaxNum;
    Node * minNode = curr;
    for(int y=0; y < GridSizeY; y++) {
        for(int x=0; x < GridSizeX; x++) {
            if(inbounds(x,y)) {
                if(nodes[y][x].weight < minWeight && !nodes[y][x].visited) {
                    minWeight = nodes[y][x].weight;
                    minNode = &nodes[y][x];
                }
            }
        }
    }
    return minNode;
}

void parentPath(int * start) {
    Node * temp = &nodes[start[0]][start[1]];
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
