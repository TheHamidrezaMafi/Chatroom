#ifndef HELPER_H
#define HELPER_H

#include "iostream"
#include "vector"
#include <regex>
using namespace std;

vector<string>* split_message(string message, string wrapper, int pos_start=1);
void check_message_size(vector<string>* message_splitted, int i=-1);

#endif