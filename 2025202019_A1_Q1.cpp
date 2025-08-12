#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <filesystem>
using namespace std;

//  for displaying progress percentage
void printprog(off_t totfill, off_t filsiz)
{
    double per = (totfill * 100.00) / filsiz;
    cout << "\r" << per << "% completed         " << flush;
}

// Reverse the contents of the buffer
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
    off_t totfill = 0; // tracks total bytes written
    ssize_t rdbyt;     // number of bytes read in each iteration

    while ((rdbyt = read(filopen, buff, boxsiz)) > 0)
    {
        rev(buff, rdbyt);

        // checks the number of bytes written = number of bytes read
        if (write(filout, buff, rdbyt) != rdbyt)
        {
            perror("Error writing to output file");
            close(filopen);
            close(filout);
            delete[] buff;
            return;
        }

        totfill += rdbyt;           // update total bytes written
        printprog(totfill, filsiz); // display progress
    }

    delete[] buff;
    cout << endl;
}

void flag_1(int filopen, int filout, off_t filsiz)
{
    const size_t boxsiz = 4096; // fixed buffer size
    char *buff = new char[boxsiz];
    off_t totfill = 0;  // track total bytes written
    off_t idx = filsiz; // start from the end of the file

    // read the file in reverse,reverse each box
    while (idx > 0)
    {
        size_t rdbyt;

        // checking how many bytes to read
        if (idx >= boxsiz)
        {
            rdbyt = boxsiz;
        }
        else
        {
            rdbyt = idx;
        }
        idx -= rdbyt;

        // setting  the file pointer to correct position
        lseek(filopen, idx, SEEK_SET);
        read(filopen, buff, rdbyt);
        rev(buff, rdbyt);

        if (write(filout, buff, rdbyt) != rdbyt)
        {
            perror("Error writing to output file");
            close(filopen);
            close(filout);
            delete[] buff;
            return;
        }

        totfill += rdbyt;           // update total bytes written
        printprog(totfill, filsiz); // display progress
    }

    delete[] buff;
    cout << endl;
}

void flag_2(int filopen, int filout, off_t filsiz, off_t start, off_t end)
{
    const size_t boxsiz = 4096; // fixed buffer size
    char *buff = new char[boxsiz];
    off_t totfill = 0; // track total bytes written
    off_t idx = start; // position for before range

    //  1). reverse and write the section before start
    while (idx > 0)
    {
        size_t rdbyt;

        // checking how many bytes to read
        if (idx >= (off_t)boxsiz)
        {
            rdbyt = boxsiz;
        }
        else
        {
            rdbyt = idx;
        }
        idx -= rdbyt; // moving backwards by rdbyt bytes

        // setting the file pointer to correct position
        lseek(filopen, idx, SEEK_SET);
        read(filopen, buff, rdbyt);
        rev(buff, rdbyt);

        //  write the reversed data to output file and error checking
        if (write(filout, buff, rdbyt) != rdbyt)
        {
            perror("Error writing to output file");
            close(filopen);
            close(filout);
            delete[] buff;
            return;
        }

        totfill += rdbyt;           // update total bytes written
        printprog(totfill, filsiz); // display progress
    }

    // 2). copy the box from start to end
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

        read(filopen, buff, rdbyt); // read the data to be copied

        if (write(filout, buff, rdbyt) != rdbyt)
        {
            perror("Error writing to output file");
            close(filopen);
            close(filout);
            delete[] buff;
            return;
        }

        cpydata -= rdbyt;           // update bytes left to copy
        totfill += rdbyt;           // update total bytes written
        printprog(totfill, filsiz); // display progress
    }

    // 3). reverse and write the section after end
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
        ptr -= rdbyt; // moving backwards by rdbyt bytes

        // setting the file pointer to correct position
        lseek(filopen, ptr, SEEK_SET);
        read(filopen, buff, rdbyt);
        rev(buff, rdbyt);

        // write the reversed data to output file and error checking
        if (write(filout, buff, rdbyt) != rdbyt)
        {
            perror("Error writing to output file");
            close(filopen);
            close(filout);
            delete[] buff;
            return;
        }

        idx -= rdbyt;
        totfill += rdbyt;
        printprog(totfill, filsiz);
    }

    delete[] buff;
    cout << endl;
}

