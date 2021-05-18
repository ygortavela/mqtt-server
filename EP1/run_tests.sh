#! /bin/bash

# close all clients if script is stopped
trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

SERVER_HOST=$1
PORT=$2
CLIENTS=$3

DURATION=80
SUBS_PID=""

make clean
make

echo "Iniciando servidor na porta $PORT..."
./server $PORT &
SERVER_PID=$!
echo "SERVER PID $SERVER_PID"

sleep 1

psrecord $SERVER_PID --interval 1 --duration $DURATION --plot cpu_mem_performance_$CLIENTS.png &
PSRECORD_PID=$!

sleep 1

i=1
while [ "$i" -le $CLIENTS ]; do
  mosquitto_sub -V mqttv5 -h $SERVER_HOST -p $PORT -t 'test_topic'$i &
  SUBS_PID="$SUBS_PID $!"

  i=$(( i + 1 ))
done

sleep 2

i=5
while [ "$i" -le 60 ]; do
  j=1
  while [ "$j" -le $CLIENTS ]; do
    mosquitto_pub -V mqttv5 -h $SERVER_HOST -p $PORT -t 'test_topic'$j -m 'mensagem de teste no tópico'$j &

    j=$(( j + 1 ))
  done

  i=$(( i + 5 ))
  sleep 5
done

echo "Matando clientes subscribers..."

for sub_pid in $SUBS_PID; do
  kill -9 $sub_pid
done

sleep 1

echo "Matando servidor..."

kill -9 $SERVER_PID

sleep 1

# wait for psrecord plot rendering
wait $PSRECORD_PID

make clean
