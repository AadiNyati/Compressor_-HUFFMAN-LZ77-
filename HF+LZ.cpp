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

struct Node {
    string data;
    int freq;
    Node *left, *right;
    Node(string d, int f) {
        data = d;
        freq = f;
        left = right = nullptr;
    }
    ~Node() {
        delete left;
        delete right;
    }
};

struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

void generateCodes(Node* root, string str, map<string, string>& huffmanCode) {
    if (!root) return;
    if (!root->left && !root->right) {
        huffmanCode[root->data] = str;
    }
    generateCodes(root->left, str + "0", huffmanCode);
    generateCodes(root->right, str + "1", huffmanCode);
}

Node* buildHuffman(map<string, int>& freqs) {
    priority_queue<Node*, vector<Node*>, Compare> minHeap;
    for (auto it = freqs.begin(); it != freqs.end(); ++it) {
        minHeap.push(new Node(it->first, it->second));
    }
    while (minHeap.size() > 1) {
        Node *l = minHeap.top(); minHeap.pop();
        Node *r = minHeap.top(); minHeap.pop();
        Node *top = new Node("", l->freq + r->freq);
        top->left = l;
        top->right = r;
        minHeap.push(top);
    }
    return minHeap.top();
}

struct LZ77Token {
    int distance;
    int length;
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
            while (i + len < n && text[j + len] == text[i + len] && len < 15) {
                len++;
            }
            if (len >= matchLen && len > 0) {
                matchLen = len;
                matchDist = i - j;
            }
        }
        char next = (i + matchLen < n) ? text[i + matchLen] : '\0';
        tokens.push_back({matchDist, matchLen, next});
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
    if (input.empty()) {
        cout << "Error: test_data.txt not found!" << endl;
        return 1;
    }

    const int NUM_RUNS = 10; // Number of times to repeat each test for averaging
    ofstream results("window_sensitivity_averaged.csv");
    results << "WindowSize,CompressionRatio,AverageTimeMS\n";

    cout << "Starting Averaged Sensitivity Analysis (" << NUM_RUNS << " runs per window)..." << endl;
    cout << "Window\tAvg Ratio(%)\tAvg Time(ms)" << endl;

    for (int w = 10; w <= 500; w += 20) {
        double totalTime = 0;
        float lastRatio = 0;

        for (int run = 0; run < NUM_RUNS; run++) {
            auto start = chrono::high_resolution_clock::now();

            //Full Compression Logic 
            vector<LZ77Token> lz_tokens = lz77_compress(input, w);
            
            map<string, int> freqs;
            vector<string> tokenStrings;
            for (const auto& t : lz_tokens) {
                string s = to_string(t.distance) + "," + to_string(t.length) + "," + t.nextChar;
                tokenStrings.push_back(s);
                freqs[s]++;
            }

            Node* root = buildHuffman(freqs);
            map<string, string> huffmanCodes;
            generateCodes(root, "", huffmanCodes);

            int compressedBits = 0;
            for (const string& s : tokenStrings) {
                compressedBits += huffmanCodes[s].length();
            }

            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> elapsed = end - start;
            
            totalTime += elapsed.count();
            lastRatio = (1.0f - (float)compressedBits / (input.length() * 8)) * 100.0f;

            delete root; 
        }

        double avgTime = totalTime / NUM_RUNS;
        
        results << w << "," << lastRatio << "," << avgTime << "\n";
        cout << w << "\t" << lastRatio << "%\t\t" << avgTime << " ms" << endl;
    }

    results.close();
    return 0;
}