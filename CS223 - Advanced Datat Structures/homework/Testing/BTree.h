#include<iostream>

using namespace std;

template< class T >
class TNode {
    
    public:
	T data;
	TNode<T> * left, * right;

	TNode(const T & value ): data(v), left(NULL), right(NULL) { }
	~TNode() {
	    if( left ) delete left;
	    if( right ) delete right;
	}
};

template< class T >
class BTree {

    private:
	TNode<T> * root;
	int count;

    protected:
	/*
	void remove( TNode<T>* & p, const T& v ) {
	    if( p == NULL ) return;
	    if( p->data == v ) {
		if( p->left == p->right ) { delete p; p = NULL; }
		if( p->left == NULL || p->right == NULL ) {
		    TNode<T> * tmp = p;
		    p = p->left == NULL ? p->right : p->left;
		    tmp->left = tmp->right = NULL;
		    return;
		}
		swapAndRemove( p );
	    }
	}
	*/
	void remove( TNode<T>* & p, const T& v ) {
	    if( p == NULL ) return;
	    if( p->data == v ) {
		if( numberChildren( p ) < 2 ) remove(p, numberChildren(p));
		else swapAndRemove(p);
		return;
	    }
	    if( v < p->data ) remove( p->left, v );
	    else
		remove( p->right, v );
	}

	void swapAndRemove( TNode<T> * & p ) {
	    TNode<T> * & child;
	    findLeftRight( p, child );
	    swap( child->data, p->data );
	    int n = numberChildren( child );
	    remove( child, n );
	}

    public:
	BSTree() { root = NULL; count = 0; }

	void inOrder( ) {
	    inOrder( root );
	}

	void inOrder( const TNode * ptr ) {
	    if( ptr == NULL ) return;
	    inOrder(ptr->left);
	    cout << ptr->data << " ";
	    inOrder(ptr->right);
	}

	BTree<T>& insert( T v ) {
	    TNode<T> * nn = new TNode<T>( v );
	    insert(root, nn);
	    return *this;
	}

	void insert( TNode<T>* & parent, TNode<T> * child ) {
	    if( parent == NULL ) {
		parent = child;
		return;
	    }
	    if( parent->data < child->data ) insert( parent->right, child );
	    else insert( parent->left, child );
	}

	void remove( const T& v ) {
	    remove( root, v );
	}

	TNode<T> * findLeftRight( TNode<T> * & p ) {
	    TNode<T> * & curr = p->left;
	    while( curr->right != NULL ) curr = curr->right;
	    return curr;
	}
};
