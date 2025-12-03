#include <iostream>
#include "operations.h"
int main()
{
    ensure_file_exists("archive.txt");
    ensure_file_exists("tmp.txt");
    printf("Welcome to your diary application.\n");
    mainMenu();
    return 0;
}