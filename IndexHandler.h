#ifndef INDEXHANDLER_H
#define INDEXHANDLER_H
#include <iostream>
#include "DSAvl_tree.h"
using namespace std;

class IndexHandler{
    private:
        DSAvl_tree<string, string> words;
        DSAvl_tree<string, string> people;
        DSAvl_tree<string, string> orgs;

    public:
};

#endif