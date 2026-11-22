#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

extern HWND mainWindow;
extern HWND nameBox;
extern HWND itemList;
extern HWND treeList;
extern HWND pathText;
extern HWND statusText;

extern fs::path dataFolder;

const int ID_NAME = 100;
const int ID_LIST = 101;
const int ID_TREE = 102;

const int ID_NEW_FOLDER = 201;
const int ID_NEW_FILE = 202;
const int ID_OPEN = 203;
const int ID_DELETE = 204;
const int ID_REFRESH = 205;
const int ID_BACK = 206;

#endif
