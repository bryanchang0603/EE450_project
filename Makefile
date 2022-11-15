serverC: serverC.c
	gcc -o serverC serverC.c

serverCS: serverCS.c 
	gcc -o serverCS serverCS.c 

serverEE: serverEE.c
	gcc -o serverEE serverEE.c 

serverM: serverM.c 
	gcc -o serverM serverM.c 

client: client.c 
	gcc -o client client.c 

all: serverC serverCS serverEE serverM client

clear:
	rm client
	rm serverC
	rm serverCS
	rm serverEE
	rm serverM