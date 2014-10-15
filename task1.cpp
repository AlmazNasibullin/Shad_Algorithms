#include <iostream>
#include <vector>
#include <algorithm>

class FootballPlayer
{
public:
    FootballPlayer();
    FootballPlayer(long long efficiency, int index);
    long long getEfficiency() const;
    int getIndex() const;
    bool operator < (const FootballPlayer& footballPlayer) const;

private:
    long long efficiency_;
    int index_;
};

bool compareFootballPlayers(const FootballPlayer& firstFootballPlayer,
    const FootballPlayer& secondFootballPlayer);

std::vector<FootballPlayer> readFootballPlayers();

template <class RandomAccessIterator, class Compare>
void Merge(RandomAccessIterator first, RandomAccessIterator medium,
    RandomAccessIterator last, Compare comp);

template <class RandomAccessIterator, class Compare>
void Mergesort(RandomAccessIterator first, RandomAccessIterator last, Compare comp);

// Returns indexes of max effective solid football team and its efficiency
void FindMaxEffectiveSolidFootballTeam(const std::vector<FootballPlayer>& footballPlayers,
    std::vector<int>* indexes, int* efficiency);

void PrintMaxEffectiveSolidFootballTeam(const std::vector<int>& indexes, int efficiency);

int main()
{
    std::vector<FootballPlayer> footballPlayers = readFootballPlayers();
    std::vector<int> indexes;
    int efficiency;
    FindMaxEffectiveSolidFootballTeam(footballPlayers, &indexes, &efficiency);
    Mergesort(indexes.begin(), indexes.end(), std::less<int>());
    PrintMaxEffectiveSolidFootballTeam(indexes, efficiency);
    return 0;
}
