#include "../include/file_manager.h"
#include "../include/folder_tree.h"

extern FolderTree folderTree;

void FileManager::initialize(fs::path folder)
{
    currentFolder = folder;
    refresh();
}

void FileManager::refresh()
{
    items.clear();

    SendMessageA(itemList, LB_RESETCONTENT, 0, 0);

    for (const auto& entry : fs::directory_iterator(currentFolder))
    {
        if (entry.is_symlink())
            continue;

        items.push_back(entry.path());

        string name = entry.path().filename().string();

        if (entry.is_directory())
            name = "[Folder] " + name;

        SendMessageA(
            itemList, LB_ADDSTRING, 0,
            (LPARAM)name.c_str()
        );
    }

    string path = "Current Path: " + currentFolder.string();

    SetWindowTextA(pathText, path.c_str());

    SetWindowTextA(statusText, "Folder refreshed");

    folderTree.refresh();
}

fs::path FileManager::selectedPath()
{
    int index = (int)SendMessageA(
        itemList, LB_GETCURSEL, 0, 0
    );

    if (index == LB_ERR || index >= (int)items.size())
        return "";

    return items[index];
}

void FileManager::createFolder()
{
    char name[100];

    GetWindowTextA(nameBox, name, 100);

    if (name[0] == '\0')
        return;

    fs::create_directory(currentFolder / name);

    SetWindowTextA(nameBox, "");

    refresh();
}

void FileManager::createFile()
{
    char name[100];

    GetWindowTextA(nameBox, name, 100);

    if (name[0] == '\0')
        return;

    ofstream file(currentFolder / name);

    file.close();

    SetWindowTextA(nameBox, "");

    refresh();
}

void FileManager::openFolder(fs::path folder)
{
    if (!fs::is_directory(folder))
        return;

    backStack.push(currentFolder);

    currentFolder = folder;

    refresh();
}

void FileManager::openItem()
{
    fs::path item = selectedPath();

    if (item.empty())
        return;

    if (fs::is_symlink(item))
        return;

    if (fs::is_directory(item))
    {
        openFolder(item);
    }
    else
    {
        ShellExecuteA(
            NULL, "open",
            item.string().c_str(),
            NULL, NULL, SW_SHOWNORMAL
        );
    }
}

void FileManager::deleteItem()
{
    fs::path item = selectedPath();

    if (item.empty() || fs::is_symlink(item))
        return;

    int answer = MessageBoxA(
        mainWindow,
        "Are you sure you want to delete this item?",
        "Confirm Delete",
        MB_YESNO | MB_ICONQUESTION
    );

    if (answer == IDYES)
    {
        fs::remove_all(item);
        refresh();
    }
}

void FileManager::goBack()
{
    if (backStack.empty())
    {
        SetWindowTextA(statusText, "No previous folder");
        return;
    }

    currentFolder = backStack.top();

    backStack.pop();

    refresh();
}
