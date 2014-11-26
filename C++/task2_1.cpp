#include <iostream>
#include <string>
#include <vector>

std::string removeCommentsNoBackSlash(const std::string& input, bool& stillComment,
    std::string::size_type currentPosition)
{
    std::string result = "";
    std::string::size_type twoSlash = input.find("//", currentPosition);
    std::string::size_type slash = input.find("/*", currentPosition);

    while (twoSlash != std::string::npos && slash != std::string::npos) {
        if (twoSlash < slash) {
            result += input.substr(currentPosition, twoSlash - currentPosition);
            stillComment = false;
            return result;
        } else {
            result += input.substr(currentPosition, slash - currentPosition);
            std::string::size_type backSlash = input.find("*/", slash + 2);
            if (backSlash == std::string::npos) {
                stillComment = true;
                return result;
            } else {
                currentPosition = backSlash + 2;
            }
        }
        twoSlash = input.find("//", currentPosition);
        slash = input.find("/*", currentPosition);
    }


    if (twoSlash != std::string::npos) {
        result += input.substr(currentPosition, twoSlash - currentPosition);
        stillComment = false;
        return result;
    }
    if (slash != std::string::npos) {
        result += input.substr(currentPosition, slash - currentPosition);
        std::string::size_type backSlash = input.find("*/", slash);
        while (backSlash != std::string::npos) {
            currentPosition = backSlash + 2;
            slash = input.find("/*", currentPosition);
            if (slash == std::string::npos) {
                result += input.substr(currentPosition);
                stillComment = false;
                return result;
            }
            result += input.substr(currentPosition, slash - currentPosition);
        }
        stillComment = true;
        return result;
    }
    stillComment = false;
    result += input.substr(currentPosition);
    return result;
}

std::string removeComments(const std::string& input, bool& stillComment)
{
    if (stillComment) {
        std::string::size_type currentPosition = 0;
        std::string::size_type backSlash = input.find("*/", currentPosition);
        if (backSlash == std::string::npos) {
            return "";
        }
        currentPosition = backSlash + 2;
        return removeCommentsNoBackSlash(input, stillComment, currentPosition);
    } else {
        return removeCommentsNoBackSlash(input, stillComment, 0);
    }
}

int main()
{
    std::vector<std::string> result;
    std::string input;
    bool stillComment = false;
    while (std::getline(std::cin, input)) {
        if (input == "") {
            result.push_back("");
        } else {
            std::string noComments = removeComments(input, stillComment);
            if (noComments != "") {
                result.push_back(noComments);
            }
        }
    }
    std::cout << "\n";
    for (std::string& string : result) {
        std::cout << string << "\n";
    }
    return 0;
}
