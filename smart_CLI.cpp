#include<bits/stdc++.h>
#include<fstream>
#include<sstream>
#include<termios.h>
#include<unistd.h>
#include<sys/ioctl.h>
using namespace std;
struct TrieNode {
    unordered_map<char, TrieNode*> children;
    bool isEnd;
    int freq;
    TrieNode() : isEnd(false), freq(0) {}
};
struct Trie {
    TrieNode* root;
    unordered_map<string, int> freqMap;
    Trie() { root = new TrieNode(); }
    void insert(const string& word) {
        freqMap[word]++;
        TrieNode* cur = root;
        for (char c : word) {
            if (!cur->children.count(c))
                cur->children[c] = new TrieNode();
            cur = cur->children[c];
        }
        cur->isEnd = true;
        cur->freq = freqMap[word];
    }
    void collectAll(TrieNode* node, string current, vector<pair<int,string>>& results) {
        if (node->isEnd)
            results.push_back({node->freq, current});
        for (auto& p : node->children)
            collectAll(p.second, current + p.first, results);
    }
    string predict(const string& prefix) {
        TrieNode* cur = root;
        for (char c : prefix) {
            if (!cur->children.count(c)) return "";
            cur = cur->children[c];
        }
        vector<pair<int,string>> results;
        collectAll(cur, prefix, results);
        if (results.empty()) return "";
        sort(results.begin(), results.end(), [](const pair<int,string>& a, const pair<int,string>& b){
            return a.first > b.first;
        });
        return results[0].second;
    }
    void load(const string& histFile) {
        ifstream f(histFile);
        string line;
        while (getline(f, line)) {
            if (!line.empty()) insert(line);
        }
    }
    void save(const string& histFile) {
        ofstream f(histFile);
        for (auto& p : freqMap) {
            for (int i = 0; i < p.second; i++)
                f << p.first << "\n";
        }
    }
};
void setRawMode(bool enable) {
    static struct termios oldt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);
        struct termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}
