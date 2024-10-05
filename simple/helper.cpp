#include "helper.h"

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
    std::stringstream ss;

    // Format the date as MM-DD-YY
    ss << "SIMPLE-COMPILE-"
        << std::setfill('0') << std::setw(2) << (now.tm_mon + 1) << "-"
        << std::setfill('0') << std::setw(2) << now.tm_mday << "-"
        << std::setfill('0') << std::setw(2) << (now.tm_year % 100);

    // Append a random number for uniqueness
    int randomNum = std::rand() % 10000;
    ss << "-" << std::setfill('0') << std::setw(4) << randomNum;

    // Add the file extension
    ss << ".sbcc";

    return ss.str();
}

#ifdef __cplusplus
}
#endif