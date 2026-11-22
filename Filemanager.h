#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "common.h"

class FileManager
{
private:
    fs::path currentFolder;

    vector<fs::path> items;

    stack<fs::path> backStack;

public:
    void initialize(fs::path folder);

    void refresh();

    void createFolder();

    void createFile();

    void openItem();

    void deleteItem();

    void goBack();

    void openFolder(fs::path folder);

    fs::path selectedPath();
};

#endif
