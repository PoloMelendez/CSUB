#ifndef BSTREE_H
#define BSTREE_H

#include<iostream>
#include<iomanip>
#include "screen.h"

using namespace std;

template< class T >
class TNode {
    public:
	T data;
	TNode<T> *left, *right;

	TNode( const T & v ): data(v), left(NULL), right(NULL) { }
	~TNode() {
	    if( left ) delete left;
	    if( right ) delete right;
	}
};

template< class T >
class BSTree {
    private:
	TNode<T> * root;
	int count;

    protected:
	void recursiveInsert( TNode<T>* & parent, const T & v ) {
	    if( parent == NULL ) { 
		parent = new TNode<T>( v );
	       	count++; 
		return;
	    }
	    if( v <= parent->data )
		recursiveInsert( parent->left, v );
	    else recursiveInsert( parent->right, v );
	}

	bool recursiveSearch( const T & v, TNode<T> * parent ) {
	    if( parent == NULL ) return false;
	    if( v < parent->data )
		return recursiveSearch( v, parent->left );
	    else if( parent->data < v )
		return recursiveSearch( v, parent->right);
	    else return true;
	}

	void delSearch( const T & v, bool & found, TNode<T> *& ptr, TNode<T> *& parent ) {
	    ptr = root;
	    parent = NULL;
	    found = false;
	    while( !found && ptr != NULL ) {
		if( v < ptr->data ) {
		    parent = ptr;
		    ptr = ptr->left;
		}
		else if( ptr->data < v ) {
		    parent = ptr;
		    ptr = ptr->right;
		}
		else found = true;
	    }
	}

    public:
	BSTree() { root = NULL; count = 0; }
	BSTree( const BSTree<T> & source ) { copyTree( root, source.root ); }
	~BSTree() { if( root != NULL ) delete root; count = 0; }

	void copyTree( TNode<T> *& copy, TNode<T> * source ) {
	    if( source == NULL ) copy = NULL;
	    else {
		copy = new TNode<T>(source->data);
		copyTree(copy->left, source->left);
		copyTree(copy->right, source->right);
	    }
	}

	void clear() { if( root != NULL ) delete root; root = NULL; count = 0; }

	BSTree<T>& insert( const T & v ) {
	    recursiveInsert( root, v );
	    return *this;
	}

	T erase( const T & v ) {
	    bool found;
	    TNode<T> * delNode, * parent;
	    delSearch( v, found, delNode, parent );
	    if( !found ) {
		cout << "Item not in the Binary Search Tree\n";
		return v;
	    }
	    if( delNode->left != NULL && delNode->right != NULL ) {
		TNode<T> * child = delNode->right;
		parent = delNode;
		while( child->left != NULL ) {
		    parent = child;
		    child = child->left;
		}
		delNode->data = child->data;
		delNode = child;
	    }
	    
	    TNode<T> * subTree = delNode->left;
	    if( subTree == NULL )
		subTree = delNode->right;
	    if( parent == NULL )
		root = subTree;
	    else if( parent->left == delNode )
		parent->left = subTree;
	    else parent->right = subTree;
	    delNode->left=delNode->right=NULL;
	    delete delNode;
	    count--;
	}

	int height() const { return longestPath( root ); }
	int shortHeight() const { return shortestPath( root ); }

	int sizeofTree() { return size( root ); }

	int size( TNode<T> * leaf ) const {
	    int count = 1;
	    if (leaf->left != NULL) count += size(leaf->left);
	    if (leaf->right != NULL) count += size(leaf->right);
	    return count;
	}

	bool search( const T & v ) {
	    return recursiveSearch( v, root );
	}

	bool normSearch( const T & v ) {
	    TNode<T> * ptr = root;
	    bool found = false;
	    while( !found && ptr != NULL ) {
		if( v < ptr->data )
		    ptr = ptr->left;
		else if( ptr->data < v )
		    ptr = ptr->right;
		else found = true;
	    }
	    return found;
	}

	int shortestPath( TNode<T> * rt ) const {
	    if( rt == NULL )
		return 0;
	    else {
		int leftSub = shortestPath( rt->left );
		int rightSub = shortestPath( rt->right );

		if( leftSub < rightSub ){
		    return( leftSub+1 );
		}
		else {
		    return( rightSub+1 );
		}
	    }
	    cout << endl;
	}

	int longestPath( TNode<T> * rt ) const {
	    if( rt == NULL )
		return 0;
	    else {
		int leftSub = longestPath( rt->left );
		int rightSub = longestPath( rt->right );

		if( leftSub > rightSub ) {
		    return( leftSub+1 );
		}
		else {
		    return( rightSub+1 );
		}
	    }
		cout << endl;
	}

	void printOrder() {
	    cout << "inOrder : ";
	    inOrder( root );
	    cout << endl;
	    cout << "preOrder : ";
	    preOrder( root );
	    cout << endl;
	    cout << "postOrder : ";
	    postOrder( root );
	    cout << endl;
	    cout << "RVLOrder : ";
	    RVLOrder( root );
	    cout << endl;
	}

	void inOrder( const TNode<T> * ptr ) {
	    if( ptr == NULL ) return;
	    inOrder(ptr->left);
	    cout << ptr->data << " ";
	    inOrder(ptr->right);
	}

	void preOrder( const TNode<T> * ptr ) {
	    if( ptr == NULL ) return;
	    cout << ptr->data << " ";
	    preOrder(ptr->left);
	    preOrder(ptr->right);
	}

	void postOrder( const TNode<T> * ptr ) {
	    if( ptr == NULL ) return;
	    postOrder(ptr->left);
	    postOrder(ptr->right);
	    cout << ptr->data << " ";
	}

	void RVLOrder( const TNode<T> * ptr ) {
	    if( ptr == NULL ) return;
	    RVLOrder(ptr->right);
	    cout << ptr->data << " ";
	    RVLOrder(ptr->left);
	}

	BSTree<T> * getRoot() { 
	    const TNode<T> * ptr = root;
	    return ptr; }

	static const int treeWidth = 80;
	static const int levelDistance = 2;
	void printNode(const T & data, int Level, int NodeNum ) {
	    int X, Y ;
	    float TotalNodesAtLevel, NodeWidth, RelativeNodeNum;
	    Y = 1 + Level * levelDistance;
	    TotalNodesAtLevel = 1;
	    for (int i = 1; i <=Level; i ++ )
		TotalNodesAtLevel = TotalNodesAtLevel * 2;
	    NodeWidth = ((float) treeWidth / TotalNodesAtLevel);
	    RelativeNodeNum = NodeNum - TotalNodesAtLevel;
	    X = (int)((RelativeNodeNum +0.5)*(float) treeWidth / TotalNodesAtLevel);
	    Screen::moveCursorTo(X, Y);
	    cout << data;
	}

	void printTree(TNode<T>* n, int Level, int NodeNum ) {
	    if ( n ) {
		printNode(n->data, Level, NodeNum);
		printTree(n->left, Level + 1, NodeNum * 2);
		printTree(n->right, Level + 1, NodeNum * 2 + 1);
	    }
	}

	void print() {
	    Screen::clear();
	    printTree(root, 0, 1) ;
	    Screen::moveCursorTo(0, 24);
	}
};
#endif
