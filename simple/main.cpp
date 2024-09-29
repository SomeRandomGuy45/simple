#include "simple.h"

static std::string getInput()
{
	std::string input;
	std::cout << "> ";
	std::getline(std::cin, input);
	return input;
}

int main(int argc, char** argv)
{
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Create a temporary file path
    std::filesystem::path tempPath = std::filesystem::temp_directory_path() / ".simple";
    std::ofstream outputFile(tempPath, std::ios::in | std::ios::out | std::ios::trunc);
    std::cout << "[MAIN] Created file with path " << tempPath << "\n";

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
    }
    std::cout << "Simple compiler " << SIMPLE_FULL_VERSION << " Arg count is: " << std::to_string(argc) << "\n";
    while (true)
    {
        outputFile.seekp(0);
        outputFile << "";
        std::string newLine = getInput();
        if (newLine.empty())
        {
            continue;
        }
        outputFile << newLine << std::endl;
        try {
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
    }
    delete token; // Free allocated memory
    return EXIT_SUCCESS;
}