declare cla32{
  input a<32>, b<32>, cin;
  output sum<32>, cout;
  output pout, qout;
  instrin do;

  instr_arg do(a, b, cin);
}
