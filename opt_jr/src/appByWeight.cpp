#include "appByWeight.hh"

bool Compare2::operator()(const Application& a, const Application& b){
  return a.w> b.w; //NB: siccome voglio ordine crescente e di default priority queue è decrescente, uso il maggiore !
}
