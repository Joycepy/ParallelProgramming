#include <bits/stdc++.h>
#include <immintrin.h>
#include <windows.h>
using namespace std;
//64,128,256,512,1024,2048
const int N =2048;
float A[N][N];
void init()
{
    for(int i=0;i<N;i++)
        A[i][i]=1;
}
#define plain {\
    for(int k=0;k<N;k++)\
    {\
        for(int j=k+1;j<N;j++)\
            A[k][j]=A[k][j]/A[k][k];\
        A[k][k]=1;\
        for(int i=k+1;i<N;i++)\
        {\
            for(int j=k+1;j<N;j++)\
                A[i][j]= A[i][j] - A[i][k]*A[k][j];\
            A[i][k]=0;\
        }\
    }\
}
#define sse_unaligned_p {\
    for(int k=0;k<N;k++)\
    {\
        __m128 t1,t2,t3,t4;\
        for(int j=k+1;j<N;j++)\
            A[k][j]=A[k][j]/A[k][k];\
        A[k][k]=1;\
        for(int i=k+1;i<N;i++)\
        {\
            /*先把不能凑够4个的部分串行计算*/\
            int left=(N-k-1)%4;\
            for(int j=k+1;j<=k+left;j++)\
                A[i][j]= A[i][j] - A[i][k]*A[k][j];\
             t1 = _mm_set1_ps(A[i][k]);\
             /*并行计算剩下4的倍数个float*/\
             for(int j=k+left+1;j<N;j+=4)\
             {\
                 t2=_mm_loadu_ps(A[k]+j);\
                 t3=_mm_mul_ps(t1,t2);\
                 t4=_mm_loadu_ps(A[i]+j);\
                 t4=_mm_sub_ps(t4,t3);\
                 _mm_storeu_ps(A[i]+j,t4);\
             }\
             A[i][k]=0;\
        }\
    }\
}
#define sse_aligned_p {\
    for(int k=0;k<N;k++)\
    {\
        __m128 t1,t2,t3,t4;\
        for(int j=k+1;j<N;j++)\
            A[k][j]=A[k][j]/A[k][k];\
        A[k][k]=1;\
        for(int i=k+1;i<N;i++)\
        {\
            /*先把不能凑够4个的部分串行计算*/\
            int left=(N-k-1)%4;\
            for(int j=k+1;j<=k+left;j++)\
                A[i][j]= A[i][j] - A[i][k]*A[k][j];\
             t1 = _mm_set1_ps(A[i][k]);\
             /*并行计算剩下4的倍数个float*/\
             for(int j=k+left+1;j<N;j+=4)\
             {\
                 t2=_mm_load_ps(A[k]+j);\
                 t3=_mm_mul_ps(t1,t2);\
                 t4=_mm_load_ps(A[i]+j);\
                 t4=_mm_sub_ps(t4,t3);\
                 _mm_store_ps(A[i]+j,t4);\
             }\
             A[i][k]=0;\
        }\
    }\
}
#define sse_aligned_all {\
    for(int k=0;k<N;k++)\
    {\
        __m128 t1,t2,t3,t4;\
        /*不能凑够4个的部分串行计算*/\
        int left=(N-k-1)%4;\
        for(int j=k+1;j<=k+left;j++)\
            A[k][j]=A[k][j]/A[k][k];\
        /*并行计算剩下4的倍数个，减少使用除法*/\
        t1 = _mm_set1_ps(1/A[k][k]);\
        for(int j=k+left+1;j<N;j+=4)\
        {\
            t2=_mm_load_ps(A[k]+j);\
            t2=_mm_mul_ps(t1,t2);\
            _mm_store_ps(A[k]+j,t2);\
        }\
        A[k][k]=1;\
        for(int i=k+1;i<N;i++)\
        {\
            /*不能凑够4个的部分串行计算*/\
            for(int j=k+1;j<=k+left;j++)\
                A[i][j]= A[i][j] - A[i][k]*A[k][j];\
             t1 = _mm_set1_ps(A[i][k]);\
             /*并行计算剩下4的倍数个float*/\
             for(int j=k+left+1;j<N;j+=4)\
             {\
                 t2=_mm_load_ps(A[k]+j);\
                 t3=_mm_mul_ps(t1,t2);\
                 t4=_mm_load_ps(A[i]+j);\
                 t4=_mm_sub_ps(t4,t3);\
                 _mm_store_ps(A[i]+j,t4);\
             }\
             A[i][k]=0;\
        }\
    }\
}
#define sse_unaligned_all {\
    for(int k=0;k<N;k++)\
    {\
        __m128 t1,t2,t3,t4;\
        /*不能凑够4个的部分串行计算*/\
        int left=(N-k-1)%4;\
        for(int j=k+1;j<=k+left;j++)\
            A[k][j]=A[k][j]/A[k][k];\
        /*并行计算剩下4的倍数个，减少使用除法*/\
        t1 = _mm_set1_ps(1/A[k][k]);\
        for(int j=k+left+1;j<N;j+=4)\
        {\
            t2=_mm_loadu_ps(A[k]+j);\
            t2=_mm_mul_ps(t1,t2);\
            _mm_storeu_ps(A[k]+j,t2);\
        }\
        A[k][k]=1;\
        for(int i=k+1;i<N;i++)\
        {\
            /*不能凑够4个的部分串行计算*/\
            for(int j=k+1;j<=k+left;j++)\
                A[i][j]= A[i][j] - A[i][k]*A[k][j];\
             t1 = _mm_set1_ps(A[i][k]);\
             /*并行计算剩下4的倍数个float*/\
             for(int j=k+left+1;j<N;j+=4)\
             {\
                 t2=_mm_loadu_ps(A[k]+j);\
                 t3=_mm_mul_ps(t1,t2);\
                 t4=_mm_loadu_ps(A[i]+j);\
                 t4=_mm_sub_ps(t4,t3);\
                 _mm_storeu_ps(A[i]+j,t4);\
             }\
             A[i][k]=0;\
        }\
    }\
}
#define avx_unaligned_all {\
    for(int k=0;k<N;k++)\
    {\
        __m256 t1,t2,t3,t4;\
        /*不能凑够8个的部分串行计算*/\
        int left=(N-k-1)%8;\
        for(int j=k+1;j<=k+left;j++)\
            A[k][j]=A[k][j]/A[k][k];\
        /*并行计算剩下8的倍数个，减少使用除法*/\
        t1 = _mm256_set1_ps(1/A[k][k]);\
        for(int j=k+left+1;j<N;j+=8)\
        {\
            t2=_mm256_loadu_ps(A[k]+j);\
            t2=_mm256_mul_ps(t1,t2);\
            _mm256_storeu_ps(A[k]+j,t2);\
        }\
        A[k][k]=1;\
        for(int i=k+1;i<N;i++)\
        {\
            /*不能凑够8个的部分串行计算*/\
            for(int j=k+1;j<=k+left;j++)\
                A[i][j]= A[i][j] - A[i][k]*A[k][j];\
             t1 = _mm256_set1_ps(A[i][k]);\
             /*并行计算剩下8的倍数个float*/\
             for(int j=k+left+1;j<N;j+=8)\
             {\
                 t2=_mm256_loadu_ps(A[k]+j);\
                 t3=_mm256_mul_ps(t1,t2);\
                 t4=_mm256_loadu_ps(A[i]+j);\
                 t4=_mm256_sub_ps(t4,t3);\
                 _mm256_storeu_ps(A[i]+j,t4);\
             }\
             A[i][k]=0;\
        }\
    }\
}
void print()
{
    fstream fout;
	fout.open("res-plain.txt", std::ios::out | std::ios::app);
    fout<<setiosflags(ios::fixed)<<setprecision(4);
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
            fout<<setw(10)<<A[i][j];
        fout<<endl;
    }
    fout.close();
}
const char* path[6]={"time_plain.txt","time_sse_unaligned_p.txt","time_sse_unaligned_all.txt","time_sse_aligned_p.txt","time_sse_aligned_all.txt","time_avx_unaligned_all.txt"};
void get_time(int index)
{
    long long head,tail,freq,Tsum=0;
    fstream f;
	//f.open(path[index], std::ios::out | std::ios::app);
	f.open("time_plain_auto.txt", std::ios::out | std::ios::app);
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	int count=5000/N;
	int i=count;
	while(i--)
    {
        QueryPerformanceCounter((LARGE_INTEGER*)&head);


    for(int k=0;k<N;k++)
    {
        for(int j=k+1;j<N;j++)
            A[k][j]=A[k][j]/A[k][k];
        A[k][k]=1;
        for(int i=k+1;i<N;i++)
        {
            for(int j=k+1;j<N;j++)
                A[i][j]= A[i][j] - A[i][k]*A[k][j];
            A[i][k]=0;
        }
    }
        //plain;
        //sse_unaligned_p;
        //sse_unaligned_all;
        //sse_aligned_p;
        //sse_aligned_all;
        //avx_unaligned_all;
        //plain_col;
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        Tsum+=(tail-head);
    }
    f<<N<<": "<<Tsum*1000.0/freq/count<<"ms"<<endl;
    f.close();
}
int main()
{
    init();
    get_time(0);
    //print();

    return 0;
}
