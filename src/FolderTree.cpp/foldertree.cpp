#include "../include/folder_tree.h"

FolderTree::FolderTree()
{
    root = nullptr;
}

FolderTree::~FolderTree()
{
    clearTree(root);
}

void FolderTree::clearTree(Node* node)
{
    if (node == nullptr)
        return;

    for (Node* child : node->children)
        clearTree(child);

    delete node;
}

void FolderTree::buildTree(Node* node, int level)
{
    string name = node->folder.filename().string();

    if (name.empty())
        name = node->folder.string();

    string display = "";

    for (int i = 0; i < level; i++)
        display += "   ";

    display += "[Folder] " + name;

    treeItems.push_back(node->folder);

    SendMessageA(
        treeList, LB_ADDSTRING, 0,
        (LPARAM)display.c_str()
    );

    if (level >= 3)
        return;

    for (const auto& entry : fs::directory_iterator(node->folder))
    {
        if (entry.is_directory() && !entry.is_symlink())
        {
            Node* child = new Node;

            child->folder = entry.path();

            node->children.push_back(child);

            buildTree(child, level + 1);
        }
    }
}

void FolderTree::refresh()
{
    clearTree(root);

    root = new Node;

    root->folder = dataFolder;

    treeItems.clear();

    SendMessageA(treeList, LB_RESETCONTENT, 0, 0);

    buildTree(root, 0);
}

fs::path FolderTree::selectedPath()
{
    int index = (int)SendMessageA(
        treeList, LB_GETCURSEL, 0, 0
    );

    if (index == LB_ERR || index >= (int)treeItems.size())
        return "";

    return treeItems[index];
}
