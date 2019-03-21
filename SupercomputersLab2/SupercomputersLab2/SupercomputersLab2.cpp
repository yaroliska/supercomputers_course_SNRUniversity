// SupercomputersLab2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "pch.h"
#include <iostream>
#include <windows.h>
#include <random>
#include <ctime>
#include <cstdlib>
#include <omp.h>

using namespace std;
int const N = 2000;
__int64 start_count;
__int64	end_count;
__int64 tps;

int A[N][N];
int B[N][N];
int C[N][N];

//возвращает рандомное число double
double getRandomNumber(int min, int max)
{
	return ((double)min + (double)rand() / (double)RAND_MAX)*10;
}

void randomMatr() {
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++) {
			A[i][j] = rand() % 1000;
			B[i][j] = rand() % 1000;
			C[i][j] = 0;
		}
}

//заполняет матрицу рандомными числами
void randomizeMatrix(double* matrix, int size) {
	for (int i = 0; i < size*size; i++) {
		matrix[i] = getRandomNumber(0, 100);
	}
}

//функция инициализации матриц нулями
double* createNullable(int size) {
	double* matrix = new double[size*size];
	for (int i = 0; i < size*size; i++)
		matrix[i] = 0;
	return matrix;
}
void initialize_C(int n) {
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			C[i][j] = 0;
		}
	}
}
//вывод матрицы на экран
void printMatrix(double* matrix, int size) {
	printf_s("Matrix\n");
	for (int i = 0; i < size * size; i++)
	{
		printf_s("%f ", matrix[i]);
		if (i % size == size - 1)
			printf_s("\n");
	}
}
//for testing
void initializeMatrix(double* &matrixA, double* &matrixB, double* &matrixC) {
	matrixA = new double[9]{ -1, 3, 5, -4, 7, 1, 9, 2, 0 };
	matrixB = new double[9]{ 3, 2, -8, -7, 4, 5, 1, 0, -6 };
	matrixC = new double[9]{ 0,0,0,0,0,0,0,0,0};
}
void copyMatr(double *from, double* &to,int size) {
	for (int i = 0; i <size*size; i++) {
		to[i] = from[i];
	}
}
int getPosition(int i, int j, int size) {
	int pos = 0;
	pos = i * size + j;
	return pos;
}

void start_t() {
	QueryPerformanceFrequency((LARGE_INTEGER *)&tps);
	QueryPerformanceCounter((LARGE_INTEGER *)&start_count);
}

void stop_t() {
	QueryPerformanceCounter((LARGE_INTEGER *)&end_count);
}

double get_res_time() {
	return (double)(end_count - start_count) / tps;
}

double* transposeMatrix(double* matrixA, int size) {
	double* result;
	result = createNullable(size);
	
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[j*size+i] = matrixA[i*size+j];
		}
	}
	return result;
}

void multiplyMatrix(double* matrixA, double* matrixB, double* &matrixC, int size) {
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j) {
			for (int k = 0; k < size; ++k)
				matrixC[i*size+j] = matrixA[i*size+k] * matrixB[k*size+j];
		}

}
void multiplyMatrixTranspose(double* matrixA, double* matrixB, double* &matrixC, int size) {
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j) {
			for (int k = 0; k < size; ++k)
				matrixC[i*size + j] = matrixA[i*size + k] * matrixB[j*size + k];
		}
}

void block_multiple(int block_count, int block_size, int size) {
	for (int a = 0; a < block_count; a++) {//от 0 до количества блоков 
		for (int b = 0; b < block_count; b++) {//от 0 до количества блоков (a,b -определяют результирующий блок в итоговой матрице над которым работаем)
			for (int c = 0; c < block_count; c++) {//0 до количества блоков (c - совершает перемножение нужных блоков с их суммированием в итоговый
				for (int i = a * block_size; i < (a + 1)*block_size; i++)
					for (int j = b * block_size; j < (b + 1)*block_size; j++)
						for (int k = c * block_size; k < (c + 1)*block_size; k++)
							C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

int main(int argc, char **argv) {

	double* matrixC; //объявление матриц
	double* matrixA;
	double* matrixBNormal;
	double* matrixBTranspose;
	int size = 1024;//размеры матриц

	//инициализации матриц
	matrixA = createNullable(size);
	matrixBNormal = createNullable(size);
	matrixBTranspose = createNullable(size);
	matrixC = createNullable(size);
	
	//initializeMatrix(matrixA, matrixBNormal, matrixC);
	//заполнение матриц рандомными числами
	randomizeMatrix(matrixA, size);
	randomizeMatrix(matrixBNormal, size);
	//printMatrix(matrixA, size);

	//транспонирование матрицы B
	matrixBTranspose=transposeMatrix(matrixBNormal, size);
	/*printMatrix(matrixA, size);
	printMatrix(matrixBNormal, size);
	printMatrix(matrixC, size);
	printMatrix(matrixBTranspose,size);*/

	unsigned __int64 t11, t12,t111,t122;
	const unsigned __int64 f = 2400000000;
	t11 = __rdtsc();
	//функция умножения матриц
	multiplyMatrix(matrixA, matrixBNormal, matrixC, size);
	t12 = __rdtsc();
	printf("matrixBNormal");
	cout << "rdtsc, " << (((t12 - t11) / (double)f) * 1000) << " milliseconds" << endl;
	printf(" ");

	t111 = __rdtsc();
	//функция умножения матриц
	multiplyMatrixTranspose(matrixA, matrixBTranspose, matrixC, size);
	t122 = __rdtsc();
	printf("matrixBTranspose");
	cout << "rdtsc, " << (((t122 - t111) / (double)f) * 1000) << " milliseconds" << endl;
	printf(" ");




	printf("blockMultiplication");
	int n = 1024;
	int sizes[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
	int count = 10;
	int state = 0;
	int block_size = sizes[state];
	randomMatr();

	bool flag = true;
	while (flag) {

		cout << "block size = " << block_size << endl;

		int block_count = n / block_size;

		initialize_C(n);
		start_t();
		block_multiple(block_count, block_size, n);
		stop_t();

		cout << "time = " << get_res_time() << endl;

		state++;
		if (state > count - 1) {
			flag = false;
		}
		else
		{
			block_size = sizes[state];
		}

	}


	system("pause");
	return 0;
}

