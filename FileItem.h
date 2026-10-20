#ifndef FILEITEM_H
#define FILEITEM_H

#include <string>
using namespace std;

class FileItem
{
private:
    string name;
    string type;
    long size;

public:
    FileItem();
    FileItem(string n, string t, long s);

    string getName();
    string getType();
    long getSize();

    void display();
};

#endif#pragma once
