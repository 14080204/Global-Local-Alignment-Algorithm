#include "pch.h"
#include "Alignment.h"

Alignment::Alignment(char* inputFile, char* configFile) {
	inputFileName = inputFile;
	// Parse input file to get access to strings 1 and 2.
	ifstream inFile(inputFile);
	if (inFile.is_open()) {
		string line;
		getline(inFile, line);
		int pos = line.find(' ', 0);
		s1Name = line.substr(1, pos);
		getline(inFile, line);
		while (line[0] != '>') {
			s1.append(line);
			getline(inFile, line);
		}
		pos = line.find(' ', 0);
		s2Name = line.substr(1, pos);

		while (getline(inFile, line)) {
			s2.append(line);
		}
	}
	else {
		cout << "Input file could not be read!" << endl;
	}
	inFile.close();

	m = s1.length();//get number of m
	n = s2.length();//get number of n

	cout << "\n\tRead from inputFile\ns1: " << s1Name << "\n\ns2: " << s2Name << endl << endl;
	cout << "s1 length: " << m << "\ns2 length: " << n << endl << endl;
	
	SetParameters(configFile);// Read parameters from config file.

	
	T = vector<vector<DP_cell>>(m + 1);
	for (int i = 0; i < m + 1; i++) {
		T[i].resize(n + 1);
	} //Creating table with m rows n column
}

GlobalAlignment::GlobalAlignment(char* inputFile, char* configFile) : Alignment(inputFile, configFile) {}

LocalAlignment::LocalAlignment(char* inputFile, char* configFile) : Alignment(inputFile, configFile) {}

void Alignment::SetParameters(char* configFile) {
	ifstream file(configFile);
	if (file.is_open()) {
		string temp;

		file >> temp;
		file >> match;
		file >> temp;
		file >> mismatch;
		file >> temp;
		file >> h;
		file >> temp;
		file >> g;

		file.close();
	}

	/*cout << "\tRead parameters:\nMatch: " << match << "\nMismatch: " << mismatch
		<< "\nh: " << h << "\ng: " << g << endl;*/
}

void Alignment::Init() { /*Left for subclasses.*/ }

void GlobalAlignment::Init() {
	T[0][0].substitutionScore = 0;
	T[0][0].deletionScore = INT_MIN / 4;
	T[0][0].insertionScore = INT_MIN / 4;

	for (int i = 1; i < m + 1; i++) {
		T[i][0].substitutionScore = INT_MIN / 4;
		T[i][0].deletionScore = h + (i * g);
		T[i][0].insertionScore = INT_MIN / 4;
	}

	for (int j = 1; j < n + 1; j++) {
		T[0][j].substitutionScore = INT_MIN / 4;
		T[0][j].deletionScore = INT_MIN / 4;
		T[0][j].insertionScore = h + (j * g);
	}
}//initialize values in the table (according to Feb/1 class)

void LocalAlignment::Init()
{
	T[0][0].substitutionScore = 0;
	T[0][0].deletionScore = 0;
	T[0][0].insertionScore = 0;

	for (int i = 1; i < m + 1; i++) {
		T[i][0].substitutionScore = 0;
		T[i][0].deletionScore = h + (i * g);
		T[i][0].insertionScore = 0;
	}

	for (int j = 1; j < n + 1; j++) {
		T[0][j].substitutionScore = 0;
		T[0][j].deletionScore = 0;
		T[0][j].insertionScore = h + (j * g);
	}
}//initialize values in the table (according to Feb/1 class)

int Alignment::SubstitutionScore(char a, char b)
{
	if (a == b)
		return match;
	return mismatch;
}

int GlobalAlignment::MaxSubstitution(int i, int j)
{
	DP_cell cell = T[i][j];
	int subScore = cell.substitutionScore;
	int delScore = cell.deletionScore;
	int insScore = cell.insertionScore;

	int max = subScore < delScore ? delScore : subScore;
	max = max < insScore ? insScore : max;
	return max + SubstitutionScore(s1[i], s2[j]);
}

int GlobalAlignment::MaxDeletion(int i, int j)
{
	int d, s, in;
	DP_cell cell = T[i][j];
	d = cell.deletionScore + g;
	s = cell.substitutionScore + h + g;
	in = cell.insertionScore + h + g;

	int max = d < s ? s : d;
	max = max < in ? in : max;
	return max;
}

int GlobalAlignment::MaxInsertion(int i, int j)
{
	int d, s, in;
	DP_cell cell = T[i][j];
	d = cell.deletionScore + h + g;
	s = cell.substitutionScore + h + g;
	in = cell.insertionScore + g;

	int max = d < s ? s : d;
	max = max < in ? in : max;
	return max;
}

