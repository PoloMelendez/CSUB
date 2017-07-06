//Leopoldo Melendez
//Lab or HW #
//Date
#include<iostream>
#include<cctype>
#include<cstring>
#include<cstdlib>
#include<string>
#include<fstream>
using namespace std;

int load_file(string, string []);

int main()
{
    string nouns[4]; 
    string adjectives[4];
    string verbs[3];
    string words[400];

    string nounFile = "nouns.txt";
    string adjectiveFile = "adjectives.txt";
    string verbFile = "verbs.txt";
    string wordFile = "words.txt";

    int nounsPlace=0;
    int adjectivesPlace=0;
    int verbsPlace=0;

    int placeholder = load_file(nounFile, nouns);
    placeholder = load_file(adjectiveFile, adjectives);
    placeholder = load_file(verbFile, verbs);
    placeholder = load_file(wordFile, words);

    for(int i=0;i<placeholder;i++)
    {
	if(words[i]=="#nouns")
	{
	    words[i]=nouns[nounsPlace];
	    nounsPlace++;
	}
    }

    for(int i=0;i<placeholder;i++)
    {
        if(words[i]=="#adjectives")
        {
            words[i]=adjectives[adjectivesPlace];
            adjectivesPlace++;
        }
    }

    for(int i=0;i<placeholder;i++)
    {
        if(words[i]=="#verbs")
        {
            words[i]=verbs[verbsPlace];
            verbsPlace++;
        }
    }

    ofstream ofs("MADLIB.txt");
    for(int i=0;i<placeholder;i++)
    {
	ofs<<words[i]<<" ";
    }
    ofs.close();

    return 0;
}

int load_file(string filename, string items[])
{
    int size=0;
    int i=0;

    ifstream myFile(filename.c_str());
    while(!myFile.eof())
    {
	getline(myFile, items[i]);
	i++;
	size++;
    }
    myFile.close();
    return size;
}

