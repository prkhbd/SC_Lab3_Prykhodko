#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <numeric>

using namespace std;

struct EuclidData {
    long long gcd;
    long long x;
    long long y;
};

struct ResultText {
    long long a;
    long long b;
    int score;
    string text;
};

const int alphabetSize = 31;
const int modValue = alphabetSize * alphabetSize;

const vector<string> russianTopBigrams = {
    "ст",
    "но",
    "то",
    "на",
    "ен"
};

const vector<string> forbiddenBigrams = {
    "аь",
    "оь",
    "еь",
    "иь",
    "уы",
    "ьы",
    "йь",
    "щщ",
    "жы",
    "шы"
};

const vector<string> commonCrossBigrams = {
    "пр",
    "ос",
    "ен",
    "ни",
    "по",
    "ра",
    "ко"
};

const vector<string> commonTrigrams = {
    "про",
    "что",
    "ени",
    "ств",
    "ого",
    "это",
    "она"
};

const string alphabet1 =
    "абвгдежзийклмнопрстуфхцчшщьыэюя";

const string alphabet2 =
    "абвгдежзийклмнопрстуфхцчшщыьэюя";

long long mod(long long value, long long m) {

    value %= m;

    if (value < 0) {
        value += m;
    }

    return value;
}

EuclidData extendedEuclid(long long a,
                          long long b) {

    if (b == 0) {
        return {a, 1, 0};
    }

    EuclidData next =
        extendedEuclid(b, a % b);

    return {
        next.gcd,
        next.y,
        next.x - (a / b) * next.y
    };
}

bool inverse(long long value,
             long long modNumber,
             long long& result) {

    EuclidData data =
        extendedEuclid(value, modNumber);

    if (data.gcd != 1) {
        return false;
    }

    result = mod(data.x, modNumber);

    return true;
}

vector<long long> solveEquation(long long a,
                                long long b,
                                long long modNumber) {

    vector<long long> solutions;

    long long d =
        std::gcd(a, modNumber);

    if (b % d != 0) {
        return solutions;
    }

    if (d == 1) {

        long long inv;

        if (!inverse(a, modNumber, inv)) {
            return solutions;
        }

        solutions.push_back(
            mod(inv * b, modNumber)
        );

        return solutions;
    }

    long long reducedA = a / d;
    long long reducedB = b / d;
    long long reducedMod = modNumber / d;

    long long inv;

    if (!inverse(reducedA,
                 reducedMod,
                 inv)) {

        return solutions;
    }

    long long first =
        mod(inv * reducedB,
            reducedMod);

    for (long long i = 0; i < d; ++i) {

        solutions.push_back(
            first + i * reducedMod
        );
    }

    return solutions;
}

vector<string> splitUtf8(const string& text) {

    vector<string> result;

    for (size_t i = 0; i < text.size();) {

        unsigned char current = text[i];

        int length = 1;

        if ((current & 0xE0) == 0xC0) {
            length = 2;
        }
        else if ((current & 0xF0) == 0xE0) {
            length = 3;
        }
        else if ((current & 0xF8) == 0xF0) {
            length = 4;
        }

        result.push_back(
            text.substr(i, length)
        );

        i += length;
    }

    return result;
}

vector<string> alphabetToVector(const string& alphabet) {
    return splitUtf8(alphabet);
}

unordered_map<string, int>
buildAlphabetMap(const vector<string>& alphabet) {

    unordered_map<string, int> result;

    for (int i = 0; i < alphabet.size(); ++i) {
        result[alphabet[i]] = i;
    }

    return result;
}

string readText(const string& filename,
                const set<string>& alphabetSet) {

    ifstream file(filename);

    if (!file.is_open()) {

        cerr << "Cannot open file\n";
        exit(1);
    }

    string line;
    string result;

    while (getline(file, line)) {

        vector<string> letters =
            splitUtf8(line);

        for (const string& letter : letters) {

            if (alphabetSet.count(letter)) {
                result += letter;
            }
        }
    }

    return result;
}

int bigramNumber(const string& first,
                 const string& second,
                 const unordered_map<string, int>& alphabetMap) {

    return alphabetMap.at(first) * alphabetSize
           + alphabetMap.at(second);
}

