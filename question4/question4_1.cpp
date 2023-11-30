#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <chrono>  
#include <iomanip>
#include <thread> 
#include <mutex>
#include <fstream>

using namespace std;
namespace fs = filesystem;

class Shell {
public:
    void run_shell() {
    for (;;) {
        cout<<"Project_shell-> ";
        string input;
        getline(cin, input);

        if (input.empty()) {
            cout<<"Exiting shell."<<endl;
            break;
        }

       startTimer(); // Start timer before executing the command
            p_and_e(input);
            long long execution_time = stopTimer(); // Stop timer after executing the command

            // Log the execution time to a file
            logExecutionTime(input, execution_time);
        
    }
}


private:
    mutex mtx;

     std::chrono::steady_clock::time_point start_time;

    // Function to start the timer
    void startTimer() {
        start_time = std::chrono::steady_clock::now();
    }

    // Function to stop the timer and return the elapsed time in milliseconds
    long long stopTimer() {
        auto end_time = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    }

    // Function to log the command and its execution time to a file
    void logExecutionTime(const std::string& command, long long execution_time) {
        std::ofstream logFile("execution_log2.txt", std::ios_base::app);
        logFile <<" using normal execution "<< "Command: " << command << "\tExecution Time: " << execution_time << " milliseconds\n";
    }

    void p_and_e(const string& input) {
        vector<string> tokens = tokenize(input);
        if (tokens.empty()) {
            return;
        }

        string command = tokens[0];
        tokens.erase(tokens.begin()); // Remove the command from tokens

        if (command == "cd") {
            dir_change(tokens);
        } else if (command == "ls") {
            dir_list(tokens);
        } else if (command == "mv") {
            moveFile(tokens);
        } else if (command == "rm") {
            file_rm(tokens);
        } else if (command == "cp") {
            copyFile(tokens);
        } else if (command == "exit") {
            cout<<"Exiting shell."<<endl;
            exit(0);
        } else {
            cout<<"Unknown command: "<<command<<endl;
        }
    }

    vector<string> tokenize(const string& input) {
        vector<string> tokens;
        istringstream iss(input);
        string token;

        while (iss>>token) {
            tokens.push_back(token);
        }

        return tokens;
    }


void dir_change(const vector<string>& tokens) {
        if (tokens.empty()) {
            cout<<"cd: Missing directory operand"<<endl;
        } else {
            if (fs::exists(tokens[0]) && fs::is_directory(tokens[0])) {
                fs::current_path(tokens[0]);
            } else {
                cout<<"cd: No such file or directory: "<<tokens[0]<<endl;
            }
        }
    }


    void dir_list(const vector<string>& tokens) {
        bool recursive = false;

        for (const auto& token : tokens) {
            if (token == "-r" || token == "--recursive") {
                recursive = true;
            } else if (token == "-h" || token == "--help") {
                print_data("ls");
                return;
            }
        }

        vector<fs::path> input_entries;
        for (const auto& entry : fs::directory_iterator(fs::current_path())) {
            input_entries.push_back(entry.path());
        }

        // Create threads for each entry
        vector<thread> threads;
        for (const auto& entry : input_entries) {
            threads.emplace_back(&Shell::listEntry, this, entry, recursive);
        }

        // Wait for all threads to finish
        for (auto& thread : threads) {
            thread.join();
        }
    }

    void listEntry(const fs::path& entry, bool recursive) {
        lock_guard<mutex> lock(mtx); // Lock to avoid race conditions

        // Print the thread ID and the entry name
        cout<<"Thread ID: "<<this_thread::get_id()<<"\tEntry: "<<entry.filename().string()<<endl;

        if (recursive && fs::is_directory(entry)) {
            listRecursive(entry);
        }
    }

    void listRecursive(const fs::path& path) {
        for (const auto& entry : fs::directory_iterator(path)) {
            listEntry(entry.path(), true);
        }
    }

   void file_rm(const vector<string>& tokens) {
        bool recursive = false;
        bool interactive = false;
        bool i_force = false;

        for (const auto& token : tokens) {
            if (token == "-r" || token == "--recursive") {
                recursive = true;
            } else if (token == "-i") {
                interactive = true;
            } else if (token == "-f") {
                i_force = true;
            } else if (token == "-h" || token == "--help") {
                print_data("rm");
                return;
            }
        }

        for (const auto& file : tokens) {
            if (fs::exists(file)) {
                if (fs::is_directory(file)) {
                    if (!recursive && !i_force) {
                        if (interactive) {
                            cout<<"rm: remove directory '"<<file<<"'? (y/n): ";
                            char response;
                            cin>>response;
                            if (response != 'y') {
                                continue;
                            }
                        } else {
                            cout<<"rm: cannot remove '"<<file<<"': Is a directory. Use -r to remove directories."<<endl;
                            continue;
                        }
                    }

                    fs::remove_all(file);
                } else {
                    if (!i_force) {
                        if (interactive) {
                            cout<<"rm: remove file '"<<file<<"'? (y/n): ";
                            char response;
                            cin>>response;
                            if (response != 'y') {
                                continue;
                            }
                        } else {
                            cout<<"rm: cannot remove '"<<file<<"': Not a directory. Use -f to i_force removal."<<endl;
                            continue;
                        }
                    }

                    fs::remove(file);
                }
            } else {
                cout<<"rm: No such file or directory: "<<file<<endl;
            }
        }
    }

