#define MIN(x,y)             ((x) >  (y)  ? (y)  : (x))

typedef unsigned char	byte;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
       R A N K  A L G O R I T H M  F U N C T I O N  P R O T O T Y P E S
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int				computeRank(int M, int Q, byte **matrix);
void			perform_elementary_row_operations(int flag, int i, int M, int Q, byte **A);
int				find_unit_element_and_swap(int flag, int i, int M, int Q, byte **A);
int				swap_rows(int i, int index, int Q, byte **A);
int				determine_rank(int m, int M, int Q, byte **A);
byte**			create_matrix(int M, int Q);
int**			create_integer_matrix(int M, int Q);
void			display_matrix(int M, int Q, byte **m);
void			display_int_matrix(int M, int Q, int **m);
void			def_matrix(int M, int Q, byte **m,int k);
void			delete_matrix(int M, byte **matrix);