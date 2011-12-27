#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "Error.h"
#include "LevenshteinDistance.h"

//*****************************************************************************
/**
 *   Delete memory for a matrix
 */

static int deleteMatrix(int** matrix, int m, int n)
{

  if(matrix == NULL)
  {

    return LEVENSTEIN_ERROR;
  }

  // Free up the column for each matrix row
  int i;
  for(i = 0; i < m+1; i++)
  {

    if(matrix[i] == NULL)
    {

      return LEVENSTEIN_ERROR;
    }

    free(matrix[i]);

  }

  // Free up the rows
  free(matrix);

  return LEVENSTEIN_SUCCESS;

}

//*****************************************************************************
/**
 *   Return the min of 3 intergers
 */

int minimum(int a, int b, int c)
{

  int min = a;
  if(b < min)
  {
    min = b;
  }
  if(c < min)
  {
    min = c;
  }
  return min;
}


//*****************************************************************************
/**
 *  Computer the LevenShtein Distance of 2 strings to determine
 *    how close of a match they are.
 */

int levenshteinDistance(const char* s1, const char* s2)
{

  int result = 0;

  if(s1 == NULL || s2 == NULL)
  {

    return LEVENSTEIN_ERROR;

  }

  int m = strlen(s1);
  int n = strlen(s2);

  // Allocate all matrix rows
  int** matrix = NULL;
  matrix = (int**) malloc(sizeof(int*) * (m+1));
  memset(matrix, 0, sizeof(int*)*(m+1));

  // Allocate a column for each matrix row
  int matrix_row = 0;
  for(matrix_row = 0; matrix_row < (m+1); matrix_row++)
  {

    matrix[matrix_row] = (int*)malloc(sizeof(int)*(n+1));
    memset(matrix[matrix_row], 0, sizeof(int)*(n+1));
  }

  int i, j;
  for(i = 0; i <= m; i++)
  {

    matrix[i][0] = i;
  }

  for(j = 0; j <= n; j++)
  {

    matrix[0][j] = j;
  }

  for(j = 1; j <= n; j++)
  {

    for(i = 1; i <= m; i++)
    {

      if(s1[i-1] == s2[j-1])
      {

        matrix[i][j] = matrix[i-1][j-1];
      }
      else
      {

        matrix[i][j] = 
          minimum(
              // A deletion
              matrix[i-1][j] +1, 
              // Insertion
              matrix[i][j-1] +1, 
              // a substitution
              matrix[i-1][j-1] +1
              );

      }

    }

  }

  result = matrix[m][n];
  deleteMatrix(matrix, m, n);

  return result;

}



