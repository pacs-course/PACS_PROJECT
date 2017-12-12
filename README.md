# PACS PROJECT DESCRIPTION:

OPT_JR_CPP is a program that manages soft deadline application when heavy load occurs. It provides the best cores allocation to the invoking applications by performing a local neighborhood search. OPT_JR uses a predictor (actually only dagSim is supported) and relies on the initial solution provided by OPT_IC.

The original version of this program is written in C and it’s available at: https://github.com/eubr-bigsea/opt_jr

This new version has been rewritten in C++ with some improvements and parallelization (using openMP).
 

# USAGE:

`./OPT_JR:CPP -f <filename.csv> -n <N> -k <Limit> -d <Y/y|N/n> -c <Y/y|N/n> -g=<Y/y|N/n> -s <dagSim|lundstrom> -i <iterations>`

where:

`<filename.csv> is the csv file defining the profile applications under $UPLOAD_HOME in wsi_config.xml;`

`<N> is the total number of cores;`

`<Limit> is the maximum number of considered candidates (if equal to 0, all the candidates are considered).`

`-d represents debug (on/off)`

`-c represents cache (on/off)`

`-g prints the value of the global objective function (on/off)`

`-i represents the maximum number of iterations`

#Example:

`./OPT_JR -f="Test1.csv" -n=150  -i=10 -k=0 -d=y -c=y -s=dagSim -g=Y`
       
In this case, OPT_JR is executed on a file Test1.csv, the total number of cores is 150, all the candidates are considered, the predictor used is dagSim, the maximum number of iterations is 10, the value of global objective function for each iteration will be printed in output. 




# REQUIREMENTS:

This program requires mysql server and connector.

To install mysql server with yum (following the instructions at https://dev.mysql.com/doc/mysql-yum-repo-quick-guide/en/#repo-qg-yum-fresh-install):

1) Go to the download page for MySQL Yum repository at http://dev.mysql.com/downloads/repo/yum/.

2) Select and download the release package for your platform.

3) Install the downloaded release package with the following command, replacing platform-and-version-specific-package-name with the name of the downloaded package:   
  > sudo rpm -Uvh platform-and-version-specific-package-name.rpm

4) Install MySQL by the following command: 
  > sudo yum install mysql-community-server

5) Start the MySQL server with the following command:  
  > sudo service mysqld start

6)A superuser account 'root'@'localhost' is created. A password for the superuser is set and stored in the error log file. To reveal it, use the following command: sudo grep 'temporary password' /var/log/mysqld.log

7)Change the root password as soon as possible by logging in with the generated, temporary password and set a custom password for the superuser account: 
  > mysql -uroot -p
  > ALTER USER 'root'@'localhost' IDENTIFIED BY 'MyNewPass4!';

NOTE: MySQL's validate_password plugin is installed by default. This will require that passwords contain at least one upper case letter, one lower case letter, one digit, and one special character, and that the total password length is at least 8 characters.

8)Install the connector:
  > yum install mysql-connector-odbc



# BUILD THE PROGRAM:


1) Build the database:

 #log in in mysql server (the password will be asked)
 > mysql -uroot -p


 #create a database ():
 > CREATE DATABASE database_name;

 #use that database
 > USE database_name;

2) Run creationDB.sql which creates the tables
 > source creationDB.sql;

3) Launch the script which creates and populates two tables OPTIMIZER_CONFIGURATION_TABLE.sql, PREDICTOR_CACHE_TABLE.sql:
 > source OPTIMIZER_CONFIGURATION_TABLE.sql;
 > source PREDICTOR_CACHE_TABLE.sql;


4) set wsi_config.xml configuration file


5) set and run the environmental variables in the settingEV.sh file

6) Compile dagsim simulator: move in dagsim-master and type “make”

7) move in opt_jr directory and type: 
  > make


look at first point for usage instructions 






# BUILD DOCUMENTATION:

Requirements: doxygen and graphviz

If you have yum installed you can install doxygen and graphviz typing:
1) sudo yum install doxygen
2) sudo yum install graphviz


Once doxygen and graphviz are installed, follow the instructions below to build the documentation :
1) move in the doc directory
2) run:  doxygen opt_jr_doxy

You will find two directory in doc named “html”, and “latex”. Open the index.html in the html directory to explore the structure of the program.
If you want a pdf document you should compile the reman.tex file in the latex directory.



# TESTS:
Some test file are provided in the test directory


