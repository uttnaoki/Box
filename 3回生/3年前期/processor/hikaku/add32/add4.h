/* (add4.h) */

declare add32 {
    input a<4>;
    input b<4>;
    input cin;       /* carry in */
    output sum<4>;
    output cout;     /* carry out */
    instrin do;

    instr_arg do(a, b, cin);
} /* declare add4 */

/* End of file (add4.h) */
