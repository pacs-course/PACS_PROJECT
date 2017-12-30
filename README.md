# PACS PROJECT DESCRIPTION:

OPT_JR_CPP is a program that manages soft deadline application when heavy load occurs. It provides the best cores allocation to the invoking applications by performing a local neighborhood search. OPT_JR uses a predictor (actually only dagSim is supported) and relies on the initial solution provided by OPT_IC.

The original version of this program is written in C and it’s available at: https://github.com/eubr-bigsea/opt_jr

This new version has been rewritten in C++ with some improvements and parallelization (using openMP).
 

# USAGE:

`./OPT_JR_CPP -f <filename.csv> -n <N> -k <Limit> -d <Y/y|N/n> -c <Y/y|N/n> -g=<Y/y|N/n> -i <iterations> -st<a/A|s/S>`

where:

`<filename.csv> is the csv file defining the profile applications under $UPLOAD_HOME in wsi_config.xml;`

`<N> is the total number of cores;`

`<Limit> is the maximum number of considered candidates (if equal to 0, all the candidates are considered).`

`-d represents debug (on/off)`

`-c represents cache (on/off)`

`-g prints the value of the global objective function (on/off)`

`-i represents the maximum number of iterations`

`-st represents the type of local search: a/A alternates approximate evaluation of objective function with dagsim evaluation, while s/S performs separately an approximate loop and a dagSim loop. If -st=s/S -k should be set to 0.`




# EXAMPLE:

`./OPT_JR_CPP -f="Test1.csv" -n=150  -i=10 -k=0 -d=n -c=y -g=y -st=a`
       
In this case, OPT_JR_CPP is executed on a file Test1.csv, the total number of cores is 150, all the candidates are considered, the maximum number of iterations is 10, the value of global objective function for each iteration will be printed in output, the type of local search is Alterning. 


# TEST:
Some testing files are provided in the tests directory. 



# REQUIREMENTS:
The program requires a filesystems which returns real info in the d_type field (look at the readdir(3) man page at https://linux.die.net/man/3/readdir for more details)
This program requires mysql server and connector.

To install mysql server with yum (following the instructions at https://dev.mysql.com/doc/mysql-yum-repo-quick-guide/en/#repo-qg-yum-fresh-install):

1) Go to the download page for MySQL Yum repository at http://dev.mysql.com/downloads/repo/yum/ .

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

8)Exit and install the connector and the library:
  > exit
  > sudo yum install mysql-connector-odbc
  > sudo yum install mysql-devel



# BUILD THE PROGRAM:

0) Download:
 - OPTIMIZER_CONFIGURATION_TABLE.sql, PREDICTOR_CACHE_TABLE.sql and dagSim-master.zip from  https://github.com/eubr-bigsea/opt_jr 
 - creationDB.sql from https://github.com/eubr-bigsea/wsi/tree/master/Database
 - insertFakeProfile.sql from https://github.com/eubr-bigsea/wsi/tree/master/Database
 - LogP8.tar.gz from https://github.com/eubr-bigsea/OPT_DATA_CONFIG


1) Build the database:

 #log in in mysql server (the password will be asked)
 > mysql -uroot -p


 #create a database ():
 > CREATE DATABASE database_name;

 #use that database
 > USE database_name;

2) Run creationDB.sql which creates the tables  
 > source creationDB.sql;

3) Launch the script which creates and populates three tables OPTIMIZER_CONFIGURATION_TABLE.sql, PREDICTOR_CACHE_TABLE.sql, insertFakeProfile.sql:
 > source OPTIMIZER_CONFIGURATION_TABLE.sql;
 > source PREDICTOR_CACHE_TABLE.sql;
 > source insertFakeProfile.sql;
 > exit



4) Extract and move LogP8 directory: 
 > sudo mkdir /home/work
 > sudo mv LogP8 /home/work/LogP8

5) Extract dagSim-master.zip, move in dagSim-master directory and type “make”

6) set wsi_config.xml configuration file; change: 
 - THREADS_NUMBER (set the desired number of threads)
 - DAGSIM_HOME (set the path to the dagSim-master directory)
 - OptDB_dbName (set the name of the database)
 - OptDB_pass (set the password for mysql)
 - UPLOAD_HOME (set the path to the Tests directory)


7) Set the environmental variables in the settingEV.sh file with the correct paths and run the script.



8) move in opt_jr directory and type: 
 > make




Look at first point for usage instructions.






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



