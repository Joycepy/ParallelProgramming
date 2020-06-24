#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;
int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	int my_rank, group_size;
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &group_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	double start_time;
	int N = 16;
	float** A;
	if (my_rank == 0) {
		start_time = MPI_Wtime();
		//��ʼ������
		A =new float*[N];
		for (int i = 0; i < N; i++)
			A[i] = new float[N];
		for (int i = 0; i < N; i++)
			A[i][i] = 4;
		for (int i = 0; i < N; i++)
			for (int j = i + 1; j < N; j++)
				A[i][j] = 2;
		for (int i = 0; i < N; i++)
			for (int j = 0; j < i; j++)
				A[i][j] = 1;
	}
	/*�����������̵��Ӿ����СΪn*N*/
	int n = N / group_size + (N%group_size ? 1 : 0);
	float **subA = new float*[n];
	for (int i = 0; i < n; i++)
		subA[i] = new float[N];

	if (my_rank == 0)
	{
		for (int i = 0; i < n; i++)
			for (int j = 0; j < N; j++)
				subA[i][j] = A[i*group_size][j];
		for (int i = 0; i < N; i++)
		{//�н��滮��
			if (i%group_size != 0)
				//����ÿһ�з��͵�dest=i%group_size, �������� tag=i / group_size + 1
				MPI_Send(&A[i][0], N, MPI_FLOAT, i%group_size, i / group_size + 1, MPI_COMM_WORLD);
			//group_size��������������rank=0����
		}
	}
	else
	{
		for (int i = 0; i < n; i++)
			MPI_Recv(&subA[i][0], N, MPI_FLOAT, 0, i + 1, MPI_COMM_WORLD,&status);
	}
	//��ʼ����ͨ����ϣ���ʼ����
	float* line = new float[N];//����Ԫ��
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < group_size; j++)
		{//���е���i����ĵ�j��
			int row = i * group_size + j;
			if (my_rank == j)
			{
				for (int k = row; k < N; k++)
					line[k] = subA[i][k];
			}
			MPI_Bcast(line, N, MPI_FLOAT, j, MPI_COMM_WORLD);
			//��������һ�п�ͷ��Ԫ��
			for (int k = i + 1; k < n; k++)
				subA[k][row] = subA[k][row] / line[row];
			//�������½�
			for (int k = i + 1; k < n; k++)
				for (int w = row + 1; w < N; w++)
					subA[k][w] = subA[k][w] - line[w] * subA[k][row];
		}
	}
	//rank=0�Ľ����ռ�������
	if (my_rank == 0)
	{
		for (int i = 1; i < group_size; i++)
		{
			for (int j = 0; j < n; j++)
			{
				MPI_Recv(&A[j*group_size + i][0], N, MPI_FLOAT, i, j, MPI_COMM_WORLD, &status);
			}
		}

		for (int i = 0; i < n; i++)
			for (int j = 0; j < N; j++)
				A[i*group_size][j] = subA[i][j];
		cout << MPI_Wtime() - start_time << endl;

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				cout << A[i][j] << " ";
			cout << endl;
		}
	}
	else
	{
		for (int i = 0; i < n; i++)
			MPI_Send(&subA[i][0], N, MPI_FLOAT, 0, i, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}