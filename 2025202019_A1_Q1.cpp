#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

void printprog(off_t totfill, off_t filsiz)
{
    double per = (totfill * 100.00) / filsiz;
    cout << "\r" << per << "% completed         " << flush;
}

void flag_0(int filopen, int filout, size_t boxsiz, off_t filsiz)
{
    char *buff = new char[boxsiz];
    off_t totfill = 0;
    ssize_t rdbyt;
    while ((rdbyt = read(filopen, buff, boxsiz)) > 0)
    {
        for (int i = 0; i < rdbyt / 2; i++)
        {
            swap(buff[i], buff[rdbyt - i - 1]);
        }

        if (write(filout, buff, rdbyt) != rdbyt)
        {
            cout << "Error writing to output file";
            close(filopen);
            close(filout);
            delete[] buff;
        }
        totfill += rdbyt;
        printprog(totfill, filsiz);
    }
    delete[] buff;
    cout << endl;
}

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
    lseek(filopen, 0, SEEK_SET);
    // cout << "File size: " << filsiz << " bytes";

    string filpath = inpufil;
    size_t last = filpath.find_last_of('/');

    string filnam;

    if (last < filpath.length())
    {
        filnam = filpath.substr(last + 1);
    }
    else
    {
        filnam = filpath;
    }
    // cout << "File name: " << filnam;

    string outpath = "Assignment1/" + flag + filnam;

    // cout<< "Output file path: " << outpath << endl;

    int filout = open(outpath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);

    if (filout == -1)
    {
        cout << "Error creating output file";
        close(filopen);
    }

    if (flag == 0)
    {
        if (argc < 4)
        {
            cout << "Enter the correct number of arguments";
            close(filopen);
            close(filout);
        }

        size_t boxsiz = atoi(argv[3]);
        if (boxsiz <= 0)
        {
            cout << "Box size must be greater than 0";
            close(filopen);
            close(filout);
        }
        flag_0(filopen, filout, boxsiz, filsiz);
    }
}