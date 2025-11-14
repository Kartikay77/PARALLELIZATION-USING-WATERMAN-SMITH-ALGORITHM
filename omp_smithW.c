#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

/*--------------------------------------------------------------------
 * Text Tweaks
 */
#define RESET   "\033[0m"
#define BOLDRED "\033[1m\033[31m"
/* End of text tweaks */

/*--------------------------------------------------------------------
 * Constants
 */
#define PATH -1
#define NONE 0
#define UP 1
#define LEFT 2
#define DIAGONAL 3
/* End of constants */

/*--------------------------------------------------------------------
 * Helpers
 */
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(a,b)  (((a) > (b)) ? (a) : (b))
/* End helpers */

/*--------------------------------------------------------------------
 * Function Prototypes
 */
void similarityScore(long long int i, long long int j, int* H, int* P, long long int* maxPos);
int  matchMissmatchScore(long long int i, long long int j);
void backtrack(int* P, long long int maxPos);
void printMatrix(int* matrix);
void printPredecessorMatrix(int* matrix);
void generate(void);
long long int nElement(long long int diag);
void calcFirstDiagElement(long long int diag, long long int *si, long long int *sj);
/* End prototypes */

/*--------------------------------------------------------------------
 * Global variables
 */
long long int m;  // length of a  (will be incremented by 1 for DP)
long long int n;  // length of b  (will be incremented by 1 for DP)

int matchScore     = 5;
int missmatchScore = -3;
int gapScore       = -4;

char *a;
char *b;
/* End globals */


/*--------------------------------------------------------------------
 * main()
 */
int main(int argc, char* argv[]) {

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <threads> <m> <n> [print_flag]\n", argv[0]);
        return 1;
    }

    int thread_count = strtol(argv[1], NULL, 10);
    m = strtoll(argv[2], NULL, 10);
    n = strtoll(argv[3], NULL, 10);

    int print_flag = 0;  // 1 = print matrices, 0 = only time
    if (argc >= 5) {
        print_flag = atoi(argv[4]);
    }

    /* allocate sequences of original size */
    a = malloc(m * sizeof(char));
    b = malloc(n * sizeof(char));

    if (!a || !b) {
        fprintf(stderr, "Memory allocation error for sequences\n");
        return 1;
    }

    /* fill sequences BEFORE incrementing m,n */
    generate();

    /* Now expand for DP matrix (extra row+col of zeros) */
    m++;
    n++;

    /* allocate H, P matrices */
    int *H = calloc(m * n, sizeof(int));
    int *P = calloc(m * n, sizeof(int));

    if (!H || !P) {
        fprintf(stderr, "Memory allocation error for matrices\n");
        return 1;
    }

    long long int maxPos = 0;

    double start = omp_get_wtime();

    long long int nDiag = m + n - 3;

    #pragma omp parallel num_threads(thread_count) \
        default(none) shared(H, P, maxPos, nDiag)
    {
        long long int diag, si, sj, j, ai, aj, nEle;

        for (diag = 1; diag <= nDiag; diag++) {

            nEle = nElement(diag);
            calcFirstDiagElement(diag, &si, &sj);

            #pragma omp for
            for (j = 1; j <= nEle; j++) {
                ai = si - j + 1;
                aj = sj + j - 1;
                similarityScore(ai, aj, H, P, &maxPos);
            }
        }
    }

    backtrack(P, maxPos);

    double end = omp_get_wtime();
    printf("\nElapsed time: %f seconds\n", end - start);

    if (print_flag) {
        printf("\nSimilarity Matrix:\n");
        printMatrix(H);

        printf("\nPredecessor Matrix:\n");
        printPredecessorMatrix(P);
    }

    free(H);
    free(P);
    free(a);
    free(b);

    return 0;
}

/*--------------------------------------------------------------------
 * Count elements in diagonal
 */
long long int nElement(long long int diag) {
    if (diag < m && diag < n)
        return diag;

    long long int mn = min(m, n);

    if (diag < max(m, n))
        return mn - 1;

    return 2 * mn - diag + llabs(m - n) - 2;
}

/*--------------------------------------------------------------------
 * First element of diagonal
 */
void calcFirstDiagElement(long long int diag, long long int *si, long long int *sj) {
    if (diag < n) {
        *si = diag;
        *sj = 1;
    } else {
        *si = n - 1;
        *sj = diag - n + 2;
    }
}

/*--------------------------------------------------------------------
 * Scoring kernel
 */
void similarityScore(long long int i, long long int j, int* H, int* P, long long int* maxPos) {

    long long int idx = m * i + j;

    int up   = H[idx - m]     + gapScore;
    int left = H[idx - 1]     + gapScore;
    int diag = H[idx - m - 1] + matchMissmatchScore(i, j);

    int best = NONE;
    int pred = NONE;

    if (diag > best) { best = diag; pred = DIAGONAL; }
    if (up   > best) { best = up;   pred = UP;       }
    if (left > best) { best = left; pred = LEFT;     }

    H[idx] = best;
    P[idx] = pred;

    #pragma omp critical
    if (best > H[*maxPos])
        *maxPos = idx;
}

/*--------------------------------------------------------------------
 * Match score
 */
int matchMissmatchScore(long long int i, long long int j) {
    return (a[j - 1] == b[i - 1]) ? matchScore : missmatchScore;
}

/*--------------------------------------------------------------------
 * Backtracking
 */
void backtrack(int* P, long long int maxPos) {
    long long int prev;

    do {
        if      (P[maxPos] == DIAGONAL) prev = maxPos - m - 1;
        else if (P[maxPos] == UP)       prev = maxPos - m;
        else if (P[maxPos] == LEFT)     prev = maxPos - 1;
        else break;

        P[maxPos] *= PATH;
        maxPos = prev;

    } while (P[maxPos] != NONE);
}

/*--------------------------------------------------------------------
 * Print similarity matrix
 */
void printMatrix(int* matrix) {
    long long int i, j;
    for (i = 0; i < n; i++) {          // n and m are globals (with +1)
        for (j = 0; j < m; j++) {
            printf("%d\t", matrix[m * i + j]);
        }
        printf("\n");
    }
}

/*--------------------------------------------------------------------
 * Print predecessor matrix (with red optimal path)
 */
void printPredecessorMatrix(int* matrix) {
    long long int i, j, idx;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            idx = m * i + j;
            int val = matrix[idx];

            int neg = (val < 0);
            if (neg) {
                val = -val;
                printf(BOLDRED);
            }

            if      (val == UP)       printf("↑ ");
            else if (val == LEFT)     printf("← ");
            else if (val == DIAGONAL) printf("↖ ");
            else                      printf("- ");

            if (neg) {
                printf(RESET);
            }
        }
        printf("\n");
    }
}

/*--------------------------------------------------------------------
 * Generate random DNA sequences
 */
void generate() {
    srand(time(NULL));

    for (long long int i = 0; i < m; i++) {
        int r = rand() % 4;
        a[i] = (r == 0 ? 'A' : r == 1 ? 'T' : r == 2 ? 'C' : 'G');
    }
    for (long long int i = 0; i < n; i++) {
        int r = rand() % 4;
        b[i] = (r == 0 ? 'A' : r == 1 ? 'T' : r == 2 ? 'C' : 'G');
    }
}
