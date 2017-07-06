//Leopoldo Melendez
//1-13-2013
//Homework2
#include<iostream>
#include<string.h>
#include<iomanip>
#include<new>
using namespace std;

class ProductInfo{
    private:
	char name[13];
	char description[50];
    	char UPC[13];
    	int stock;

    public:
	ProductInfo();
	ProductInfo(char[], char[], char[], int);

	const char* getName() const{ return name; }
	const char* getDescription() const{ return description; }
	const char* getUPC() const{ return UPC;	}
	const int getStock() const{ return stock; }

	bool setName(char*);
	bool setDescription(char*);
	bool setUPC(char*);
	bool setStock(int);

	void inputProduct();
	void printProduct();
};

ProductInfo* allocateArray(int);

int main() 
{
  int size,in1, in2, in3;
  char selection;
  ProductInfo *products = NULL;

  do
  {
    cout << "  Welcome to the CS222 Homework 2 Menu\n";
    cout << "  ====================================\n";
    cout << "  1.  Input all product information from the keyboard\n";
    cout << "  2.  Print all product information to the screen\n";
    cout << "  3.  Edit the stock level for a single product\n";
    cout << "\n";
    cout << "  0.  Exit\n";
    cout << "  ====================================\n";
    cout << "  Enter selection: ";
    cin >> selection;
    cout << endl;

    switch(selection)
    {
      case '0':
	  if(products != NULL)
	  {
	      delete[] products;
	  }
	  else
	      cout << "  Nothing to delete." << endl;
        cout << "Bye.";
        break;

      case '1':
        cout << "  Enter the number of products: ";
        cin >> size;
	if(size > 0)
	{
	    if(products != NULL)
	    {
		delete[] products;
	    }
	    products = allocateArray(size);
	}

	else
	    break;
	for(int i = 0;i < size;i++)
	{
	    products[i].inputProduct();
	}
        break;

      case '2':
	if( products == NULL)
	{
	    cout << "  There is no inventory. Use Option 1 first." << endl;
	    break;
	}
	cout << "  Name		     Description		     UPC	Stock\n";
	cout << "  ==================================================================\n";
        for(int i = 0;i < size; i++)
	{
	    cout << "  " << i + 1 << ". ";
	    products[i].printProduct();
	}
        break;

      case '3':
	if( products == NULL)
	{
	    cout << "  There is no inventory. Use Option 1 first." << endl;
	    break;
	}
	for(int i = 0;i < size; i++)
	    cout << "  " << i + 1 << ". " << setw(14) << left << products[i].getName() << "\t\t" <<products[i].getStock() << endl;
	int index;
	do{
	    cout << "  Enter the number of the product you wish to modify: ";
	    cin >> index;
	}while(index > size || index < 1);
	int newStock;
	cout << "  Enter the new stock number: ";
	cin >> newStock;
	cout << endl;
	products[index - 1].setStock(newStock);
	cout << "  " << index << ". " << setw(14) << left << products[index - 1].getName() << "\t\t" << products[index - 1].getStock() << endl;
        break;

      default:
        cout << selection << " is not a valid menu item.\n";
    }
    cout << endl;
  } while( selection != '0' );

  return 0;
}

ProductInfo::ProductInfo(){
    name[0] = '\0';
    description[0] = '\0';
    UPC[0] = '\0';
    stock = 0;
}

ProductInfo::ProductInfo(char nme[], char desc[], char upcode[], int stck){
    if(!setName(nme) || !setDescription(desc) || !setUPC(upcode) || !setStock(stck)){
	name[0] = '\0';
	description[0] = '\0';
	UPC[0] = '\0';
	stock = 0;
    }
}

bool ProductInfo::setName(char* nme){
    strncpy(name, nme, 25);
    return true;
}

bool ProductInfo::setDescription(char* desc){
    strncpy(description, desc, 50);
    return true;
}

bool ProductInfo::setUPC(char* upcode){
    strncpy(UPC, upcode, 13);
    return true;
}

bool ProductInfo::setStock(int stck){
    stock = stck;
    return true;
}

void ProductInfo::inputProduct(){
    char n[25];
    char d[50];
    char u[13];
    int s;
    do{
	cout << "  Enter the name of the product: ";
	if(cin.peek() == '\n')
	    cin.ignore();
	cin.getline(n, 25);
    }while(!setName(n));
    do{
	cout << "  Enter the description of the product: ";
	if(cin.peek() == '\n')
	    cin.ignore();
	cin.getline(d, 50);
    }while(!setDescription(d));
    do{
	cout << "  Enter the UPC of the product: ";
	if(cin.peek() == '\n')
	    cin.ignore();
	cin.getline(u, 13);
    }while(!setUPC(u));
    do{
	cout << "  Enter the amount of stock: ";
	cin >> s;
    }while(!setStock(s));
    cout << endl;
}

void ProductInfo::printProduct(){
    cout << "  " << left << setw(14) << name << setw(32) << description << UPC << "\t" << stock;
    cout << endl << endl;
}

ProductInfo *allocateArray(int size){
    ProductInfo *tmp = NULL;
    if(size < 1)
	return NULL;
    try{
	tmp = new ProductInfo[size];
    }
    catch(bad_alloc ex){
	cout << "  Exception: " << ex.what();
	tmp = NULL;
    }
    return tmp;
}