int GlobalAlignment::Max(int m, int n)
{
	DP_cell cell = T[m][n];

	int max = cell.substitutionScore < cell.deletionScore ? cell.deletionScore : cell.substitutionScore;
	max = max < cell.insertionScore ? cell.insertionScore : max;
	return max;
}
//
int LocalAlignment::Max(int m, int n)
{
	DP_cell cell = T[m][n];

	int max = cell.substitutionScore < cell.deletionScore ? cell.deletionScore : cell.substitutionScore;
	max = max < cell.insertionScore ? cell.insertionScore : max;
	max = max < 0 ? 0 : max;
	return max;
}
//
int LocalAlignment::MaxSubstitution(int i, int j)
{
	DP_cell cell = T[i][j];

	int max = cell.substitutionScore < cell.deletionScore ? cell.deletionScore : cell.substitutionScore;
	max = max < cell.insertionScore ? cell.insertionScore : max;
	max = max < 0 ? 0 : max;
	return max + SubstitutionScore(s1[i], s2[j]);
}

int LocalAlignment::MaxDeletion(int i, int j)
{
	int d, s, in;
	DP_cell cell = T[i][j];

	d = cell.deletionScore + g;
	s = cell.substitutionScore + h + g;
	in = cell.insertionScore + h + g;

	int max = d < s ? s : d;
	max = max < in ? in : max;
	max = max < 0 ? 0 : max;
	return max;
}

int LocalAlignment::MaxInsertion(int i, int j)
{
	int d, s, in;
	DP_cell cell = T[i][j];

	d = cell.deletionScore + h + g;
	s = cell.substitutionScore + h + g;
	in = cell.insertionScore + g;

	int max = d < s ? s : d;
	max = max < in ? in : max;
	max = max < 0 ? 0 : max;
	return max;
}

void Alignment::ComputeAlignment() {
	cout << "\nGood job initializing everything! Create an instance of the Global- or LocalAlignment class to compute the respective alignment." << endl;
}

void GlobalAlignment::ComputeAlignment()
{
	Init();

	cout << "\nComputing Global Alignment" << endl;

	for (int i = 1; i < m + 1; i++) {
		for (int j = 1; j < n + 1; j++) {
			T[i][j].substitutionScore = MaxSubstitution(i - 1, j - 1);
			T[i][j].deletionScore = MaxDeletion(i - 1, j);
			T[i][j].insertionScore = MaxInsertion(i, j - 1);
		}
	}
	optimalScore = Max(m, n);
	cout << "Finished. \tOptimal Score: " << optimalScore << endl;
}

void LocalAlignment::ComputeAlignment()
{
	Init();

	cout << "\nComputing Local Alignment" << endl;

	int maxScoreSoFar = 0;

	for (int i = 1; i < m + 1; i++) {
		for (int j = 1; j < n + 1; j++) {
			T[i][j].substitutionScore = MaxSubstitution(i - 1, j - 1);
			T[i][j].deletionScore = MaxDeletion(i - 1, j);
			T[i][j].insertionScore = MaxInsertion(i, j - 1);
			if (maxScoreSoFar <= T[i][j].substitutionScore) {
				maxScoreSoFar = T[i][j].substitutionScore;
				maxScoreIndex = tuple<int, int>(i, j);
			}
			if (maxScoreSoFar <= T[i][j].deletionScore) {
				maxScoreSoFar = T[i][j].deletionScore;
				maxScoreIndex = tuple<int, int>(i, j);
			}
			if (maxScoreSoFar <= T[i][j].insertionScore) {
				maxScoreSoFar = T[i][j].insertionScore;
				maxScoreIndex = tuple<int, int>(i, j);
			}
		}
	}

	optimalScore = maxScoreSoFar;
	cout << "Finished. \tOptimal Score: " << optimalScore << endl;
}

