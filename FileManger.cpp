#include <iostream>
#include "FileManager.h"
using namespace std;

FileManager::FileManager()
{
    count = 0;
}

void FileManager::addItem(FileItem item)
{
    if (count < 100)
    {
        items[count] = item;
        count++;
    }
    else
    {
        cout << "Storage is full!" << endl;
    }
}

void FileManager::displayItems()
{
    cout << "\nNAME\tTYPE\tSIZE\n";
    cout << "-----------------------------\n";

    for (int i = 0; i < count; i++)
    {
        items[i].display();
    }
}

void FileManager::displayDetails(int index)
{
    if (index >= 0 && index < count)
    {
        cout << "\nFile Details\n";
        cout << "Name: " << items[index].getName() << endl;
        cout << "Type: " << items[index].getType() << endl;
        cout << "Size: " << items[index].getSize()
            << " bytes" << endl;
    }
    else
    {
        cout << "Invalid selection!" << endl;
    }
}

int FileManager::getCount()
{
    return count;
}