int main(int argc, char *argv[])
{

    // create the output directory if it doesn't exist
    if (mkdir("Assignment1", 0700) == -1)
    {
        if (errno != EEXIST)
        {
            perror("Error creating directory: ");
            return 1;
        }
    }

    //  check for minimum number of required arguments
    if (argc < 3)
    {
        cout << "Enter the correct number of arguments" << endl;
        return 1;
    }

    // get input file path and flag from arguments
    const char *inpufil = argv[1];
    int flag;
    try
    {
        flag = stoi(argv[2]);
    }
    catch (const exception &e)
    {
        cout << "Invalid flag value it should be numeric" << endl;
        return 1;
    }
    
    // checking input file is text file
    if (std::filesystem::path(inpufil).extension() != ".txt")
    {
        cout << "Input file must be a .txt file" << endl;
        return 1;
    }

    int filopen = open(inpufil, O_RDONLY); // open input file in read-only mode
    if (filopen == -1)
    {
        perror("File not found");
        return 1;
    }

    off_t filsiz = lseek(filopen, 0, SEEK_END); // get the input file size
    lseek(filopen, 0, SEEK_SET);

    // check for empty file
    if (filsiz <= 0)
    {
        cout << "File is empty" << endl;
        close(filopen);
        return 1;
    }

    // extract file name for output file naming
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

    // create output file path
    string outpath = "Assignment1/" + to_string(flag) + "_" + filnam;

    // open output file in write mode, create if it doesn't exist, truncate if it does
    int filout = open(outpath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (filout == -1)
    {
        perror("Error creating output file");
        close(filopen);
        return 1;
    }

    // Flag 0: reverse box of specified size
    if (flag == 0)
    {
        if (argc < 4)
        {
            cout << "Enter the correct number of arguments" << endl;
            close(filopen);
            close(filout);
            return 1;
        }

        // get box size from arguments
        size_t boxsiz;
        try
        {
            boxsiz = stoi(argv[3]);
        }
        catch (const exception &e)
        {
            cout << "Invalid box size it should be numeric" << endl;
            close(filopen);
            close(filout);
            return 1;
        }
        if (boxsiz <= 0)
        {
            cout << "Box size must be greater than 0" << endl;
            close(filopen);
            close(filout);
            return 1;
        }
        flag_0(filopen, filout, boxsiz, filsiz);
    }

    // Flag 1: reverse the entire file
    else if (flag == 1)
    {
        if (argc != 3)
        {
            cout << "Enter the correct number of arguments" << endl;
            close(filopen);
            close(filout);
            return 1;
        }
        flag_1(filopen, filout, filsiz);
    }

    // Flag 2: reverse before/after range and keep range as it is
    else if (flag == 2)
    {
        if (argc != 5)
        {
            cout << "Enter the correct number of arguments" << endl;
            close(filopen);
            close(filout);
            return 1;
        }

        // get start and end positions from arguments
        off_t start, end;
        try
        {
            start = stoi(argv[3]);
            end = stoi(argv[4]);
        }
        catch (const exception &e)
        {
            cout << "Invalid start or end position it should be numeric" << endl;
        }

        // validate start and end positions
        if (start < 0 || end < 0 || start >= filsiz || end >= filsiz || start > end)
        {
            cout << "Invalid start or end position" << endl;
            close(filopen);
            close(filout);
            return 1;
        }
        flag_2(filopen, filout, filsiz, start, end);
    }

    // check for invalid flag value
    else
    {
        cout << "Invalid flag value" << endl;
        return 1;
    }
}