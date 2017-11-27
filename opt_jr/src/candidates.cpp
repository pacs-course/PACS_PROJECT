#include "candidates.hh"
#include "utility.hh"



bool Compare1::operator()(const Candidate& a, const Candidate& b){
  return a.deltaFO > b.deltaFO; //NB: siccome voglio ordine crescente e di default priority queue è decrescente, uso il maggiore !
}




/*
 *       Name:               addCandidate
 *       Description:         This function adds all the information regarding the localSearch deltafo calculation.
 *                            The list is sorted by deltafo value.
 */
void addCandidate(sCandidates  &cand,  Application &app_i, Application &app_j, int contr1, int contr2, double delta, double delta_i, double delta_j)
{
     cand.emplace(app_i,app_j, contr1, contr2, delta, delta_i, delta_j);
}
