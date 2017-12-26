#include "statistics.hh"

int Statistic_iter::get_iteration()
{
  return iteration;
}

int Statistic_iter::get_size()
{
  return size;
}

double Statistic_iter::get_FO_Total()
{
  return FO_Total;
}





void Statistics::add_statistics(int iteration, int size, double FO_total)
{
  Statistic_iter tmp(iteration,size,FO_total); // cambia
  statistics.push_back(tmp);
}

void Statistics::read_statistics(OPT_JR_parameters &par)
{

  std::cout << "\n\n*************************************************\n";
	std::cout << "***********  Statistics list content:  **********\n";
  std::cout << "*************************************************\n";

	if (par.get_number_of_threads() > 0) std::cout <<"(OpenMP: yes) Iteration   List Size  Total_FO\n";
	else std::cout <<"(OpenMP: no) Iteration   List Size  Total_FO\n";
	for (auto &el : statistics)
	{
    std::cout <<"              "<< el.get_iteration() <<"           "<< el.get_size() << "           " << el.get_FO_Total()<< "\n";
	}
  std::cout<<"*************************************************\n\n";

}
