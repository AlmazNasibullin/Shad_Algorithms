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
    for (int index = 1; index <= numberOfFootballPlayers; ++index) {
        long long efficiency;
        std::cin >> efficiency;
        footballPlayers.push_back(FootballPlayer(efficiency, index));
    }
    return footballPlayers;
}

template <class RandomAccessIterator, class Compare>
void Merge(RandomAccessIterator first, RandomAccessIterator medium,
    RandomAccessIterator last, Compare compare)
{
    typedef typename std::iterator_traits<RandomAccessIterator>::value_type type;
    std::vector<type> buffer(last - first);
    typename std::vector<type>::iterator bufferIterator = buffer.begin();
    RandomAccessIterator middle = medium;
    RandomAccessIterator begin = first;
    while (first != medium && middle != last) {
        if (compare(*first, *middle)) {
            *(bufferIterator++) = *(first++);
        } else {
            *(bufferIterator++) = *(middle++);
        }
    }
    std::copy(first, medium, bufferIterator);
    std::copy(middle, last, bufferIterator);
    std::copy(buffer.begin(), buffer.end(), begin);
}

template <class RandomAccessIterator, class Compare>
void MergeSort(RandomAccessIterator first, RandomAccessIterator last, Compare compare)
{
    if (last - first <= 1) {
        return;
    }
    RandomAccessIterator medium = first + (last - first) / 2;
    MergeSort(first, medium, compare);
    MergeSort(medium, last, compare);
    Merge(first, medium, last, compare);
}

std::vector<FootballPlayer> FindMaxEffectiveSolidFootballTeam(
    std::vector<FootballPlayer> footballPlayers)
{
    MergeSort(footballPlayers.begin(), footballPlayers.end(), CompareEffectivenessLess);


    struct FootballTeam {
        long long efficiency;
        std::vector<FootballPlayer>::const_iterator start;
        std::vector<FootballPlayer>::const_iterator end;
    };
    FootballTeam currentFootballTeam = {footballPlayers.begin()->getEfficiency(),
        footballPlayers.begin(), footballPlayers.begin() + 1};
    FootballTeam maxEffectiveSolidFootballTeam = currentFootballTeam;

    while (currentFootballTeam.end != footballPlayers.end()) {
        currentFootballTeam.efficiency += currentFootballTeam.end->getEfficiency();
        while (currentFootballTeam.end - currentFootballTeam.start > 1
                && currentFootballTeam.start->getEfficiency()
                + (currentFootballTeam.start + 1)->getEfficiency()
                < currentFootballTeam.end->getEfficiency()) {
            currentFootballTeam.efficiency -= currentFootballTeam.start->getEfficiency();
            ++currentFootballTeam.start;
        }
        ++currentFootballTeam.end;
        if (currentFootballTeam.efficiency > maxEffectiveSolidFootballTeam.efficiency) {
            maxEffectiveSolidFootballTeam = currentFootballTeam;
        }
    }
    return {maxEffectiveSolidFootballTeam.start, maxEffectiveSolidFootballTeam.end};
}

// prints football team in ascending order of the index
void PrintFootballTeam(const std::vector<FootballPlayer>& footballTeam)
{
    std::vector<int> indexes(footballTeam.size());
    long long efficiency = 0;
    for (size_t i = 0; i < footballTeam.size(); ++i) {
        indexes[i] = footballTeam[i].getIndex();
        efficiency += footballTeam[i].getEfficiency();
    }
    std::cout << efficiency << "\n";
    MergeSort(indexes.begin(), indexes.end(), std::less<int>());
    for (int index : indexes) {
        std::cout << index << " ";
    }
    std::cout << "\n";
}

int main()
{
    PrintFootballTeam(FindMaxEffectiveSolidFootballTeam(ReadFootballPlayers()));
    return 0;
}