void GlobalAlignment::Retrace()
{
	int i = m;
	int j = n;
	bool openingGapIns = true;
	bool openingGapDel = true;
	numMatches = 0;
	numMismatches = 0;
	numOpeningGaps = 0;
	numGaps = 0;

	while (i >= 0 && j >= 0) {
		if (T[i][j].substitutionScore >= T[i][j].insertionScore && T[i][j].substitutionScore >= T[i][j].deletionScore) {
			s1Result.push_back(s1[i]);
			s2Result.push_back(s2[j]);
			gapsAndMatches.push_back('|');

			if (s1[i] == s2[j])
				numMatches++;
			else
				numMismatches++;

			openingGapIns = true;
			openingGapDel = true;
			i--;
			j--;
		}
		else if (T[i][j].deletionScore >= T[i][j].insertionScore && T[i][j].deletionScore >= T[i][j].substitutionScore) {
			s1Result.push_back(s1[i]);
			s2Result.push_back('-');
			gapsAndMatches.push_back(' ');
			if (openingGapDel) {
				numOpeningGaps++;
				openingGapDel = false;
			}
			openingGapIns = true;
			numGaps++;
			i--;
		}
		else {
			s1Result.push_back('-');
			s2Result.push_back(s2[j]);
			gapsAndMatches.push_back(' ');
			if (openingGapIns) {
				numOpeningGaps++;
				openingGapIns = false;
			}
			openingGapDel = false;
			numGaps++;
			j--;
		}
	}

	s1Result = s1Result.substr(1, s1Result.length());
	s2Result = s2Result.substr(1, s2Result.length());
	gapsAndMatches = gapsAndMatches.substr(1, gapsAndMatches.length());

	// Reverse strings.
	std::reverse(s1Result.begin(), s1Result.end());
	std::reverse(s2Result.begin(), s2Result.end());
	std::reverse(gapsAndMatches.begin(), gapsAndMatches.end());

	Output();
}

void LocalAlignment::Retrace()
{
	int i = get<0>(maxScoreIndex)-1;
	int j = get<1>(maxScoreIndex)-1;
	bool continueFlag = true;
	bool openingGapIns = true;
	bool openingGapDel = true;
	numMatches = 0;
	numMismatches = 0;
	numOpeningGaps = 0;
	numGaps = 0;
	cout << "I:" << i+1 << endl;
	cout << "J:" << j+1 << endl;
	cout <<"last one is:" <<s1[i] << endl;
	for (int i = 0; i < s1.length(); i++) {
		cout << s1[i];
	}
	cout << "---------------------------------" << endl;
	while (i >= 0 && j >= 0 && continueFlag) {
		if (T[i][j].substitutionScore >= T[i][j].insertionScore && T[i][j].substitutionScore >= T[i][j].deletionScore) {
			s1Result.push_back(s1[i]);
			s2Result.push_back(s2[j]);
			gapsAndMatches.push_back('|');
			//cout << "s1Result from situation1:" << s1[i] << endl;
			//cout << "s2Result from situation1:" << s2[j] << endl;

			if (s1[i] == s2[j])
				numMatches++;
			else
				numMismatches++;

			openingGapIns = true;
			openingGapDel = true;
			i--;
			j--;
		}
		else if (T[i][j].deletionScore >= T[i][j].insertionScore && T[i][j].deletionScore >= T[i][j].substitutionScore) {
			s1Result.push_back(s1[i]);
			s2Result.push_back('-');
			gapsAndMatches.push_back(' ');
			//cout << "s1Result from situation2:" << s1[i] << endl;
			//cout << "s2Resul:" << s2[j] << endl;
			if (openingGapDel) {
				numOpeningGaps++;
				openingGapDel = false;
			}
			openingGapIns = true;
			numGaps++;
			i--;
		}
		else {
			s1Result.push_back('-');
			s2Result.push_back(s2[j]);
			gapsAndMatches.push_back(' ');
			//cout << "s1Result from situation3:" << s1[i] << endl;
			//cout << "s2Resul:" << s2[j] << endl;
			if (openingGapIns) {
				numOpeningGaps++;
				openingGapIns = false;
			}
			openingGapDel = false;
			numGaps++;
			j--;
		}

		if (T[i][j].deletionScore == 0 && T[i][j].insertionScore == 0 && T[i][j].substitutionScore == 0) {
			continueFlag = false;
			cout << "continueFlag=flase" << endl;
		}
	}
	cout << "length of s1Result is :" << s1Result.length() << endl;
	cout << "length of s2Result is :" << s2Result.length() << endl;
	s1Result = s1Result.substr(1, s1Result.length());
	s2Result = s2Result.substr(1, s2Result.length());
	gapsAndMatches = gapsAndMatches.substr(1, gapsAndMatches.length());

	// Reverse strings.
	std::reverse(s1Result.begin(), s1Result.end());
	std::reverse(s2Result.begin(), s2Result.end());
	std::reverse(gapsAndMatches.begin(), gapsAndMatches.end());

	//cout << "------------Length s1Result: " << s1Result.length() << "\tLength s2Result: " << s2Result.length() << "-----------" << endl;

	Output();
}