    void copyFile(const vector<string>& tokens) {
        bool interactive = false;
        bool recursive = false;
        bool i_force = false;

        for (const auto& token : tokens) {
            if (token == "-r" || token == "--recursive") {
                recursive = true;
            } else if (token == "-i") {
                interactive = true;
            } else if (token == "-f") {
                i_force = true;
            } else if (token == "-h" || token == "--help") {
                print_data("cp");
                return;
            }
        }

        if (tokens.size() < 2) {
            cout<<"cp: Missing source and/or destination operands"<<endl;
            return;
        }
        unsigned int numThreads = thread::hardware_concurrency();
        vector<thread> threads;
        for (unsigned int i = 0; i < numThreads; ++i) {
            threads.emplace_back(&Shell::c_F_T, this, ref(tokens), recursive, interactive, i_force, i, numThreads);
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }


    void moveFile(const vector<string>& tokens) {
        bool recursive = false;
        bool interactive = false;
        bool i_force = false;

        for (const auto& token : tokens) {
            if (token == "-r" || token == "--recursive") {
                recursive = true;
            } else if (token == "-i") {
                interactive = true;
            } else if (token == "-f") {
                i_force = true;
            } else if (token == "-h" || token == "--help") {
                print_data("mv");
                return;
            }
        }

        if (tokens.size() < 2) {
            cout<<"mv: Missing source and/or destination operands"<<endl;
            return;
        }

        for (const auto& source : tokens) {
            if (source != tokens.back()) {
                fs::path destination(tokens.back());
                if (fs::is_directory(destination)) {
                    destination /= fs::path(source).filename();
                }

                if (fs::exists(source)) {
                    if (fs::exists(destination) && !i_force) {
                        if (interactive) {
                            cout<<"mv: overwrite '"<<destination<<"'? (y/n): ";
                            char response;
                            cin>>response;
                            if (response != 'y') {
                                continue;
                            }
                        } else {
                            cout<<"mv: destination file '"<<destination<<"' already exists. Use -f to overwrite."<<endl;
                            continue;
                        }
                    }

                    if (recursive && fs::is_directory(source)) {
                        fs::copy(source, destination, fs::copy_options::recursive);
                        fs::remove_all(source);
                    } else {
                        fs::rename(source, destination);
                    }
                } else {
                    cout<<"mv: No such file or directory: "<<source<<endl;
                }
            }
        }
    }

    

    

   void c_F_T(const vector<string>& tokens, bool recursive, bool interactive, bool i_force, unsigned int threadId, unsigned int numThreads) {

    for (unsigned int i = threadId; i < tokens.size() - 1; i += numThreads) {
        const auto& source = tokens[i];
        fs::path destination(tokens.back());
        if (fs::is_directory(destination)) {
            destination /= fs::path(source).filename();
        }

        if (fs::exists(source)) {
            if (fs::exists(destination) && !i_force) {
                if (interactive) {
                    cout<<"cp: overwrite '"<<destination<<"'? (y/n): ";
                    char response;
                    cin>>response;
                    if (response != 'y') {
                        continue;
                    }
                } else {
                    cout<<"cp: destination file '"<<destination<<"' already exists. Use -f to overwrite."<<endl;
                    continue;
                }
            }

            lock_guard<mutex> lock(mtx);
            cout<<"Thread ID: "<<this_thread::get_id()<<"\tCopying: "<<source<<" to "<<destination<<endl;

            if (recursive && fs::is_directory(source)) {
                fs::copy(source, destination, fs::copy_options::recursive);
            } else {
                fs::copy(source, destination);
            }
        } else {
            cout<<"cp: No such file or directory: "<<source<<endl;
        }
    }
}

    void print_data(const string& command) {
        if (command == "ls") {
            cout<<"Usage: ls [OPTION]... [FILE]...\n";
            cout<<"List information about the FILEs (the current directory by default).\n\n";
            cout<<"Options:\n";
            cout<<"  -r, --recursive   list subdirectories recursively\n";
            cout<<"  -l               use a long listing format\n";
            cout<<"  -a, --all        do not ignore input_entries starting with .\n";
            cout<<"  -t               sort by modification time, newest first\n";
            cout<<"  -h, --help        display this help and exit\n";
        } else if (command == "mv") {
            cout<<"Usage: mv [OPTION]... SOURCE... DEST\n";
            cout<<"Move SOURCE to DEST, or move SOURCE(s) to DIRECTORY.\n\n";
            cout<<"Options:\n";
            cout<<"  -r, --recursive   remove directories and their contents recursively\n";
            cout<<"  -i               prompt before overwriting existing files\n";
            cout<<"  -f               i_forcefully overwrite existing files without prompting\n";
            cout<<"  -h, --help        display this help and exit\n";
        } else if (command == "rm") {
            cout<<"Usage: rm [OPTION]... FILE...\n";
            cout<<"Remove (unlink) the FILE(s).\n\n";
            cout<<"Options:\n";
            cout<<"  -r, --recursive   remove directories and their contents recursively\n";
            cout<<"  -i               prompt before removing each file\n";
            cout<<"  -f               i_forcefully remove files without prompting\n";
            cout<<"  -h, --help        display this help and exit\n";
        } else if (command == "cp") {
            cout<<"Usage: cp [OPTION]... SOURCE... DEST\n";
            cout<<"Copy SOURCE to DEST, or copy SOURCE(s) to DIRECTORY.\n\n";
            cout<<"Options:\n";
            cout<<"  -r, --recursive   copy directories and their contents recursively\n";
            cout<<"  -i               prompt before overwriting existing files\n";
            cout<<"  -f               i_forcefully overwrite existing files without prompting\n";
            cout<<"  -h, --help        display this help and exit\n";
        }
    }
};

int main() {
    Shell myShell;
    myShell.run_shell();
    return 0;
}
