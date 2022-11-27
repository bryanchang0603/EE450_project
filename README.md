# EE450_project
This is the EE450 final project. It has four servers, serverM, serverCS, serverEE, and serverC. the serverM is the controller, and serverCS, serverEE, serverC are the module servers that only serverM will call. serverCS and serverEE have very similiar functionality and hence very similiar codes. serverC is the authenciation server.
The program is running on C.\<br>

##Makefile commands:
make all: compile all c files \<br>
make clear: remove all executable files \<br>
make serverC: compile serverC alone \<br>
make serverCS: compile serverCS alone \<br>
make serverEE: compile serverEE alone \<br>
make serverM: compile serverM alone \<br>
make client: compile client alone \<br>

##Initial Startup Process:
1. make all
2. ./serverM
3. ./serverC, serverCS, ./serverEE
  Note: since serverM and other servers are connected through UDP, order of step 2 and 3 does not matter
5. ./client
  This must run at last

##input file constraints:
must follow the format as provided in the example \<br>
max length of username and password cannot exceed 50 characters. Otherwise buffer will overflow \<br>

##client:
  ###I/O handling:
  ####username: 5-50 character. complete by pressing Enter. do not contain space in the input, otherwise client will fail but not terminate <br>
  ####password: 5-50 character. complete by pressing Enter. do not contain space in the input, otherwise client will fail but not terminate \<br>
  ####course query: accept both single course query and multiple course query. courses in multiple course query nees to be splited by ' ', whitespace. \<br>
    #####multi course query
      course must start with "EE" or "CS". serverM will not pass and other courses back to serverCS nor serverEE and return course not found \<br>
      if duplicated courses are enteres, the course info will be displayed firat time it show up in the multi course query \<br>
      if course is not found, system will coutput course not found \<br>
    #####single course query
      when entering the category looking for, do not input any ' ', whitespace. client will fail but not terminate \<br>
      the category input is case sensetive. Please enter the category based on the on screen message. \<br>
