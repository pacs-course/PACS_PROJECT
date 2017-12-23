#include "statistics.hh"

int Statistic::get_iteration()
{
  return iteration;
}

int Statistic::get_size()
{
  return size;
}

double Statistic::get_FO_Total()
{
  return FO_Total;
}





void sStatistics::addStatistics(int iteration, int size, double FO_total)
{
  Statistic tmp(iteration,size,FO_total);
  statistics.push_back(tmp);
}

void sStatistics::readStatistics(optJrParameters &par)
{

  std::cout << "\n\n*************************************************\n";
	std::cout << "***********  Statistics list content:  **********\n";
  std::cout << "*************************************************\n";

	if (par.get_numberOfThreads() > 0) std::cout <<"(OpenMP: yes) Iteration   List Size  Total_FO\n";
	else std::cout <<"(OpenMP: no) Iteration   List Size  Total_FO\n";
	for (auto &el : statistics)
	{
    std::cout <<"              "<< el.get_iteration() <<"           "<< el.get_size() << "           " << el.get_FO_Total()<< "\n";
	}
  std::cout<<"*************************************************\n\n";

}
