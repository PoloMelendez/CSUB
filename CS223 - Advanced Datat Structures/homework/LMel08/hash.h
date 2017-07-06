#ifndef HASH_H
#define HASH_H
#include<iostream>
#include<iomanip>
#include<cstdlib>
#include<cstdio>
#include<ctime>
using namespace std;

class Integer {
    public:
	int key;
	float value;

	Integer( int k = 999, float v = 9.999 ){ key = k; value = v; }
	Integer( const Integer & v ){ key = v.key; value = v.value; }
	int hashCode(){ return key; }

	Integer change() {
	    key = rand() % 100;
	    value = rand() % 400 / 100.0;
	    return * this;
	}

	char * toString(){
	    static char s[21];
	    sprintf(s, "%10d", key );
	    return s;
	}

	friend ostream& operator<<( ostream& out, const Integer& val );
};

ostream& operator<<( ostream & out, const Integer& value ) {
    out << value.key;
    return out;
}

template< class T >
class HashTable {
    protected:
	T ** table;
	int iSize, iCap;
	float loadFactor, percInc;

	int distance( int from, int to ) {
	    return from>=to ? from - to : iCap - to + from;
	}

	void rehash(){
	    int newCap = (int)((1 + percInc) * iCap) + 1;
	    T ** tmp = new T*[newCap];
	    for(int i = 0; i < newCap; i++ ) tmp[i] = NULL;
	    swap( tmp, table ); swap( iCap, newCap );
	    iSize = 0;
	    for(int i = 0;i < newCap; i++) {
		if( tmp[i] != NULL )
		    insert( tmp[i]->hashCode() );
		    tmp[i] = NULL;
	    }
	    delete tmp;
	    tmp = NULL;
	}

    public:
	HashTable( int iCap = 10, float loadFactor = 0.8f, float percInc = 0.2) {
	    this->iCap = iCap;
	    this->loadFactor = loadFactor;
	    this->percInc = percInc;
	    table = new T * [iCap];
	    iSize = 0;
	    for( int i = 0; i < iCap; i++ )
		table[i] = NULL;
	}
	~HashTable(){ 
	    for( int i = 0; i < iCap; i++)
		if( table[i] != NULL ) delete table[i];
	    delete[] table; 
	}

	int hash( const T& v ){ return v.hashCode()%iCap; }
	int hash( int key ){ return (key%iCap); }

	void add( T * ptr ) {
	    int home = hash(ptr->hashCode());
	    while( table[home] != NULL ) home = (home+1)%iCap;
	    table[home] = ptr;
	    iSize++;
	    if( iSize/(float)iCap > loadFactor ) rehash();
	}
	void insert( const T& itm ) { 
	    add( new T(itm) );
	    cout << itm.key << " " << itm.value << endl;	    
	}

	void remove( T * itm ) {
	    int home = hash( itm->hashCode() );
	    while( table[home]->hashCode() != itm->hashCode() ){
		home = (home + 1)%iCap;
		if( table[home] != NULL ) return;
	    }
	    if( table[home] == NULL ) return;
	    delete table[home];
	    table[home] = NULL;
	    iSize--;
	    shift( home, (home + 1)%iCap );
	}

	void subtract( int itm ){
	    Integer * value = new T( itm );
	    remove( value );
	    delete value;
	    value == NULL;
	}

	void shift( int empty, int from ){
	    if( table[from] == NULL ) return;
	    int home = hash( table[from]->hashCode() );
	    int d1 = distance(from, home), d2 = distance(empty, home);
	    if( d2 < d1 ) {
		table[empty] = table[from];
		table[from] = NULL;
		shift( from, (from + 1)%iCap );
	    }
	    shift( empty, (from + 1)%iCap);
	}


	int search( const T & itm );

	void display() {
	    printf("%15s %15s %15s %15s\n", "Object","Current Address","Home Address", "Distance");
	    printf("%15s %15s %15s %15s\n", "======","===============","============", "========");
	    const char * obj;
	    int home, dist;
	    cout << "Current Cap: " << iCap << endl;
	    for( int i = 0; i < iCap; i++ ){
		obj = table[i] == NULL ? "NULL" : table[i]->toString();
		home = table[i] == NULL ? -1 : hash( table[i]->hashCode() );
		dist = distance( i, home );
		printf("%15s %15d %15d %15d\n", obj, i, home, dist );
	    }
	}

};
#endif
