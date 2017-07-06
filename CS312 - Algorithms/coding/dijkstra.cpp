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
		parent[0]=-1;
		parent[1]=-1;
	}
} nodes[20][20];
Node *queue[400];
int front=0, back=0;
int n = 0;
void showGrid();
int off[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
int inbounds(int x, int y);

int main()
{
	for (int i=0; i<20; i++)
   		for (int j=0; j<20; j++) {
			nodes[i][j].x = i;
			nodes[i][j].y = j;
		}
	nodes[17][2].symbol = '$';
	nodes[2][16].symbol = 'G';
	int found=0;
	int start[2] = {2,17};
	int goal[2] = {16,3};
	queue[back] = &nodes[start[0]][start[1]];
	back = (back+1) % 400;
	nodes[start[0]][start[1]].visited = 1;
	do {
		Node *curr = queue[front];
		front = (front+1) % 400;
		for (int k=0; k<4; k++) {
			int x = curr->x + off[k][0];
			int y = curr->y + off[k][1];
			if (inbounds(x,y)) {
				if (nodes[x][y].visited != 1) {
					queue[back] = &nodes[x][y];
					back = (back+1) % 400;
					nodes[x][y].visited = 1;
					nodes[x][y].symbol = 'v';
					nodes[x][y].parent[0] = curr->x;
					nodes[x][y].parent[1] = curr->y;
					//check for goal
					if (x == goal[0] && y == goal[1]) {
						found = 1;
						break;
					}
				}
			}
		}
		showGrid();
		usleep(100000);
	} while (!found);

	return 0;
}

int inbounds(int x, int y) {
	return (x >= 0 && x < 20 && y >= 0 && y < 20);
}

void showGrid()
{
	system("clear");
	for (int i=0; i<20; i++) {
		for (int j=0; j<20; j++) {
			cout << nodes[j][i].symbol << " ";
		}
		cout << endl;
	}
}







