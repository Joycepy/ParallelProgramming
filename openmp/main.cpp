#include <bits/stdc++.h>
#include <omp.h>
#include <windows.h>
using namespace std;
int N=100000;
int numThread=4;
int arr[100000];

template<class T>
double calcu_time(void(*fun)(T)) {
	long long head, tail, freq;
	double t = 0;
	int rc = 0;
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	while (t<freq)
	{
		rc++;
		QueryPerformanceCounter((LARGE_INTEGER *)&head);
		fun(arr);
		QueryPerformanceCounter((LARGE_INTEGER *)&tail);
		t += (tail - head)*1000.0;
	}
	return (t / freq) / rc;
}

template<class T>
void sequential(T A)
{
	int temp;
	for (int i = 0; i < N; i++)
	{
		if (N % 2 == 0)
		{
			for (int j = N - 1; j > 0; j -= 2)
			{
				if (arr[j] < arr[j - 1])
				{
					temp = arr[j];
					arr[j] = arr[j - 1];
					arr[j - 1] = temp;
				}
			}
		}
		else
		{
			for (int j = N - 2; j > 0; j -= 2)
			{
				if (arr[j] < arr[j - 1])
				{
					temp = arr[j];
					arr[j] = arr[j - 1];
					arr[j - 1] = temp;
				}
			}
		}
	}
}
template<class T>
void OddEvenSort1(T A) {
	int exch0, exch1 = 1, trips = 0;
	while (exch1) {
		exch0 = 0;
		exch1 = 0;
#pragma omp parallel num_threads(numThread)
		{
#pragma omp for
			for (int i = 0; i < N - 1; i += 2) {
				if (A[i] > A[i + 1]) {
					int temp = A[i];
					A[i] = A[i + 1];
					A[i + 1] = temp;
					exch0 = 1;
				}
			}
			if (exch0 || !trips) {
#pragma omp for
				for (int i = 1; i < N - 1; i += 2) {
					if (A[i] > A[i + 1]) {
						int temp = A[i];
						A[i] = A[i + 1];
						A[i + 1] = temp;
						exch1 = 1;
					}
				}
			}
		}
		trips = 1;
	}
}
template<class T>
void OddEvenSort2(T A) {
    int exch0, exch1 = 1, trips = 0;
#pragma omp parallel num_threads(numThread)
	while (exch1) {
#pragma omp barrier
#pragma omp single
        {exch0 = 0;exch1 = 0;}
#pragma omp for
        for (int i = 0; i < N - 1; i += 2) {
            if (A[i] > A[i + 1]) {
                int temp = A[i];
                A[i] = A[i + 1];
                A[i + 1] = temp;
                exch0 = 1;
            }
        }
        if (exch0 || !trips) {
#pragma omp for
            for (int i = 1; i < N - 1; i += 2) {
                if (A[i] > A[i + 1]) {
                    int temp = A[i];
                    A[i] = A[i + 1];
                    A[i + 1] = temp;
                    exch1 = 1;
                }
            }
        }
		trips = 1;
	}
}
int main()
{
	ifstream infile;
	infile.open("test100000.txt");
	int* ptr = &arr[0];
	while (!infile.eof())
	{
		infile >> *ptr;
		ptr++;
	}
	infile.close();

	//double t1 = calcu_time(OddEvenSort1<decltype(arr)>);
	//cout<<"OddEvenSort1: "<<t1<<" ms"<<endl;

	double t2 = calcu_time(OddEvenSort2<decltype(arr)>);
	cout<<"OddEvenSort2: "<<t2<<" ms"<<endl;

	return 0;
}
