# EE450_project

Name: Baosheng Chang<br />
Student ID: 9660914048<br />
This is the EE450 final project. It has four servers, serverM, serverCS, serverEE, and serverC. the serverM is the controller, and serverCS, serverEE, serverC are the module servers that only serverM will call. serverCS and serverEE have very similiar functionality and hence very similiar codes. serverC is the authenciation server.<br />
The program is running on C. It supports user login, password encrpytion, single course query, and multiple course query<br />

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

### I/O handling

#### username

5-50 character. complete by pressing Enter. do not contain space in the input, otherwise client will fail but not terminate <br />

#### password

5-50 character. complete by pressing Enter. do not contain space in the input, otherwise client will fail but not terminate <br />
course query
accept both single course query and multiple course query. courses in multiple course query nees to be splited by ' ', whitespace. <br />
multi course query <br />
course must start with "EE" or "CS". serverM will not pass and other courses back to serverCS nor serverEE and return course not found <br />
if duplicated courses are enteres, the course info will be displayed firat time it show up in the multi course query <br />
if course is not found, system will coutput course not found <br />
single course query <br />
when entering the category looking for, do not input any ' ', whitespace. client will fail but not terminate <br />
the category input is case sensetive. Please enter the category based on the on screen message. <br />

## Message Format
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
if auth passed: "pass" <br />
if user is not in cred.txt: "user not found" <br />
if password is incorrect: "password mismatch" <br /><br />

### single query
#### client to serverM
\<course code>,\<category> <br /><br />
#### serverM to serverCS/serverEE
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
