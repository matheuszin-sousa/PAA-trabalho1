#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include <time.h>
#include <math.h>

typedef struct AlgoData
{
    int tam_lista;
    char *algo_name;
    char *tipo;
    double tempo;
    long long comparacoes;
    long long trocas;
} AlgoData;

typedef struct FileData
{
    char *file_type;
    int file_tam;
} FileData;

FileData readFileData(char *filepath)
{
    FileData f;
    const char *delimiter = "_";
    char *tokenSec, *temp;

    temp = strtok(filepath, "/");
    tokenSec = strtok(NULL, "/");

    char *ntoken = strtok(tokenSec, delimiter);
    f.file_type = ntoken;
    ntoken = strtok(NULL, delimiter);
    f.file_tam = atoi(ntoken);
    return f;
}

glob_t getArquivosInfos()
{
    glob_t gstruct;
    int code;
    code = glob("inputs/*", GLOB_ERR, NULL, &gstruct);

    if (code != 0)
    {
        if (code == GLOB_NOMATCH)
            fprintf(stderr, "No matches!!!!\n");
        exit(1);
    }
    return gstruct;
}

int *returnArrayFromFile(FILE *p, int listSize)
{
    int *arr = (int *)malloc(listSize * sizeof(int));
    for (int i = 0; i < listSize; i++)
        fscanf(p, "%d,", &arr[i]);
    return arr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int updatedGap(int gap)
{
    gap = floor(gap / 1.3);
    if (gap < 1)
        return 1;
    return gap;
}

void merge_sort(int i, int j, int a[], int aux[], AlgoData *A)
{
    if (j <= i)
    {
        return; 
    }
    int mid = (i + j) / 2;

    merge_sort(i, mid, a, aux, A);     
    merge_sort(mid + 1, j, a, aux, A); 
    int pointer_left = i;              
    int pointer_right = mid + 1;      
    int k;                             
    for (k = i; k <= j; k++)
    {
        if (pointer_left == mid + 1)
        { 
            aux[k] = a[pointer_right];
            pointer_right++;
        }
        else if (pointer_right == j + 1)
        { 
            aux[k] = a[pointer_left];
            pointer_left++;
        }
        else if (a[pointer_left] < a[pointer_right])
        { 
            aux[k] = a[pointer_left];
            pointer_left++;
        }
        else
        { 
            aux[k] = a[pointer_right];
            pointer_right++;
        }
        A->comparacoes++;
    }
    for (k = i; k <= j; k++)
    { 
        a[k] = aux[k];
    }
}

void shellSort(int *array, int n, AlgoData *a)
{
    for (int interval = n / 2; interval > 0; interval /= 2)
    {
        for (int i = interval; i < n; i += 1)
        {
            int temp = array[i];
            int j;
            a->comparacoes++;

            for (j = i; j >= interval && array[j - interval] > temp; j -= interval)
            {
                array[j] = array[j - interval];
                a->trocas++;
            }
            array[j] = temp;
        }
    }
}

void combSort(int *a, int n, AlgoData *A)
{
    int gap = n;
    int swapped = 1;
    while (gap != 1 || swapped == 1)
    {
        A->comparacoes++;
        gap = updatedGap(gap);

        swapped = 0;
        for (int i = 0; i < n - gap; i++)
        {
            if (a[i] > a[i + gap])
            {
                A->comparacoes++;
                int temp = a[i];
                a[i] = a[i + gap];
                a[i + gap] = temp;
                A->trocas++;
                swapped = 1;
            }
        }
    }
}

void swap(int *xp, int *yp, AlgoData *a)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
    a->trocas += 1;
}

void quick_sort(int *a, int left, int right, AlgoData *A)
{
    int i, j, x, y;

    i = left;
    j = right;
    x = a[(left + right) / 2];

    while (i <= j)
    {
        while (a[i] < x && i < right)
        {
            A->comparacoes += 1;
            i++;
        }
        while (a[j] > x && j > left)
        {
            A->comparacoes += 1;
            j--;
        }
        if (i <= j)
        {
            A->comparacoes += 1;
            y = a[i];
            a[i] = a[j];
            a[j] = y;
            A->trocas += 1;
            i++;
            j--;
        }
    }

    if (j > left)
    {
        quick_sort(a, left, j, A);
    }
    if (i < right)
    {
        quick_sort(a, i, right, A);
    }
}

void bubbleSort(int *arr, int n, AlgoData *a)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(&arr[j], &arr[j + 1], a);
                a->trocas++;
            }
            a->comparacoes += 1;
        }
    }
}

void insertionSort(int *arr, int n, AlgoData *a)
{
    for (int i = 1; i < n; i++)
    {
        int j = i;
        a->comparacoes++;
        while ((j > 0) && (arr[j - 1] > arr[j]))
        {
            if (arr[j - 1] > arr[j])
            {
                a->comparacoes++;
            }
            int temp = arr[j - 1];
            arr[j - 1] = arr[j];
            arr[j] = temp;
            j--;

            a->trocas++;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

FILE *createCSVResultFile()
{
    FILE *f = fopen("bubble.csv", "wb+");
    fprintf(f, "tam_lista, algoritmo, tipo, tempo, comparacoes, trocas\n");
    return f;
}

void writeOnCSV(FILE *p, AlgoData a)
{
    fprintf(p, "%d, %s, %s, %f, %lld, %lld\n", a.tam_lista, a.algo_name, a.tipo, a.tempo, a.comparacoes, a.trocas);
}

int main()
{
    glob_t infoArquivos = getArquivosInfos();

    char **files = infoArquivos.gl_pathv;
    FILE *resultFile = createCSVResultFile();
    fclose(resultFile);

    while (*files)
    {
        FILE *contentFile = fopen(*files, "r");
        FILE *resultFile = fopen("bubble.csv", "a");

        printf("Começando o arquivo - %s ... ", *files);
        FileData fd = readFileData(*files);
        int *dataArr = returnArrayFromFile(contentFile, fd.file_tam);
        int *aux = (int *)malloc(fd.file_tam * sizeof(int));
        AlgoData data = {.algo_name = "bubblesort",
                         .comparacoes = 0,
                         .tam_lista = fd.file_tam,
                         .tempo = 0,
                         .tipo = fd.file_type,
                         .trocas = 0};

        clock_t t = clock();

        bubbleSort(dataArr, fd.file_tam, &data);
        t = clock() - t;
        data.tempo = ((double)t) / CLOCKS_PER_SEC;
        writeOnCSV(resultFile, data);

        fclose(contentFile);
        fclose(resultFile);
        printf("completo!\n");
        files++;
    }
    printf("Terminado!!\n");
    return 0;
}