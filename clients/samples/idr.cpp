#include <iostream>
#include <cstdlib>

#include <rocalution.hpp>

using namespace rocalution;

int main(int argc, char* argv[]) {

  if (argc == 1) { 
    std::cerr << argv[0] << " <matrix> [Num threads]" << std::endl;
    exit(1);
  }

  init_rocalution();

  if (argc > 2) {
    set_omp_threads_rocalution(atoi(argv[2]));
  } 

  info_rocalution();

  LocalVector<double> x;
  LocalVector<double> rhs;

  LocalMatrix<double> mat;
  mat.ReadFileMTX(std::string(argv[1]));

  mat.MoveToAccelerator();
  x.MoveToAccelerator();
  rhs.MoveToAccelerator();

  mat.Info();

  x.Allocate("x", mat.GetN());
  rhs.Allocate("rhs", mat.GetM());

  // Linear Solver
  IDR<LocalMatrix<double>, LocalVector<double>, double > ls;

  // Preconditioner
  SPAI<LocalMatrix<double>, LocalVector<double>, double > p;

  double tick, tack;

  rhs.Ones();
  x.Zeros(); 

  ls.SetOperator(mat);
  ls.SetPreconditioner(p);
  ls.SetShadowSpace(4);

  ls.Build();

//  ls.Verbose(2);

  mat.Info();

  tick = rocalution_time();

  ls.Solve(rhs, &x);

  tack = rocalution_time();
  std::cout << "Solver execution:" << (tack-tick)/1000000 << " sec" << std::endl;

  ls.Clear();

  stop_rocalution();

  return 0;
}
