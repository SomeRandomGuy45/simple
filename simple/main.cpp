#include "simple.h"

#ifdef _WIN32
#include <windows.h>
#endif

static std::string getInput()
{
	std::string input;
	std::cout << "> ";
	std::getline(std::cin, input);
	return input;
}

std::wstring StringToWString(const std::string& str) {
    // Calculate the required buffer size
    size_t size_needed = mbstowcs(nullptr, str.c_str(), 0);
    std::vector<wchar_t> buffer(size_needed + 1, L'\0'); // +1 for null terminator

    // Convert to wide string
    mbstowcs(buffer.data(), str.c_str(), size_needed);
    return std::wstring(buffer.data());
}

static void clearFile(const std::string& tempPath)
{
    std::ofstream file(tempPath, std::ios::trunc);
    file.close();
}

static bool runArgs(const std::string& arg)
{
    if (arg == "--help" || arg == "-h" || arg == "-?")
    {
        #ifdef _WIN32
         std::wstring name = L"Simple compiler " + std::wstring(StringToWString(std::string(SIMPLE_FULL_VERSION))) + 
                        L"\n" + 
                        L"Usage: simple [options] [script.simple]\n" + 
                        L"Options:\n" + 
                        L"  -h, --help, -?     Display this help message\n" + 
                        L"  -v, --version      Display the version of Simple compiler\n";
        MessageBoxW(NULL, name.c_str(), L"Help", MB_OK | MB_ICONQUESTION);
        #else
        std::cout << "Simple compiler " << SIMPLE_FULL_VERSION << "\n";
        std::cout << "Usage: simple [options] [script.simple]\n";
        std::cout << "Options:\n";
        std::cout << "  -h, --help, -?     Display this help message\n";
        std::cout << "  -v, --version      Display the version of Simple compiler\n";
        #endif
        return true;
    }
    else if (arg == "-v" || arg == "--version")
    {
        std::cout << "Simple compiler " << SIMPLE_FULL_VERSION << "\n";
        return true;
    }
    return false;
}

static bool allArgs(int argc, char** argv)
{
    bool KILL = false;
    if (argc >= 2)
    {
        for (int i = 0; i < argc; i++)
        {
            bool r_val = runArgs(std::string(argv[i]));
            if (r_val)
            {
                KILL = true;
            }
        }
    }
    return KILL;
}

int main(int argc, char** argv)
{
    bool shouldKill = allArgs(argc, argv);
    if (shouldKill)
    {
        return EXIT_SUCCESS;
    }
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Create a temporary file path
    std::filesystem::path tempPath = std::filesystem::temp_directory_path() / ".simple";
    std::ofstream outputFile(tempPath, std::ios::in | std::ios::out | std::ios::trunc);

    Token* token = new Token(tempPath.string());
    
    if (argc >= 2) {
        try {
            // Reopen the file to the 2nd arg since we are going to compile it now
            token->ReOpenFile(argv[1]);
            token->StartReadingFile();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
            delete token; // Clean up
            return EXIT_FAILURE;
        }
        catch (...) {
            std::cerr << "Unknown error occurred.\n";
            delete token; // Clean up
            return EXIT_FAILURE;
        }
        delete token;
        return EXIT_SUCCESS;
    }
    std::cout << "Simple compiler " << SIMPLE_FULL_VERSION << " Arg count is: " << std::to_string(argc) << "\n";
    while (true)
    {
        outputFile.seekp(0);
        outputFile << "";
        clearFile(tempPath.string());
        std::string newLine = getInput();
        if (newLine.empty())
        {
            continue;
        }
        if (newLine == "exit()" || newLine == "exit" || newLine == ":q" || newLine == "quit" || newLine == "quit()")
        {
            break;
        }
        outputFile << newLine << std::endl;
        delete token;
        token = new Token(tempPath.string());
        try {
            token->StartReadingFile();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
        catch (...) {
            std::cerr << "Unknown error occurred.\n";
        }
    }
    delete token; // Free allocated memory
    return EXIT_SUCCESS;
}