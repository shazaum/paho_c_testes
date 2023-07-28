#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MQTTClient.h>
#include <unistd.h>
#include <time.h>

#define MQTT_ADDRESS   "tcp://mqtt..."
#define CLIENTID       "paho-renato"

#define MQTT_PUBLISH_TOPIC     "um topico"
#define MQTT_SUBSCRIBE_TOPIC   "outro topico ou o mesmo"

#define PAYLOAD     "Hi"
#define QOS         0
#define TIMEOUT     10000L
#define USERNAME    "user"
#define PASSWORD    "passwd"

MQTTClient client;

void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);

void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 0;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;

    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(int argc, char *argv[]){
    int rc;

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    /* Inicializacao do MQTT (conexao & subscribe) */
    MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    rc = MQTTClient_connect(client, &conn_opts);

    if (rc != MQTTCLIENT_SUCCESS)
    {
       printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
       exit(-1);
    } else {
       printf("\n\rConectado... Cod: %d\n", rc);
       publish(client, MQTT_PUBLISH_TOPIC, "Conectado ao server mqtt da expectrun.");
    }

   MQTTClient_subscribe(client, MQTT_SUBSCRIBE_TOPIC, 0);
    while(1) {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        int h,min,sec;
        h = tm->tm_hour;
        min = tm->tm_min;
        sec = tm->tm_sec;
        char ping[100];
        snprintf(ping, 100,"Ainda estou por aqui... %d:%d:%d",h,min,sec);

        sleep(10);
        publish(client, MQTT_PUBLISH_TOPIC, ping);
    }
}