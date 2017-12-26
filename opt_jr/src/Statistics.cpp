#include "statistics.hh"





///add_statistics is used to add information about an iteration to the Statistics object.
void Statistics::add_statistics(int iteration, int size, double FO_total)
{
  stat.emplace_back(iteration,size,FO_total);
}

///read_statistics shows the statistics about localsearch iterations.
void Statistics::read_statistics(Opt_jr_parameters &par)
{

  std::cout << "\n\n*************************************************\n";
	std::cout << "***********  Statistics list content:  **********\n";
  std::cout << "*************************************************\n";

	if (par.get_number_of_threads() > 0) std::cout <<"(OpenMP: yes) Iteration   List Size  Total_FO\n";
	else std::cout <<"(OpenMP: no) Iteration   List Size  Total_FO\n";
	for (auto &el : stat)
	{
    std::cout <<"              "<< el.get_iteration() <<"           "<< el.get_size() << "           " << el.get_FO_Total()<< "\n";
	}
  std::cout<<"*************************************************\n\n";

}
