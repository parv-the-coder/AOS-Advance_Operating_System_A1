#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

void printprog(off_t totfill, off_t filsiz)
{
    double per = (totfill * 100.00) / filsiz;
    cout << "\r" << per << "% completed         " << flush;
}

void rev(char *buff, ssize_t rdbyt)
{
    for (int i = 0; i < rdbyt / 2; i++)
    {
        swap(buff[i], buff[rdbyt - i - 1]);
    }
}

void flag_0(int filopen, int filout, size_t boxsiz, off_t filsiz)
{
    char *buff = new char[boxsiz];
    off_t totfill = 0;
    ssize_t rdbyt;
    while ((rdbyt = read(filopen, buff, boxsiz)) > 0)
    {
        rev(buff, rdbyt);
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

void flag_1(int filopen, int filout, off_t filsiz)
{
    const size_t boxsiz = 4096;
    char *buff = new char[boxsiz];
    off_t totfill = 0;
    off_t idx = filsiz;

    while (idx > 0)
    {
        size_t rdbyt;
        if (idx >= boxsiz)
        {
            rdbyt = boxsiz;
        }
        else
        {
            rdbyt = idx;
        }
        idx -= rdbyt;

        lseek(filopen, idx, SEEK_SET);
        read(filopen, buff, rdbyt);
        rev(buff, rdbyt);

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

void flag_2(int filopen, int filout, off_t filsiz, off_t start, off_t end)
{
    const size_t boxsiz = 4096;
    char *buff = new char[boxsiz];
    off_t totfill = 0;

    off_t idx = start;
    while (idx > 0)
    {
        size_t rdbyt;
        if (idx >= (off_t)boxsiz)
        {
            rdbyt = boxsiz;
        }
        else
        {
            rdbyt = idx;
        }
        idx -= rdbyt;
        lseek(filopen, idx, SEEK_SET);
        read(filopen, buff, rdbyt);
        rev(buff, rdbyt);
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

    off_t cpydata = end - start + 1;
    lseek(filopen, start, SEEK_SET);
    while (cpydata > 0)
    {
        size_t rdbyt;
        if (cpydata >= (off_t)boxsiz)
        {
            rdbyt = boxsiz;
        }
        else
        {
            rdbyt = cpydata;
        }
        read(filopen, buff, rdbyt);
        if (write(filout, buff, rdbyt) != rdbyt)
        {
            cout << "Error writing to output file";
            close(filopen);
            close(filout);
            delete[] buff;
        }
        cpydata -= rdbyt;
        totfill += rdbyt;
        printprog(totfill, filsiz);
    }

    idx = filsiz - end - 1;
    off_t ptr = filsiz;
    while (idx > 0)
    {
        size_t rdbyt;
        if (idx >= (off_t)boxsiz)
        {
            rdbyt = boxsiz;
        }
        else
        {
            rdbyt = idx;
        }
        ptr -= rdbyt;
        lseek(filopen, ptr, SEEK_SET);
        read(filopen, buff, rdbyt);
        rev(buff, rdbyt);
        write(filout, buff, rdbyt);
        idx -= rdbyt;
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
    if (filsiz <= 0)
    {
        cout << "File is empty";
        close(filopen);
    }
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

    string outpath = "Assignment1/" + to_string(flag) + filnam;

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
    else if (flag == 1)
    {
        if (argc != 3)
        {
            cout << "Enter the correct number of arguments";
            close(filopen);
            close(filout);
        }
        flag_1(filopen, filout, filsiz);
    }
    else if (flag == 2)
    {
        if (argc != 5)
        {
            cout << "Enter the correct number of arguments";
            close(filopen);
            close(filout);
        }
        off_t start = atoi(argv[3]);
        off_t end = atoi(argv[4]);
        if (start < 0 || end < 0 || start >= filsiz || end >= filsiz || start > end)
        {
            cout << "Invalid start or end position";
            close(filopen);
            close(filout);
        }
        flag_2(filopen, filout, filsiz, start, end);
    }
    else
    {
        cout << "Invalid flag value";
    }
}