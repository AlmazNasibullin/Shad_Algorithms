#include <iostream>
#include <string>
#include <vector>

class Date
{
public:
    Date() : day_(1), month_(1), year_(2014) {}
    Date(int day, int month, int year) : day_(day), month_(month), year_(year) {}
    int getDay() const
    {
        return day_;
    }
    int getMonth() const
    {
        return month_;
    }
    int getYear() const
    {
        return year_;
    }
    void print() const
    {
        std::cout << day_ << "." << month_ << "." << year_; 
    }

private:
    int day_;
    int month_;
    int year_;
};

bool isYearLeap(int year)
{
    if (year % 4 == 0 and year % 100 != 0 or year % 400 == 0) {
        return true;
    }
    return false;
}

bool isDataCorrect(const Date& date)
{
    if (date.getYear() < 0) {
        date.print();
        std::cout << " Incorrect date: " << date.getYear() << " is a negative number\n";
        return false;
    }
    if (date.getYear() <= 1583) {
        date.print();
        std::cout << " Incorrect date: year must be more than 1583\n"; 
        return false;
    }
    if (date.getMonth() < 1 || date.getMonth() > 12) {
        date.print();
        std::cout << " Incorrect date: month must be between 1 to 12\n";
        return false;
    }
    int numberOfDaysInMonths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (isYearLeap(date.getYear())) {
        numberOfDaysInMonths[1] = 29;
    }
    std::string months[] = {"January", "February", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December"};
    if (date.getDay() < 1 || date.getDay() > numberOfDaysInMonths[date.getMonth() - 1]) {
        date.print();
        std::cout << " Incorrect date: day must be between 1 to " <<
        numberOfDaysInMonths[date.getMonth() - 1] << " for " << months[date.getMonth() - 1];
        if(date.getMonth() == 2) {
            std::cout << " in " << date.getYear();
        }
        std::cout << "\n";
        return false;
    }
    return true;
}

std::string whatDay(const Date& date)
{
    std::string days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
    "Saturday"};
    int isJanuaryOrFebruary = (14 - date.getMonth()) / 12;
    int offsetYear = date.getYear() - isJanuaryOrFebruary;
    int offsetMonth = date.getMonth() + isJanuaryOrFebruary * 12 - 2;
    return days[(7000 + (date.getDay() + offsetYear + offsetYear / 4 - offsetYear / 100 +
        offsetYear / 400 + (31 * offsetMonth) / 12)) % 7];
}

void exampleOfWhatDay()
{
    // incorrect dates
    std::vector<Date> incorrectDates = {Date(1, 1, -100), Date(1, 1, 1583), Date(1, 13, 2014),
        Date(29, 2, 2014)};
    for (Date incorrectDate : incorrectDates) {
        if (isDataCorrect(incorrectDate)) {
            incorrectDate.print();
            std::cout << " is " << whatDay(incorrectDate) << "\n";
        }
    }
    std::cout << "\n";
    // correct dates
    std::vector<Date> correctDates = {Date(1, 1, 2014), Date(10, 10, 2010), Date(11, 11, 1911),
        Date(29, 2, 2012), Date(19, 10, 2014)};
    for (Date correctDate : correctDates) {
        if (isDataCorrect(correctDate)) {
            correctDate.print();
            std::cout << " is " << whatDay(correctDate) << "\n";
        }
    }
}

void exampleOfIsYearLeap()
{
    std::vector<int> years = {2014, 400, 404, 700};
    for (int year : years) {
        std::cout << year << " ";
        if (isYearLeap(year)) {
            std::cout << " is leap\n";
        } else {
            std::cout << " isn't leap\n";
        }
    }
}

int main()
{
    exampleOfWhatDay();
    std::cout << "\n";
    exampleOfIsYearLeap();
    return 0;
}