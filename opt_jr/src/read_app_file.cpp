#include "read_app_file.hh"
#include <string.h>
#include <iostream>



char * get_field(char* line, int num)
{
  char* tok;

  if ((num < 1) || (num > PARAMETERS)) printf("get_field: num %d out of bound\n", num);
  else for (tok = strtok( line, ","); tok && *tok; tok = strtok(NULL, ",\n"))
  if (!--num) return tok;

  return NULL;
}


std::vector<Application>  read_app_file(FILE* stream)
{
  std::vector<Application> APPs;

  char line[1024];
  double w;
  double chi_0;
  double chi_C;
  double m;
  double M;
  double V;
  double v;
  double D;
  double csi;
  char * session_app_id;
  char * app_id;
  char * St;
  int dataset_size;
  int rows = 1;

  session_app_id = (char *)malloc(MAX_APP_LENGTH);
  if (session_app_id == NULL)
  {
        printf("session_app_id: malloc_failure in main\n");
        exit(-1);
  }

  app_id = (char *)malloc(MAX_APP_LENGTH);
  if (app_id == NULL)
  {
        printf("app_id: malloc_failure in main\n");
        exit(-1);
  }

  St = (char *)malloc(1024);
      if (St == NULL)
      {
          printf("app_id1: malloc_failure in main\n");
          exit(-1);
      }

  while (fgets(line, MAX_LINE_LENGTH, stream))
  {


    char* tmp = strdup(line);

    if ((strlen(line)==0) || (strstr(line, "#")==NULL)) // Skip if it's comment or empty line
    {
      strcpy(session_app_id, get_field(tmp, _SESSION_APP_ID));
      tmp = strdup(line);
      strcpy(app_id, get_field(tmp, _APP_ID));tmp = strdup(line);
      w = 	atof(get_field(tmp, _W));tmp = strdup(line);
      chi_0 = atof(get_field(tmp, _CHI_0));tmp = strdup(line);
      chi_C = atof(get_field(tmp, _CHI_C));tmp = strdup(line);
      M = 	atof(get_field(tmp, _M));tmp = strdup(line);
      m = 	atof(get_field(tmp, _m));tmp = strdup(line);
      V = 	atof(get_field(tmp, _V));tmp = strdup(line);
      v = 	atof(get_field(tmp, _v));tmp = strdup(line);
      D = 	atoi(get_field(tmp, _D));tmp = strdup(line);
      strcpy(St, get_field(tmp, _St));tmp = strdup(line);
      dataset_size = 	atoi(get_field(tmp, _Dsz));
      csi=std::max(M/m,V/v);
      /* Add application parameters to the List */
      APPs.emplace_back(session_app_id,app_id, w, chi_0, chi_C, m, M, V, v, D, csi, St, dataset_size);
      rows++;
      free(tmp);
    }
  }

  return APPs;
}
