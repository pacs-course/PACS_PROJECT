#include "appByWeight.hh"


/*
 *  It saves Application* in decreasing weight "w" order
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
    if ( (*it)->get_w() < App.get_w())
    {
      LP.emplace(it, &App);
      return;
    }
  }
  LP.emplace(LP.end(), &App);
}
