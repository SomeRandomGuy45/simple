#include "helper.h"

std::vector<std::string> libLocations = {
    LIBPATH,
};

#ifdef __cplusplus
extern "C" {
#endif

//These 2 functions helps out with removeWhitespace
static std::string trimLeft(const std::string& str) {
	size_t start = str.find_first_not_of(" \t\n\r\f\v");
	return (start == std::string::npos) ? "" : str.substr(start);
}

static std::string trimRight(const std::string& str) {
	size_t end = str.find_last_not_of(" \t\n\r\f\v");
	return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

//This function helps remove white space with the arg NO_SPACE
//If the arg NO_SPACE is set to true then it will remove ALL SPACES
//If not then it would remove the front and back spaces
std::string removeWhitespace(std::string str, bool NO_SPACE)
{
	if (NO_SPACE)
	{
		std::string result;
		std::copy_if(str.begin(), str.end(), std::back_inserter(result),
			[](unsigned char c) { return !std::isspace(c); });
		return result;
	}
	return trimRight(trimLeft(str));
}
//This function creates a random file name for std::filesystem::temp_directory_path()

std::string getRandomFileName() {
    // Get the current time
    std::time_t t = std::time(nullptr);
    std::tm now;

#if defined(_MSC_VER) // Check if we are using MSVC
    localtime_s(&now, &t); // Safe version for MSVC
#else
    std::tm* temp = std::localtime(&t); // For other compilers
    now = *temp; // Copy to a local tm object
#endif

    // Create a stringstream to format the file name
    std::string ss;

    size_t len = 0;

    // Format the date as MM-DD-YY
    ss += "threadsimplebytecode";

    for (const auto& path : std::filesystem::directory_iterator(std::filesystem::temp_directory_path()))
    {
        if (path.is_regular_file())
        {
            std::string fileName = path.path().filename().string();
            if (fileName.substr(0, 20) == "threadsimplebytecode" && path.path().extension() == ".sbcc")
            {
                len++;
            }
        }
    }
    ss += "-" + std::to_string(len);

    // Add the file extension
    ss += ".sbcc";

    return ss;
}

std::string expandHomeDirectory(const std::string& path) {
    if (!path.empty() && path[0] == '~') {
        const char* home = getenv("HOME");
        if (home) {
            return std::string(home) + path.substr(1);
        } else {
            std::cerr << "Error: HOME environment variable not set." << std::endl;
            return path; // Return the original path as a fallback
        }
    }
    return path; // Return unchanged if not starting with '~'
}

std::string removeComments(const std::string& input) {
    std::string result;
    std::istringstream stream(input);
    std::string line;
    while (std::getline(stream, line)) {
        // Ignore everything after "//"
        size_t commentPos = line.find("//");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        result += line + "\n";
    }
    return result;
}

#ifdef __cplusplus
}
#endif