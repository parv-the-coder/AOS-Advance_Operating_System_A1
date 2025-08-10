#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
using namespace std;

bool dircheck(const char *dir)
{
    struct stat stats;
    return (stat(dir, &stats) == 0 && S_ISDIR(stats.st_mode));
}

bool concheck_0(int filold, int filnew, size_t boxsiz)
{
    char *boxin = new char[boxsiz];
    char *boxout = new char[boxsiz];
    ssize_t rdbyin, rdbytout;

    while ((rdbytout = read(filnew, boxout, boxsiz)) > 0)
    {
        rdbyin = read(filold, boxin, boxsiz);
        if (rdbyin != rdbytout)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        for (size_t i = 0; i < rdbytout / 2; i++)
        {
            swap(boxout[i], boxout[rdbytout - i - 1]);
        }

        if (memcmp(boxin, boxout, rdbytout) != 0)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }
    }
    delete[] boxin;
    delete[] boxout;
    return true;
}

bool concheck_1(int filold, int filnew, off_t filsiz)
{
    size_t boxsiz = 4096;
    char *boxin = new char[boxsiz];
    char *boxout = new char[boxsiz];
    off_t idx = 0;

    while (idx < filsiz)
    {
        size_t rdbyt;
        if (filsiz - idx >= boxsiz)
        {
            rdbyt = boxsiz;
        }
        else
        {
            rdbyt = filsiz - idx;
        }

        ssize_t rdbyin = read(filold, boxin, rdbyt);
        if (rdbyin != (ssize_t)rdbyt)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        off_t ptr = filsiz - idx - rdbyt;
        if (lseek(filnew, ptr, SEEK_SET) == -1)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        ssize_t rdbytout = read(filnew, boxout, rdbyt);
        if (rdbytout != (ssize_t)rdbyt)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        for (size_t i = 0; i < rdbyt / 2; i++)
        {
            swap(boxout[i], boxout[rdbyt - i - 1]);
        }

        if (memcmp(boxin, boxout, rdbyt) != 0)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }
        idx += rdbyt;
    }

    delete[] boxin;
    delete[] boxout;
    return true;
}

bool concheck_2(int filold, int filnew, off_t filsiz, off_t start, off_t end)
{
    size_t boxsiz = 4096;
    char *boxin = new char[boxsiz];
    char *boxout = new char[boxsiz];
    off_t idx = start;
    off_t proc = 0;

    while (proc < idx)
    {
        size_t rdbyt;
        if (idx - proc >= boxsiz)
        {
            rdbyt = boxsiz;
        }
        else
        {
            rdbyt = idx - proc;
        }

        if (lseek(filold, proc, SEEK_SET) == -1 || lseek(filnew, proc, SEEK_SET) == -1)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        ssize_t rdbyin = read(filold, boxin, rdbyt);
        ssize_t rdbytout = read(filnew, boxout, rdbyt);

        if (rdbyin != (ssize_t)rdbyt || rdbytout != (ssize_t)rdbyt)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        for (size_t i = 0; i < rdbyt / 2; i++)
        {
            swap(boxout[i], boxout[rdbyt - i - 1]);
        }

        if (memcmp(boxin, boxout, rdbyt) != 0)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        proc += rdbyt;
    }

    idx = end - start + 1;
    proc = 0;

    while (proc < idx)
    {
        size_t rdbyt;
        if (idx - proc >= boxsiz)
        {
            rdbyt = boxsiz;
        }
        else
        {
            rdbyt = idx - proc;
        }

        if (lseek(filold, start + proc, SEEK_SET) == -1 || lseek(filnew, start + proc, SEEK_SET) == -1)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        size_t rdbyin = read(filold, boxin, rdbyt);
        size_t rdbytout = read(filnew, boxout, rdbyt);

        if (rdbyin != (ssize_t)rdbyt || rdbytout != (ssize_t)rdbyt)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        if (memcmp(boxin, boxout, rdbyt) != 0)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        proc += rdbyt;
    }

    idx = filsiz - (end + 1);
    proc = 0;

    while (proc < idx)
    {
        size_t rdbyt;
        if (idx - proc >= boxsiz)
        {
            rdbyt = boxsiz;
        }
        else
        {
            rdbyt = idx - proc;
        }
        if (lseek(filold, end + 1 + proc, SEEK_SET) == -1 || lseek(filnew, filsiz - proc - rdbyt, SEEK_SET) == -1)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        size_t rdbyin = read(filold, boxin, rdbyt);
        size_t rdbytout = read(filnew, boxout, rdbyt);

        if (rdbyin != (ssize_t)rdbyt || rdbytout != (ssize_t)rdbyt)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        for (size_t i = 0; i < rdbyt / 2; i++)
        {
            swap(boxout[i], boxout[rdbyt - i - 1]);
        }

        if (memcmp(boxin, boxout, rdbyt) != 0)
        {
            delete[] boxin;
            delete[] boxout;
            return false;
        }

        proc += rdbyt;
    }
    delete[] boxin;
    delete[] boxout;
    return true;
}

