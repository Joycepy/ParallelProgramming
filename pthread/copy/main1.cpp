#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <immintrin.h> //AVX、AVX2
#include <windows.h>
#define NUM_THREADS 8
using namespace std;
int N;
float A[4096][4096];
typedef struct {
	int threadId;
	int startPos;
	bool cont;
} threadParm_t;
sem_t sem_start;
sem_t sem_parent;
sem_t sem_children;

void init() {
	for (int i = 0; i<N; i++)
		A[i][i] = 1;
}
void init2() {
    for (int i = 0; i < N; i++)
        A[i][i] = 4;
    for (int i = 0; i < N; i++)
        for (int j = i + 1; j < N; j++)
            A[i][j] = 2;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < i; j++)
            A[i][j] = 1;
}
bool check_res()
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (i == j){
				if (A[i][j] != 1)return false;
			}
			else {
				if (A[i][j] != 0)return false;
			}
	return true;

}

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
		fun(A);
		QueryPerformanceCounter((LARGE_INTEGER *)&tail);
		t += (tail - head)*1000.0;
	}
	return (t / freq) / rc;
}

template<class T>
void lu(T A) {
	for (int k = 0; k<N; k++) {
		for (int j = k + 1; j<N; j++)
			A[k][j] = A[k][j] / A[k][k];
		A[k][k] = 1;
		for (int i = k + 1; i<N; i++) {
			for (int j = k + 1; j<N; j++)
				A[i][j] = A[i][j] - A[i][k] * A[k][j];
			A[i][k] = 0;
		}
	}
}

//按行划分的多线程 LU 的子线程函数
template<class T>
void *threadFunc_row(void *parm)
{
	threadParm_t *p = (threadParm_t *)parm;
	while (true)
	{
		sem_wait(&sem_start);
		if (!p->cont)break;
		for (int i = p->startPos + p->threadId + 1; i < N; i += NUM_THREADS) {
			for (int j = p->startPos + 1; j < N; j++) {
				A[i][j] = A[i][j] - A[i][p->startPos] * A[p->startPos][j];
			}
			A[i][p->startPos] = 0.0;
		}
		sem_post(&sem_parent);
		sem_wait(&sem_children);
	}
	pthread_exit(nullptr);//返回结果给调用者
}

template<class T>
void pt_row(T A)
{
	threadParm_t threadParm[NUM_THREADS];
	pthread_t thread[NUM_THREADS];
	sem_init(&sem_parent, 0, 0);
	sem_init(&sem_start, 0, 0);
	sem_init(&sem_children, 0, 0);
	for (int i = 0; i < NUM_THREADS; i++)
	{
		threadParm[i].threadId = i;
		threadParm[i].cont = true;
		pthread_create(&thread[i], nullptr, threadFunc_row<T>, (void *)&threadParm[i]);
	}
	for (int k = 0; k < N; k++)
	{
		for (int j = k + 1; j < N; j++)
			A[k][j] /= A[k][k];
		A[k][k] = 1;
		for (int i = 0; i < NUM_THREADS; i++)
			threadParm[i].startPos = k;
		for (int i = 0; i < NUM_THREADS; i++)
			sem_post(&sem_start);
		for (int i = 0; i < NUM_THREADS; i++)
			sem_wait(&sem_parent);
		for (int i = 0; i < NUM_THREADS; i++)
			sem_post(&sem_children);
	}
	for (int i = 0; i < NUM_THREADS; i++)
		threadParm[i].cont = false;
	for (int i = 0; i < NUM_THREADS; i++)
		sem_post(&sem_start);
	for (int i = 0; i < NUM_THREADS; i++)
		pthread_join(thread[i], nullptr);
	sem_destroy(&sem_start);
	sem_destroy(&sem_children);
	sem_destroy(&sem_parent);
}

template<class T>
void *threadFunc_col(void *parm)
{
	threadParm_t *p = (threadParm_t *)parm;
	while (true)
	{
		sem_wait(&sem_start);
		if (!p->cont)break;
		for (int j = p->startPos + p->threadId + 1; j < N; j += NUM_THREADS) {
			for (int i = p->startPos + 1; i < N; i++) {
				A[i][j] = A[i][j] - A[i][p->startPos] * A[p->startPos][j];
			}
		}
		sem_post(&sem_parent);
		sem_wait(&sem_children);
	}
	pthread_exit(nullptr);
}

