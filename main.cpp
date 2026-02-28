// Winter'24
// Instructor: Diba Mirza
// Student name: 
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <cstring>
#include <algorithm>
#include <limits.h>
#include <iomanip>
#include <set>
#include <queue>
#include <sstream>
#include <map>
#include <unordered_map>
using namespace std;

#include "utilities.h"
#include "movies.h"

bool parseLine(string &line, string &movieName, double &movieRating);

int main(int argc, char** argv){
    // auto timer = clock();
    if (argc < 2){
        cerr << "Not enough arguments provided (need at least 1 argument)." << endl;
        cerr << "Usage: " << argv[ 0 ] << " moviesFilename prefixFilename " << endl;
        exit(1);
    }

    if (argc == 2){

        ifstream movieFile (argv[1]);
 
        if (movieFile.fail()){
            cerr << "Could not open file " << argv[1];
            exit(1);
        }

        // data structure for part 1
        // Create an object of a STL data-structure to store all the movies
        map<string, double> movies;

        string line, movieName;
        double movieRating;

        
        // Read each file and store the name and rating
        while (getline (movieFile, line) && parseLine(line, movieName, movieRating)){
                // Use std::string movieName and double movieRating
                // to construct your Movie objects
                // cout << movieName << " has rating " << movieRating << endl;
                // insert elements into your data structure
                movies[movieName] = movieRating;
        }

        movieFile.close();

        //print all the movies in ascending alphabetical order of movie names
        for (auto m : movies) {
            cout << m.first << ", " << m.second << endl;
        }
        return 0;
    }

    // begin of prefix case
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // auto timeResultA = clock() - timer;

    ifstream prefixFile (argv[2]);

    if (prefixFile.fail()) {
        cerr << "Could not open file " << argv[2];
        exit(1);
    }

    vector<string> prefixes;

    string line, movieName;
    while (getline (prefixFile, line)) {
        if (!line.empty()) {
            prefixes.push_back(line);
        }
    }
    prefixFile.close();

    //order matters at end

    ifstream movieFile (argv[1]);
 
    if (movieFile.fail()){
        cerr << "Could not open file " << argv[1];
        exit(1);
    }

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // beginning of changes
    // think about using find() rather than contains()
    // auto timeResultB = clock() - timer;

    const int prefixCount = prefixes.size();

    struct Compare {
    bool operator()(const pair<double, string>& a, const pair<double, string>& b) {
        if (a.first == b.first) {
            return a.second > b.second; // string
        }
        return a.first < b.first; // double
    }
    };

    struct speedyStructNode {

        bool active;
        bool isLeaf;
        priority_queue<pair<double, string>, vector<pair<double, string>>, Compare> movies;
        string duplicateOutput;

        speedyStructNode(const bool& isEnd) {
            active = isEnd;
            isLeaf = isEnd;
        }
        speedyStructNode() {}
    };


    unordered_map<string, speedyStructNode> speedyStruct;
    vector<speedyStructNode*> speedyNodeByIndex(prefixCount);

    const int sTemp = prefixes.size();
    for (int preIndex = 0; preIndex < sTemp; preIndex++) {
        const string& pre = prefixes[preIndex];
        int size = pre.size();
        for (int i = 1; i < size; i++) {
            string subPre = pre.substr(0, i);
            if (speedyStruct.contains(subPre)) {
                speedyStruct[subPre].isLeaf = false;
            } else {
                speedyStruct.insert({subPre, speedyStructNode(false)});
            }
        }
        if (speedyStruct.contains(pre)) {
            speedyNodeByIndex[preIndex] = &speedyStruct[pre];
            speedyNodeByIndex[preIndex]->active = true;
        } else {
            speedyNodeByIndex[preIndex] = &speedyStruct[pre];
            *speedyNodeByIndex[preIndex] = speedyStructNode(true);
            //speedyStruct.insert({pre, speedyStructNode(true)});
        }

    }

    // auto timeResultC = clock() - timer;

    double movieRating;

    //vector<priority_queue<pair<double, string>>> movies(prefixCount);
    //old implementation

    //double first for sorting
    //indices match prefixes
    
    // Read each file and store the name and rating
    while (getline (movieFile, line) && parseLine(line, movieName, movieRating)){

        // for (int i = 0; i < prefixCount; i++) {
        //     if ((movies[i].empty() || movies[i].top().first <= movieRating) && (movieName.substr(0, prefixes[i].size()) == prefixes[i])) {
        //         movies[i].push({movieRating, movieName});
        //     }
        // }

        int i = 0;
        int movieNameLen = movieName.size();
        while (i < movieNameLen) {
            string currentPrefix = movieName.substr(0, i+1);
            if (speedyStruct.contains(currentPrefix)) {
                auto& nodeRef = speedyStruct[currentPrefix];
                if (nodeRef.active) {
                    nodeRef.movies.push({movieRating, movieName});
                    //cout << currentPrefix << endl;
                }
                if (nodeRef.isLeaf) {break;}
            } else {
                break;
            }
            i++;
        }
        
    }

    // Data Structures are complete at this point

    // auto timeResultD = clock() - timer;
    // For each prefix
    // Print all movies out in score order or
    // No movies found with prefix <prefix_value>
    // Then put the best movie back in the queue

    string output = "";
    std::fixed;
    std::setprecision(1);

    for (int i = 0; i < prefixCount; i++) {
        //auto& pre = movies[i];
        //auto& pre = speedyStruct[prefixes[i]].movies;
        auto& pre = speedyNodeByIndex[i]->movies;
        if (pre.empty()) {
            output.append("No movies found with prefix ").append(prefixes[i]).append("\n");
            //cout << "No movies found with prefix " << prefixes[i] << "\n";
        } else {
            int sizeA = pre.size();
            int i = 0;
            vector<pair<double, string>> temp(sizeA);
            while (i<sizeA) {
                string s = to_string(pre.top().first);
                output.append(pre.top().second).append(", ").append(s.substr(0, s.find('.') + 2)).append("\n");
                //cout << pre.top().second << ", " << pre.top().first << "\n";
                temp[i] = pre.top();
                pre.pop();
                i++;
            }
            for (auto m : temp) {
                pre.push(m);
            }
            output.append("\n");
            //cout << "\n";
        }
        
    }

    //cout << output;
    
    // auto timeResultE = clock() - timer;

    //  For each prefix,
    //  Print the highest rated movie with that prefix if it exists.

    // fix code for duplicate prefixes
    for (int i = 0; i < prefixCount; i++) {

        //auto& best = movies[i];
        //auto& best = speedyStruct[prefixes[i]].movies;
        auto& best = speedyNodeByIndex[i]->movies;

        //prefix duplicate case
        if (!(*speedyNodeByIndex[i]).active) {
            output.append((*speedyNodeByIndex[i]).duplicateOutput);
        }
        else if (best.empty()) {
            //cout << "No movies found with prefix " << prefixes[i] << "\n";
            //output.append("No movies found with prefix ").append(prefixes[i]).append("\n");
        } else {
            (*speedyNodeByIndex[i]).active = false;
            string s = to_string(best.top().first);
            (*speedyNodeByIndex[i]).duplicateOutput.append("Best movie with prefix ").append(prefixes[i]).append(" is: ").append(
                best.top().second).append(" with rating ").append(s.substr(0, s.find('.') + 2)).append("\n");
            //cout << "Best movie with prefix " << prefixes[i] << " is: " << best.top().second << " with rating " << std::fixed << std::setprecision(1) << best.top().first << "\n";
            output.append((*speedyNodeByIndex[i]).duplicateOutput);
        }
    }

    // auto timeResultF = clock() - timer;
    ios_base::sync_with_stdio(false);
    //cout << output;
    fwrite(output.c_str(), 1, output.size(), stdout);
    // auto timeResultG = clock() - timer;
    
    // auto timeResult = clock() - timer;
    // timeResultG -= timeResultF;
    // timeResultF -= timeResultE;
    // timeResultE -= timeResultD;
    // timeResultD -= timeResultC;
    // timeResultC -= timeResultB;
    // timeResultB -= timeResultA;

    // cout << "runtime: " << timeResult << " Ms = " << timeResult*1000/CLOCKS_PER_SEC << "ms" << endl;
    // cout << "part a: " << timeResultA << " Ms" << endl;
    // cout << "part b: " << timeResultB << " Ms" << endl;
    // cout << "part c: " << timeResultC << " Ms" << endl;
    // cout << "part d: " << timeResultD << " Ms" << endl;
    // cout << "part e: " << timeResultE << " Ms" << endl;
    // cout << "part f: " << timeResultF << " Ms" << endl;
    // cout << "part g: " << timeResultG << " Ms" << endl;

    return 0;
}

