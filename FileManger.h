#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "FileItem.h"

class FileManager
{
private:
    FileItem items[100];
    int count;

public:
    FileManager();

    void addItem(FileItem item);
    void displayItems();
    void displayDetails(int index);
    int getCount();
};

#endif#pragma once
