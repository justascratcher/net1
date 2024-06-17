#include <iostream>
#include <string>
#include <vector>
#include <array>
using namespace std;

using ULL = unsigned long long;
const ULL originSeed = 234531, originSeed2 = 8735434;
const int N = 8;

char conv(unsigned long long k) {
	if (k < 10) {
		return '0' + k;
	}
	else {
		return 'a' - 10 + k;
	}
}

unsigned int conv(char k) {
	if ('0' <= k && k <= '9') {
		return k - '0';
	}
	else {
		return k - 'a' + 10;
	}
}

class RANDOM
{
	static ULL seed;
	static const unsigned SIZE = 128;
	static ULL rand_consts[SIZE];
	static const ULL v1 = 12463423, v2 = 9263354725;
	static size_t idx;

	static ULL shuffle1(ULL a) {
		return (a >> 19) ^ (a >> 2) ^ (~a << 24) ^ (~a);
	}	

	static ULL shuffle2(ULL a, ULL b) {
		return (~b << 36) ^ (~b >> 6) ^ (~b << 43) ^ (~a >> 34) ^ a ^ (~a << 4) ^ v2;
	}

	static ULL shuffle3(ULL a) {
		return (~a << 42) ^ (~a & v2) ^ (~a << 17) ^ (~a >> 34) ^ (~a << 4);
	}

public:
	static void resetRandSeed(ULL newSeed) {
		seed = newSeed;
		idx = seed % SIZE;
		rand_consts[0] = seed ^ ( seed << 30 | v1);
		for (size_t i = 1; i < SIZE; i++) {
			rand_consts[i] = shuffle1(rand_consts[i - 1]);
		}
	}

	static ULL random() {
		rand_consts[idx] = shuffle2(rand_consts[idx], seed);
		seed ^= shuffle3(rand_consts[idx]);
		idx++, idx %= SIZE;
		return seed;
	}
};

ULL RANDOM::seed;
ULL RANDOM::rand_consts[RANDOM::SIZE];
size_t RANDOM::idx;

ULL rand1(ULL a, ULL b, ULL c) {
	return (a >> 30) ^ ~(b >> 14) ^ (c << 25);
}

ULL rand2(ULL a, ULL b, ULL c) {
	return (a >> 20) ^ ~(b >> 10) ^ ~(c >> 20);
}

ULL rand3(ULL a) {
	return (a >> 27) ^ ~(a << 18);
}

ULL rand4(ULL a, ULL b, ULL c) {
	return (a >> 12) ^ (b >> 30) ^ ~(b & 0xb1cfd623dc62123f);
}

ULL rand5(ULL a, ULL b) {
	return ~a ^ (a << 12 & b >> 13) ^ (b << 18) ^ (b >> 27);
}

ULL getAt(const vector<ULL>& bin, int idx) {
	return bin[idx % bin.size()];
}

string convertToHex(ULL v) {
	string ret;
	ret.resize(16, ' ');
	for (unsigned char k = 0; k < 16; k++) {
		ret[15 - k] = conv(v % 16);
		v >>= 4;
	}
	return ret;
}

vector<ULL> convertToInteger(string v) {
	vector<ULL> ret;
	if (v.empty()) return ret;
	for (int i = 0; i < v.size();) {
		ret.push_back(0);
		auto& last = ret.back();
		for (int j = 0; j < 8 && i < v.size(); j++, i++) {
			last <<= 8;
			last += v[i];
		}
	}
	return ret;
}

template <typename T>
string convertToHex(T v) {
	string ret;
	for (auto& i : v) {
		ret += convertToHex(i);
	}
	return ret;
}

string convertToChar(ULL v) {
	string ret;
	ret.resize(8, ' ');
	for (int j = 0; j < 8; j++) {
		ret[7-j] = char(v % 256);
		v >>= 8;
	}
	return ret;
}

template <typename T>
string convertToChar(T v) {
	string ret;
	for (auto i : v) {
		ret += convertToChar(i);
	}
	return ret;
}


vector<ULL> convertFromHexToInteger(string v) {
	vector<ULL> ret;
	if (v.empty()) return ret;
	for (int i = 0; i < v.size();) {
		ret.push_back(0);
		auto& last = ret.back();
		for (int j = 0; j < 16 && i < v.size(); j++, i++) {
			last <<= 4;
			last += conv(v[i]);
		}
	}
	return ret;
}

string HASH(string orig) {
	array<ULL, N> h;
	RANDOM::resetRandSeed(originSeed);
	for (int i = 0; i < N; i++) h[i] = RANDOM::random();
	while (orig.size() % N != 0) orig += "s";
	vector<unsigned long long> bin(orig.size() / 8, 0);
	for (int i = 0; i < orig.size(); i += 8) {
		for (int j = 0; j < 8; j++) {
			bin[i >> 3] <<= 8;
			bin[i >> 3] += orig[i + j];
		}
	}

	for (int i = 0; i < 64; i++) {
		for (int k = 0; k < bin.size(); k++) {
			ULL& ref = h[(k + i) % N];
			ref ^= rand4(getAt(bin, 6 * k + 14), getAt(bin, 4 * k + 1), getAt(bin, k + 12));
			ref ^= rand1(getAt(bin, k + 13), getAt(bin, 2 * k + 1), getAt(bin, k + 3));
			ref <<= bin[k] % 3;
			ref ^= rand2(getAt(bin, k), getAt(bin, 2 * k + 5), getAt(bin, 3 * k + 12));
			ref ^= rand3(getAt(bin, 4 * k + 123));
		}
	}

	for (int i = 0; i < 64; i++) {
		ULL store1 = h[7], store2 = h[6], store3 = h[4];
		h[7] = rand2(h[1], h[2], h[6]) + rand5(h[2], h[4]);
		h[6] = rand3(h[6]) + rand5(h[1], h[3]);
		h[5] = rand5(h[2], h[4]) + RANDOM::random();
		h[4] = rand3(h[3]) + rand4(h[1], h[0], h[3]) + RANDOM::random();
		h[3] = rand3(h[1]) + RANDOM::random();
		h[2] = rand3(h[0]) + rand4(h[1], h[1], h[0]);
		h[1] = rand1(store3, store1, store2) + RANDOM::random();
		h[0] = rand5(store1, store2);
		ULL v = h[7];
		for (int i = 7; i >= 1; i--) {
			h[i] = h[i - 1];
		}
		h[0] = v;
	}

	return convertToHex(h);
}

