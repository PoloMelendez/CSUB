//Leopoldo Melendez
//1-9-2013
//Homework1
#include<iostream>
#include<string.h>
#include<iomanip>
using namespace std;

struct ProductInfo { 
    char name[25];
    char description[50];
    char UPC[13];
    int stock;
};

ProductInfo *allocateArray(int);
void addProduct(struct ProductInfo *n);
void showProduct(struct ProductInfo *n);
void showProductTable();

int main(){

    int size;
    ProductInfo *ptr;

    do{
	ptr = NULL;
	cout << "Enter the number of products you wish to add to inventory, or 0 to quit: ";
	cin >> size;
	cout << endl;
	
	if(size == 0)
	    break;
	if(size < 0)
	{
	    cout << "You must have at least one product" << endl << endl;
	    continue;
	}

	ptr = allocateArray(size);
	
	if(ptr == NULL)
	    continue;
	
	for(int i = 0;i < size; i++)
	{
	    ProductInfo temp;
	    addProduct(&temp);
	    ptr[i] = temp;
	}

	cout << "Name" << "\t\t\t" << "Description" << "\t\t\t" << "UPC" << "\t\t" << "Stock" << endl;

	for(int i = 0;i < size; i++)
	{
	    showProduct(&ptr[i]);
	}


	delete[] ptr;

    }while(size != 0);

    cout << endl << endl;    

    return 0;
}

ProductInfo *allocateArray(int size)
{
    ProductInfo *tmp = NULL;
    if(size < 1)
       return NULL;
    try{
	tmp = new ProductInfo[size];
    }
    catch(bad_alloc){
	tmp = NULL;
    }
    return tmp;
}

void addProduct(ProductInfo *product)
{
    cin.ignore();
    cout << "Enter the name of the product: ";
    cin.getline(product->name, 25);
    cout << "Enter the description of the product: ";
    cin.getline(product->description, 50);
    cout << "Enter the UPC of the product: ";
    cin >> product->UPC;
    cout << "Enter the amount of stock: ";
    cin >> product->stock;
    cout << endl;
}

void showProduct(ProductInfo *product)
{
    cout << left << setw(24) << product->name << setw(32) << product->description << setw(16) << product ->UPC << setw(16) << product->stock << endl << endl;
}
