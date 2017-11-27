#include "candidates.hh"


/*
 *       Name:               addCandidates
 *       Input parameters:      sCandidates ** first, sCandidates ** current,  char * app_id1, char * app_id2, int contr1, int contr2, double delta
 *       Output parameters:      Updated pointers to the first and current element of the list
 *       Description:         This function adds all the information regarding the localSearch deltafo calculation. The list is sorted by deltafo value.
 *
 */
void addCandidate(sCandidates  &first, sCandidates & current,  Application &app_i, Application &app_j, int contr1, int contr2, double delta, double delta_i, double delta_j)
{
     sCandidates *new = (sCandidates*) malloc(sizeof(sCandidates));
     if (new == NULL)
     {
        printf("addCandidateParameters: Fatal Error: malloc failure\n");
        exit(-1);
     }

     new->app_i = app_i;
     new->app_j = app_j;
     new->newCoreAssignment_i = contr1;
     new->newCoreAssignment_j = contr2;
     new->deltaFO = delta;
     new->delta_i = delta_i;
     new->delta_j = delta_j;
     new->next = NULL;

/*
     if (*first == NULL) *first = new;
     else (*current)->next = new;
     *current = new;*/

     if (*first == NULL) *first = new;
                 else
                    if (doubleCompare((*first)->deltaFO, delta) == 1)
                    {
                       new->next = *first;
                       *first = new;
                    }
                        else
                      {
                          sCandidates * previous = *first;
                          sCandidates * current = (*first)->next;

                         while (current != NULL && doubleCompare(current->deltaFO, delta) == -1)
                         {
                            previous = current;
                            current = current->next;
                         }

                         previous->next = new;
                         new->next = current;
                      }
}
