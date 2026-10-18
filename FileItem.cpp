#include <iostream>
#include "FileItem.h"
using namespace std;

FileItem::FileItem()
{
    name = "";
    type = "";
    size = 0;
}

FileItem::FileItem(string n, string t, long s)
{
    name = n;
    type = t;
    size = s;
}

string FileItem::getName()
{
    return name;
}

string FileItem::getType()
{
    return type;
}

long FileItem::getSize()
{
    return size;
}

void FileItem::display()
{
    cout << name << "\t"
        << type << "\t"
        << size << " bytes" << endl;
}