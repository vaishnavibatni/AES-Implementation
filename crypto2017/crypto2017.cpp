/**
* AES implementation,
* Author : Anush Shrestha, Vaishanvi Batni, Arindam Mitra
* This is an implementation of FIPS 197 (csrc.nist.gov/publications/fips/fips197/fips-197.pdf)
* This has security optimisations and prevention from side channel attack.
*/

#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include "cipher.h"

using namespace std;
#define Nr 10
#define NB 4

//declare memory
int cipherkey[16] = { 0 };
int iv[16] = { 0 };


void AddRoundKey(int state[][4], int * roundKey, int round) {

	for (int i = 0; i < NB; ++i) {
		for (int j = 0; j < NB; ++j) {
			state[j][i] ^= roundKey[round * NB * 4 + i * NB + j];
		}
	}
}

// Cipher is the main function that encrypts the PlainText.
// update the state to cipher text
void Cipher(int state[][4], int * cipherKey) {
	int * expandedKeys = KeyExpansion(cipherKey, 128);
	uint8_t round = 0;

	// Add the First round key to the state before starting the rounds.
	AddRoundKey(state, expandedKeys, round);

	// There will be Nr rounds.
	// The first Nr-1 rounds are identical.
	// These Nr-1 rounds are executed in the loop below.
	for (round = 1; round < Nr; ++round)
	{
		SubBytes(state);
		ShiftRows(state);
		mixColumns(state);
		AddRoundKey(state, expandedKeys, round);
	}

	// The last round is given below.
	// The MixColumns function is not here in the last round.
	SubBytes(state);
	ShiftRows(state);
	AddRoundKey(state, expandedKeys, Nr);
}

void convertHexStringToState(char* pliantext, int state[][4]) {
	for (int i = 0; i < NB; i++) {
		for (int j = 0; j < NB; j++) {
			char c = pliantext[i * 8 + 2 * j];
			char c2 = pliantext[i * 8 + 2 * j + 1];
			int msb = ('0' <= c&&'9' >= c ? c - '0' : 10 + c - 'a');
			int lsb = ('0' <= c2&&'9' >= c2 ? c2 - '0' : 10 + c2 - 'a');
			state[j][i] = (msb *16 + lsb);
		}
	}
}

void convertStateToHexString(int state[][4], char * ciphertext) {
	int pos = 0;
	for (int i = 0; i < NB; i++) {
		for (int j = 0; j < NB; j++) {
			int lsb = state[j][i] % 16;
			int msb = state[j][i] / 16;
			ciphertext[pos++] = (msb <= 9 ? '0' : 'a' - 10) + msb;
			ciphertext[pos++] = (lsb <= 9 ? '0' : 'a' - 10) + lsb;
		}
	}
	ciphertext[32] = '\0';
}

void convertHexStringToKey(char * hexkey, int* cipherkey) {
	for (int i = 0; i < 33; i += 2) {
		char c = hexkey[i];
		char c2 = hexkey[i + 1];
		int msb = ('0' <= c&&'9' >= c ? c - '0' : 10 + c - 'a');
		int lsb = ('0' <= c2&&'9' >= c2 ? c2 - '0' : 10 + c2 - 'a');
		cipherkey[i / 2] = ( msb*16 + lsb);
	}
}

void displayState(int state[][4]) {
	cout << endl;
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			cout << hex << state[row][column] << ' ';
		}
		cout << endl;
	}
}



/** CBC implementation starts here**/
void XorWithIv(int state[][4], int *iv) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			state[j][i] ^= iv[i * 4 + j];
		}
	}
}

void convert2Dto1D(int state[][4], int *iv) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			iv[i * 4 + j] = state[j][i];
		}
	}
}

void AES_CBC_encrypt(char* input, int length, char* key, char* IV, char* output) {
	
	
	int state[4][4];
	char ciphertext[33];
	convertHexStringToKey(key, cipherkey);
	convertHexStringToKey(IV, iv);

	for (int i = 0; i < length; i++) {
		convertHexStringToState(input, state);
		XorWithIv(state, iv);
		
		Cipher(state, cipherkey);
		convertStateToHexString(state, output);
		
		
		convert2Dto1D(state, iv);
		input += 32;
		output += 32;
	}
}

void knownAnswerTest() {
	//declare memory
	
	int state[4][4];
	char ciphertext[33];
	ciphertext[32] = '\0';
	// convert key
	char key[33] = "00000000000000000000000000000000";

	convertHexStringToKey(key, cipherkey);

	// convert state
	char tv1[33] = "f34481ec3cc627bacd5dc3fb08f273e6";
	char to1[33] = "0336763e966d92595a567cc9ce537f5e";
	convertHexStringToState(tv1, state);
	//displayState(state);

	// pass it to cipher
	Cipher(state, cipherkey);
	// convert back the cipher text
	convertStateToHexString(state, ciphertext);



	cout << "Expected: " << to1 << endl;
	cout << "Computed: " << ciphertext << endl;
	if (strcmp(ciphertext, to1) == 0) {
		cout << "test case 1 passed" << endl;
	}
	else {
		cout << "test case 1 failed" << endl;
	}
}


void multiblockMessageTest() {
	char plaintext[65] = "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e51";
	char key[33] = "2b7e151628aed2a6abf7158809cf4f3c";
	char iv[33] = "000102030405060708090a0b0c0d0e0f";
	char output[65] = "7649abac8119b246cee98e9b12e9197d5086cb9b507219ee95db113a917678b2";
	char ciphertext[65];


	AES_CBC_encrypt(plaintext, 2, key, iv, ciphertext);
	cout << "Expected: " << output << endl;
	cout << "Computed: " << ciphertext << endl;

	if (strcmp(ciphertext, output) == 0) {
		cout << "test case 1 passed" << endl;
	}
	else {
		cout << "test case 1 failed" << endl;
	}

}

int main()
{	
	// Test cipher key from FIPS 197 Appendix A Page 27
	cout << "Testing single block message" << endl;
	cout << "----------------------------" << endl;
	knownAnswerTest();
	cout << "Testing single block message" << endl;
	cout << "----------------------------" << endl;
	multiblockMessageTest();
	std::getchar();
	return 0;
}