pair<string, string>
numberBigram(int value,
             const vector<string>& alphabet) {

    int first = value / alphabetSize;
    int second = value % alphabetSize;

    return {
        alphabet[first],
        alphabet[second]
    };
}

map<string, int>
countBigrams(const vector<string>& letters,
             int shift) {

    map<string, int> result;

    for (size_t i = shift;
         i + 1 < letters.size();
         i += 2) {

        string bigram =
            letters[i] + letters[i + 1];

        result[bigram]++;
    }

    return result;
}

vector<pair<string, int>>
topFive(const map<string, int>& frequencies) {

    vector<pair<string, int>> result;

    for (const auto& item : frequencies) {
        result.push_back(item);
    }

    sort(result.begin(),
         result.end(),
         [](const auto& left,
            const auto& right) {

             return left.second > right.second;
         });

    if (result.size() > 5) {
        result.resize(5);
    }

    return result;
}

double letterFreq(const vector<string>& letters,
                  const string& symbol) {

    if (letters.empty()) {
        return 0.0;
    }

    int count = 0;

    for (const string& current : letters) {

        if (current == symbol) {
            count++;
        }
    }

    return static_cast<double>(count)
           / static_cast<double>(letters.size());
}

bool hasForbiddenBigram(const string& text) {

    for (const string& bigram : forbiddenBigrams) {

        if (text.find(bigram) != string::npos) {
            return true;
        }
    }

    return false;
}

int countFragment(const string& text,
                  const string& fragment) {

    int count = 0;

    size_t pos = 0;

    while (true) {

        pos = text.find(fragment, pos);

        if (pos == string::npos) {
            break;
        }

        count++;
        pos++;
    }

    return count;
}

int scoreText(const string& text) {

    if (hasForbiddenBigram(text)) {
        return -100000;
    }

    vector<string> letters =
        splitUtf8(text);

    if (letters.empty()) {
        return -100000;
    }

    int score = 0;

    double oFreq =
        letterFreq(letters, "о");

    double aFreq =
        letterFreq(letters, "а");

    double eFreq =
        letterFreq(letters, "е");

    double fFreq =
        letterFreq(letters, "ф");

    double shchaFreq =
        letterFreq(letters, "щ");

    double softFreq =
        letterFreq(letters, "ь");

    if (oFreq > 0.06) score += 20;
    if (aFreq > 0.03) score += 10;
    if (eFreq > 0.02) score += 10;

    if (fFreq < 0.02) score += 10;
    if (shchaFreq < 0.01) score += 10;
    if (softFreq < 0.05) score += 10;

    for (const string& bigram :
         russianTopBigrams) {

        score +=
            countFragment(text,
                          bigram) * 3;
    }

    for (const string& trigram :
         commonTrigrams) {

        score +=
            countFragment(text,
                          trigram) * 5;
    }

    auto cross =
        countBigrams(letters, 1);

    for (const string& bigram :
         commonCrossBigrams) {

        if (cross.count(bigram)) {
            score += cross[bigram] * 2;
        }
    }

    return score;
}

string decrypt(const vector<string>& cipher,
               long long a,
               long long b,
               const vector<string>& alphabet,
               const unordered_map<string, int>& alphabetMap) {

    long long inverseA;

    if (!inverse(a,
                 modValue,
                 inverseA)) {

        return "";
    }

    string result;

    size_t limit = cipher.size();

    if (limit % 2 != 0) {
        limit--;
    }

    for (size_t i = 0;
         i + 1 < limit;
         i += 2) {

        int y =
            bigramNumber(
                cipher[i],
                cipher[i + 1],
                alphabetMap
            );

        int x =
            mod(
                inverseA * (y - b),
                modValue
            );

        auto decoded =
            numberBigram(x,
                         alphabet);

        result += decoded.first;
        result += decoded.second;
    }

    return result;
}

