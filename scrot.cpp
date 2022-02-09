#include <iostream>
#include <string>
#include <sstream>
#include <time.h>

static inline uint16_t rol(uint16_t n, uint16_t c)
{
	const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);

	c &= mask;
	return (n << c) | (n >> ((-c)&mask));
}

uint16_t* G(uint16_t a, uint16_t b, uint16_t c, uint16_t d, uint16_t x, uint16_t y, uint16_t* value)
{
	a = (((a + b) % 65536) + x) % 65536;
	d = rol((d ^ a), 5);
	c = (c + d) % 65536;
	b = rol((b ^ c), 7);
	a = (((a + b) % 65536) + y) % 65536;
	d = rol((d ^ a), 3);
	c = (c + d) % 65536;
	b = rol((b ^ c), 14);
	value[0] = a;
	value[1] = b;
	value[2] = c;
	value[3] = d;

	return value;
}

uint16_t* permutation(uint16_t* m)
{
	uint16_t* x = new uint16_t[16];
	
	for(int i=0;i<16;++i)
		x[i]=m[i];

	m[0] = x[1];
	m[1] = x[14];
	m[2] = x[15];
	m[3] = x[13];
	m[4] = x[7];
	m[5] = x[2];
	m[6] = x[3];
	m[7] = x[6];
	m[8] = x[4];
	m[9] = x[8];
	m[10] = x[9];
	m[11] = x[12];
	m[12] = x[0];
	m[13] = x[10];
	m[14] = x[5];
	m[15] = x[11];
	
	delete[] x;

	return m;
}

uint16_t** makeRound(uint16_t** v, uint16_t* m)
{
	uint16_t* value = new uint16_t[4];

	value = G(v[0][0], v[1][0], v[2][0], v[3][0], m[0], m[1], value);
	v[0][0] = value[0];
	v[1][0] = value[1];
	v[2][0] = value[2];
	v[3][0] = value[3];
	value = G(v[0][1], v[1][1], v[2][1], v[3][1], m[2], m[3], value);
	v[0][1] = value[0];
	v[1][1] = value[1];
	v[2][1] = value[2];
	v[3][1] = value[3];
	value = G(v[0][2], v[1][2], v[2][2], v[3][2], m[4], m[5], value);
	v[0][2] = value[0];
	v[1][2] = value[1];
	v[2][2] = value[2];
	v[3][2] = value[3];
	value = G(v[0][3], v[1][3], v[2][3], v[3][3], m[6], m[7], value);
	v[0][3] = value[0];
	v[1][3] = value[1];
	v[2][3] = value[2];
	v[3][3] = value[3];

	value = G(v[0][0], v[1][1], v[2][2], v[3][3], m[8], m[9], value);
	v[0][0] = value[0];
	v[1][1] = value[1];
	v[2][2] = value[2];
	v[3][3] = value[3];
	value = G(v[0][1], v[1][2], v[2][3], v[3][0], m[10], m[11], value);
	v[0][1] = value[0];
	v[1][2] = value[1];
	v[2][3] = value[2];
	v[3][0] = value[3];
	value = G(v[0][2], v[1][3], v[2][0], v[3][1], m[12], m[13], value);
	v[0][2] = value[0];
	v[1][3] = value[1];
	v[2][0] = value[2];
	v[3][1] = value[3];
	value = G(v[0][3], v[1][0], v[2][1], v[3][2], m[14], m[15], value);
	v[0][3] = value[0];
	v[1][0] = value[1];
	v[2][1] = value[2];
	v[3][2] = value[3];

	delete[] value;

	return v;
}

uint16_t* prepareData(uint16_t* m, std::string wejscie)
{
	uint16_t q[32];
	for (int i = 0; i < wejscie.size(); ++i)
		q[i] = uint16_t(wejscie[i]);

	uint16_t dopelnienie = 32 - wejscie.size();
	q[wejscie.size()] = 128;
	for (int i = wejscie.size() + 1; i < dopelnienie+wejscie.size(); ++i)
		q[i] = 0;

	for (int i = 0, j = 0; i < 32; i += 2, ++j)
		m[j] = (q[i] << 8) ^ q[i + 1];
	
	return m;
}

std::string Blake3(const char* wejscie)
{
	uint16_t* m = new uint16_t[16];
	uint16_t w[8];
	for (int i = 0; i < 8; ++i)
		w[i] = 0;

	uint16_t** v = new uint16_t*[4];
	for (int i = 0; i < 4; ++i)
		v[i] = new uint16_t[4];

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			if (i == 2)
			{
				v[2][0] = 0x3553;
				v[2][1] = 0x74C1;
				v[2][2] = 0x0BDA;
				v[2][3] = 0x29F8;
				break;
			}
			else
				v[i][j] = 0;
		}

	prepareData(m, wejscie);
	
	for (int i = 0; i < 6; ++i)
	{
		v = makeRound(v, m);
		m = permutation(m);
	}


	for (int i = 0; i < 4; ++i)
	{
		w[i] = w[i] ^ v[0][i] ^ v[2][i];
		w[i + 4] = w[i + 4] ^ v[1][i] ^ v[3][i];
	}

	
	std::stringstream stream;
	for(int i=0;i<8;++i)
		stream << std::hex << w[i];

	delete[] m;
	for (int i = 0; i < 4; ++i)
		delete[] v[i];
	delete[] v;
	

	return stream.str();
}

bool compareHash(std::string wejscie)
{	
	 std::string hash = "ddc7cd0aa88f9930b1f559b5d58a7ac9"; // dla 2 znakow
	// std::string hash = "6cd99f585f3deacf1a71ea70485cd884"; // dla 3 znakow
	// std::string hash = "7a205d215c1b2a4432ed6c4a56288c92"; // dla 4 znakow
	// std::string hash = "2f74633826b6f261448e622ca84038d9"; // dla 5 znakow
	//std::string hash = "62c6c1f56881f7d185d95ac4ca38e1de"; // dla 6 znakow
	std::string possible = Blake3(wejscie.c_str());
	if (hash == possible)
	{
		std::cout << "ZNALAZLEM HASH!" << wejscie << '\n';
		return true;
	}
	return false;
}

bool printAllKLengthRec(const char* set, std::string prefix, int n, int k)
{
	if (k == 0)
	{
		std::cout << prefix << '\n';
		return compareHash(prefix);
	}

	
	for (int i = 0; i < n; i++)
	{
		std::string newPrefix = prefix + set[i];
				
		if (printAllKLengthRec(set, newPrefix, n, k - 1))
			return true;
	}
	return false;
}


int main(int argc, char **argv)
{
	char x[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890!@#$%^&*-_=+([{<)]\\}>\'\";:?,./|";
	int k = 2;

	clock_t time = clock();
	printAllKLengthRec(x, "", 91, k);
	time = clock() - time;

	std::cout << "done "<< (float)time/CLOCKS_PER_SEC;

	return(0);
}