string readLineWithPrediction(Trie& trie, vector<string>& history) {
    string input = "";
    string suggestion = "";
    int histIdx = (int)history.size();
    setRawMode(true);
    auto clearLine = [&]() {
        int total = 1 + input.size() + suggestion.size();
        for (int i = 0; i < (int)total; i++) cout << "\b \b";
    };
    auto redraw = [&]() {
        cout << "\r>";
        cout << input;
        if (!suggestion.empty() && suggestion.size() > input.size()) {
            string suf = suggestion.substr(input.size());
            cout << "\033[2m" << suf << "\033[0m";
            for (int i = 0; i < (int)suf.size(); i++) cout << "\b";
        }
        cout.flush();
    };
    while (true) {
        char c = getchar();
        if (c == '\n' || c == '\r') {
            if (!suggestion.empty() && suggestion.size() > input.size()) {
                int suf = suggestion.size() - input.size();
                for (int i = 0; i < suf; i++) cout << " ";
            }
            cout << "\n";
            break;
        } else if (c == '\t') {
            if (!suggestion.empty() && suggestion.size() > input.size()) {
                clearLine();
                input = suggestion;
                suggestion = "";
                redraw();
            }
        } else if (c == 127 || c == '\b') {
            if (!input.empty()) {
                clearLine();
                input.pop_back();
                string firstToken = "";
                stringstream ss(input);
                ss >> firstToken;
                if (!firstToken.empty())
                    suggestion = trie.predict(firstToken);
                else
                    suggestion = "";
                redraw();
            }
        } else if (c == 27) {
            char seq1 = getchar();
            if (seq1 == '[') {
                char seq2 = getchar();
                if (seq2 == 'A') {
                    if (!history.empty() && histIdx > 0) {
                        histIdx--;
                        clearLine();
                        input = history[histIdx];
                        suggestion = "";
                        redraw();
                    }
                } else if (seq2 == 'B') {
                    if (histIdx < (int)history.size() - 1) {
                        histIdx++;
                        clearLine();
                        input = history[histIdx];
                        suggestion = "";
                        redraw();
                    } else {
                        histIdx = (int)history.size();
                        clearLine();
                        input = "";
                        suggestion = "";
                        redraw();
                    }
                }
            }
        } else {
            clearLine();
            input += c;
            histIdx = (int)history.size();
            string firstToken = "";
            stringstream ss(input);
            ss >> firstToken;
            if (!firstToken.empty())
                suggestion = trie.predict(firstToken);
            else
                suggestion = "";
            redraw();
        }
    }
    setRawMode(false);
    return input;
}
void echo(vector<string>& tokens) {
    for (int i = 1; i < (int)tokens.size(); i++) {
        for (char ch : tokens[i]) {
            if (ch != '"') cout << ch;
        }
        if (i != (int)tokens.size() - 1) cout << " ";
    }
    cout << "\n";
}
void cat(vector<string>& tokens) {
    if (tokens.size() < 2) { cout << "cat: missing file operand\n"; return; }
    for (int i = 1; i < (int)tokens.size(); i++) {
        ifstream f(tokens[i]);
        if (!f) { cout << "cat: " << tokens[i] << ": No such file or directory\n"; continue; }
        string line;
        while (getline(f, line)) cout << line << "\n";
    }
}
void touch(vector<string>& tokens) {
    if (tokens.size() < 2) { cout << "touch: missing file operand\n"; return; }
    for (int i = 1; i < (int)tokens.size(); i++) {
        ofstream f(tokens[i], ios::app);
        if (!f) cout << "touch: cannot create file '" << tokens[i] << "'\n";
    }
}
void head(vector<string>& tokens) {
    int n = 10;
    string filename = "";
    if (tokens.size() == 4 && tokens[1] == "-n") {
        n = stoi(tokens[2]);
        filename = tokens[3];
    } else if (tokens.size() == 2) {
        filename = tokens[1];
    } else {
        cout << "Usage: head [-n count] <file>\n"; return;
    }
    ifstream f(filename);
    if (!f) { cout << "head: " << filename << ": No such file or directory\n"; return; }
    string line;
    int cnt = 0;
    while (cnt < n && getline(f, line)) { cout << line << "\n"; cnt++; }
}
void tail(vector<string>& tokens) {
    int n = 10;
    string filename = "";
    if (tokens.size() == 4 && tokens[1] == "-n") {
        n = stoi(tokens[2]);
        filename = tokens[3];
    } else if (tokens.size() == 2) {
        filename = tokens[1];
    } else {
        cout << "Usage: tail [-n count] <file>\n"; return;
    }
    ifstream f(filename);
    if (!f) { cout << "tail: " << filename << ": No such file or directory\n"; return; }
    vector<string> lines;
    string line;
    while (getline(f, line)) lines.push_back(line);
    int start = max(0, (int)lines.size() - n);
    for (int i = start; i < (int)lines.size(); i++) cout << lines[i] << "\n";
}
void nl(vector<string>& tokens) {
    if (tokens.size() < 2) { cout << "nl: missing file operand\n"; return; }
    ifstream f(tokens[1]);
    if (!f) { cout << "nl: " << tokens[1] << ": No such file or directory\n"; return; }
    string line;
    int num = 1;
    while (getline(f, line)) {
        if (!line.empty()) cout << "\t" << num++ << "\t" << line << "\n";
        else cout << "\n";
    }
}
void wc(vector<string>& tokens) {
    if (tokens.size() < 2) { cout << "wc: missing file operand\n"; return; }
    string filename = tokens.back();
    bool countLines = false, countWords = false, countChars = false;
    bool flagGiven = false;
    for (int i = 1; i < (int)tokens.size() - 1; i++) {
        if (tokens[i] == "-l") { countLines = true; flagGiven = true; }
        else if (tokens[i] == "-w") { countWords = true; flagGiven = true; }
        else if (tokens[i] == "-c") { countChars = true; flagGiven = true; }
    }
    if (!flagGiven) { countLines = countWords = countChars = true; }
    ifstream f(filename);
    if (!f) { cout << "wc: " << filename << ": No such file or directory\n"; return; }
    int lines = 0, words = 0, chars = 0;
    string line;
    while (getline(f, line)) {
        lines++;
        chars += line.size() + 1;
        stringstream ss(line);
        string w;
        while (ss >> w) words++;
    }
    if (countLines) cout << "\t" << lines;
    if (countWords) cout << "\t" << words;
    if (countChars) cout << "\t" << chars;
    cout << "\t" << filename << "\n";
}
void bc(vector<string>& tokens) {
    if (tokens.size() < 2) { cout << "bc: missing expression\n"; return; }
    string expr = "";
    for (int i = 1; i < (int)tokens.size(); i++) expr += tokens[i];

    auto precedence = [](char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        return 0;
    };
    auto applyOp = [](double a, double b, char op) -> double {
        if (op == '+') return a + b;
        if (op == '-') return a - b;
        if (op == '*') return a * b;
        if (op == '/') return (b == 0) ? 0 : a / b;
        return 0;
    };
    stack<double> vals;
    stack<char> ops;
    int i = 0;
    bool valid = true;
    while (i < (int)expr.size() && valid) {
        if (isspace(expr[i])) { i++; continue; }
        if (isdigit(expr[i]) || (expr[i] == '.' && i+1 < (int)expr.size() && isdigit(expr[i+1]))) {
            string num = "";
            while (i < (int)expr.size() && (isdigit(expr[i]) || expr[i] == '.')) num += expr[i++];
            vals.push(stod(num));
        } else if (expr[i] == '(') {
            ops.push('('); i++;
        } else if (expr[i] == ')') {
            while (!ops.empty() && ops.top() != '(') {
                double b = vals.top(); vals.pop();
                double a = vals.top(); vals.pop();
                char op = ops.top(); ops.pop();
                vals.push(applyOp(a, b, op));
            }
            if (!ops.empty()) ops.pop();
            i++;
        } else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
            while (!ops.empty() && precedence(ops.top()) >= precedence(expr[i])) {
                double b = vals.top(); vals.pop();
                double a = vals.top(); vals.pop();
                char op = ops.top(); ops.pop();
                vals.push(applyOp(a, b, op));
            }
            ops.push(expr[i++]);
        } else {
            cout << "bc: invalid character '" << expr[i] << "'\n";
            valid = false;
        }
    }
    if (!valid) return;
    while (!ops.empty()) {
        double b = vals.top(); vals.pop();
        double a = vals.top(); vals.pop();
        char op = ops.top(); ops.pop();
        vals.push(applyOp(a, b, op));
    }
    if (!vals.empty()) {
        double result = vals.top();
        if (result == (long long)result) cout << (long long)result << "\n";
        else cout << fixed << setprecision(6) << result << "\n";
    }
}
void exprCmd(vector<string>& tokens) {
    if (tokens.size() < 4) { cout << "expr: missing operands\n"; return; }
    long long a, b;
    try { a = stoll(tokens[1]); b = stoll(tokens[3]); }
    catch (...) { cout << "expr: invalid operand\n"; return; }
    string op = tokens[2];
    if (op == "+") cout << a + b << "\n";
    else if (op == "-") cout << a - b << "\n";
    else if (op == "*") cout << a * b << "\n";
    else if (op == "/") { if (b == 0) cout << "expr: division by zero\n"; else cout << a / b << "\n"; }
    else if (op == "%") { if (b == 0) cout << "expr: division by zero\n"; else cout << a % b << "\n"; }
    else cout << "expr: unknown operator '" << op << "'\n";
}
void fact(vector<string>& tokens) {
    if (tokens.size() < 2) { cout << "factor: missing number\n"; return; }
    long long n;
    try { n = stoll(tokens[1]); }
    catch (...) { cout << "factor: invalid number\n"; return; }
    if (n <= 0) { cout << "factor: argument must be positive\n"; return; }
    cout << n << ":";
    long long temp = n;
    for (long long i = 2; i * i <= temp; i++) {
        while (temp % i == 0) { cout << " " << i; temp /= i; }
    }
    if (temp > 1) cout << " " << temp;
    cout << "\n";
}
void grep(vector<string>& tokens) {
    bool caseInsensitive = false;
    bool lineNumbers = false;
    bool invert = false;
    string pattern = "";
    string filename = "";
    int argIdx = 1;
    while (argIdx < (int)tokens.size()) {
        if (tokens[argIdx] == "-i") { caseInsensitive = true; argIdx++; }
        else if (tokens[argIdx] == "-n") { lineNumbers = true; argIdx++; }
        else if (tokens[argIdx] == "-v") { invert = true; argIdx++; }
        else if (pattern.empty()) { pattern = tokens[argIdx++]; }
        else { filename = tokens[argIdx++]; }
    }
    if (pattern.empty() || filename.empty()) { cout << "Usage: grep [-i] [-n] [-v] <pattern> <file>\n"; return; }
    ifstream f(filename);
    if (!f) { cout << "grep: " << filename << ": No such file or directory\n"; return; }
    string line;
    int lineNum = 0;
    string searchPattern = caseInsensitive ? pattern : pattern;
    if (caseInsensitive) for (char& c : searchPattern) c = tolower(c);
    while (getline(f, line)) {
        lineNum++;
        string searchLine = line;
        if (caseInsensitive) for (char& c : searchLine) c = tolower(c);
        bool found = searchLine.find(searchPattern) != string::npos;
        if (found != invert) {
            if (lineNumbers) cout << lineNum << ":";
            cout << line << "\n";
        }
    }
}
int main() {
    Trie trie;
    string histFile = ".smart_cli_history";
    trie.load(histFile);
    cout << "\033[1;36m";
    cout << "  ____                       _      ____ _     ___\n";
    cout << " / ___| _ __ ___   __ _ _ __| |_   / ___| |   |_ _|\n";
    cout << " \\___ \\| '_ ` _ \\ / _` | '__| __| | |   | |    | |\n";
    cout << "  ___) | | | | | | (_| | |  | |_  | |___| |___ | |\n";
    cout << " |____/|_| |_| |_|\\__,_|_|   \\__|  \\____|_____|___|\n";
    cout << "\033[0m";
    cout << "\033[1;33m Welcome to Smart CLI — type 'help' to list commands\033[0m\n\n";
    vector<string> history;
    while (true) {
        string com = readLineWithPrediction(trie, history);
        if (com.empty()) continue;
        stringstream ss(com);
        vector<string> tokens;
        string word;
        while (ss >> word) tokens.push_back(word);
        if (tokens.empty()) continue;
        history.push_back(com);
        trie.insert(tokens[0]);
        trie.save(histFile);
        if (tokens[0] == "exit") {
            cout << "Thanks For Using Smart CLI\n";
            return 0;
        } else if (tokens[0] == "help") {
            cout << "\n\033[1;32mAvailable Commands:\033[0m\n";
            cout << "  echo  <text>                  — Print text\n";
            cout << "  cat   <file>                  — Print file contents\n";
            cout << "  touch <file>                  — Create file(s)\n";
            cout << "  head  [-n N] <file>           — First N lines (default 10)\n";
            cout << "  tail  [-n N] <file>           — Last N lines (default 10)\n";
            cout << "  nl    <file>                  — Number non-empty lines\n";
            cout << "  wc    [-l|-w|-c] <file>       — Word/line/char count\n";
            cout << "  bc    <expression>            — Evaluate math expression\n";
            cout << "  expr  <a> <op> <b>            — Simple arithmetic\n";
            cout << "  factor <n>                    — Prime factorization\n";
            cout << "  grep  [-i] [-n] [-v] <p> <f> — Search pattern in file\n";
            cout << "  exit                          — Quit Smart CLI\n\n";
            cout << "  \033[2mTip: Start typing a command and press TAB to autocomplete\033[0m\n\n";
        } else if (tokens[0] == "cat") {
            cat(tokens);
        } else if (tokens[0] == "echo") {
            echo(tokens);
        } else if (tokens[0] == "touch") {
            touch(tokens);
        } else if (tokens[0] == "bc") {
            bc(tokens);
        } else if (tokens[0] == "head") {
            head(tokens);
        } else if (tokens[0] == "tail") {
            tail(tokens);
        } else if (tokens[0] == "nl") {
            nl(tokens);
        } else if (tokens[0] == "wc") {
            wc(tokens);
        } else if (tokens[0] == "expr") {
            exprCmd(tokens);
        } else if (tokens[0] == "factor") {
            fact(tokens);
        } else if (tokens[0] == "grep") {
            grep(tokens);
        } else {
            cout << "Sorry!!! We're still working on our commands... We could not find the command \""
                 << tokens[0] << "\"  Keep exploring...Keep using Smart CLI\n";
        }
    }
}