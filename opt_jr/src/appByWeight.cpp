#include "appByWeight.hh"

/*bool Compare2::operator()(const Application& a, const Application& b){
  return a.w> b.w; //NB: siccome voglio ordine crescente e di default priority queue è decrescente, uso il maggiore !
}
*/


/*
 *  SALVO PUNTATORI A LISTA IN ORDINE DECRESCENTE DI PESO W
 */

void addApplicationPointer(appByWeight &LP, Application &App)
{
  if (LP.empty())
  {
    LP.emplace(LP.begin(), &App);
    return;
  }
  for (auto it= LP.begin(); it!=LP.end(); it++)
  {
    if ( (*it)->w < App.w)
    {
      LP.emplace(it, &App);
      return;
    }
  }
  LP.emplace(LP.end(), &App);
}
