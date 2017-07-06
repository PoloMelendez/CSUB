//cmps312
//dijkstra's algorithm
//gordon griesel
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
using namespace std;
struct Node {
	char symbol;
	int x,y;
	int visited;
	int parent[2];
	Node() {
		symbol='.';
		visited=0;
		parent[0] = parent[1] = -1;
	}
} nodes[20][20];
Node *queue[400];
int front=0, back=0;
int n = 0;
void showGrid();
int off[8][2] = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
int inbounds(int x, int y);

int main()
{
	int x,y;
	for (int i=0; i<20; i++) {
   		for (int j=0; j<20; j++) {
			nodes[i][j].y = i;
			nodes[i][j].x = j;
		}
	}
	//wall
	nodes[10][9].symbol = 'W';
	nodes[10][10].symbol = 'W';
	nodes[11][9].symbol = 'W';
	nodes[11][10].symbol = 'W';
	//
	int found=0;
	int start[2] = {2,17};
	int goal[2] = {16,3};
	nodes[start[1]][start[0]].symbol = '$';
	nodes[goal[1]][goal[0]].symbol = 'G';
	queue[back] = &nodes[start[1]][start[0]];
	back = (back+1) % 400;
	nodes[start[1]][start[0]].visited = 1;
	do {
		Node *curr = queue[front];
		front = (front+1) % 400;
		for (int k=0; k<8; k++) {
			x = curr->x + off[k][0];
			y = curr->y + off[k][1];
			if (inbounds(x,y)) {
				if (nodes[y][x].visited != 1) {
					nodes[y][x].visited = 1;
					nodes[y][x].symbol = 'v';
					nodes[y][x].parent[0] = curr->x;
					nodes[y][x].parent[1] = curr->y;
					queue[back] = &nodes[y][x];
					back = (back+1) % 400;
					//check for goal
					if (x == goal[0] && y == goal[1]) {
						found = 1;
						break;
					}
				}
			}
		}
		showGrid();
		usleep(10000);
	} while (!found);
	//
	//path from goal back to start...
	Node *curr = &nodes[y][x];
	while (curr->parent[0] >= 0) {
		x = curr->parent[0];
		y = curr->parent[1];
		nodes[y][x].symbol = '*';
		curr = &nodes[y][x];
		showGrid();
		usleep(80000);
	}
	showGrid();
	return 0;
}

int inbounds(int x, int y)
{
	if (x >= 0 && x < 20 && y >= 0 && y < 20) {
		if (nodes[y][x].symbol == 'W')
			return 0;
		return 1;
	}
	return 0;
}

void showGrid()
{
	system("clear");
	for (int i=0; i<20; i++) {
		for (int j=0; j<20; j++) {
			cout << nodes[i][j].symbol << " ";
		}
		cout << endl;
	}
}







