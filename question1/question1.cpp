#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <chrono> 
#include <fstream>  // Add this include for time-related functionalities
using namespace std;

namespace fs =  filesystem;


class Shell {
public:
    void run_shell() {
    for (;;) {
        cout << "Project_shell-> ";
        string input;
        getline(cin, input);

        if (input.empty()) {
            cout << "Exiting shell." << endl;
            break;
        }
        
            p_and_e(input);
            
        
    }
}


private:

 

    void p_and_e(const  string& input) {
         vector< string> tokens = tokenize(input);
        if (tokens.empty()) {
            return;
        }

         string insert_command = tokens[0];
        tokens.erase(tokens.begin()); // Remove the insert_command from tokens

        if (insert_command == "cd") {
            Dir_change(tokens);
        } 
        else if (insert_command == "ls") {
            listDirectory(tokens);
        } 
        else if (insert_command == "mv") {
            moving_file(tokens);
        } 
        else if (insert_command == "rm") {
            removeFile(tokens);
        } 
        else if (insert_command == "cp") {
            file_copy(tokens);
        } 
        else if (insert_command == "exit") {
             cout << "Exiting shell." <<  endl;
             exit(0);
        } 
        else {
             cout << "Insert a Valid command : " << insert_command <<  endl;
        }
    }

     vector< string> tokenize(const  string& input) {
         vector< string> tokens;
         istringstream iss(input);
         string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        return tokens;
    }

    void Dir_change(const  vector< string>& tokens) {
        if (tokens.empty()) {
             cout<<"cd: Missing directory operand" <<  endl;
        } else {
            if (fs::exists(tokens[0]) && fs::is_directory(tokens[0])) {
                fs::current_path(tokens[0]);
            } else {
                 cout << "cd: No such file or directory: " << tokens[0] <<  endl;
            }
        }
    }

    void listDirectory(const  vector< string>& tokens) {
        bool s_hidden_data = false;
        bool time_short = false;
        bool recursive = false;
        bool det_list = false;
        

        for (const auto& token : tokens) {
            if (token == "-r" || token == "--recursive") {
                recursive = true;
            } else if (token == "-l") {
                det_list = true;
            } else if (token == "-a" || token == "--all") {
                s_hidden_data = true;
            } else if (token == "-t") {
                time_short = true;
            } else if (token == "-h" || token == "--help") {
                printing_help_fun("ls");
                return;
            }
        }

         vector<fs::path> input_entries;
        for (const auto& entry : fs::directory_iterator(fs::current_path())) {
            if (!s_hidden_data && entry.path().filename().string().substr(0, 1) == ".") {
                continue;
            }
            input_entries.push_back(entry.path());
        }

        if (time_short) {
             sort(input_entries.begin(), input_entries.end(), [](const fs::path& a, const fs::path& b) {
                return fs::last_write_time(a) > fs::last_write_time(b);
            });
        }

        for (const auto& entry : input_entries) {
            if (recursive && fs::is_directory(entry)) {
                listRecursive(entry);
            } else {
                if (det_list) {
                    auto writeTime = fs::last_write_time(entry);
                     cout << fs::file_size(entry) << " B\t";
                }
                 cout << entry.filename().string() <<  endl;
            }
        }
    }

    void listRecursive(const fs::path& path) {
        for (const auto& entry : fs::directory_iterator(path)) {
             cout << entry.path().filename().string() <<  endl;
            if (fs::is_directory(entry)) {
                listRecursive(entry.path());
            }
        }
    }

    void removeFile(const  vector< string>& tokens) {
        bool recursive = false;
        bool interactive = false;
        bool force = false;

        for (const auto& token : tokens) {
            if (token == "-r" || token == "--recursive") {
                recursive = true;
            }else if (token == "-f") {
                force = true; 
            
            }else if (token == "-i") {
                interactive = true; 
            } else if (token == "-h" || token == "--help") {
                printing_help_fun("rm");
                return;
            }
        }

        for (const auto& file : tokens) {
            if (fs::exists(file)) {
                if (fs::is_directory(file)) {
                    if (!recursive && !force) {
                        if (interactive) {
                             cout << "rm: Remove directory '"<<file<<"'? (y/n): ";
                            char input_response;
                             cin >> input_response;
                            if (input_response != 'y') {
                                continue;
                            }
                        } else {
                             cout << "rm: cannot remove '" << file << "': Is a directory. Use -r to remove directories." <<  endl;
                            continue;
                        }
                    }

                    fs::remove_all(file);
                } else {
                    if (!force) {
                        if (interactive) {
                             cout << "rm: remove file '" << file << "'? (y/n): ";
                            char input_response;
                             cin >> input_response;
                            if (input_response != 'y') {
                                continue;
                            }
                        } else {
                             cout << "rm: can not remove '" << file << "': Not a directory. Use -f to force removal." <<  endl;
                            continue;
                        }
                    }

                    fs::remove(file);
                }
            } else {
                 cout << "rm: No such file or directory: " << file <<  endl;
            }
        }
    }

