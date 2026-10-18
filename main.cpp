#include <iostream>
#include "FileManager.h"
using namespace std;

int main()
{
    FileManager manager;

    manager.addItem(FileItem("Notes.pdf", "File", 2500));
    manager.addItem(FileItem("Project.cpp", "File", 1800));
    manager.addItem(FileItem("Pictures", "Folder", 0));
    manager.addItem(FileItem("Report.docx", "File", 3200));

    int choice;

    do
    {
        cout << "\n===== RS FILE EXPLORER =====\n";
        cout << "1. Display Files and Folders\n";
        cout << "2. View Item Details\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        cin >> choice;

        switch (choice)
        {
        case 1:
            manager.displayItems();
            break;

        case 2:
        {
            int index;

            manager.displayItems();

            cout << "\nEnter item index (0 onwards): ";
            cin >> index;

            manager.displayDetails(index);
            break;
        }

        case 3:
            cout << "Exiting File Explorer...\n";
            break;

        default:
            cout << "Invalid choice!\n";
        }

    } while (choice != 3);

    return 0;
}