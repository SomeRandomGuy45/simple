#include "simple.h"
#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
// Handle CTRL+C in Windows
static BOOL WINAPI console_ctrl_handler(DWORD dwCtrlType) {
    switch (dwCtrlType) {
        case CTRL_C_EVENT:
            std::cout << "^C" << std::endl;
            return TRUE;
        case CTRL_BREAK_EVENT:
            std::cout << "^Break" << std::endl;
            return TRUE;
        case CTRL_CLOSE_EVENT:
            return TRUE;
        case CTRL_LOGOFF_EVENT:
            return TRUE;
        case CTRL_SHUTDOWN_EVENT:
            return TRUE;
    }
    return FALSE;
}

static std::string getInput() {
    std::string input;
    std::cout << "> ";

    while (true) {
        if (_kbhit()) { // Check if a key has been pressed
            char ch = _getch(); // Get the character without waiting for enter

            switch (ch) {
                case '\r': // If Enter key is pressed
                    std::cout << std::endl;
                    return input;

                case 8: // Backspace character
                    if (!input.empty()) {
                        input.pop_back();
                        std::cout << "\b \b";
                    }
                    break;

                case 3: // Ctrl+C
                    std::cout << "^C" << std::endl;
                    break;

                case 4: // Ctrl+D
                    std::cout << "^D" << std::endl;
                    return "^D"; // Simulate EOF on Windows

                case 32: // Space
                    input += ' ';
                    std::cout << ' ';
                    break;

                default:
                    if (ch < 32) { // Control characters are typically less than 32 in ASCII
                        std::cout << "^" << static_cast<char>(ch + 'A' - 1); // Convert control characters to '^X' form
                    } else {
                        input += ch; // Append the character to input string
                        std::cout << ch; // Echo the character
                    }
                    break;
            }
        }
        Sleep(50);
    }

    return input;
}


#else
#include <unistd.h>
#include <limits.h>
#include <termios.h>

// Function to read a single character
char getKey() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

static std::string getInput() {
    std::string input;
    std::cout << "> ";

    while (true) {
        char ch = getKey(); // Get the character without waiting for enter

        switch (ch) {
            case '\n': // If Enter key is pressed
                std::cout << std::endl;
                return input;

            case 8: // Backspace character
                if (!input.empty()) {
                    input.pop_back();
                    std::cout << "\b \b";
                }
                break;

            case 3: // Ctrl+C
                std::cout << "^C" << std::endl;
                break;

            case 4: // Ctrl+D
                std::cout << "^D" << std::endl;
                return ""; // Simulate EOF

            default:
                if (ch < 32) { // Handle control characters
                    std::cout << "^" << static_cast<char>(ch + 'A' - 1);
                } else {
                    input += ch;
                    std::cout << ch;
                }
                break;
        }
        usleep(50000);
    }

    return input;
}

void catchSignal(int sigNumber) {
    return;
}
#endif

std::wstring StringToWString(const std::string& str) {
    size_t size_needed = mbstowcs(nullptr, str.c_str(), 0);
    std::vector<wchar_t> buffer(size_needed + 1, L'\0'); // +1 for null terminator
    mbstowcs(buffer.data(), str.c_str(), size_needed);
    return std::wstring(buffer.data());
}

static void clearFile(const std::string& tempPath) {
    std::ofstream file(tempPath, std::ios::trunc);
    file.close();
}

