#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

class FootballPlayer
{
public:
    FootballPlayer(): efficiency_(0), index_(0) {}
    FootballPlayer(long long efficiency, int index): efficiency_(efficiency), index_(index) {}
    long long getEfficiency() const
    {
        return efficiency_;
    }
    int getIndex() const
    {
        return index_;
    }
private:
    long long efficiency_;
    int index_;
};

// compares two football players on effectivness, one player less than the second one if the first
// player's effectivness less than the second player's effectivness
bool CompareEffectiveness(const FootballPlayer& firstFootballPlayer,
    const FootballPlayer& secondFootballPlayer)
{
    return firstFootballPlayer.getEfficiency() < secondFootballPlayer.getEfficiency();
}

std::vector<FootballPlayer> ReadFootballPlayers()
{
    int numberOfFootballPlayers;
    std::cin >> numberOfFootballPlayers;
    std::vector<FootballPlayer> footballPlayers;
    for (int index = 0; index < numberOfFootballPlayers; ++index) {
        long long int efficiency;
        std::cin >> efficiency;
        footballPlayers.push_back(FootballPlayer(efficiency, index));
    }
    return footballPlayers;
}

template <class RandomAccessIterator, class Compare>
void Merge(RandomAccessIterator first, RandomAccessIterator medium,
    RandomAccessIterator last, Compare comp)
{
    std::vector<typename decltype(first)::value_type> copyVector(first, last);
    RandomAccessIterator leftIterator = copyVector.begin();
    RandomAccessIterator mediumIterator = copyVector.begin() + (medium - first);
    RandomAccessIterator rightIterator = mediumIterator + 1;
    RandomAccessIterator iteration = first;
    while (leftIterator != mediumIterator + 1 && rightIterator != copyVector.end()) {
        if (comp(*leftIterator, *rightIterator)) {
            *iteration = *leftIterator;
            ++leftIterator;
        } else {
            *iteration = *rightIterator;
            ++rightIterator;
        }
        ++iteration;
    }
    for (; leftIterator != mediumIterator + 1; ++leftIterator) {
        *iteration = *leftIterator;
        ++iteration;
    }
    for (; rightIterator != copyVector.end(); ++rightIterator) {
        *iteration = *rightIterator;
        ++iteration;
    }
}

template <class RandomAccessIterator, class Compare>
void MergeSort(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    if (last - first == 1) {
        return;
    }
    if (last - first == 2) {
        if (comp(*(first + 1), *first)) {
            std::swap(*first, *(first + 1));
        }
        return;
    }
    RandomAccessIterator medium = first + (last - first) / 2;
    MergeSort(first, medium + 1, comp);
    MergeSort(medium + 1, last, comp);
    Merge(first, medium, last, comp);
}

std::vector<FootballPlayer> FindMaxEffectiveSolidFootballTeam(
    std::vector<FootballPlayer> footballPlayers)
{
    MergeSort(footballPlayers.begin(), footballPlayers.end(), CompareEffectiveness);
    std::vector<FootballPlayer>::const_iterator leftBorderOfMaxSum = footballPlayers.begin();
    std::vector<FootballPlayer>::const_iterator rightBorderOfMaxSum = footballPlayers.begin();
    long long int maxSum = 0;
    long long int currentSum = 0;
    bool prevIsLeft = false;
    std::vector<FootballPlayer>::const_iterator leftBorder = footballPlayers.begin();
    std::vector<FootballPlayer>::const_iterator rightBorder = footballPlayers.begin();
    for (; rightBorder != footballPlayers.end();) {
        if (prevIsLeft) {
            currentSum -= (leftBorder - 1)->getEfficiency();
        } else {
            currentSum += rightBorder->getEfficiency();
        }
        if (rightBorder - leftBorder > 1) {
            if (leftBorder->getEfficiency() + (leftBorder + 1)->getEfficiency() >=
                rightBorder->getEfficiency()) {
                if (currentSum > maxSum) {
                    maxSum = currentSum;
                    leftBorderOfMaxSum = leftBorder;
                    rightBorderOfMaxSum = rightBorder;
                }
                ++rightBorder;
                prevIsLeft = false;
            } else {
                ++leftBorder;
                prevIsLeft = true;
            }
        } else {
            if (currentSum > maxSum) {
                maxSum = currentSum;
                leftBorderOfMaxSum = leftBorder;
                rightBorderOfMaxSum = rightBorder;
            }
            ++rightBorder;
            prevIsLeft = false;
        }
    }
    return std::vector<FootballPlayer>(leftBorderOfMaxSum, rightBorderOfMaxSum + 1);
}

// compares two football players on index, one player less than the second one if the first
// player's index less than the second player's index
bool CompareIndex(const FootballPlayer& firstFootballPlayer,
    const FootballPlayer& secondFootballPlayer)
{
    return firstFootballPlayer.getIndex() < secondFootballPlayer.getIndex();
}

// prints football team in ascending order of the index
void PrintFootballTeam(std::vector<FootballPlayer> footballTeam)
{
    MergeSort(footballTeam.begin(), footballTeam.end(), CompareIndex);
    for (std::vector<FootballPlayer>::iterator iterator = footballTeam.begin(); iterator !=
        footballTeam.end(); ++iterator) {
        std::cout << iterator->getIndex() + 1 << " ";
    }
    std::cout << "\n";
}

int main()
{
    PrintFootballTeam(FindMaxEffectiveSolidFootballTeam(ReadFootballPlayers()));
    return 0;
}
