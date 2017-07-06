//Polo Melendez
//Coding Assignment 5
//CS312
#include<iostream>
#include<cstdio>
#include<cstring>
#include<fstream>

using namespace std;

float lz77_code(const char*, int, int, int, ofstream&);
float findOptimal(const char*, ofstream&);

int main(int argc, char *argv[]) {

    if(argc != 3) {
        cout << "Usage: ./lmelendez-cs312-code5 <inFile> <outFile>\n";
        return 0;
    }

    char *fileNameIn = argv[1];
    char *fileNameOut = argv[2];

    ifstream infile;
    infile.open(fileNameIn);
    if( !infile.is_open()) {
        cout << "Could not open the input file\n";
        return 0;
    }
    ofstream outfile;
    outfile.open(fileNameOut);
    if( !outfile.is_open()) {
        cout << "Could not open the output file\n";
        return 0;
    }

    char text[100000];
    for(int j=0; j<100000; j++) {
        text[j] = '\0';
    }
    int i = 0;
    while(!infile.eof()) {
        infile.read(text + i, 1);
        i++;
    }

    lz77_code(text, strlen(text), 255, 12, outfile);
    //findOptimal(text, outfile);
    return 0;
}

float findOptimal(const char *text, ofstream &outfile) {
    int bestDictSize = 0;
    int bestBuffSize = 0;
    float bestRatio = 999999999;
    float temp;
        for(int j = 0; j < 255; j++) {
            temp = lz77_code(text, strlen(text), 255, j, outfile);
            if(bestRatio > temp) {
                bestBuffSize = j;
                bestRatio = temp;
            }
    }
    cout << "Optimal dictSize: " << 255 << endl;
    cout << "Optimal buffSize: " << bestBuffSize << endl;
    cout << "Ratio: " << bestRatio << endl;
    return bestRatio;
}

float lz77_code(const char *data, int dataSize, int dictSize, int buffSize, ofstream &outfile) {
    //dictionary: previously coded
    //buffer: look ahead buffer
    int pos = 0;    //position in buffer
    int offset, length;
    int compressedSize = 0;
    while(pos < dataSize) {
        //look for longest match
        offset = length = 0;
        for(int i=1; i <=buffSize; i++) {
            if(pos + i - 1 > dataSize) {
                continue;
            }
            //start at each character of dictionary
            for(int j=0; j<dictSize; j++) {
                int start = pos - dictSize + j;
                if( start  < 0)
                    continue;
                //compare i characters
                bool match = true;
                for (int k = 0; k < i; k++) {
                    if(data[pos+k] != data[start + k]) {
                        match = false;
                        break;
                    }
                }
                if(match) {
                    offset = pos - start;
                    length = i;
                }
            }
        }
        //outfile << offset << length << data[pos+length];
        compressedSize += 3;
        //fprintf(outfile, "(%i %i '%c')\n", (char)offset, (char)length, data[pos+length]);
        //outfile << "(" << (char)offset << " " << (char)length << " '" <<  data[pos+length]
        outfile << "(" << (unsigned char)offset << " " << (unsigned char)length << " '" <<  data[pos+length]
            << "')\n";
        pos += (length + 1);
    }
    float ratio = (float)compressedSize / (float)dataSize;
    outfile << "compressedSize: " << compressedSize << endl;
    outfile << "compressedRatio: " << ratio << endl;
    return ratio;
}
