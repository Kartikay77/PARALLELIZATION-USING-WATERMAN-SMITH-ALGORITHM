#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>   // used only for omp_get_wtime()

/*--------------------------------------------------------------------
 * Text Tweaks
 */
#define RESET   "\033[0m"
#define BOLDRED "\033[1m\033[31m"
/* End of text tweaks */

/*--------------------------------------------------------------------
 * Constants
 */
#define PATH     -1
#define NONE      0
#define UP        1
#define LEFT      2
#define DIAGONAL  3

// #define DEBUG

/*--------------------------------------------------------------------
 * Function Prototypes
 */
void similarityScore(long long int i, long long int j, int* H, int* P, long long int* maxPos);
int  matchMissmatchScore(long long int i, long long int j);
void backtrack(int* P, long long int maxPos);
void printMatrix(int* matrix);
void printPredecessorMatrix(int* matrix);
void generate(void);

/*--------------------------------------------------------------------
 * Global Variables
 */
//Defines size of strings to be compared (original sizes)
long long int m = 11; //Columns - Size of string a
long long int n = 7;  //Lines - Size of string b

//Defines scores
int matchScore     = 5;
int missmatchScore = -3;
int gapScore       = -4; 

//Strings over the Alphabet Sigma
char *a, *b;

/*--------------------------------------------------------------------
 * Function:    main
 */
int main(int argc, char* argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <m> <n>\n", argv[0]);
        return 1;
    }

    m = strtoll(argv[1], NULL, 10);
    n = strtoll(argv[2], NULL, 10);
    
#ifdef DEBUG
    printf("\nMatrix[%lld][%lld]\n", n, m);
#endif

    // Allocates a and b with ORIGINAL sizes
    a = malloc(m * sizeof(char));
    b = malloc(n * sizeof(char));
    if (!a || !b) {
        fprintf(stderr, "Memory allocation failed for sequences\n");
        return 1;
    }

    // Generate random sequences with length m and n
    generate();

    // Add extra row/column for DP matrices
    m++;
    n++;
    
    // Allocates similarity matrix H
    int *H = calloc(m * n, sizeof(int));
    // Allocates predecessor matrix P
    int *P = calloc(m * n, sizeof(int));

    if (!H || !P) {
        fprintf(stderr, "Memory allocation failed for matrices\n");
        return 1;
    }

    // Start position for backtrack
    long long int maxPos = 0;

    //Calculates the similarity matrix
    long long int i, j;

    double initialTime = omp_get_wtime();
    
    for (i = 1; i < n; i++) { //Lines
        for (j = 1; j < m; j++) { //Columns
            similarityScore(i, j, H, P, &maxPos);
        }
    }

    backtrack(P, maxPos);

    //Gets final time
    double finalTime = omp_get_wtime();
    printf("\nElapsed time: %f\n\n", finalTime - initialTime);

#ifdef DEBUG
    printf("\nSimilarity Matrix:\n");
    printMatrix(H);

    printf("\nPredecessor Matrix:\n");
    printPredecessorMatrix(P);
#endif

    //Frees similarity matrices
    free(H);
    free(P);

    //Frees input arrays
    free(a);
    free(b);

    return 0;
}  /* End of main */


/*--------------------------------------------------------------------
 * Function:    SimilarityScore
 * Purpose:     Calculate the maximum Similarity-Score H(i,j)
 */
void similarityScore(long long int i, long long int j, int* H, int* P, long long int* maxPos) {

    int up, left, diag;

    //Stores index of element
    long long int index = m * i + j;

    //Get element above
    up = H[index - m] + gapScore;

    //Get element on the left
    left = H[index - 1] + gapScore;

    //Get element on the diagonal
    diag = H[index - m - 1] + matchMissmatchScore(i, j);

    //Calculates the maximum
    int max = NONE;
    int pred = NONE;

    if (diag > max) { // same letter ↖
        max = diag;
        pred = DIAGONAL;
    }

    if (up > max) { // remove letter ↑ 
        max = up;
        pred = UP;
    }
    
    if (left > max) { // insert letter ←
        max = left;
        pred = LEFT;
    }

    //Inserts the value in the similarity and predecessor matrices
    H[index] = max;
    P[index] = pred;

    //Updates maximum score to be used as seed on backtrack 
    if (max > H[*maxPos]) {
        *maxPos = index;
    }

}  /* End of similarityScore */


/*--------------------------------------------------------------------
 * Function:    matchMissmatchScore
 * Purpose:     Similarity function on the alphabet for match/missmatch
 */
int matchMissmatchScore(long long int i, long long int j) {
    if (a[j-1] == b[i-1])
        return matchScore;
    else
        return missmatchScore;
}  /* End of matchMissmatchScore */


/*--------------------------------------------------------------------
 * Function:    backtrack
 * Purpose:     Modify matrix to print, path change from value to PATH
 */
void backtrack(int* P, long long int maxPos) {
    //hold maxPos value
    long long int predPos;

    //backtrack from maxPos to startPos = 0 
    do {
        if      (P[maxPos] == DIAGONAL)
            predPos = maxPos - m - 1;
        else if (P[maxPos] == UP)
            predPos = maxPos - m;
        else if (P[maxPos] == LEFT)
            predPos = maxPos - 1;
        else
            break;

        P[maxPos] *= PATH;
        maxPos = predPos;

    } while (P[maxPos] != NONE);
}  /* End of backtrack */


/*--------------------------------------------------------------------
 * Function:    printMatrix
 * Purpose:     Print Matrix
 */
void printMatrix(int* matrix) {
    long long int i, j;
    for (i = 0; i < n; i++) { //Lines
        for (j = 0; j < m; j++) {
            printf("%d\t", matrix[m * i + j]);
        }
        printf("\n");
    }

}  /* End of printMatrix */


/*--------------------------------------------------------------------
 * Function:    printPredecessorMatrix
 * Purpose:     Print predecessor matrix
 */
void printPredecessorMatrix(int* matrix) {
    long long int i, j, index;
    for (i = 0; i < n; i++) { //Lines
        for (j = 0; j < m; j++) {
            index = m * i + j;
            if (matrix[index] < 0) {
                printf(BOLDRED);
                if      (matrix[index] == -UP)
                    printf("↑ ");
                else if (matrix[index] == -LEFT)
                    printf("← ");
                else if (matrix[index] == -DIAGONAL)
                    printf("↖ ");
                else
                    printf("- ");
                printf(RESET);
            } else {
                if      (matrix[index] == UP)
                    printf("↑ ");
                else if (matrix[index] == LEFT)
                    printf("← ");
                else if (matrix[index] == DIAGONAL)
                    printf("↖ ");
                else
                    printf("- ");
            }
        }
        printf("\n");
    }

}  /* End of printPredecessorMatrix */


/*--------------------------------------------------------------------
 * Function:    generate
 * Purpose:     Generate arrays a and b
 */
void generate() {
    long long int i;
    for (i = 0; i < m; i++) {
        int aux = rand() % 4;
        if      (aux == 0) a[i] = 'A';
        else if (aux == 1) a[i] = 'T';
        else if (aux == 2) a[i] = 'C';
        else               a[i] = 'G';
    }

    for (i = 0; i < n; i++) {
        int aux = rand() % 4;
        if      (aux == 0) b[i] = 'A';
        else if (aux == 1) b[i] = 'T';
        else if (aux == 2) b[i] = 'C';
        else               b[i] = 'G';
    }
}
