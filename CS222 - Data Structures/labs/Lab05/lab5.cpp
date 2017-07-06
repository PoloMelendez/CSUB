#include <iostream>
#include <cstdlib>
#include <stdio.h>
using namespace std;

class DoubleList
{
    private:
	double *array;
	int cap;
	int count;

    public:
	DoubleList();
	DoubleList(const double *, int);
	~DoubleList();

	void report();

	friend ostream & operator <<(ostream &o, const DoubleList &right);
};

DoubleList::DoubleList()
{
    array = NULL;
    count = 0;
    cap = 0;
}

DoubleList::DoubleList(const double *a, int sz)
{
    array = NULL;

    if(sz < 0) count = 0;
    else count = sz;

    cap = count;

    if(count > 0)
    {
	try {
	    array = new double[cap];
	} catch(bad_alloc) {
	    array = NULL;
	}
	if(array == NULL)
	{
	    count = 0;
	    cap = 0;
	}
	else 
	{
	    for(int i = 0; i < sz; i++)
	    {
		array[i] = a[i];
	    }
	}
    }
}

DoubleList::~DoubleList()
{
    if(array != NULL) 
    {
	delete [] array;
    }
}

void DoubleList::report()
{
    printf("array pointer address=0x%x count=%d cap=%d\n", array, count, cap);
}

ostream & operator <<(ostream &o, const DoubleList &right)
{
    for(int i = 0; i < right.count; i++)
    {
	if(i) o << " ";
	o << right.array[i];
    }

    return o;
}

int main()
{
    double tmp1[] = {4.5, 6.5, 2.3, 1.2, 6.2, 9.8, 12.3, 4.2, 11.1, 9.4};
    double tmp2[] = {5.2, 4.3, 7.1, 8.5, 3.7, 2.8};
    DoubleList a, b(tmp1, 10), c(tmp2, 6);

    cout << "Report and value of a (empty list):" << endl;
    a.report();
    cout << a << endl << endl;

    cout << "Report and value of b (10 values from array 1):" << endl;
    b.report();
    cout << b << endl << endl;

    cout << "Report and value of c (6 values from array 2):" << endl;
    c.report();
    cout << c << endl << endl;

    DoubleList clone(c);

    cout << "Report and value for the clone of c (copy constructor):" << endl;
    clone.report();
    cout << clone << endl << endl;

    a = c;
    cout << "Report and value of a after using a = c (assignment operator):" << endl;
    a.report();
    cout << a << endl << endl;

    a = b;
    cout << "Report and value of a after using a = b (assignment operator):" << endl;
    a.report();
    cout << a << endl;

    return 0;
}
