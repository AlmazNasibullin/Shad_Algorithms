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

private:
    long long efficiency_;
    int index_;
};

bool compareFootballPlayersOnEffEctiveness(const FootballPlayer& firstFootballPlayer,
    const FootballPlayer& secondFootballPlayer);

std::vector<FootballPlayer> readFootballPlayers();

template <class RandomAccessIterator, class Compare>
void Merge(RandomAccessIterator first, RandomAccessIterator medium,
    RandomAccessIterator last, Compare comp);

template <class RandomAccessIterator, class Compare>
void Mergesort(RandomAccessIterator first, RandomAccessIterator last, Compare comp);

// Returns max effective solid football team
std::vector<FootballPlayer> FindMaxEffectiveSolidFootballTeam(
    const std::vector<FootballPlayer>& footballPlayers);

void PrintMaxEffectiveSolidFootballTeam(std::vector<FootballPlayer>& maxEffectiveSolidFootballTeam);

int main()
{
    std::vector<FootballPlayer> footballPlayers = readFootballPlayers();
    std::vector<FootballPlayer> maxEffectiveSolidFootballTeam = 
    FindMaxEffectiveSolidFootballTeam(footballPlayers);
    PrintMaxEffectiveSolidFootballTeam(maxEffectiveSolidFootballTeam);
    return 0;
}