template<class T>
void pt_col(T A)
{
	threadParm_t threadParm[NUM_THREADS];
	pthread_t thread[NUM_THREADS];
	sem_init(&sem_parent, 0, 0);
	sem_init(&sem_start, 0, 0);
	sem_init(&sem_children, 0, 0);
	for (int i = 0; i < NUM_THREADS; i++)
	{
		threadParm[i].threadId = i;
		threadParm[i].cont = true;
		pthread_create(&thread[i], nullptr, threadFunc_row<T>, (void *)&threadParm[i]);
	}
	for (int k = 0; k < N; k++)
	{
		for (int j = k + 1; j < N; j++)
			A[k][j] /= A[k][k];
		A[k][k] = 1;
		for (int i = 0; i < NUM_THREADS; i++)
			threadParm[i].startPos = k;
		for (int i = 0; i < NUM_THREADS; i++)
			sem_post(&sem_start);
		for (int i = 0; i < NUM_THREADS; i++)
			sem_wait(&sem_parent);
		//主线程将第k列的k行以下的元素全置为0，在子线程完成之后进行
		for (int i = k + 1; i < N; i++)
			A[i][k] = 0.0;
		for (int i = 0; i < NUM_THREADS; i++)
			sem_post(&sem_children);
	}
	for (int i = 0; i < NUM_THREADS; i++)
		threadParm[i].cont = false;
	for (int i = 0; i < NUM_THREADS; i++)
		sem_post(&sem_start);
	for (int i = 0; i < NUM_THREADS; i++)
		pthread_join(thread[i], nullptr);
	sem_destroy(&sem_start);
	sem_destroy(&sem_children);
	sem_destroy(&sem_parent);
}

template<class T>
void *threadFunc_sse_row(void *parm)
{
	__m128 t1, t2, t3, t4;
	threadParm_t *p = (threadParm_t *)parm;
	while (true)
	{
		sem_wait(&sem_start);
		if (!p->cont)break;
		int k = p->startPos;
		for (int i =k+ p->threadId + 1; i < N; i += NUM_THREADS)
		{
			int left = (N-k-1) % 4;
			for (int j = k + 1; j < k + 1 + left; j++)
				A[i][j] = A[i][j] - A[i][k] * A[k][j];
			t1 = _mm_set1_ps(A[i][k]);
			for (int j = k + 1 + left; j < N; j += 4)
			{
				t2 = _mm_loadu_ps(A[k] + j);
				t3 = _mm_mul_ps(t1, t2);
				t4 = _mm_loadu_ps(A[i] + j);
				t4 = _mm_sub_ps(t4, t3);
				_mm_storeu_ps(A[i] + j, t4);
			}
			A[i][k] = 0;
		}
		sem_post(&sem_parent);
		sem_wait(&sem_children);
	}
	pthread_exit(nullptr);
}

template<class T>
void pt_sse_row(T A)
{
	threadParm_t threadParm[NUM_THREADS];
	pthread_t thread[NUM_THREADS];
	sem_init(&sem_parent, 0, 0);
	sem_init(&sem_start, 0, 0);
	sem_init(&sem_children, 0, 0);
	for (int i = 0; i < NUM_THREADS; i++)
	{
		threadParm[i].threadId = i;
		threadParm[i].cont = true;
		pthread_create(&thread[i], nullptr, threadFunc_sse_row<T>, (void *)&threadParm[i]);
	}
	__m128 t1, t2;
	for (int k = 0; k < N; k++)
	{
		//不能凑够4个，串行计算
		int left = (N-k-1) % 4;
		float f = 1 / A[k][k];
		for (int j = k + 1; j <= k + left; j++)
			A[k][j] = A[k][j] * f;
		t1 = _mm_set1_ps(f);
		for (int j = k + left + 1; j < N; j += 4)
		{
			t2 = _mm_load_ps(A[k] + j);
			t2 = _mm_mul_ps(t1, t2);
			_mm_storeu_ps(A[k] + j, t2);
		}
		A[k][k] = 1;
		for (int i = 0; i < NUM_THREADS; i++)
			threadParm[i].startPos = k;
		for (int i = 0; i < NUM_THREADS; i++)
			sem_post(&sem_start);
		for (int i = 0; i < NUM_THREADS; i++)
			sem_wait(&sem_parent);
		for (int i = 0; i < NUM_THREADS; i++)
			sem_post(&sem_children);
	}
	for (int i = 0; i < NUM_THREADS; i++)
		threadParm[i].cont = false;
	for (int i = 0; i < NUM_THREADS; i++)
		sem_post(&sem_start);
	for (int i = 0; i < NUM_THREADS; i++)
		pthread_join(thread[i], nullptr);
	sem_destroy(&sem_start);
	sem_destroy(&sem_children);
	sem_destroy(&sem_parent);
}