set<pair<long long, long long>>
generateKeys(
    const vector<pair<string, int>>& topCipher,
    const unordered_map<string, int>& alphabetMap) {

    set<pair<long long, long long>> keys;

    for (int x1 = 0;
         x1 < russianTopBigrams.size();
         ++x1) {

        for (int x2 = 0;
             x2 < russianTopBigrams.size();
             ++x2) {

            if (x1 == x2) {
                continue;
            }

            vector<string> firstX =
                splitUtf8(
                    russianTopBigrams[x1]
                );

            vector<string> secondX =
                splitUtf8(
                    russianTopBigrams[x2]
                );

            int X1 =
                bigramNumber(
                    firstX[0],
                    firstX[1],
                    alphabetMap
                );

            int X2 =
                bigramNumber(
                    secondX[0],
                    secondX[1],
                    alphabetMap
                );

            for (int y1 = 0;
                 y1 < topCipher.size();
                 ++y1) {

                for (int y2 = 0;
                     y2 < topCipher.size();
                     ++y2) {

                    if (y1 == y2) {
                        continue;
                    }

                    vector<string> firstY =
                        splitUtf8(
                            topCipher[y1].first
                        );

                    vector<string> secondY =
                        splitUtf8(
                            topCipher[y2].first
                        );

                    int Y1 =
                        bigramNumber(
                            firstY[0],
                            firstY[1],
                            alphabetMap
                        );

                    int Y2 =
                        bigramNumber(
                            secondY[0],
                            secondY[1],
                            alphabetMap
                        );

                    long long left =
                        mod(X1 - X2,
                            modValue);

                    long long right =
                        mod(Y1 - Y2,
                            modValue);

                    auto possibleA =
                        solveEquation(
                            left,
                            right,
                            modValue
                        );

                    for (long long a : possibleA) {

                        if (std::gcd(a,
                                     (long long)modValue) != 1) {

                            continue;
                        }

                        long long b =
                            mod(
                                Y1 - a * X1,
                                modValue
                            );

                        keys.insert({a, b});
                    }
                }
            }
        }
    }

    return keys;
}

void analyze(const string& filename,
             const string& alphabetText,
             const string& title) {

    auto alphabet =
        alphabetToVector(alphabetText);

    set<string> alphabetSet(
        alphabet.begin(),
        alphabet.end()
    );

    auto alphabetMap =
        buildAlphabetMap(alphabet);

    string cipherText =
        readText(filename,
                 alphabetSet);

    auto letters =
        splitUtf8(cipherText);

    auto frequencies =
        countBigrams(letters, 0);

    auto topBigrams =
        topFive(frequencies);

    cout << "\n";
    cout << "========== "
         << title
         << " ==========\n\n";

    cout << "Top 5 bigrams:\n";

    for (const auto& item : topBigrams) {

        cout << item.first
             << " -> "
             << item.second
             << "\n";
    }

    auto keys =
        generateKeys(topBigrams,
                     alphabetMap);

    cout << "\nGenerated keys: "
         << keys.size()
         << "\n\n";

    vector<ResultText> accepted;

    int rejected = 0;

    for (const auto& key : keys) {

        string text =
            decrypt(
                letters,
                key.first,
                key.second,
                alphabet,
                alphabetMap
            );

        int score =
            scoreText(text);

        if (score < 120) {
            rejected++;
            continue;
        }

        accepted.push_back({
            key.first,
            key.second,
            score,
            text
        });
    }

    sort(accepted.begin(),
         accepted.end(),
         [](const ResultText& left,
            const ResultText& right) {

             return left.score > right.score;
         });

    cout << "Accepted keys: "
         << accepted.size()
         << "\n";

    cout << "Rejected keys: "
         << rejected
         << "\n\n";

    if (accepted.empty()) {

        cout << "No valid text found\n";
        return;
    }

    for (const auto& result : accepted) {

        cout << "FOUND KEY\n\n";

        cout << "a = "
             << result.a
             << "\n";

        cout << "b = "
             << result.b
             << "\n";

        cout << "score = "
             << result.score
             << "\n\n";

        cout << result.text
             << "\n\n";

        cout << "============================\n\n";
    }
}

int main() {

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string filename;

    cout << "Enter cipher file name: ";
    cin >> filename;

    analyze(
        filename,
        alphabet1,
        "Variant: ь after ы"
    );

    analyze(
        filename,
        alphabet2,
        "Variant: ь before ы"
    );

    return 0;
}