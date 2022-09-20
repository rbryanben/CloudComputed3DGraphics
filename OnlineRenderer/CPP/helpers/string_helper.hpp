#ifndef helper
#define helper
#include <iostream>
#include <strings.h>
#include <vector>

using namespace std;
//splits string into lines
vector<string> split(string raw){
    vector<string> res;
    string current;
    for (int i=0; i != raw.length() - 1; i++){
        if (raw[i] == '/' && raw[i+1] == 'n'){
            res.push_back(current);
            i += 1;
            current = "";
        }
        else {
            current += raw[i];
        }
    }

    if (current != ""){
        res.push_back(current+raw[raw.length()-1]);
    }

    return res;
}

vector<string> splitDelimeter(string raw,char delimeter){
    vector<string> res;
    string current;
    for (int i=0; i != raw.length(); i++){
        if (raw[i] == delimeter){
            res.push_back(current);
            current = "";
        }
        else {
            current += raw[i];
        }
    }

    if (current != ""){
        res.push_back(current);
    }

    return res;
}

#endif