template<class T>
void *threadFunc_sse_col(void *parm)
{
	__m128 t1, t2, t3, t4;
	float F1[4],F2[4];
	threadParm_t *p = (threadParm_t *)parm;
	while (true)
	{
		sem_wait(&sem_start);
		if (!p->cont)break;
		int k = p->startPos;
		for (int j = k + p->threadId + 1; j < N; j += NUM_THREADS)
		{

			int left = (N-k-1) % 4;
			for (int i = k + 1; i < k + 1 + left; i++)
				A[i][j] = A[i][j] - A[i][k] * A[k][j];
			t1 = _mm_set1_ps(A[k][j]);
			for (int i = k + 1 + left; i < N; i += 4)
			{
				//把同一列连续的4个元素取出来放入128寄存器
				for (int w = 0; w < 4; w++)
				{
					F1[w] = A[i + w][k];
					F2[w] = A[i + w][j];
				}
				t2 = _mm_loadu_ps(F1);
				t3 = _mm_mul_ps(t1, t2);
				t4 = _mm_loadu_ps(F2);
				t4 = _mm_sub_ps(t4, t3);
				_mm_storeu_ps(F1, t4);
				for (int w = 0; w < 4; w++)
					A[i + w][j] = F1[w];
			}
		}
		sem_post(&sem_parent);
		sem_wait(&sem_children);
	}
	pthread_exit(nullptr);
}

template<class T>
void pt_sse_col(T A)
{
	threadParm_t threadParm[NUM_THREADS];
	pthread_t thread[NUM_THREADS];
	sem_init(&sem_parent, 0, 0);
	sem_init(&sem_start, 0, 0);
	sem_init(&sem_children, 0, 0);
	for (int i = 0; i < NUM_THREADS; i++)
	{
		threadParm[i].threadId = i;
		threadParm[i].cont = true;
		pthread_create(&thread[i], nullptr, threadFunc_sse_col<T>, (void *)&threadParm[i]);
	}
	__m128 t1, t2;
	for (int k = 0; k < N; k++)
	{
		int left = (N-k-1) % 4;
		float f = 1 / A[k][k];
		for (int j = k + 1; j <= k + left; j++)
			A[k][j] = A[k][j] * f;
		t1 = _mm_set1_ps(f);
		for (int j = k + left + 1; j < N; j += 4)
		{
			t2 = _mm_load_ps(A[k] + j);
			t2 = _mm_mul_ps(t1, t2);
			_mm_storeu_ps(A[k] + j, t2);
		}
		A[k][k] = 1;
		for (int i = 0; i < NUM_THREADS; i++)
			threadParm[i].startPos = k;
		for (int i = 0; i < NUM_THREADS; i++)
			sem_post(&sem_start);
		for (int i = 0; i < NUM_THREADS; i++)
			sem_wait(&sem_parent);
		for (int i = k + 1; i < N; i++)
			A[i][k] = 0;
		for (int i = 0; i < NUM_THREADS; i++)
			sem_post(&sem_children);
	}
	for (int i = 0; i < NUM_THREADS; i++)
		threadParm[i].cont = false;
	for (int i = 0; i < NUM_THREADS; i++)
		sem_post(&sem_start);
	for (int i = 0; i < NUM_THREADS; i++)
		pthread_join(thread[i], nullptr);
	sem_destroy(&sem_start);
	sem_destroy(&sem_children);
	sem_destroy(&sem_parent);
}

template<class T>
void *threadFunc_avx_row(void *parm)
{
	__m256 t1, t2, t3, t4;
	threadParm_t *p = (threadParm_t *)parm;
	while (true)
	{
		sem_wait(&sem_start);
		if (!p->cont)break;
		int k = p->startPos;
		for (int i = k + p->threadId + 1; i < N; i += NUM_THREADS)
		{

			int left = (N-k-1) % 8;
			for (int j = k + 1; j < k + 1 + left; j++)
				A[i][j] = A[i][j] - A[i][k] * A[k][j];
			t1 = _mm256_set1_ps(A[i][k]);
			for (int j = k + 1 + left; j < N; j += 8)
			{
				t2 = _mm256_loadu_ps(A[k] + j);
				t3 = _mm256_mul_ps(t1, t2);
				t4 = _mm256_loadu_ps(A[i] + j);
				t4 = _mm256_sub_ps(t4, t3);
				_mm256_storeu_ps(A[i] + j, t4);
			}
			A[i][k] = 0;
		}
		sem_post(&sem_parent);
		sem_wait(&sem_children);
	}
	pthread_exit(nullptr);
}

