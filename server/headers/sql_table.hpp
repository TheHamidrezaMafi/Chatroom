#ifndef SERVER_H
#define SERVER_h

#include <string>
#include <fstream>
#include <mutex>

using namespace std;

class tbl
{
tbl(string);
string name;
fstream txt;
mutex txt_mtx;
};


#endif
