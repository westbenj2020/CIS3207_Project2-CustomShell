#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <strstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dirent.h"
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <vector>
#include <queue>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
using namespace std;
using std::vector;
extern char **environ;

void tokenize(char * input, char **output) {
    const char delim[4] = " \t\n";
    size_t i = 0;
    char *token = strtok(input, delim);
    while (token != NULL){
        output[i++] = token;
        token = strtok(NULL, delim);
        output[i] = NULL;
    }
}

void cd(string path);

void cd(string path) {
    if (chdir(path.c_str())) {
        switch (errno) {
            case ENOENT:
                break;
            case EINVAL:
                printf("Invalid buffer.\n");
                break;
            default:
                printf("Unknown error.\n");
        }
    }
}

void cd() {
    system("dir *.exe");
}

void clearScreen() {
    cout << string(100,'\n');
}

void dir() {
    DIR *dr;
    struct dirent *en;
    dr = opendir("."); // open all directory
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            cout << "\n" << en->d_name; // print all directory name
        }
        closedir(dr); // close all directory
    }
}

vector<string> enviro() {
    vector<string> envStrings;
    int i = 1;
    char *s = *environ;
    for (; s; i++) {
        envStrings.emplace_back(s);
        s = *(environ+i);
    }
    return envStrings;
}

void echo(string comment) {
    cout << comment + "\n";
}

void echo(vector<string> comment) {
    for (string s: comment) {
        cout << s + " ";
    }
}

void help() {
    //printf("\n");
    cout << "cd <directory> - Change the current default directory to <directory>."
            "\nIf the <directory> argument is not present, reports the current directory."
            "\nIf the directory does not exist an appropriate error will be reported."
            "\nThis command will also change the PWD environment variable.\n"
            "clr - clears the screen.\n"
            "dir <directory> - Lists the contents of directory <directory>.\n"
            "environ - Lists all the environment strings.\n"
            "echo - Displays <comment> on the display followed by a new line.\n"
            "help - Displays the user manual using the more filter.\n"
            "pause - Pauses operation of the shell until 'Enter' is pressed.\n"
            "quit - Quit the shell.\n";
}

void pauseProcessing() {
    string dummy;
    cout << "\nPress enter to continue...";
    cin.ignore();
}