template<class T>
void pt_avx_row(T A)
{
	threadParm_t threadParm[NUM_THREADS];
	pthread_t thread[NUM_THREADS];
	sem_init(&sem_parent, 0, 0);
	sem_init(&sem_start, 0, 0);
	sem_init(&sem_children, 0, 0);
	for (int i = 0; i < NUM_THREADS; i++)
	{
		threadParm[i].threadId = i;
		threadParm[i].cont = true;
		pthread_create(&thread[i], nullptr, threadFunc_avx_row<T>, (void *)&threadParm[i]);
	}
	__m256 t1, t2;
	for (int k = 0; k < N; k++)
	{
		//不能凑够8个，串行计算
		int left = (N-k-1) % 8;
		float f = 1 / A[k][k];
		for (int j = k + 1; j <= k + left; j++)
			A[k][j] = A[k][j] * f;
		t1 = _mm256_set1_ps(f);
		for (int j = k + left + 1; j < N; j += 8)
		{
			t2 = _mm256_load_ps(A[k] + j);
			t2 = _mm256_mul_ps(t1, t2);
			_mm256_storeu_ps(A[k] + j, t2);
		}
		A[k][k] = 1;
		for (int i = 0; i < NUM_THREADS; i++)
			threadParm[i].startPos = k;
		for (int i = 0; i < NUM_THREADS; i++)
			sem_post(&sem_start);
		for (int i = 0; i < NUM_THREADS; i++)
			sem_wait(&sem_parent);
		for (int i = 0; i < NUM_THREADS; i++)
			sem_post(&sem_children);
	}
	for (int i = 0; i < NUM_THREADS; i++)
		threadParm[i].cont = false;
	for (int i = 0; i < NUM_THREADS; i++)
		sem_post(&sem_start);
	for (int i = 0; i < NUM_THREADS; i++)
		pthread_join(thread[i], nullptr);
	sem_destroy(&sem_start);
	sem_destroy(&sem_children);
	sem_destroy(&sem_parent);
}
int main()
{
	fstream fout; double t0,t1;
	vector<int> vec = {4,8,16,32,64,128,256,512,1024,2048,3072,4096};
	for (int i = 0; i < vec.size(); i++)
	{
		N = vec[i];
        init();
		fout << setiosflags(ios::fixed) << setprecision(4);
		fout.open("lu.txt", std::ios::out | std::ios::app);
		t0 = calcu_time(lu<decltype(A)>);
		fout<<NUM_THREADS<<" " << N << " " << t0 << " 1" << endl;
		fout.close();
		if (!check_res()){
			cout << "error with lu() at "<<N<< endl;
			break;
		}
		fout.open("pt_row.txt", std::ios::out | std::ios::app);
		t1 = calcu_time(pt_row<decltype(A)>);
		fout<<NUM_THREADS<<" " << N << " " << t1 << " " << t0 / t1 << endl;
		fout.close();
		if (!check_res()){
			cout << "error with pt_row() at "<<N<< endl;
			break;
		}
		fout.open("pt_col.txt", std::ios::out | std::ios::app);
		t1 = calcu_time(pt_col<decltype(A)>);
		fout<<NUM_THREADS<<" " << N << " " << t1 << " " << t0 / t1 << endl;
		fout.close();
		if (!check_res()){
			cout << "error with pt_col() at "<<N<< endl;
			break;
		}
		fout.open("pt_sse_row.txt", std::ios::out | std::ios::app);
		t1 = calcu_time(pt_sse_row<decltype(A)>);
		fout<<NUM_THREADS<<" " << N << " " << t1 << " " << t0 / t1 << endl;
		fout.close();
		if (!check_res()) {
			cout << "error with pt_sse_row() at "<<N<< endl;
			break;
		}
		fout.open("pt_sse_col.txt", std::ios::out | std::ios::app);
		t1 = calcu_time(pt_sse_col<decltype(A)>);
		fout<<NUM_THREADS<<" " << N << " " << t1 << " " << t0 / t1 << endl;
		fout.close();
		if (!check_res()) {
			cout << "error with pt_sse_col() at "<<N<< endl;
			break;
		}
		fout.open("pt_avx_row.txt", std::ios::out | std::ios::app);
		t1 = calcu_time(pt_avx_row<decltype(A)>);
		fout<<NUM_THREADS<<" " << N << " " << t1 << " " << t0 / t1 << endl;
		fout.close();
		if (!check_res()) {
			cout << "error with pt_avx_row() at "<<N<< endl;
			break;
		}
		cout<<"come to N = "<<N<<endl;
	}
	return 0;
}