bool filsizcheck(int filopen, int filnew)
{
    off_t filsiz1 = lseek(filopen, 0, SEEK_END);
    off_t filsiz2 = lseek(filnew, 0, SEEK_END);
    lseek(filopen, 0, SEEK_SET);
    lseek(filnew, 0, SEEK_SET);
    return filsiz1 == filsiz2;
}

void percheck(const char *route, const char *title)
{
    struct stat stats;
    if (stat(route, &stats) == -1)
    {
        perror(("Error"));
        return;
    }

    int mode = stats.st_mode;

    cout << "User has read permission on " << title << ": " << ((mode & S_IRUSR) ? "Yes" : "No") << endl;
    cout << "User has write permission on " << title << ": " << ((mode & S_IWUSR) ? "Yes" : "No") << endl;
    cout << "User has execute permission on " << title << ": " << ((mode & S_IXUSR) ? "Yes" : "No") << endl;

    cout << "Group has read permission on " << title << ": " << ((mode & S_IRGRP) ? "Yes" : "No") << endl;
    cout << "Group has write permission on " << title << ": " << ((mode & S_IWGRP) ? "Yes" : "No") << endl;
    cout << "Group has execute permission on " << title << ": " << ((mode & S_IXGRP) ? "Yes" : "No") << endl;

    cout << "Others have read permission on " << title << ": " << ((mode & S_IROTH) ? "Yes" : "No") << endl;
    cout << "Others have write permission on " << title << ": " << ((mode & S_IWOTH) ? "Yes" : "No") << endl;
    cout << "Others have execute permission on " << title << ": " << ((mode & S_IXOTH) ? "Yes" : " No") << endl;
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cout << "Enter the correct number of arguments" << endl;
        return 1;
    }
    const char *orgfil = argv[1];
    const char *outfil = argv[2];
    const char *dir = argv[3];
    int flag = atoi(argv[4]);

    cout << "Directory is created? ";
    if (dircheck(dir))
    {
        cout << "Yes" << endl;
    }
    else
    {
        cout << "No" << endl;
    }

    int filold = open(orgfil, O_RDONLY);
    int filnew = open(outfil, O_RDONLY);

    if (filold == -1 || filnew == -1)
    {
        perror("Error opening files");
        return 1;
    }

    off_t filsiz = lseek(filold, 0, SEEK_END);
    lseek(filold, 0, SEEK_SET);
    lseek(filnew, 0, SEEK_SET);

    bool con = false;
    if (flag == 0)
    {
        if (argc != 6)
        {
            cout << "Enter the correct number of arguments" << endl;
            close(filold);
            close(filnew);
            return 1;
        }
        size_t boxsiz = atoi(argv[5]);
        con = concheck_0(filold, filnew, boxsiz);
    }

    else if (flag == 1)
    {
        if (argc != 5)
        {
            cout << "Enter the correct number of arguments" << endl;
            close(filold);
            close(filnew);
            return 1;
        }
        con = concheck_1(filold, filnew, filsiz);
    }

    else if (flag == 2)
    {
        if (argc != 7)
        {
            cout << "Enter the correct number of arguments" << endl;
            close(filold);
            close(filnew);
            return 1;
        }
        off_t start = atoi(argv[5]);
        off_t end = atoi(argv[6]);
        con = concheck_2(filold, filnew, filsiz, start, end);
    }

    else
    {
        cout << "Invalid flag value" << endl;
        close(filold);
        close(filnew);
        return 1;
    }

    cout << "Whether the file contents are correctly processed? ";
    if (con)
    {
        cout << "Yes" << endl;
    }
    else
    {
        cout << "No" << endl;
    }

    cout << "Both Files Sizes are Same: ";
    if (filsizcheck(filold, filnew))
    {
        cout << "Yes" << endl;
    }
    else
    {
        cout << "No" << endl;
    }

    percheck(dir, "directory");
    percheck(orgfil, "original file");
    percheck(outfil, "output file");

    close(filold);
    close(filnew);
    return 0;
}