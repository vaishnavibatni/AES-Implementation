#include "stdafx.h"
#include <iostream>
#include "stdint.h"
#include <string>
#include <vector>
#include "stdio.h"
#include "keyexpansion.h"
#include "cipher.h"
using namespace std;

// Performs the SubWord substitution for KeyExpansion. 
vector <int> KeySubWord(vector <int> word)
{
	for (int row = 0; row < 4; row++) {
		word[row] = sbox[word[row] >> 4][word[row] & 0x0F];
		cout << hex << word[row] << ' ';
	}
	return(word);
}