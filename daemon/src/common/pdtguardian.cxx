#include <iostream>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdexcept>

int createFile (const std::string& filename, bool truncate) {
    struct stat lstatinfo;
    int fildes;

    /*
     * lstat() the file. If it doesn't exist, create it with O_EXCL.
     * If it does exist, open it for writing and perform the fstat()
     * check.
     */
    if (::lstat(filename.c_str(), &lstatinfo) < 0) {
        /*
         * If lstat() failed for any reason other than "file not
         * existing", exit.
         */
        if (errno != ENOENT) {
            std::string msg = "Error checking file ";
            msg += filename;
            msg += ", ";
            msg += strerror(errno);

            throw std::runtime_error(msg);
        }

        /*
         * The file doesn't exist, so create it with O_EXCL to make
         * sure an attacker can't slip in a file between the lstat()
         * and open()
         */
        if ((fildes =
                    ::open(filename.c_str(), O_RDWR | O_CREAT | O_EXCL, 0644)) < 0) {
            std::string msg = "Could not create file ";
            msg += filename;
            msg += ", ";
            msg += strerror(errno);

            throw std::runtime_error(msg);
        }
    } else {
        struct stat fstatinfo;
        int flags;

        flags = O_RDWR;
        if (!truncate)
            flags |= O_APPEND;

        /*
         * Open an existing file.
         */
        if ((fildes = ::open(filename.c_str(), flags)) < 0) {
            std::string msg = "Could not open file ";
            msg += filename;
            msg += ", ";
            msg += strerror(errno);

            throw std::runtime_error(msg);
        }

        /*
         * fstat() the opened file and check that the file mode bits,
         * inode, and device match.
         */
        if (::fstat(fildes, &fstatinfo) < 0
                || lstatinfo.st_mode != fstatinfo.st_mode
                || lstatinfo.st_ino != fstatinfo.st_ino
                || lstatinfo.st_dev != fstatinfo.st_dev) {

            std::string msg = "File ";
            msg += filename;
            msg += "has been changed before it could be opened, ";
            msg += strerror(errno);

            close(fildes);

            throw std::runtime_error(msg);
        }

        /*
         * If the above check was passed, we know that the lstat()
         * and fstat() were done on the same file. Now we check that
         * there's only one link, and that it's a normal file (this
         * isn't strictly necessary because the fstat() vs lstat()
         * st_mode check would also find this)
         */
        if (fstatinfo.st_nlink > 1 || !S_ISREG(lstatinfo.st_mode)) {
            std::string msg = "File ";
            msg += filename;
            msg += "has too many links, or is not a regular file, ";
            msg += strerror(errno);

            close(fildes);

            throw std::runtime_error(msg);
        }

        /*
         * Just return the file descriptor if we _don't_ want the file
         * truncated.
         */
        if (!truncate)
            return fildes;

        /*
         * On systems which don't support ftruncate() the best we can
         * do is to close the file and reopen it in create mode, which
         * unfortunately leads to a race condition, however "systems
         * which don't support ftruncate()" is pretty much SCO only,
         * and if you're using that you deserve what you get.
         * ("Little sympathy has been extended")
         */
        ::ftruncate(fildes, 0);
    }

    return fildes;
}
void daemonize(const std::string & out, const std::string & err) {
    int lPid;

    lPid = fork();

    /* An error occurred */
    if (lPid < 0) exit(1); /* fork error */
    /* Success: Let the parent terminate */
    if (lPid > 0) exit(0); /* parent exits */

    /* child (daemon) continues */
    /* obtain a new process group */
    if ( setsid() < 0 ) {
        exit(1);
    } 

    int fdout = -1;
    try {
        fdout = createFile(out, false);
    } catch (std::runtime_error& e) {
        std::cerr << "Failed to create file " << out << ": " << e.what() << std::endl;
    }

    int fderr = -1;
    try {
        fderr = createFile(err, false);
    } catch (std::runtime_error& e) {
        std::cerr << "Failed to create file " << err << ": " << e.what() << std::endl;
    }

    for ( int i = 0 ; i <= 2 ; i++ ) close(i); /* close all standard file descriptors */
    /* Close all open file descriptors */
    // for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        // close (x);
    // }

    dup2(fdout, 1); /* redirect stdout */
    dup2(fderr, 2); /* redirect stderr */

    chdir("/tmp"); /* change running directory */

    signal(SIGCHLD, SIG_DFL); /* ignore child */
    signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGHUP, SIG_IGN); /* catch hangup signal */
    //signal(SIGTERM,signal_handler); /* catch kill signal */
}

/**
 * @brief      { function_description }
 *
 * @param[in]  argc  The argc
 * @param      argv  The argv
 *
 * @return     { description_of_the_return_value }
 */
int main(int argc, char const *argv[]) {
    std::cout << "PDT Guardian" << std::endl;
    /* code */

    daemonize("pdt.out", "pdt.err");

    std::cout << "Test stdout" << std::endl;
    std::cerr << "Test stderr" << std::endl;
    return 0;
}