static bool runArgs(const std::string& arg, const std::string& command, const std::string& commandBefore) {
    if (arg == "--help" || arg == "-h" || arg == "-?") {
        #ifdef _WIN32
        std::wstring name = L"Simple compiler " + std::wstring(StringToWString(SIMPLE_FULL_VERSION)) +
                            L"\nUsage: simple [options] [script.simple]\nOptions:\n" +
                            L"  -h, --help, -?     Display this help message\n" +
                            L"  -v, --version      Display the version of Simple compiler\n" +
                            L"  -o, --output       Build's a .simple file and does not run it\n";
        MessageBoxW(NULL, name.c_str(), L"Help", MB_OK | MB_ICONQUESTION);
        #endif
        std::cout << "Simple compiler " << SIMPLE_FULL_VERSION << "\n";
        std::cout << "Usage: simple [options] [script.simple]\n";
        std::cout << "Options:\n";
        std::cout << "  -h, --help, -?     Display this help message\n";
        std::cout << "  -v, --version      Display the version of Simple compiler\n";
        std::cout << "  -o, --output       Build's a .simple file and does not run it\n";
        return true;
    } else if (arg == "-v" || arg == "--version") {
        std::cout << "Simple compiler " << SIMPLE_FULL_VERSION << "\n";
        return true;
    } else if (arg == "-o" || arg == "--output") {
        Token* token = new Token(commandBefore, command, true);
        try {
            token->StartReadingFile();
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        } catch (...) {
            std::cerr << "Unknown error occurred!\n";
        }
        delete token;
        return true;
    }
    return false;
}

static bool allArgs(int argc, char** argv) {
    bool KILL = false;
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {  // Ensure i starts from 1 to avoid invalid `argv[i-1]`
            bool r_val = false;
            std::string argBefore = (i > 0) ? argv[i - 1] : "";  // Check bounds
            std::string argAfter = (i < argc - 1) ? argv[i + 1] : ""; // Check bounds
            r_val = runArgs(std::string(argv[i]), argAfter, argBefore);
            if (r_val) {
                KILL = true;
            }
        }
    }
    return KILL;
}

int main(int argc, char** argv) {
    bool shouldKill = allArgs(argc, argv);
    if (shouldKill) {
        return EXIT_SUCCESS;
    }

    #ifdef _WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)console_ctrl_handler, TRUE);
    #else
    signal(SIGINT, catchSignal);
    signal(SIGTERM, catchSignal);
    #endif

    std::srand(static_cast<unsigned int>(std::time(0)));
    std::filesystem::path tempPath = std::filesystem::temp_directory_path() / ".simple";

    Token* token = new Token(tempPath.string());

    if (argc >= 2) {
        try {
            token->ReOpenFile(argv[1]);
            token->StartReadingFile();
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
            delete token;
            return EXIT_FAILURE;
        } catch (...) {
            std::cerr << "Unknown error occurred!\n";
            delete token;
            return EXIT_FAILURE;
        }
        delete token;
        return EXIT_SUCCESS;
    }

    std::ofstream outputFile(tempPath, std::ios::in | std::ios::out | std::ios::trunc);

    std::cout << "Simple compiler " << SIMPLE_FULL_VERSION << " Arg count is: " << std::to_string(argc) << "\n";

    while (true) {
        outputFile.seekp(0);
        outputFile << "";
        clearFile(tempPath.string());
        std::string newLine = getInput();

        if (std::cin.eof()) {
            break;
        }

        if (newLine.empty()) continue;
        
        if (newLine == "exit()" || newLine == "quit()" || newLine == "^D") {
            break;
        }

        if (newLine == "clear()") {
            std::cout << "\033[2J\033[1;1H";
            std::cout << "Simple compiler " << SIMPLE_FULL_VERSION << " Arg count is: " << std::to_string(argc) << "\n";
        }

        if (newLine == "clear") {
            std::cout << "[HINT] Use clear() to clear the terminal.\n";
        }

        if (newLine == "exit" || newLine == ":q" || newLine == "quit") {
            std::cout << "[HINT] Use exit(), quit() or CTRL+D (EOF) to exit the program.\n";
        }

        outputFile << newLine << std::endl;

        delete token; // Ensure proper cleanup
        token = new Token(tempPath.string());

        try {
            token->StartReadingFile();
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        } catch (...) {
            std::cerr << "Unknown error occurred!\n";
        }
    }

    delete token; // Final cleanup
    return EXIT_SUCCESS;
}