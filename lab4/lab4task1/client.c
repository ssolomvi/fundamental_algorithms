#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	struct {
		long mestype; /*описание структуры сообщения*/
		long mes;
	} messageto;

	struct {
		long mestype; /*описание структуры сообшения*/
		char mes[100];
	} messagefrom;

	key_t key;
	int mesid;
	long pid = getpid();
	key = ftok("example", 'r');
	mesid = msgget(key, 0); /*присоединение к очереди сообщений*/
	messageto.mestype = 1;
	messageto.mes = pid;
	msgsnd(mesid, &messageto, sizeof(messageto), 0); /* отправка */

	while (msgrcv(mesid, &messagefrom, sizeof(messagefrom), pid, 0) <= 0);

	/*прием сообщения */

	printf("%s\n", messagefrom.mes);

	return 0;

}