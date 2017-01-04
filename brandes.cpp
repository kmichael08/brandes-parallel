#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
using namespace std;

unordered_map<int, vector<int> > graph;

int main(int argc, char* argv[]) {
int a, b;

int threadNumber = stoi(argv[1]);
ifstream input;
ofstream output;
input.open(argv[2]);
output.open(argv[3]);


for (int i = 0; i < argc; i++)
	cout << argv[i] << endl;

while (!input.eof()) {
   input >> a >> b;
   graph[a].push_back(b);
}

for (auto it = graph.begin(); it!= graph.end(); it++)
{
    cout << (*it).first << "  ";
    for (auto inner = (*it).second.begin(); inner != (*it).second.end(); 
inner++)
	cout << (*inner) << " ";
	cout << endl;
}

input.close();
output.close();
return 0;
}