void GlobalAlignment::Output()
{
	//Alignment::Output();

	// Print results to cmd.
	cout << "Scores:\t\tmatch = " << match << ", mismatch = " << mismatch << ", h = " << h << ", g = " << g << endl << endl;
	cout << "Sequence 1 = \"" << s1Name << "\", length = " << s1.length() << " characters" << endl;
	cout << "Sequence 2 = \"" << s2Name << "\", length = " << s2.length() << " characters" << endl << endl;

	int numS1 = 1;
	int numS2 = 1;

	for (int i = 0; i < s1Result.length(); i += 60) {
		int tempstrlastS1;
		int tempstrlastS2;
		string tempS1 = s1Result.substr(i, 60);
		int numgapS1 = count(tempS1.begin(), tempS1.end(), '-');
		string tempS2 = s2Result.substr(i, 60);
		int numgapS2 = count(tempS2.begin(), tempS2.end(), '-');
		string tempPipes = gapsAndMatches.substr(i, 60);
		if (numgapS1 == 60) {
			tempstrlastS1 = numS1;
		}
		else {
			tempstrlastS1 = numS1 + tempS1.length() - numgapS1-1;
		}
		if (numgapS2 == 60) {
			tempstrlastS2 = numS2;
		}
		else {
			tempstrlastS2 = numS2 + tempS2.length() - numgapS2-1;
		}

		cout << "s1\t" << numS1 << "\t" << tempS1 << "\t" << tempstrlastS1 << endl;
		cout << "\t\t" << tempPipes << endl;
		cout << "s2\t" << numS2 << "\t" << tempS2 << "\t" << tempstrlastS2 << endl << endl;
		//cout << "Last one is:" <<tempS1[tempS1.length() - 1] << endl;
		if (tempS1[tempS1.length() - 1] != '-') {
			numS1 = tempstrlastS1 + 1;
		}
		if (tempS2[tempS2.length() - 1] != '-') {
			numS2 = tempstrlastS2 + 1;
		}
	}

	cout << "Report:\n\nGlobal optimal score = " << optimalScore << endl << endl;
	cout << "Number of: matches = " << numMatches << ", mismatches = " << numMismatches << ", gaps = " << numGaps << ", opening gaps = " << numOpeningGaps << endl << endl;
	cout << "Identities = " << numMatches << "/" << s1Result.length() << " (" << ((double)numMatches / s1Result.length()) * 100 << "%)"
		<< ", Gaps = " << numGaps << "/" << s1Result.length() << " (" << ((double)numGaps / s1Result.length()) * 100 << "%)" << endl;

	// Print results to outfile.
	ofstream outfile("GlobalAlignment_" + inputFileName + ".txt");

	outfile << "Scores:\t\tmatch = " << match << ", mismatch = " << mismatch << ", h = " << h << ", g = " << g << endl << endl;
	outfile << "Sequence 1 = \"" << s1Name << "\", length = " << s1.length() << " characters" << endl;
	outfile << "Sequence 2 = \"" << s2Name << "\", length = " << s2.length() << " characters" << endl << endl;

	numS1 = 1;
	numS2 = 1;

	for (int i = 0; i < s1Result.length(); i += 60) {
		int tempstrlastS1;
		int tempstrlastS2;
		string tempS1 = s1Result.substr(i, 60);
		int numgapS1 = count(tempS1.begin(), tempS1.end(), '-');
		string tempS2 = s2Result.substr(i, 60);
		int numgapS2 = count(tempS2.begin(), tempS2.end(), '-');
		string tempPipes = gapsAndMatches.substr(i, 60);
		if (numgapS1 == 60) {
			tempstrlastS1 = numS1;
		}
		else {
			tempstrlastS1 = numS1 + tempS1.length() - numgapS1 - 1;
		}
		if (numgapS2 == 60) {
			tempstrlastS2 = numS2;
		}
		else {
			tempstrlastS2 = numS2 + tempS2.length() - numgapS2 - 1;
		}

		outfile << "s1\t" << numS1 << "\t" << tempS1 << "\t" << tempstrlastS1 << endl;
		outfile << "\t\t" << tempPipes << endl;
		outfile << "s2\t" << numS2 << "\t" << tempS2 << "\t" << tempstrlastS2 << endl << endl;
		//outfile << "Last one is:" << tempS1[tempS1.length() - 1] << endl;
		if (tempS1[tempS1.length() - 1] != '-') {
			numS1 = tempstrlastS1 + 1;
		}
		if (tempS2[tempS2.length() - 1] != '-') {
			numS2 = tempstrlastS2 + 1;
		}
	}

	outfile << "Report:\n\nGlobal optimal score = " << optimalScore << endl << endl;
	outfile << "Number of: matches = " << numMatches << ", mismatches = " << numMismatches << ", gaps = " << numGaps << ", opening gaps = " << numOpeningGaps << endl << endl;
	outfile << "Identities = " << numMatches << "/" << s1Result.length() << " (" << ((double)numMatches / s1Result.length()) * 100 << "%)"
		<< ", Gaps = " << numGaps << "/" << s1Result.length() << " (" << ((double)numGaps / s1Result.length()) * 100 << "%)" << endl;
	outfile.close();
}

