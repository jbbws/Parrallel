#include <stdio.h>
#include <mpi/mpi.h>
#include <math.h>
double FInt(double argument)
{
    return 31*argument - log(5*argument)+5;
}

int main(int argc, char *argv[])
{ //Запуск будет произведен на 4 процессах
  //Этот файл разбивает формулу на части и каждый процесс вычисляет собственную часть формулы
    int procid,cntproc; //procid - номер процесса cntproc количество процессов коммутаторе
    int N=10; //число интервалов интегрирования
    double a = 1, b = 1000,result;//a-b -начальная и конечная точки,reuslt - буффер процессора
    double h=(b-a)/2*N; //шаг для вычисления узлов интегрирования, каждый из N интервалов делиться на два одинаковых на концах
    double sum,time,totaltime,fn,st;        //и середение которых вычисляются значения функций для полиноминальной функции
    MPI_Status stat;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&cntproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&procid);
    FILE* file;
    switch (procid)
    {
     case 0:
        sum = 0;
        st = MPI_Wtime();
        for(int i = 1;i<=3;i++)
            {
                MPI_Recv(&result,1,MPI_DOUBLE,MPI_ANY_SOURCE,666,MPI_COMM_WORLD,&stat);
                sum += result;
                MPI_Recv(&time,1,MPI_DOUBLE,MPI_ANY_SOURCE,13,MPI_COMM_WORLD,&stat);
                totaltime += time;
            }
            sum = h*sum/3;
         fn = MPI_Wtime();
         totaltime = totaltime + (fn-st);
         file = fopen("proc #0 log","w");
         fprintf(file,"Файл процесса %d",procid);
         fprintf(file,"Значение определенного интеграла %f",sum);
         fprintf(file,"Общезатраченное время %f",totaltime);
         fclose(file);
        break;
    case 1:                                     //первый процесс вычисляет сумму значений на концах общего интервала;
        result = 0;
        st = MPI_Wtime();
        result += FInt(a);
        result += FInt(b);
        fn = MPI_Wtime();
        MPI_Send(&result,1,MPI_DOUBLE,0,666,MPI_COMM_WORLD);
        time = fn-st;
        MPI_Send(&time,1,MPI_DOUBLE,0,13,MPI_COMM_WORLD);
        file = fopen("proc #1 log","w");
        fprintf(file,"Файл процесса %d",procid);
        fprintf(file,"Время выполенения на процессе %d : %f",procid,time);
        fclose(file);
        break;
    case 2:                             //второй процесс вычисляет сумму нечетных узлов
        result = 0;
        st = MPI_Wtime();
        for(int i=1;i<=N;i++)
            {
                int index = 2*i-1;
                result += FInt(a+index*h);
            }
        result *= 4;
        fn = MPI_Wtime();
        time = fn-st;
        MPI_Send(&result,1,MPI_DOUBLE,0,666,MPI_COMM_WORLD);
        MPI_Send(&time,1,MPI_DOUBLE,0,13,MPI_COMM_WORLD);
        file = fopen("proc #2 log","w");
        fprintf(file,"Файл процесса %d",procid);
        fprintf(file,"Время выполенения на процессе %d : %f",procid,time);
        fclose(file);
        break;
    case 3:                              //третий процесс вычисляет четных узлов
        break;
        result=0;
        st = MPI_Wtime();
        for(int i = 1;i<= N-1;i++)
            {
                int index = 2*i;
                result += FInt(a+index*h);
            }
        result *= 2;
        fn = MPI_Wtime();
        MPI_Send(&result,1,MPI_DOUBLE,0,666,MPI_COMM_WORLD);
        time = fn-st;
        MPI_Send(&time,1,MPI_DOUBLE,0,13,MPI_COMM_WORLD);
        file = fopen("proc #3 log","w");
        fprintf(file,"Файл процесса %d",procid);
        fprintf(file,"Время выполенения на процессе %d : %f",procid,time);
        fclose(file);
    default:
        break;
    };
    MPI_Finalize();
}
