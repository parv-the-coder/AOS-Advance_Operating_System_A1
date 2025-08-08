#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

int main(int argc, char *argv[])
{

    if (mkdir("Assignment1", 0700) == -1)
    {
        if (errno != EEXIST)
        {
            cout << "Error creating directory: ";
        }
    }

    if (argc < 3)
    {
        cout << "Enter the correct number of arguments";
    }

    const char *inpufil = argv[1];
    int flag = atoi(argv[2]);

    int filopen = open(inpufil, O_RDONLY);

    if (filopen == -1)
    {
        cout << "File not found";
    }

    off_t filsiz = lseek(filopen, 0, SEEK_END);
    // cout << "File size: " << filsiz << " bytes";

    
}