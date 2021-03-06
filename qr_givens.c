//
//  qr_givens.c
//
//
//  Created by Amilcar Meneses Viveros on 22/06/20.
//
//

#include "qr_givens.h"
#include <omp.h>

double *a;
double *q;

void readMatrix(double *a, int m, int n, char *f) {
    FILE *fp;
    int i, l;
    l = m * n;
    //Abro el archivo para lectura y leo los datos de la misma
    fp = fopen(f, "r");
    for (i = 0; i < l; i++)
        fscanf(fp, "%lf", &(a[i]));
    //Cierro el fichero
    fclose(fp);
}

void printMatrix(double *a, int m, int n) {
    int i, j;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            printf("%4.3lf\t", a[i * n + j]);
        }
        printf("\n");
    }
}

void multiplyMatrices(double *a, double *b, double *c, int n) {
    int i, j, k;
    double tmp;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            tmp = 0.0;
            for (k = 0; k < n; k++) {
                tmp += b[i * n + k] * c[k * n + j];
            }
            a[i * n + j] = tmp;
        }
    }
}

void applyGivens(double *m, double *q, int i, int j, int n) {
    int k;
    double alfa, beta;
    //|a|
    //|b|
    //Obtengo el valor a procesar
    double b = m[i * n + j];
    //Obtengo su valor en la fila anterior(Esto no causa problema porque la posicion [0][0] no se procesa)
    double a = m[(i - 1) * n + j];
    //Obtengo r que es el valor or el que se sustituirá a
    double r = sqrt(a * a + b * b);
    //Obtnego el coseno
    double c = a / r;
    //Obtengo el seno
    double s = b / r;

    //Actualizo el valor de a
    m[(i - 1) * n + j] = r;
    //Hago 0 el valor de b
    m[i * n + j] = 0;

    //Actualizo los restantes valores en la matriz A
    for (k = j + 1; k < n; k++) {
        a = m[(i - 1) * n + k];//Obtendo el valor que voy a actualizar enla fila i-1  uI
        b = m[i * n + k];//Obtendo el valor que voy a utilizar para ello de la fila i vI
        m[(i - 1) * n + k] = c * a + s * b;
        m[i * n + k] = -s * a + c * b;
    }

    for (k = 0; k < n; k++) {
        alfa = q[(i - 1) * n + k];
        beta = q[i * n + k];
        if(alfa!=0.0 || beta!=0.0){
            q[(i - 1) * n + k] = c * alfa + s * beta;
            q[i * n + k] = -s * alfa + c * beta;
        }
    }
}

int canRun(int *posicion, int column,int row, int n){
    for (int i = 0; i <n ; ++i) {
        if(column-posicion[i*2]==1)
            return row-posicion[i*2+1]>=2;
    }
    return 1;
}

int nextColumn(int *posicion, int n,int maxColumn){
    int max=-1;
    for (int i = 0; i <n ; ++i) {
        if(posicion[i*2]>max)
            max=posicion[i*2];
    }
    max++;
    return max==maxColumn ? -1 : max;
}

void QR_Givens(double *a, double *q, int n) {
    int i, j;
    double g[2][2];
    int *positions;
    int nthreads, tid, column, row, finish,nextCol;
    finish = 0;
    omp_set_num_threads(3);

#pragma omp parallel shared(positions, nthreads) private(tid,i,column,row,nextCol)
    {
        tid = omp_get_thread_num();

#pragma omp master
        {
            nthreads = omp_get_num_threads();
            positions = (int *) calloc(nthreads * 2, sizeof(int));
            for (j = 0; j < nthreads; j++) {
                positions[j * 2] = j;
                positions[j * 2 + 1] = n - 1;
            }
        }

#pragma omp barrier

        column = positions[tid * 2];
        row = positions[tid * 2 + 1];

        while (finish == 0 && column != -1) {
            if ((column == 0) || (canRun(positions, column, row, nthreads)) == 1) {
                for (i = row; i > column; i--) {
                    if (a[i * n + column] != 0.0)
                        applyGivens(a, q, i, column, n);
                    positions[tid * 2 + 1]--;
                }
                if (column == n - 1){
                    finish = 1;
                }
                else {
                    #pragma omp critical
                    {
                        nextCol=nextColumn(positions, nthreads, n);
                        printf("Hilo %d, columna %d\n",tid, nextCol);
                        positions[tid * 2] =nextCol;
                        positions[tid * 2 + 1] = n - 1;
                        column = positions[tid * 2];
                        row = positions[tid * 2 + 1];
                    }
                }
            }

        }

        #pragma omp barrier
    }
    free(positions);
}

void swap(double *a, double *b) {
    double tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void transposeMatrix(double *a, int n) {
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            swap(&(a[i * n + j]), &(a[j * n + i]));
        }
    }
}

int main(int argc, char **argv) {
    int n;
    int i;
    double *p;
    // double t0, t1, t2, t3;

    // t0 = omp_get_wtime();
    n = atoi(argv[1]);
    //Inicializo la matriz cuadrada de datos(SERA LA TRIANGULAR SUPERIOR)
    a = (double *) malloc(n * n * sizeof(double));
    //Inicializo la matriz cuadrada que será Q
    q = (double *) calloc(n * n, sizeof(double));
    //Inicializo la matriz donde comprobaré si q*a da la matriz original
    p = (double *) malloc(n * n * sizeof(double));
    //Inicializo Q como la matriz identidad
    for (i = 0; i < n; i++)
        q[i * n + i] = 1.0;

    readMatrix(a, n, n, argv[2]);
    //  t1 = omp_get_wtime();
    QR_Givens(a, q, n);
    //  t2 = omp_get_wtime();
    printf("\n========Imprime A(Mtz Triangular Superior)=========\n");
    printMatrix(a, n, n);
    printf("\n=======Imprime Q==========\n");
    printMatrix(q, n, n);
    transposeMatrix(q, n);
    multiplyMatrices(p, q, a, n);
    printf("\n=======Comprueba que se obtenga la mtz original==========\n");
    printMatrix(p, n, n);
    //t3 = omp_get_wtime();

    free(a);
    free(q);
    free(p);
}
