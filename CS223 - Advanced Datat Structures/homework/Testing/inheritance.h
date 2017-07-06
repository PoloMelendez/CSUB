template< class T >
class TList {

    protected:
    TNode<T> * front, * rear;
    int count;

    public:
    TList<T>& insert( const T & v, int pos ) {
    }
    void remove( int pos ){
    }
    bool empty() { return count <= 0; }
};

template< class T >
class TStack : protected TList<T> {
    
    public:
	TStack<T> & push( const T & v ) { insert( v, 0 ); return *this; }
	void pop() { remove( 0 ); }
	T top() { return front->data; }
};

template< class T >
class TQueue : protected TList<T> {
    
    public:
	TQueue<T>& enque( const T & v ) { insert( v, count ); return *this; }
	void deque() { remove( 0 ); }
	T frnt() { return front->data; }
};

template< class T >
class TSQueue : protected TList<T> {

    public:
	TSQueue<T>& insert( const T& v ){
	    TNode<T> * nn = new TNode<T>( v );
	    //if squeue is empty set front and rear to to nn -- insert( v, 0)
	    //if smallest value, insert at front -- insert( v, 0 )
	    //if largest value, insert at rear -- insert( v, count )
	    //else find proper position to insert between nodes
	    //dont incremement count if you call the old
	    //insert function from inheritance
	    //return *this
	}
	void remove( const T& v ){
	    if( count <= 0 || v < front->data || v > rear->data) return;
	    else if( v == front->data ) { remove( 0 ); return }
	    else if( v == rear->data ) { remove( count-1 ); return; }
	    else {
		TNode<T> * curr = front;
		while( curr->next->data < v ) curr = curr->next;
		if( curr->next->data != v ) return;
		TNode<T>* dn = curr->next;
		curr->next = curr->next->next;
		dn->next = NULL;
		delete dn;
		count--;
	    }
	}
};
