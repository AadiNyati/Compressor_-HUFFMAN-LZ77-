#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;

//Huffman
struct Node {
    string data;
    int freq;
    Node *left, *right;
    Node(string d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
    ~Node() { delete left; delete right; }
};

struct Compare {
    bool operator()(Node* l, Node* r) { return l->freq > r->freq; }
};

void generateCodes(Node* root, string str, map<string, string>& huffmanCode) {
    if (!root) return;
    if (!root->left && !root->right) huffmanCode[root->data] = str;
    generateCodes(root->left, str + "0", huffmanCode);
    generateCodes(root->right, str + "1", huffmanCode);
}

Node* buildHuffman(map<string, int>& freqs) {
    if (freqs.empty()) return nullptr;
    priority_queue<Node*, vector<Node*>, Compare> minHeap;
    for (auto it = freqs.begin(); it != freqs.end(); ++it) {
        minHeap.push(new Node(it->first, it->second));
    }

    while (minHeap.size() > 1) {
        Node *l = minHeap.top(); minHeap.pop();
        Node *r = minHeap.top(); minHeap.pop();
        Node *top = new Node("", l->freq + r->freq);
        top->left = l; top->right = r;
        minHeap.push(top);
    }
    return minHeap.top();
}

int calculateBitsForMap(map<string, int>& freqs) {
    Node* root = buildHuffman(freqs);
    if (!root) return 0;
    map<string, string> codes;
    generateCodes(root, "", codes);
    
    int totalBits = 0;
    for (auto it = freqs.begin(); it != freqs.end(); ++it) {
        totalBits += it->second * codes[it->first].length();
    }
    delete root;
    return totalBits;
}

//LZ77
struct LZ77Token {
    int distance, length;
    char nextChar;
};

vector<LZ77Token> lz77_compress(string text, int windowSize) {
    vector<LZ77Token> tokens;
    int n = text.length();
    for (int i = 0; i < n; ) {
        int matchDist = 0, matchLen = 0;
        int start = max(0, i - windowSize);
        for (int j = start; j < i; j++) {
            int len = 0;
            while (i + len < n && text[j + len] == text[i + len] && len < 15) len++;
            if (len >= matchLen && len > 0) { matchLen = len; matchDist = i - j; }
        }
        tokens.push_back({matchDist, matchLen, (i + matchLen < n) ? text[i + matchLen] : '\0'});
        i += matchLen + 1;
    }
    return tokens;
}

string readFile(string filename) {
    ifstream t(filename);
    if (!t.is_open()) return "";
    stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

int main() {
    string input = readFile("test_data.txt");
    if (input.empty()) return 1;

    ofstream results("huffman_comparison.csv");
    results << "WindowSize,FusedRatio,SplitRatio,FusedTime,SplitTime\n";

    cout << "Comparing Huffman Strategies and Runtimes..." << endl;
    cout << "Window\tFused%\tSplit%\tF-Time(ms)\tS-Time(ms)" << endl;

    for (int w = 10; w <= 500; w += 20) {
        //LZ77
        auto t0 = chrono::high_resolution_clock::now();
        vector<LZ77Token> tokens = lz77_compress(input, w);
        auto t1 = chrono::high_resolution_clock::now();
        double lzMilli = chrono::duration<double, milli>(t1 - t0).count();

        //FUSED
        auto tf0 = chrono::high_resolution_clock::now();
        map<string, int> fusedFreqs;
        for (const auto& t : tokens) {
            string s = to_string(t.distance) + "," + to_string(t.length) + "," + t.nextChar;
            fusedFreqs[s]++;
        }
        int fusedBits = calculateBitsForMap(fusedFreqs);
        auto tf1 = chrono::high_resolution_clock::now();
        double fusedTotalTime = lzMilli + chrono::duration<double, milli>(tf1 - tf0).count();

        //SPLIT
        auto ts0 = chrono::high_resolution_clock::now();
        map<string, int> distFreqs, lenFreqs, charFreqs;
        for (const auto& t : tokens) {
            distFreqs[to_string(t.distance)]++;
            lenFreqs[to_string(t.length)]++;
            charFreqs[string(1, t.nextChar)]++;
        }
        int splitBits = calculateBitsForMap(distFreqs) + 
                        calculateBitsForMap(lenFreqs) + 
                        calculateBitsForMap(charFreqs);
        auto ts1 = chrono::high_resolution_clock::now();
        double splitTotalTime = lzMilli + chrono::duration<double, milli>(ts1 - ts0).count();

        float fusedRatio = (1.0f - (float)fusedBits / (input.length() * 8)) * 100.0f;
        float splitRatio = (1.0f - (float)splitBits / (input.length() * 8)) * 100.0f;

        results << w << "," << fusedRatio << "," << splitRatio << "," << fusedTotalTime << "," << splitTotalTime << "\n";
        
        cout << w << "\t" << fusedRatio << "%\t" << splitRatio << "%\t" 
             << fusedTotalTime << "\t\t" << splitTotalTime << endl;
    }

    results.close();
    return 0;
}