void quit() {
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc==1) { // run shell in interactive mode
        do {
            printf("\n");
            string input = "";
            size_t size;
            string CurrentPath;
            char *path = NULL;
            path = getcwd(path, size);
            CurrentPath = path;
            cout << CurrentPath << "/myshell> ";
            getline(cin, input);
            vector <string> persistentStrings;
            stringstream ss(input);
            string token;
            while (getline(ss, token, ' ')) {
                persistentStrings.push_back(token); // persistent storage of token for later access; not popped
            }
            vector <string> envStrings = enviro();
            int indices = persistentStrings.size() - 1;
            /**
             *
             * BUILT-IN FUNCTIONS
             *
             */
            if (persistentStrings[0] == "quit" && persistentStrings.size() == 1) {
                exit(0);
            }
            if (persistentStrings[0] == "pause" && persistentStrings.size() == 1) {
                pauseProcessing();
                continue;
            }
            if (persistentStrings[0] == "cd" && persistentStrings.size() == 2) {
                cd(persistentStrings[1]);
                continue;
            }
            if (persistentStrings[0] == "cd" && persistentStrings.size() == 1) {
                cd();
                continue;
            }
            if (persistentStrings[0] == "clr" && persistentStrings.size() == 1) {
                clearScreen();
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings.size() == 1) {
                dir();
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">" && persistentStrings.size() == 3) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                dir();
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                dir();
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                dir();
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">>" && persistentStrings.size() == 3) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                DIR *dr;
                struct dirent *en;
                dr = opendir("."); // open all directory
                if (dr) {
                    while ((en = readdir(dr)) != nullptr) {
                        cout << "\n" << en->d_name; // print all directory name
                    }
                    closedir(dr); // close all directory
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                DIR *dr;
                struct dirent *en;
                dr = opendir("."); // open all directory
                if (dr) {
                    while ((en = readdir(dr)) != nullptr) {
                        cout << "\n" << en->d_name; // print all directory name
                    }
                    closedir(dr); // close all directory
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                DIR *dr;
                struct dirent *en;
                dr = opendir("."); // open all directory
                if (dr) {
                    while ((en = readdir(dr)) != nullptr) {
                        cout << "\n" << en->d_name; // print all directory name
                    }
                    closedir(dr); // close all directory
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings.size() == 1) {
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">" && persistentStrings.size() == 3) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">>" && persistentStrings.size() == 3) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings.size() == 1) {
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings.size() == 2) {
                //vector<string> comment;
                for (int j = 1; j < persistentStrings.size(); j++) {
                    cout << persistentStrings[j];
                }
                //echo(comment);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size() - 2] != ">"
                && persistentStrings[persistentStrings.size() - 2] != ">>" &&
                persistentStrings[persistentStrings.size() - 3] != ">" &&
                persistentStrings[persistentStrings.size() - 3] != ">>") {
                for (int i = 1; i < persistentStrings.size(); i++) {
                    cout << persistentStrings[i] + " ";
                }
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size() - 2] == ">" &&
                persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size() - 1].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size() - 2; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size() - 3] == ">" &&
                persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size() - 2].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size() - 3; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                //persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size() - 3] == ">" &&
                persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size() - 1].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size() - 3; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size() - 2] == ">>" &&
                persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size() - 1].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size() - 2; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size() - 3] == ">>" &&
                persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size() - 2].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size() - 3; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                //persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size() - 3] == ">>" &&
                persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size() - 1].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size() - 3; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size() - 2] == "<") {
                ifstream inputFile;
                inputFile.open(persistentStrings[2].c_str(), ios::in);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                streambuf *stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = inputFile.rdbuf();
                cin.rdbuf(stream_buffer_file);
                while (getline(inputFile, line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                cin.rdbuf(stream_buffer_cin);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(), ios::in);
                outputFile.open(persistentStrings[4].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                streambuf *stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_inputFile = inputFile.rdbuf();
                streambuf *stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile, line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(), ios::in);
                outputFile.open(persistentStrings[4].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                streambuf *stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_inputFile = inputFile.rdbuf();
                streambuf *stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile, line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(), ios::in);
                outputFile.open(persistentStrings[5].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                streambuf *stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_inputFile = inputFile.rdbuf();
                streambuf *stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile, line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">>") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(), ios::in);
                outputFile.open(persistentStrings[4].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                streambuf *stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_inputFile = inputFile.rdbuf();
                streambuf *stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile, line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">>") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(), ios::in);
                outputFile.open(persistentStrings[4].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                streambuf *stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_inputFile = inputFile.rdbuf();
                streambuf *stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile, line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">>") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(), ios::in);
                outputFile.open(persistentStrings[5].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                streambuf *stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_inputFile = inputFile.rdbuf();
                streambuf *stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile, line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings.size() == 1) {
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n"
                        "clr - clears the screen.\n"
                        "dir <directory> - Lists the contents of directory <directory>.\n"
                        "environ - Lists all the environment strings.\n"
                        "echo - Displays <comment> on the display followed by a new line.\n"
                        "help - Displays the user manual using the more filter.\n"
                        "pause - Pauses operation of the shell until 'Enter' is pressed.\n"
                        "quit - Quit the shell.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings.size() == 2) {
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings.size() == 2) {
                cout << "clr - clears the screen.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings.size() == 2) {
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings.size() == 2) {
                cout << "environ - Lists all the environment strings.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings.size() == 2) {
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings.size() == 2) {
                cout << "help - Displays the user manual using the more filter.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings.size() == 2) {
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings.size() == 2) {
                cout << "quit - Quit the shell.\n";
                persistentStrings.clear();
                continue;
            }

            /**
             *
             *
             * FILE I/O REDIRECTION FOR HELP - >
             *
             *
             */


            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == ">" && persistentStrings.size() == 3) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n"
                        "clr - clears the screen.\n"
                        "dir <directory> - Lists the contents of directory <directory>.\n"
                        "environ - Lists all the environment strings.\n"
                        "echo - Displays <comment> on the display followed by a new line.\n"
                        "help - Displays the user manual using the more filter.\n"
                        "pause - Pauses operation of the shell until 'Enter' is pressed.\n"
                        "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "help - Displays the user manual using the more filter.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "help - Displays the user manual using the more filter.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "help - Displays the user manual using the more filter.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "help - Displays the user manual using the more filter.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "help - Displays the user manual using the more filter.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">>" &&
                persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(), ios::app | ios::out);
                string line;
                streambuf *stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf *stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            else { // change else block to work with persistent strings vector instead of cmd line
                bool hasPipe = false;
                int pipeIndex = 0;
                for (int i = 0; i < persistentStrings.size(); i++) {
                    if (persistentStrings[i] == "|") {
                       hasPipe = true;
                       //pipeIndex = i;
                       //persistentStrings.erase(i);
                    }
                }
                if (hasPipe == true) {
                    char *tokens1[100];
                    char *tokens2[100];
                    char *line1 = NULL;
                    char *line2 = NULL;
                    size_t len = 0;
                    ssize_t read;
                    int pid = -1;
                    int pid2 = -1;
                    int pipe_file_descs[2];
                    printf("type first command + args here: ");
                    read = getline (&line1, &len, stdin);
                    assert(read != -1);
                    tokenize(line1, tokens1);
                    printf("type second command + args here: ");
                    read = getline (&line2, &len, stdin);
                    assert(read != -1);
                    tokenize(line2, tokens2);
                    if (pipe(pipe_file_descs) == -1){
                        perror("Failed to create pipe!");
                        exit(-1);
                    }
                    if ((pid = fork()) < 0){
                        perror("fork 1 failed!\n");
                        exit(-1);
                    }
                    if(pid == 0) {
                        close(pipe_file_descs[0]);
                        dup2(pipe_file_descs[1], 1);
                        close(pipe_file_descs[1]);
                        if (execvp(tokens1[0],tokens1) < 0){
                            perror("Could not execute command 1");
                            exit(-1);
                        }
                    }
                    if ((pid2 = fork()) < 0){
                        perror("Second fork failed");
                        exit(-1);
                    }

                    if(pid2 == 0){
                        close(pipe_file_descs[1]);
                        dup2(pipe_file_descs[0], 0);
                        close(pipe_file_descs[0]);
                        if (execvp(tokens2[0],tokens2) < 0){
                            perror("Could not execute command 2");
                            exit(-1);
                        }
                    }
                    waitpid(-1, NULL, 0);
                    //puts("both children finished!");
                    close(pipe_file_descs[0]);
                    close(pipe_file_descs[1]);
                    free(line1);
                    free(line2);
                }
                else {
                    bool ExternalInputRedirect = false;
                    bool ExternalRedirectNonAppend = false;
                    bool ExternalRedirectAppend = false;
                    bool bg = false;
                    for (int i = 0; i < persistentStrings.size(); i++) {
                        if (persistentStrings[i] == "&") {
                            persistentStrings.erase(persistentStrings.begin() + i);
                            i--;
                            bg = true;
                        }
                    }

                    // is there output redirection?
                    string intendedOutputFile;
                    for (int i =0; i < persistentStrings.size(); i++) {
                        if (persistentStrings[i] == ">") {
                            intendedOutputFile = persistentStrings[i+1];
                            ExternalRedirectNonAppend = true;
                            for (int k = i+1; k < persistentStrings.size(); k++) {
                                if (persistentStrings[k] == ">" || persistentStrings[k] == ">>") {
                                    ExternalRedirectNonAppend = false;
                                    perror("Invalid command sequence");
                                }
                            }
                            //persistentStrings.erase(persistentStrings.begin() + i);
                            for (int j = i; j < persistentStrings.size(); j++) {
                                persistentStrings.erase(persistentStrings.begin() + j);
                            }
                        }
                    }
                    for (int i =0; i < persistentStrings.size(); i++) {
                        if (persistentStrings[i] == ">>") {
                            intendedOutputFile = persistentStrings[i+1];
                            ExternalRedirectAppend = true;
                            for (int k = i+1; k < persistentStrings.size(); k++) {
                                if (persistentStrings[k] == ">" || persistentStrings[k] == ">>") {
                                    ExternalRedirectAppend = false;
                                    perror("Invalid command sequence");
                                }
                            }
                            //ExternalRedirectAppend = true;
                            //persistentStrings.erase(persistentStrings.begin() + i);
                            for (int j = i; j < persistentStrings.size(); j++) {
                                persistentStrings.erase(persistentStrings.begin() + j);
                            }
                        }
                    }

                    //is there input redirection?
                    string intendedInputFile;
                    for (int i =0; i < persistentStrings.size(); i++) {
                        if (persistentStrings[i] == "<") {
                            intendedInputFile = persistentStrings[i+1];
                            ExternalInputRedirect = true;
                            for (int k = i+1; k < persistentStrings.size(); k++) {
                                if (persistentStrings[k] == "<") {
                                    ExternalInputRedirect = false;
                                    perror("Invalid command sequence");
                                }
                            }
                            //persistentStrings.erase(persistentStrings.begin() + i);
                            for (int j = i; j < persistentStrings.size(); j++) {
                                persistentStrings.erase(persistentStrings.begin() + j);
                            }
                        }
                    }


                    char *args[persistentStrings.size()];
                    for (size_t i = 0; i < (persistentStrings.size()); ++i) {
                        args[i] = (char*)persistentStrings[i].c_str();
                    }
                    args[persistentStrings.size()] = NULL;
                    int pid = fork();
                    if (pid < 0) {
                        puts("fork failed");
                        exit(1);
                    }
                    if (pid == 0) {
                        if (ExternalInputRedirect == true) {
                            int inFile = open(intendedInputFile.c_str(), O_RDONLY);
                            dup2(inFile, 0);
                            close(inFile);
                        }
                        if (ExternalRedirectNonAppend == true) {
                            int outFile = open(intendedOutputFile.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU | S_IRWXG| S_IRWXO);
                            dup2(outFile, 1);
                            close(outFile);
                        }
                        if (ExternalRedirectAppend == true) {
                            int outFile = open(intendedOutputFile.c_str(), O_WRONLY|O_CREAT|O_APPEND, S_IRWXU | S_IRWXG| S_IRWXO);
                            dup2(outFile, 1);
                            close(outFile);
                        }
                        //puts("executing external command...");
                        //int outFile = open("output.txt", O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU | S_IRWXG| S_IRWXO);
                        //dup2(outFile, 1);
                        //close(outFile);
                        if (execvp(args[0],args) < 0) {
                            perror("Could not execute command");
                            exit(1);
                        }
                    }
                    else {
                        if (bg == false) {
                            waitpid(pid, NULL, 0);
                            //puts("done waiting!");
                        }

                    }
                    continue;
                }
            } continue;
            /**
             *
             * Below, I need to use, instead of argc, the persistentStrings data structure. Problem is that performing
             * .c_str on it returns a constant pointer to a char, not a char pointer.
             */
        }
        while (true);
    }
    bool fileFlag = false;
    ifstream batchFile;
    batchFile.open(argv[1]);
    if (batchFile.is_open()) {
        fileFlag = true;
        batchFile.close();
    }
    if (argc==2 && fileFlag) { // run shell in batch-processing mode
        queue<string> strings;
        ifstream batchFile; // batch file name = argv[1]
        batchFile.open(argv[1]);
        string lineForSplit;
        while (!getline(batchFile,lineForSplit).eof()) {
            vector<string> persistentStrings;
            stringstream ss(lineForSplit);
            string token;
            while (getline(ss, token, ' ')) {
                persistentStrings.push_back(token); // persistent storage of token for later access; not popped
            }
            vector<string> envStrings = enviro();
            int indices = persistentStrings.size() - 1;
            /**
             *
             * BUILT-IN FUNCTIONS
             *
             */
            if (persistentStrings[0] == "quit" && persistentStrings.size() == 1) {
                exit(0);
            }
            if (persistentStrings[0] == "pause" && persistentStrings.size() == 1) {
                pause();
                continue;
            }
            if (persistentStrings[0] == "cd" && persistentStrings.size() == 2) {
                cd(persistentStrings[1]);
                continue;
            }
            if (persistentStrings[0] == "cd" && persistentStrings.size() == 1) {
                cd();
                continue;
            }
            if (persistentStrings[0] == "clr" && persistentStrings.size() == 1) {
                clearScreen();
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings.size() == 1) {
                dir();
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">" && persistentStrings.size() == 3) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                dir();
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                dir();
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                dir();
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">>" && persistentStrings.size() == 3) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                DIR *dr;
                struct dirent *en;
                dr = opendir("."); // open all directory
                if (dr) {
                    while ((en = readdir(dr)) != nullptr) {
                        cout << "\n" << en->d_name; // print all directory name
                    }
                    closedir(dr); // close all directory
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                DIR *dr;
                struct dirent *en;
                dr = opendir("."); // open all directory
                if (dr) {
                    while ((en = readdir(dr)) != nullptr) {
                        cout << "\n" << en->d_name; // print all directory name
                    }
                    closedir(dr); // close all directory
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "dir" && persistentStrings[1] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                DIR *dr;
                struct dirent *en;
                dr = opendir("."); // open all directory
                if (dr) {
                    while ((en = readdir(dr)) != nullptr) {
                        cout << "\n" << en->d_name; // print all directory name
                    }
                    closedir(dr); // close all directory
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings.size() == 1) {
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">" && persistentStrings.size() == 3) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">>" && persistentStrings.size() == 3) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[2].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "environ" && persistentStrings[1] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (string s : envStrings) {
                    cout << s + "\n";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings.size() == 1) {
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings.size() == 2) {
                //vector<string> comment;
                for (int j = 1; j < persistentStrings.size(); j++) {
                    cout << persistentStrings[j];
                }
                //echo(comment);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size()-2] != ">"
                && persistentStrings[persistentStrings.size()-2] != ">>" &&
                persistentStrings[persistentStrings.size()-3] != ">" &&
                persistentStrings[persistentStrings.size()-3] != ">>") {
                for (int i = 1; i < persistentStrings.size(); i++) {
                    cout << persistentStrings[i] + " ";
                }
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size()-2] == ">" && persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size()-1].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size()-2; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size()-3] == ">" && persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size()-2].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size()-3; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                //persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size()-3] == ">" && persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size()-1].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size()-3; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size()-2] == ">>" && persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size()-1].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size()-2; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size()-3] == ">>" && persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size()-2].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size()-3; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                //persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size()-3] == ">>" && persistentStrings[1] != "<") {
                //string comment;
                fstream outputFile;
                outputFile.open(persistentStrings[persistentStrings.size()-1].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                for (int i = 1; i < persistentStrings.size()-3; i++) {
                    cout << persistentStrings[i] + " ";
                }
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[persistentStrings.size()-2] == "<") {
                ifstream inputFile;
                inputFile.open(persistentStrings[2].c_str(),ios::in);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = inputFile.rdbuf();
                cin.rdbuf(stream_buffer_file);
                while (getline(inputFile,line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                cin.rdbuf(stream_buffer_cin);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(),ios::in);
                outputFile.open(persistentStrings[4].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_inputFile = inputFile.rdbuf();
                streambuf* stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile,line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(),ios::in);
                outputFile.open(persistentStrings[4].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_inputFile = inputFile.rdbuf();
                streambuf* stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile,line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(),ios::in);
                outputFile.open(persistentStrings[5].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_inputFile = inputFile.rdbuf();
                streambuf* stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile,line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">>") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(),ios::in);
                outputFile.open(persistentStrings[4].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_inputFile = inputFile.rdbuf();
                streambuf* stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile,line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">>") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(),ios::in);
                outputFile.open(persistentStrings[4].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_inputFile = inputFile.rdbuf();
                streambuf* stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile,line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "echo" && persistentStrings[1] == "<" && persistentStrings[3] == ">>") {
                ifstream inputFile;
                fstream outputFile;
                inputFile.open(persistentStrings[2].c_str(),ios::in);
                outputFile.open(persistentStrings[5].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_inputFile = inputFile.rdbuf();
                streambuf* stream_buffer_outputFile = outputFile.rdbuf();
                cin.rdbuf(stream_buffer_inputFile);
                cout.rdbuf(stream_buffer_outputFile);
                while (getline(inputFile,line)) {
                    cout << line + "\n";
                }
                inputFile.close();
                outputFile.close();
                cin.rdbuf(stream_buffer_cin);
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings.size() == 1) {
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n"
                        "clr - clears the screen.\n"
                        "dir <directory> - Lists the contents of directory <directory>.\n"
                        "environ - Lists all the environment strings.\n"
                        "echo - Displays <comment> on the display followed by a new line.\n"
                        "help - Displays the user manual using the more filter.\n"
                        "pause - Pauses operation of the shell until 'Enter' is pressed.\n"
                        "quit - Quit the shell.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings.size() == 2) {
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings.size() == 2) {
                cout << "clr - clears the screen.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings.size() == 2) {
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings.size() == 2) {
                cout << "environ - Lists all the environment strings.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings.size() == 2) {
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings.size() == 2) {
                cout << "help - Displays the user manual using the more filter.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings.size() == 2) {
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                persistentStrings.clear();
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings.size() == 2) {
                cout << "quit - Quit the shell.\n";
                persistentStrings.clear();
                continue;
            }

            /**
             *
             *
             * FILE I/O REDIRECTION FOR HELP - >
             *
             *
             */

            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "cd" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "cd <directory> - Change the current default directory to <directory>."
                        "\nIf the <directory> argument is not present, reports the current directory."
                        "\nIf the directory does not exist an appropriate error will be reported."
                        "\nThis command will also change the PWD environment variable.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "clr" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "clr - clears the screen.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "dir" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "dir <directory> - Lists the contents of directory <directory>.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "environ" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "environ - Lists all the environment strings.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "echo" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "echo - Displays <comment> on the display followed by a new line.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings[2] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "help - Displays the user manual using the more filter.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "help - Displays the user manual using the more filter.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "help - Displays the user manual using the more filter.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings[2] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "help - Displays the user manual using the more filter.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "help - Displays the user manual using the more filter.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "help" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "help - Displays the user manual using the more filter.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "pause" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "pause - Pauses operation of the shell until 'Enter' is pressed.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">>" && persistentStrings.size() == 4) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[3].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            if (persistentStrings[0] == "help" && persistentStrings[1] == "quit" && persistentStrings[2] == ">>" && persistentStrings.size() == 5) {
                fstream outputFile;
                outputFile.open(persistentStrings[4].c_str(),ios::app | ios::out);
                string line;
                streambuf* stream_buffer_cout = cout.rdbuf();
                //streambuf* stream_buffer_cin = cin.rdbuf();
                streambuf* stream_buffer_file = outputFile.rdbuf();
                cout.rdbuf(stream_buffer_file);
                cout << "quit - Quit the shell.\n";
                outputFile.close();
                cout.rdbuf(stream_buffer_cout);
                continue;
            }
            else { // change else block to work with persistent strings vector instead of cmd line
                bool hasPipe = false;
                int pipeIndex = 0;
                for (int i = 0; i < persistentStrings.size(); i++) {
                    if (persistentStrings[i] == "|") {
                        hasPipe = true;
                        //pipeIndex = i;
                        //persistentStrings.erase(i);
                    }
                }
                if (hasPipe == true) {
                    char *tokens1[100];
                    char *tokens2[100];
                    char *line1 = NULL;
                    char *line2 = NULL;
                    size_t len = 0;
                    ssize_t read;
                    int pid = -1;
                    int pid2 = -1;
                    int pipe_file_descs[2];
                    printf("type first command + args here: ");
                    read = getline (&line1, &len, stdin);
                    assert(read != -1);
                    tokenize(line1, tokens1);
                    printf("type second command + args here: ");
                    read = getline (&line2, &len, stdin);
                    assert(read != -1);
                    tokenize(line2, tokens2);
                    if (pipe(pipe_file_descs) == -1){
                        perror("Failed to create pipe!");
                        exit(-1);
                    }
                    if ((pid = fork()) < 0){
                        perror("fork 1 failed!\n");
                        exit(-1);
                    }
                    if(pid == 0) {
                        close(pipe_file_descs[0]);
                        dup2(pipe_file_descs[1], 1);
                        close(pipe_file_descs[1]);
                        if (execvp(tokens1[0],tokens1) < 0){
                            perror("Could not execute command 1");
                            exit(-1);
                        }
                    }
                    if ((pid2 = fork()) < 0){
                        perror("Second fork failed");
                        exit(-1);
                    }

                    if(pid2 == 0){
                        close(pipe_file_descs[1]);
                        dup2(pipe_file_descs[0], 0);
                        close(pipe_file_descs[0]);
                        if (execvp(tokens2[0],tokens2) < 0){
                            perror("Could not execute command 2");
                            exit(-1);
                        }
                    }
                    waitpid(-1, NULL, 0);
                    //puts("both children finished!");
                    close(pipe_file_descs[0]);
                    close(pipe_file_descs[1]);
                    free(line1);
                    free(line2);
                }
                else {
                    bool ExternalInputRedirect = false;
                    bool ExternalRedirectNonAppend = false;
                    bool ExternalRedirectAppend = false;
                    bool bg = false;
                    for (int i = 0; i < persistentStrings.size(); i++) {
                        if (persistentStrings[i] == "&") {
                            persistentStrings.erase(persistentStrings.begin() + i);
                            i--;
                            bg = true;
                        }
                    }

                    // is there output redirection?
                    string intendedOutputFile;
                    for (int i =0; i < persistentStrings.size(); i++) {
                        if (persistentStrings[i] == ">") {
                            intendedOutputFile = persistentStrings[i+1];
                            ExternalRedirectNonAppend = true;
                            for (int k = i+1; k < persistentStrings.size(); k++) {
                                if (persistentStrings[k] == ">" || persistentStrings[k] == ">>") {
                                    ExternalRedirectNonAppend = false;
                                    perror("Invalid command sequence");
                                }
                            }
                            //persistentStrings.erase(persistentStrings.begin() + i);
                            for (int j = i; j < persistentStrings.size(); j++) {
                                persistentStrings.erase(persistentStrings.begin() + j);
                            }
                        }
                    }
                    for (int i =0; i < persistentStrings.size(); i++) {
                        if (persistentStrings[i] == ">>") {
                            intendedOutputFile = persistentStrings[i+1];
                            ExternalRedirectAppend = true;
                            for (int k = i+1; k < persistentStrings.size(); k++) {
                                if (persistentStrings[k] == ">" || persistentStrings[k] == ">>") {
                                    ExternalRedirectAppend = false;
                                    perror("Invalid command sequence");
                                }
                            }
                            //ExternalRedirectAppend = true;
                            //persistentStrings.erase(persistentStrings.begin() + i);
                            for (int j = i; j < persistentStrings.size(); j++) {
                                persistentStrings.erase(persistentStrings.begin() + j);
                            }
                        }
                    }

                    //is there input redirection?
                    string intendedInputFile;
                    for (int i =0; i < persistentStrings.size(); i++) {
                        if (persistentStrings[i] == "<") {
                            intendedInputFile = persistentStrings[i+1];
                            ExternalInputRedirect = true;
                            for (int k = i+1; k < persistentStrings.size(); k++) {
                                if (persistentStrings[k] == "<") {
                                    ExternalInputRedirect = false;
                                    perror("Invalid command sequence");
                                }
                            }
                            //persistentStrings.erase(persistentStrings.begin() + i);
                            for (int j = i; j < persistentStrings.size(); j++) {
                                persistentStrings.erase(persistentStrings.begin() + j);
                            }
                        }
                    }


                    char *args[persistentStrings.size()];
                    for (size_t i = 0; i < (persistentStrings.size()); ++i) {
                        args[i] = (char*)persistentStrings[i].c_str();
                    }
                    args[persistentStrings.size()] = NULL;
                    int pid = fork();
                    if (pid < 0) {
                        puts("fork failed");
                        exit(1);
                    }
                    if (pid == 0) {
                        if (ExternalInputRedirect == true) {
                            int inFile = open(intendedInputFile.c_str(), O_RDONLY);
                            dup2(inFile, 0);
                            close(inFile);
                        }
                        if (ExternalRedirectNonAppend == true) {
                            int outFile = open(intendedOutputFile.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU | S_IRWXG| S_IRWXO);
                            dup2(outFile, 1);
                            close(outFile);
                        }
                        if (ExternalRedirectAppend == true) {
                            int outFile = open(intendedOutputFile.c_str(), O_WRONLY|O_CREAT|O_APPEND, S_IRWXU | S_IRWXG| S_IRWXO);
                            dup2(outFile, 1);
                            close(outFile);
                        }
                        //puts("executing external command...");
                        //int outFile = open("output.txt", O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU | S_IRWXG| S_IRWXO);
                        //dup2(outFile, 1);
                        //close(outFile);
                        if (execvp(args[0],args) < 0) {
                            perror("Could not execute command");
                            exit(1);
                        }
                    }
                    else {
                        if (bg == false) {
                            waitpid(pid, NULL, 0);
                            //puts("done waiting!");
                        }

                    }
                    continue;
                }
            } continue;

        } exit(1);
    }
    return 0;
}