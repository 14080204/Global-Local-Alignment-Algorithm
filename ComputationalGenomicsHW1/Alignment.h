#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <tuple>
#include <ctime>

using namespace std;

// Used for Dynamic Programming implementation.
struct DP_cell {
	//int score;
	int substitutionScore;
	int deletionScore;
	int insertionScore;
};

class Alignment {
private:

public:
	string s1Name;
	string s2Name;
	string s1;
	string s2;
	string s1Result;
	string s2Result;
	string gapsAndMatches;
	int m, n;
	int match;	// Score
	int mismatch;	// Penalty
	int h;	// Gap opening penalty
	int g;	// Gap extension penalty
	vector<vector<DP_cell>> T;
	int optimalScore;
	int numMatches, numMismatches, numGaps, numOpeningGaps;
	string inputFileName;

	Alignment(char* inputFile, char* configFile);
	void SetParameters(char* configFile);
	void Init();
	void ComputeAlignment();
	int SubstitutionScore(char a, char b);
};

// Using Needleman-Wunsch Algorithm.
class GlobalAlignment :Alignment {
private:

public:
	GlobalAlignment(char* inputFile, char* configFile);
	int MaxSubstitution(int i, int j);
	int MaxDeletion(int i, int j);
	int MaxInsertion(int i, int j);
	int Max(int m, int n);
	void Init();
	void ComputeAlignment();
	void Retrace();
	void Output();
};

// Using Smith-Waterman Algorithm.
class LocalAlignment :Alignment {
private:
	tuple<int, int> maxScoreIndex;

public:
	LocalAlignment(char* inputFile, char* configFile);
	void Init();
	void ComputeAlignment();
	int Max(int m, int n);
	int MaxSubstitution(int i, int j);
	int MaxDeletion(int i, int j);
	int MaxInsertion(int i, int j);
	void Retrace();
	void Output();
};

