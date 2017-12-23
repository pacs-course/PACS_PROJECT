


void debugMessage(std::string &string, optJrParameters &par)
{
	if (par.get_debug() == 1)
  {
	  std::cout<<"<debug message>:"<<string<<std::endl;
  }
}
