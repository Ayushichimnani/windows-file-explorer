#ifndef FOLDER_TREE_H
#define FOLDER_TREE_H

#include "common.h"

class FolderTree
{
private:
    struct Node
    {
        fs::path folder;
        vector<Node*> children;
    };

    Node* root;

    vector<fs::path> treeItems;

    void buildTree(Node* node, int level);

    void clearTree(Node* node);

public:
    FolderTree();

    ~FolderTree();

    void refresh();

    fs::path selectedPath();
};

#endif