    void file_copy(const  vector< string>& tokens) {
        bool recursive = false;
        bool interactive = false;
        bool force = false;

        for (const auto& token : tokens) {
            if (token == "-r" || token == "--recursive") {
                recursive = true;
            } else if (token == "-i") {
                interactive = true;
            } else if (token == "-f") {
                force = true;
            } else if (token == "-h" || token == "--help") {
                printing_help_fun("cp");
                return;
            }
        }

        if (tokens.size()<2) {
             cout << "cp: Missing source and/or destination operands" <<  endl;
            return;
        }

        for (const auto& source : tokens) {
            if (source != tokens.back()) {
                fs::path destination(tokens.back());
                if (fs::is_directory(destination)) {
                    destination /= fs::path(source).filename();
                }

                if (fs::exists(source)) {
                    if (fs::exists(destination) && !force) {
                        if (interactive) {
                             cout << "cp: overwrite '" << destination << "'? (y/n): ";
                            char input_response;
                             cin >> input_response;
                            if (input_response != 'y') {
                                continue;
                            }
                        } else {
                             cout << "cp: destination file '" << destination << "' already exists. Use -f to overwrite." <<  endl;
                            continue;
                        }
                    }

                    if (recursive && fs::is_directory(source)) {
                        fs::copy(source, destination, fs::copy_options::recursive);
                    } else {
                        fs::copy(source, destination);
                    }
                } else {
                     cout << "cp: No such file or directory: " << source <<  endl;
                }
            }
        }
    }

    void moving_file(const  vector< string>& tokens) {
        bool recursive = false;
        bool interactive = false;
        bool force = false;

        for (const auto& token : tokens) {
            if (token == "-r" || token == "--recursive") {
                recursive = true;
            } else if (token == "-i") {
                interactive = true;
            } else if (token == "-f") {
                force = true;
            } else if (token == "-h" || token == "--help") {
                printing_help_fun("mv");
                return;
            }
        }

        if (tokens.size() < 2) {
             cout << "mv: Missing source and/or destination operands" <<  endl;
            return;
        }

        for (const auto& source : tokens) {
            if (source != tokens.back()) {
                fs::path destination(tokens.back());
                if (fs::is_directory(destination)) {
                    destination /= fs::path(source).filename();
                }

                if (fs::exists(source)) {
                    if (fs::exists(destination) && !force) {
                        if (interactive) {
                             cout << "mv: overwrite '" << destination << "'? (y/n): ";
                            char input_response;
                             cin >> input_response;
                            if (input_response != 'y') {
                                continue;
                            }
                        } else {
                             cout << "mv: destination file '" << destination << "' already exists. Use -f to overwrite." <<  endl;
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
                     cout << "mv: No such file or directory: " << source <<  endl;
                }
            }
        }
    }

    

    

    void printing_help_fun(const  string& insert_command) {
        if (insert_command == "ls") {
             cout << "Usage: ls [OPTION]... [FILE]...\n";
             cout << "List information about the FILEs (the current directory by default).\n\n";
             cout << "Options:\n";
             cout << "  -r, --recursive   list subdirectories recursively\n";
             cout << "  -l               use a long listing format\n";
             cout << "  -a, --all        do not ignore input_entries starting with .\n";
             cout << "  -t               sort by modification time, newest first\n";
             cout << "  -h, --help        display this help and exit\n";
        } else if (insert_command == "mv") {
             cout << "Usage: mv [OPTION]... SOURCE... DEST\n";
             cout << "Move SOURCE to DEST, or move SOURCE(s) to DIRECTORY.\n\n";
             cout << "Options:\n";
             cout << "  -r, --recursive   remove directories and their contents recursively\n";
             cout << "  -i               prompt before overwriting existing files\n";
             cout << "  -f               forcefully overwrite existing files without prompting\n";
             cout << "  -h, --help        display this help and exit\n";
        } else if (insert_command == "rm") {
             cout << "Usage: rm [OPTION]... FILE...\n";
             cout << "Remove (unlink) the FILE(s).\n\n";
             cout << "Options:\n";
             cout << "  -r, --recursive   remove directories and their contents recursively\n";
             cout << "  -i               prompt before removing each file\n";
             cout << "  -f               forcefully remove files without prompting\n";
             cout << "  -h, --help        display this help and exit\n";
        } else if (insert_command == "cp") {
             cout << "Usage: cp [OPTION]... SOURCE... DEST\n";
             cout << "Copy SOURCE to DEST, or copy SOURCE(s) to DIRECTORY.\n\n";
             cout << "Options:\n";
             cout << "  -r, --recursive   copy directories and their contents recursively\n";
             cout << "  -i               prompt before overwriting existing files\n";
             cout << "  -f               forcefully overwrite existing files without prompting\n";
             cout << "  -h, --help        display this help and exit\n";
        }
    }
};

int main() {
    Shell myShell;
    myShell.run_shell();
    return 0;
}
