#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

/*
Рассмотрим еще один пример - пусть существует процесс-сервер и несколько процессов-клиентов. 
Все они могут обмениваться данными, используя одну очередь сообщений. Для этого сообщениям, 
направляемым от клиента к серверу, присваиваем значение типа 1. При этом процесс, отправивший сообщение, 
в его теле передает некоторую информацию, позволяющую его однозначно идентифицировать. Тогда сервер, 
отправляя сообщение конкретному процессу, в качестве его типа указывает эту информацию (например, PID процесса). 
Таким образом, сервер будет читать из очереди только сообщения типа 1, 
а клиенты — сообщения с типами, равными идентификаторам их процессов.
*/


int main(int argc, char** argv)

{
    struct {
        long mestype;
        char mes[100];
    } messageto;

    struct {
        long mestype;
        long mes;

    } messagefrom;

    key_t key;

    int mesid;
    key = ftok("example", 'r');
    mesid = msgget(key, 0666 | IPC_CREAT);

    while (1)
    {
        if (msgrcv(mesid, &messagefrom, sizeof(messagefrom), 1, 0) <= 0) continue;
        messageto.mestype = messagefrom.mes;
        strcpy(messageto.mes, "Message for client");
        msgsnd(mesid, &messageto, sizeof(messageto), 0);
    }

    msgctl(mesid, IPC_RMID, 0);

    return 0;

}