/* Add your run time analysis for part 3 of the assignment here as commented block

    TIME COMPLEXITY

    assuming:
        all n movies are already stored in your data structure.
        all m prefixes are already stored in an array.
        at most k movies begin with each prefix.
        l is the maximum number of characters in a movie name

    for (int i = 0; i < prefixCount; i++) runs m loops and appears twice.

    Each loop in the second instance has O(1) time because all the data lookups are done in O(1) structures. 
    These add up to O(m) over the m loops

    The first instance includes a while loop in each loop. The while loop runs at most k times each loop. 
    Each loop copies a string of size l or less
    It can run more than n times total because multiple prefixes can store the same movie.
    These add to O(m*k*l)

    O(m*l) + O(m*k*l) = O(m*k*l)

    O(m*k*l) is also the worst case for the cout.

    The assumptions make O(m*k*l) the worst case time complexity because the code above these loops sets up the data structure storing the n movies.

    EXTRAS (without the stored assumptions):

    while (getline (movieFile, line) && parseLine(line, movieName, movieRating)) should have average time complexity O(n*min(l,q))
    with q as the maximum number of characters in a prefix
    the speedyStruct setup average time complexity is O(m*q)
    the average time complexity without the assumptions is O(m*(q+k*l)+n*min(l,q))

    Real tests:

    ./runMovies input_76920_random.csv prefix_large.txt

    runtime: 163169 Ms = 163 ms
    part a: 1 Ms
    part b: 1511 Ms
    part c: 5145 Ms
    part d: 46314 Ms
    part e: 36794 Ms
    part f: 2393 Ms
    part g: 71009 Ms

    runtime: 146804 Ms = 147 ms
    part a: 2 Ms
    part b: 1604 Ms
    part c: 4946 Ms
    part d: 45279 Ms
    part e: 32923 Ms
    part f: 1681 Ms
    part g: 60368 Ms

    ./runMovies input_1000_random.csv prefix_large.txt

    runtime: 42728 Ms = 42 ms
    part a: 2 Ms
    part b: 1582 Ms
    part c: 4612 Ms
    part d: 653 Ms
    part e: 1432 Ms
    part f: 1040 Ms
    part g: 33407 Ms

    runtime: 42971 Ms = 42 ms
    part a: 1 Ms
    part b: 1490 Ms
    part c: 4822 Ms
    part d: 505 Ms
    part e: 1432 Ms
    part f: 1021 Ms
    part g: 33699 Ms

    ./runMovies input_76920_random.csv prefix_small.txt

    runtime: 33513 Ms = 33 ms
    part a: 1 Ms
    part b: 223 Ms
    part c: 13 Ms
    part d: 16408 Ms
    part e: 7915 Ms
    part f: 3 Ms
    part g: 8949 Ms

    ./runMovies input_76920_random.csv prefix_medium.txt

    runtime: 126333 Ms = 126 ms
    part a: 1 Ms
    part b: 161 Ms
    part c: 17 Ms
    part d: 27773 Ms
    part e: 42812 Ms
    part f: 15 Ms
    part g: 55554 Ms

    ./runMovies input_20_random.csv prefix_large.txt

    runtime: 39960 Ms = 39 ms
    part a: 1 Ms
    part b: 1550 Ms
    part c: 4827 Ms
    part d: 181 Ms
    part e: 1550 Ms
    part f: 983 Ms
    part g: 30867 Ms

    The runtime trend matches my time complexity analysis included in the EXTRAs section O((m*(q+k)+n*min(l,q))*l). 
    Since q is <= 3 in all of the tests, this time complexity is effectively O(l*(m*k+n))

    The data structures are completly set up after part d. Tests with a large value n result in the setup in part d taking extra time.
    The runtime trends of steps e to g are consistant with the O(m*k*l) figure calculated with the assumptions.

    SPACE COMPLEXITY

    assuming:
        all n movies are already stored in your data structure.
        all m prefixes are already stored in an array.
        at most k movies begin with each prefix.
        l is the maximum number of characters in a movie name
    
    I consider these assumptions to be true after part d. 
    The first one is technically never true because movies that don't have any prefix to store under will not be stored.

    output stores the entirity of the output of the program.
    The total output of the program has time complexity O(m*k*l)
    This is because for each of m prefixes it must print out all movies that begin with that prefix.
    The number of movies that begin with that prefix is at most k.
    Printing out each each move requires printing it's name. In the worst case all names have length l.

    Strings are stored to speedyStruct nodes. Each string stored is also appended to output. Therefore this action does not increase the big O.

    The total worst case space complexity is O(m*k*l)

    EXTRAS (without the stored assumptions):

    q is the maximum number of characters in a prefix

    speedyStruct has worst case space complexity O(m*(q+(k*l)))
    m prefixes are stored to it as active nodes.
    For each active node there can be at most q-1 non-active nodes.
    For each active node there can be at most k moves stored with strings of at most length l.

    The total worst case space complexity is O(m*(q+(k*l)))

    TRADEOFFS

    I designed my algorithm for low time complexity. 
    My space complexity is higher than it could be.
    If I didn't store the strings to output, I could have saved space at the cost of speed.
    Additionally, if I were to store the movies in a vector I could have avoided restoring the strings.
    A differant solution to the duplicate prefix case would also allow me to cut back on space complexity.

*/

bool parseLine(string &line, string &movieName, double &movieRating) {
    int commaIndex = line.find_last_of(",");
    movieName = line.substr(0, commaIndex);
    movieRating = stod(line.substr(commaIndex+1));
    if (movieName[0] == '\"') {
        movieName = movieName.substr(1, movieName.length() - 2);
    }
    return true;
}