array<ULL, 8> HASH2(string orig) {
	array<ULL, N> h;
	RANDOM::resetRandSeed(originSeed2);
	for (int i = 0; i < N; i++) h[i] = RANDOM::random();
	while (orig.size() % N != 0) orig += "s";
	vector<unsigned long long> bin(orig.size() / 8, 0);
	for (int i = 0; i < orig.size(); i += 8) {
		for (int j = 0; j < 8; j++) {
			bin[i >> 3] <<= 8;
			bin[i >> 3] += orig[i + j];
		}
	}

	for (int i = 0; i < 64; i++) {
		for (int m = 0; m < bin.size(); m++) {
			for (int k = 0; k < bin.size(); k++) {
				ULL& ref = h[(k + i + m) % N];
				ref ^= rand4(getAt(bin, 6 * k + 14), getAt(bin, 4 * k + 1), getAt(bin, k + 12));
				ref ^= rand1(getAt(bin, k + 13), getAt(bin, 2 * k + 1), getAt(bin, k + 3));
				ref <<= bin[k] % 3;
				ref ^= rand2(getAt(bin, k), getAt(bin, 2 * k + 5), getAt(bin, 3 * k + 12));
				ref ^= rand3(getAt(bin, 4 * k + 123));
			}
		}
	}

	for (int i = 0; i < 64; i++) {
		ULL store1 = h[7], store2 = h[6], store3 = h[4];
		h[7] = rand2(h[1], h[2], h[6]) + rand5(h[2], h[4]);
		h[6] = rand3(store1) + rand5(h[1], h[3]);
		h[5] = rand5(h[2], store1) + RANDOM::random();
		h[4] = rand3(h[3]) + rand4(h[1], h[0], store2) + RANDOM::random();
		h[3] = rand3(store1) + RANDOM::random();
		h[2] = rand3(h[0]) + rand4(h[1], h[1], h[0]);
		h[1] = rand1(store3, store1, store2) + RANDOM::random();
		h[0] = rand5(store1, store2);
		ULL v = h[7];
		for (int i = 7; i >= 1; i--) {
			h[i] = h[i - 1];
		}
		h[0] = v;
	}

	return h;
}

vector<ULL> encode_Impl(vector<ULL> text, string code)
{
	size_t idx = 0;
	while (true)
	{
		auto m_hash = HASH2(code);
		for (size_t i = 0; i < m_hash.size(); i++)
		{
			if (idx >= text.size()) return text;
			text[idx] ^= m_hash[i];
			idx++;
		}
		code = convertToHex(HASH2(code));
	}
}

string encode(string plaintext, string code) {
	while (plaintext.size() % 8 != 0) plaintext += ' ';
	return convertToHex(encode_Impl(convertToInteger(plaintext), code));
}

string decode(string ciphertext, string code) {
	return convertToChar(encode_Impl(convertFromHexToInteger(ciphertext), code));
}

void output(string s[], int n) {
	for (int i = 0; i < n; i++) {
		cout << s[i] << endl;
	}
	cout << endl;
}

int main() {
	string Hash, ciphertext, code;
	bool locked = false, repeated = false, noTip = false;
	while (true) {
		if (locked)
		{
			if (!repeated)
			{		
				cout << "Hash of code(locked): " << Hash << endl;
				cout << "Ciphertext(locked): " << ciphertext << endl;
			}
			cout << "Code";
			if (!noTip) cout << "(input unlock to unlock hash and ciphertext | ntp to hide tips)";
			cout << ": ";
			cin >> code;
			repeated = true;
			if (code == "unlock") {
				locked = false;
				repeated = false;
				cout << "\nUnlocked!\n\n";
				continue;
			}
		}
		else
		{
			cout << "Hash of code";
			if (!noTip) cout << "(to confirm)";
			cout << ": ";
			cin >> Hash;
			cout << "Ciphertext: ";
			cin >> ciphertext;
			cout << "Code";
			if (!noTip) cout << "(input lock to lock hash and ciphertext | ntp to hide tips)";
			cout << ": ";
			cin >> code;
			if (code == "lock") {
				locked = true;
				repeated = false;
				cout << "\nLocked!\n\nNow input the code";
				if (!noTip) cout << "(ntp to hide tips)";
				cout << ": "; 
				cin >> code;
			}
		}

		if (code == "ntp") {
			noTip = true;
			cout << "\nTips are hidden!\n\nNow input the code: ";
			cin >> code;
		}

		if (HASH(code) != Hash)
			cout << "Wrong code.\n\n";
		else
			cout << decode(ciphertext, code) << endl << endl;
	}
	return 0;
}
