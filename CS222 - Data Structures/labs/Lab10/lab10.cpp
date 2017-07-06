#include <iostream>
#include <string>
using namespace std;

template <class T>
class Example 
{
    private:
	T data;

    public:
	Example() { data = 0; }
	void setData(T elem) { data = elem; }

	template <class U>
	friend ostream& operator << (ostream &, const Example<U>&);

	friend ostream& operator << (ostream &, const Example<char>&);

	template <class U>
	friend U operator+(const Example<U> &, const Example<U> &);

	friend string operator+(const Example<char> &, const Example<char> &);
};

template <class T>
T operator+(const Example<T> & left, const Example<T> & right){
    T data1 = left.data;
    T data2 = right.data;
    T sum = data1 + data2;
    return sum;
}

string operator+(const Example<char>& left, const Example<char>& right){
    char c = left.data;
    char d = right.data;
    string tmp1(1, c);
    string tmp2(1, d);
    string output = tmp1 + tmp2;
    return output;
}
    
template <class T>
ostream& operator << (ostream &o, const Example<T> &t)
{
    o << t.data;
    return o;
}

ostream& operator << (ostream &o, const Example<char> &t)
{
    o << "'" << t.data << "'";
    return o;
}


int main()
{
    Example<int> tInt1, tInt2;
    Example<char> tChar1, tChar2;

    tInt1.setData(15);
    tInt2.setData(30);

    tChar1.setData('A');
    tChar2.setData('B');

    cout << tInt1 << "+" << tInt2 << "=" << (tInt1 + tInt2) << endl;
    cout << tChar1 << "+" << tChar2 << "=" << (tChar1 + tChar2) << endl;
    return 0;
}