void LocalAlignment::Output()
{
	//Alignment::Output();

	// Print results to cmd.
	cout << "Scores:\t\tmatch = " << match << ", mismatch = " << mismatch << ", h = " << h << ", g = " << g << endl << endl;
	cout << "Sequence 1 = \"" << s1Name << "\", length = " << s1.length() << " characters" << endl;
	cout << "Sequence 2 = \"" << s2Name << "\", length = " << s2.length() << " characters" << endl << endl;

	int numS1 = 1;
	int numS2 = 1;

	for (int i = 0; i < s1Result.length(); i += 60) {
		int tempstrlastS1;
		int tempstrlastS2;
		string tempS1 = s1Result.substr(i, 60);
		int numgapS1 = count(tempS1.begin(), tempS1.end(), '-');
		string tempS2 = s2Result.substr(i, 60);
		int numgapS2 = count(tempS2.begin(), tempS2.end(), '-');
		string tempPipes = gapsAndMatches.substr(i, 60);
		if (numgapS1 == 60) {
			tempstrlastS1 = numS1;
		}
		else {
			tempstrlastS1 = numS1 + tempS1.length() - numgapS1 - 1;
		}
		if (numgapS2 == 60) {
			tempstrlastS2 = numS2;
		}
		else {
			tempstrlastS2 = numS2 + tempS2.length() - numgapS2 - 1;
		}

		cout << "s1\t" << numS1 << "\t" << tempS1 << "\t" << tempstrlastS1 << endl;
		cout << "\t\t" << tempPipes << endl;
		cout << "s2\t" << i + 1 << "\t" << tempS2 << "\t" << tempstrlastS2 << endl << endl;
		cout << "Last one is:" << tempS1[tempS1.length() - 1] << endl;
		if (tempS1[tempS1.length() - 1] != '-') {
			numS1 = tempstrlastS1 + 1;
		}
		if (tempS2[tempS2.length() - 1] != '-') {
			numS2 = tempstrlastS2 + 1;
		}
	}

	cout << "Report:\n\nLocal optimal score = " << optimalScore << endl << endl;
	cout << "Number of: matches = " << numMatches << ", mismatches = " << numMismatches << ", gaps = " << numGaps << ", opening gaps = " << numOpeningGaps << endl << endl;
	cout << "Identities = " << numMatches << "/" << s1Result.length() << " (" << ((double)numMatches / s1Result.length()) * 100 << "%)"
		<< ", Gaps = " << numGaps << "/" << s1Result.length() << " (" << ((double)numGaps / s1Result.length()) * 100 << "%)" << endl;

	// Print results to outfile.
	ofstream outfile("LocalAlignment_" + inputFileName + ".txt");

	outfile << "Scores:\t\tmatch = " << match << ", mismatch = " << mismatch << ", h = " << h << ", g = " << g << endl << endl;
	outfile << "Sequence 1 = \"" << s1Name << "\", length = " << s1.length() << " characters" << endl;
	outfile << "Sequence 2 = \"" << s2Name << "\", length = " << s2.length() << " characters" << endl << endl;

	for (int i = 0; i < s1Result.length(); i += 60) {
		string tempS1 = s1Result.substr(i, 60);
		string tempS2 = s2Result.substr(i, 60);
		string tempPipes = gapsAndMatches.substr(i, 60);

		outfile << "s1\t" << i + 1 << "\t" << tempS1 << "\t" << i + tempS1.length() << endl;
		outfile << "\t\t" << tempPipes << endl;
		outfile << "s2\t" << i + 1 << "\t" << tempS2 << "\t" << i + tempS2.length() << endl << endl;
	}

	outfile << "Report:\n\nLocal optimal score = " << optimalScore << endl << endl;
	outfile << "Number of: matches = " << numMatches << ", mismatches = " << numMismatches << ", gaps = " << numGaps << ", opening gaps = " << numOpeningGaps << endl << endl;
	outfile << "Identities = " << numMatches << "/" << s1Result.length() << " (" << ((double)numMatches / s1Result.length()) * 100 << "%)"
		<< ", Gaps = " << numGaps << "/" << s1Result.length() << " (" << ((double)numGaps / s1Result.length()) * 100 << "%)" << endl;
	outfile.close();
}
