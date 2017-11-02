std::vector<Application> readAppFile(FILE* stream)
{

while (fgets(line, MAX_LINE_LENGTH, stream))
{
    char* tmp = strdup(line);

    if ((strlen(line)==0) || (strstr(line, "#")==NULL)) // Skip if it's comment or empty line
    {
      strcpy(session_app_id, getfield(tmp, _SESSION_APP_ID));tmp = strdup(line);
      strcpy(app_id, getfield(tmp, _APP_ID));tmp = strdup(line);
      w = 	atof(getfield(tmp, _W));tmp = strdup(line);
      chi_0 = atof(getfield(tmp, _CHI_0));tmp = strdup(line);
      chi_C = atof(getfield(tmp, _CHI_C));tmp = strdup(line);
      M = 	atof(getfield(tmp, _M));tmp = strdup(line);
      m = 	atof(getfield(tmp, _m));tmp = strdup(line);
      V = 	atof(getfield(tmp, _V));tmp = strdup(line);
      v = 	atof(getfield(tmp, _v));tmp = strdup(line);
      D = 	atoi(getfield(tmp, _D));tmp = strdup(line);
      strcpy(St, getfield(tmp, _St));tmp = strdup(line);
      DatasetSize = 	atoi(getfield(tmp, _Dsz));
      csi = getCsi(M/m, V/v);
      /* Add application parameters to the List*/
      //addParameters(&first, &current, session_app_id, app_id, w, chi_0, chi_C, m, M, V, v, D, csi, St, DatasetSize);

      rows++;
      free(tmp);
    }
}
