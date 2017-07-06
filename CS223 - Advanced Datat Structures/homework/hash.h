#ifndef HASH_H
#define HASH_H

#include<iostream>
#include<iomanip>

using namespace std;

class Integer {
    public:
	int v;

	Integer( int i ){ v = i; }
	int hashCode(){ return v; }
	friend ostream& operator<<( ostream& out, const Integer& v );
	friend istream& operator>>( istream& in, Intger& v );
	int& value(){ return v; }
};

template< class T >
class hashTable {

    protected:
	T **table;//table array of tables
	int cap, count;
	float maxLF, percInc;

	int distance( int from, int to ) {
	    return from>=to ? from - to : cap - to + from;
	}

	void rehash() {
	    int newCap = (int)(1 + percInc) * cap + 1;
	    T ** tmp = new T[newCap];
	    for(int i = 0; i < newCap; i++) tmp[i] = NULL;
	    swap( tmp, table ); swap( cap, newCap );
	    count = 0;
	    for(int i = 0; i < newCap; i++) {
		if( tmp[i] != NULL ){
		    add(table[i]);
		    table[i] = NULL;
	    }
	}

    public:
	hashTable(int cap = 10, maxLF = 0.8f, percInc = 0.2) {
	    this->cap = cap;
	    this->maxLF = maxLF;
	    this->percInc = percInc;
	    table = new T[cap];
	    count = 0;
	    for( int i = 0; i < cap; i++ )
		table[i] = NULL; 
	}
	~hashTable() { delete[] table; }

	int hash( const T& v ){ return hash( v.hashCode() ); }
	int hash( int v ){ return v % cap; }

	hashTable<T>& add( const T& v ) {
	    return add( v.hashCode() );
	}
	hashTable<T>& add( T* ptr ) {
	    int home = hash( v.hashCode() );
	    while( table[home] != NULL ) home = (home + 1)%cap;
	    table[home] = v;
	    count++;
	    if( count/(float)cap > maxLF ) rehash();//mae a temp array of pointer with cap = (1 + percInc)*cap + 1, initialize temp array, then swap( table, tempTable ), then set count = 0;
	    return *this;
	}
	hashTable<T>& add( const T & v ){
	    return add( new T(v) );
	}

	void erase( const T & v ) {
	    int home = hash( v.hashCode() );
	    while( table[home] != NULL && *table[home] != v )
		home = (home + 1)%cap;
	    if( table[home] != NULL ) return;
	    delete table[home]; table[home] = NULL;
	    shift( home, (home + 1)%cap );
	}

	void shift( int empty, int from ){
	    if( table[from] == NULL )
		return;
	    int home = hash( table[from]->hashCode() );
	    int d1 = distance( from, home ),
		d2 = distance( empty, home );
	    if( d2 < d1 ) {
		table[empty] = table[from];
		table[from] = NULL;
		shift( from, (from + 1)%cap );
	    }
	    shift( empty, (from + 1)%cap);
	}


};
#endif
