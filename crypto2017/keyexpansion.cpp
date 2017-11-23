#include <string>
#include <vector>
#include "stdio.h"
#include "stdafx.h"
#include "stdint.h"
#include <iostream>
#include "cipher.h"
#include "keyexpansion.h"
using namespace std;

// making it global
int expandedKey[176];

int Rcon[255] = {
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
	0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
	0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
	0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
	0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
	0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
	0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
	0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
	0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
	0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
	0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
	0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
	0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
	0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
	0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
	0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb };

void rotateWord(int * word) {
	int temp = 0;
	temp = word[0];
	word[0] = word[1];
	word[1] = word[2];
	word[2] = word[3];
	word[3] = temp;
}

void substituteWord(int * word) {
	word[0] = sBoxGen(word[0]);
	word[1] = sBoxGen(word[1]);
	word[2] = sBoxGen(word[2]);
	word[3] = sBoxGen(word[3]);
}

int * KeyExpansion(int * key, int keysize) {
	
	int i, j;
	int word[4];
	int Nb = 4, Nr = 10, Nk = 4;

	// The first round key is the key itself.
	for (i = 0; i < Nk; i++) {
		expandedKey[i * 4] = key[i * 4];
		expandedKey[i * 4 + 1] = key[i * 4 + 1];
		expandedKey[i * 4 + 2] = key[i * 4 + 2];
		expandedKey[i * 4 + 3] = key[i * 4 + 3];
	}

	// All other round keys are found from the previous round keys.
	while (i < (Nb * (Nr + 1))) {
		for (j = 0; j < 4; j++) {
			word[j] = expandedKey[(i - 1) * 4 + j];
		}
		if (i % Nk == 0) {
			// Rotate 4 bytes in a word to the left.
			rotateWord(word);
			// Apply S-box to each of the four bytes.
			substituteWord(word);
			word[0] = word[0] ^ Rcon[i / Nk];
		}
		else if (Nk > 6 && i % Nk == 4) {
			substituteWord(word);
		}
		expandedKey[i * 4 + 0] = expandedKey[(i - Nk) * 4 + 0] ^ word[0];
		expandedKey[i * 4 + 1] = expandedKey[(i - Nk) * 4 + 1] ^ word[1];
		expandedKey[i * 4 + 2] = expandedKey[(i - Nk) * 4 + 2] ^ word[2];
		expandedKey[i * 4 + 3] = expandedKey[(i - Nk) * 4 + 3] ^ word[3];
		i++;
	}
	return expandedKey;
}
