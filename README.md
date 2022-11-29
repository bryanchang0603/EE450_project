# EE450_project

Name: Baosheng Chang<br />
Student ID: 9660914048<br />
This is the EE450 final project. It has four servers, serverM, serverCS, serverEE, and serverC. the serverM is the controller, and serverCS, serverEE, serverC are the module servers that only serverM will call. serverCS and serverEE have very similiar functionality and hence very similiar codes. serverC is the authenciation server.<br />
The program is running on C. It supports user login, password encrpytion, single course query, and multiple course query<br />

## Completed Part
1. Phase 0
2. Phase 1
3. Phase 2A
4. Phase 2B
5. Phase 3A
6. Phase 3B
7. Phase 4 (both)
8. Extal Credit


---

## Makefile commands

`make all` compile all c files <br />
`make clear`: remove all executable files <br />
`make serverC`: compile serverC alone <br />
`make serverCS`: compile serverCS alone <br />
`make serverEE`: compile serverEE alone <br />
`make serverM`: compile serverM alone <br />
`make client`: compile client alone <br />

---

## Initial Startup Process

1. `make all`
2. `./serverM`
3. `./serverC`, `serverCS`, `./serverEE`<br />
   Note: since serverM and other servers are connected through UDP, order of step 2 and 3 does not matter
4. `./client`
   This must run at last

---

## input file constraints

must follow the format as provided in the example <br />
max length of username and password cannot exceed 50 characters. Otherwise buffer will overflow <br />

---

## client
This file will read user's input and send to the serverM using TCP, then displays the message returned by serverM

## serverM
This program will setup a TCP server, and connect to any client connect from port 25048. Based on the client's status, serverM will send the client's requestt to the back servers, serverC or serverCS/serverEE. After receiving the response from back servers, serverM may modify the result and send to client

## serverC
This program will read cred.txt and store all credentials into a linked list. When authencation request is received, serverC will check through the linked list to check is there is any match. After checking, the result will be sent back to serverM

## serverCS
This program will read cs.txt and store all entry into a linked list. When receiving a query request, serverCS will check for the type of query, then perform the query. After the query, the result will be sent back.

## serverEE
This program is almost identical to serverCS <br />
This program will read ee.txt and store all entry into a linked list. When receiving a query request, serverEE will check for the type of query, then perform the query. After the query, the result will be sent back.

## I/O handling

### username

5-50 character. complete by pressing Enter. do not contain space in the input, otherwise client will fail but not terminate <br />

### password

5-50 character. complete by pressing Enter. do not contain space in the input, otherwise client will fail but not terminate <br />
### course query
accept both single course query and multiple course query. courses in multiple course query nees to be splited by ' ', whitespace. <br />
### multi course query
course must start with "EE" or "CS". serverM will not pass and other courses back to serverCS nor serverEE and return course not found <br />
if duplicated courses are enteres, the course info will be displayed firat time it show up in the multi course query <br />
if course is not found, system will coutput course not found <br />
### single course query 
when entering the category looking for, do not input any ' ', whitespace. client will fail but not terminate <br />
the category input is case sensetive. Please enter the category based on the on screen message. <br />

## Exchanged Message Format
### authencation process
#### client to serverM
\<username>,\<password> <br /><br />
#### serverM to serverC
\<encpyt_username>,\<encpyt_password> <br /><br />
#### serverC to serverM
if auth passed: "pass" <br />
if user is not in cred.txt: "user not found" <br />
if password is incorrect: "password mismatch" <br /><br />
#### serverM to client
serverM will not modify message from serverC and send it to client <br />
if auth passed: "pass" <br />
if user is not in cred.txt: "user not found" <br />
if password is incorrect: "password mismatch" <br /><br />

### single query
#### client to serverM
\<course code>,\<category> <br /><br />
#### serverM to serverCS/serverEE
serverM will not modify message from client and send it to serverCS/serverEE <br />
\<course code>,\<category> <br /><br />
#### serverCS/serverEE to serverM
if course code did not find: "course_not_found"<br />
if category did not find: "category_not_found"<br />
if course code is found and category matches: value stored for that course's category<br /><br />

#### serverM to client
if course does not contain code "CS" or "EE": "course_not_found"<br />
if course contains code "CS" or "EE": result received from serverCS or serverEE<br />
1. "course_not_found"
2. "category_not_found"
3. value stored for that course's category

### multi query
#### client to serverM
\<course 1> \<course 2> \<course 5> \<course 4> <br />
the courses need to be splited by whitespace

#### serverM to serverCS
<CS course 1> <CS course 2> <CS course 3> <CS course 4>

#### serverM to serverEE
<EE course 1> <EE course 2> <EE course 3> <EE course 4> 

#### serverCS/serverEE ro serverM:
CS/EE course 1: credit, professor, days, course name <br />
CS/EE course 2: credit, professor, days, course name <br />
CS/EE course 3: credit, professor, days, course name <br />
CS/EE course 4: credit, professor, days, course name <br />

#### serverM to client:
 course 1: credit, professor, days, course name <br />
 course 2: credit, professor, days, course name <br />
 course 3: credit, professor, days, course name <br />
 course 4: credit, professor, days, course name <br />
 course did not found:: course 5<br/>

#Note: duplicated courses will not be displayed in message from serverM to client



## Reused Code
1. TCP and UDP socket - Beej's coding tutorial
2. TCP, UDP server/client setup - Beej's coding tutorial
3. send, sendto, recv, recvfrom - Beej's coding tutorial, geekforgeeks
4. linkedlist - modified based on geekforgeeks
5. strtok - learnt from geekforgeeks