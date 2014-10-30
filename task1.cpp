#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>

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

bool CompareEffectivenessLess(const FootballPlayer& first, const FootballPlayer& second)
{
    return first.getEfficiency() < second.getEfficiency();
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
    RandomAccessIterator last, Compare compare)
{
    std::vector<typename decltype(first)::value_type> copyVector(first, last);
    RandomAccessIterator left = copyVector.begin();
    RandomAccessIterator separator = copyVector.begin() + (medium - first);
    RandomAccessIterator right = separator;
    RandomAccessIterator result = first;
    while (left != separator && right != copyVector.end()) {
        if (compare(*left, *right)) {
            *result = *left;
            ++left;
        } else {
            *result = *right;
            ++right;
        }
        ++result;
    }
    std::copy(left, separator, result);
    std::copy(right, copyVector.end(), result);
}

template <class RandomAccessIterator, class Compare>
void MergeSort(RandomAccessIterator first, RandomAccessIterator last, Compare compare)
{
    if (last - first <= 1) {
        return;
    }
    if (last - first == 2) {
        if (compare(*(first + 1), *first)) {
            std::swap(*first, *(first + 1));
        }
        return;
    }
    RandomAccessIterator medium = first + (last - first) / 2 + 1;
    MergeSort(first, medium, compare);
    MergeSort(medium, last, compare);
    Merge(first, medium, last, compare);
}

bool isSolidFrootballTeam(const std::vector<FootballPlayer>& sortedFootballPlayers,
    std::vector<FootballPlayer>::const_iterator leftBorder,
    std::vector<FootballPlayer>::const_iterator rightBorder)
{
    return (rightBorder - leftBorder > 1 && leftBorder->getEfficiency() +
        (leftBorder + 1)->getEfficiency() >= rightBorder->getEfficiency() || rightBorder -
        leftBorder <= 1);
}

std::vector<FootballPlayer> FindMaxEffectiveSolidFootballTeam(
    std::vector<FootballPlayer> footballPlayers)
{
    MergeSort(footballPlayers.begin(), footballPlayers.end(), CompareEffectivenessLess);
    struct MaxEffectiveSolidFootballTeam {
        long long int efficiency_;
        std::vector<FootballPlayer>::const_iterator start_;
        std::vector<FootballPlayer>::const_iterator end_;
    } maxEffectiveSolidFootballTeam;
    maxEffectiveSolidFootballTeam.start_ = footballPlayers.begin();
    maxEffectiveSolidFootballTeam.end_ = footballPlayers.begin();
    maxEffectiveSolidFootballTeam.efficiency_ = 0;


    std::vector<FootballPlayer>::const_iterator leftBorder = footballPlayers.begin();
    std::vector<FootballPlayer>::const_iterator rightBorder = footballPlayers.begin();
    long long int currentSumOfEfficiency = rightBorder->getEfficiency();
    for (; rightBorder != footballPlayers.end();) {
        if (currentSumOfEfficiency > maxEffectiveSolidFootballTeam.efficiency_ &&
            isSolidFrootballTeam(footballPlayers, leftBorder, rightBorder)) {
            maxEffectiveSolidFootballTeam.efficiency_ = currentSumOfEfficiency;
            maxEffectiveSolidFootballTeam.start_ = leftBorder;
            maxEffectiveSolidFootballTeam.end_ = rightBorder;
        }
        if (isSolidFrootballTeam(footballPlayers, leftBorder, rightBorder)) {
            ++rightBorder;
            if (rightBorder != footballPlayers.end()) {
                currentSumOfEfficiency += rightBorder->getEfficiency();
            }
        } else {
            currentSumOfEfficiency -= leftBorder->getEfficiency();
            ++leftBorder;
        }
    }
    return std::vector<FootballPlayer>(maxEffectiveSolidFootballTeam.start_,
        maxEffectiveSolidFootballTeam.end_ + 1);
}

// prints football team in ascending order of the index
void PrintFootballTeam(const std::vector<FootballPlayer>& footballTeam)
{
    std::vector<int> indexes(footballTeam.size());
    for (size_t iteration = 0; iteration < footballTeam.size(); ++iteration) {
        indexes[iteration] = footballTeam[iteration].getIndex();
    }
    MergeSort(indexes.begin(), indexes.end(), std::less<int>());
    for (int index : indexes) {
        std::cout << index + 1 << " ";
    }
    std::cout << "\n";
}

int main()
{
    PrintFootballTeam(FindMaxEffectiveSolidFootballTeam(ReadFootballPlayers()));
    return 0;
}
