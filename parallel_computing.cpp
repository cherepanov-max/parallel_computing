#include "pch.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <thread>
#include <omp.h>
#include <vector>
using namespace std;

int** matrix(int size) {
	int **p_mtx = new int*[size];
	for (int i = 0; i < size; i++) {
		p_mtx[i] = new int[size];
	}

	ofstream file;
	file.open("matrix.txt");

	for (int i = 0; i < size; i++) {
		cout << endl;
		for (int j = 0; j < size; j++) {
			p_mtx[i][j] = i;
			cout << p_mtx[i][j] << "; ";
			file << p_mtx[i][j] << endl;
		}
	}


	file.close();
	return p_mtx;
	for (int i = 0; i < size; i++) {
		delete[] p_mtx[i];
	}
	delete[] p_mtx;
}

int* vec(int size) {
	int *p_vec = new int[size];
	for (int i = 0; i < size; i++) {
		p_vec[i] = i;
	}

	ofstream file;
	file.open("vector.txt");


	for (int i = 0; i < size; i++) {
		p_vec[i] = i;
		cout << p_vec[i] << "; ";
		file << p_vec[i] << endl;
		}
	


	file.close();
	return p_vec;
	delete[] p_vec;
}

struct divide_result {
	string *vect;
	string *matr;
};

struct divide_result_fl {
	float *vect;
	float *matr;
};

divide_result str_vectors(string matrix_file, string vector_file, int size) {
	ifstream file(vector_file, ios::in | ios::binary);
	ifstream file_m(matrix_file, ios::in | ios::binary);

	string s;
	string *vec = new string[size];
	int v = 0;
	while (getline(file, s)) { // пока не достигнут конец файла класть очередную строку в переменную (s)
		/*cout << s << endl; // выводим на экран*/
		vec[v] = s;
		v++;
	}

	string m;
	string *mat = new string[size * size];
	v = 0;
	while (getline(file_m, m)) { // пока не достигнут конец файла класть очередную строку в переменную (s)
		/*cout << m << endl; // выводим на экран*/
		mat[v] = m;
		v++;
	}


	divide_result divide = { vec, mat };
	
	/*for (int j = 0; j < v; j++) {
		cout << mat[j] << endl;
	}*/
	file.close(); //Закрываем файл
	file_m.close(); //Закрываем файл
	return divide;
	delete[] vec;
	delete[] mat;
}

divide_result_fl float_vectors(divide_result vektor,  int size) {
	float *vec = new float[size];
	float *mat = new float[size * size];
	for (int q = 0; q < size; q++) {
		vec[q] = stof(vektor.vect[q]);
	}
	for (int q = 0; q < size * size; q++) {
		mat[q] = stof(vektor.matr[q]);
	}

	divide_result_fl divide = { vec, mat };
	return divide;
	delete[] vec;
	delete[] mat;
}

float *product(divide_result_fl vektors, int size) {
	float *resultat = new float[size];
	for (int i = 0; i < size; i++) {
		resultat[i] = 0;
		for (int j = 0; j < size; j++) {
			resultat[i] = resultat[i] + vektors.vect[j] * vektors.matr[i * size + j];
		}
		/*cout << resultat[i] << "; ";*/
	}
	
	return resultat;
	delete [] resultat;
}

float *product_th(divide_result_fl vektors, int size, int th_total, int th, float *resultat) {
	/*float *resultat = new float[size / th_total];*/
	int g = 0;
	
	for (int i = th * (size / th_total); i < (th + 1) * (size / th_total); i++) {
		resultat[g] = 0;
		float count = 0;
		int j = 0;
		for (j = 0; j < size; j++) {
			resultat[g] = resultat[g] + vektors.vect[j] * vektors.matr[i * size + j];
		}
		g++;
	}
	return resultat;
	delete[] resultat;
}

int main() {

	int size = 1000;
	int th_total = 8;
	float *resultat = new float[size];
	omp_set_nested(1);

	divide_result vektor = str_vectors("matrix.txt", "vector.txt", size);
	divide_result_fl f_vektor = float_vectors(vektor, size);
	clock_t t1 = clock();
	#pragma omp parallel for 
	for (int i = 0; i < size; i++) {
		resultat[i] = 0;
		int j = 0;
		float sum = 0;
		#pragma omp parallel for private (j) reduction (+:sum)
		for (j = 0; j < size; j++) {
			sum = sum + f_vektor.vect[j] * f_vektor.matr[i * size + j];
		}
		resultat[i] = sum;
	}
	clock_t t2 = clock();


	for (int q = 0; q < size; q++) {
		cout << q << " : " << resultat[q] << endl;
	}
	cout << t1 << "::" << t2 << "::" << t2 - t1;
	delete[] resultat;
	return 0;
}


