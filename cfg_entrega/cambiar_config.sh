####### CONFIGURACIONES ##################

##### DISCORDIADOR ##### 
NUEVA_IP_DISCORDIADOR=192.168.1.1
NUEVO_PUERTO_DISCORDIADOR=50

##### MI RAM ##### 
NUEVA_IP_MI_RAM=20.20.20.20
NUEVO_PUERTO_MI_RAM=23

##### STORE ##### 
NUEVA_IP_STORE=30.30.30.30
NUEVO_PUERTO_STORE=44


####################### NO MODIFICAR ####################### 

######  01.estabilidad_FIFO

CARPETA=01.estabilidad_FIFO

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"




######  01.estabilidad_RR

CARPETA=01.estabilidad_RR

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"



### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"










######  02.discordiadorCPU_FIFO

CARPETA=02.discordiadorCPU_FIFO

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"












######  02.discordiadorCPU_RR

CARPETA=02.discordiadorCPU_RR

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"





######  03.discordiadorES_FIFO

CARPETA=03.discordiadorES_FIFO

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"








######  03.discordiadorES_RR

CARPETA=03.discordiadorES_RR

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"







###### 04.memoriaSEG_BF

CARPETA=04.memoriaSEG_BF

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"








###### 04.memoriaSEG_FF

CARPETA=04.memoriaSEG_FF

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"












###### 05.memoriaPAG_CLOCK

CARPETA=05.memoriaPAG_CLOCK

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"












###### 05.memoriaPAG_LRU

CARPETA=05.memoriaPAG_LRU

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"









###### 06.filesystem

CARPETA=06.filesystem

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"


### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"











###### 07.filesystemFSCK

CARPETA=07.filesystemFSCK

### DISCORDIADOR
CONFIG_FILE="$CARPETA"/discordiador.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/IP_MI_RAM_HQ=//"`
sed "s/^IP_MI_RAM_HQ=.*/IP_MI_RAM_HQ="$NUEVA_IP_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_MI_RAM_HQ=" "$CONFIG_FILE" | sed "s/PUERTO_MI_RAM_HQ=//"`
sed "s/^PUERTO_MI_RAM_HQ=.*/PUERTO_MI_RAM_HQ="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/IP_I_MONGO_STORE=//"`
sed "s/^IP_I_MONGO_STORE=.*/IP_I_MONGO_STORE="$NUEVA_IP_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_I_MONGO_STORE=" "$CONFIG_FILE" | sed "s/PUERTO_I_MONGO_STORE=//"`
sed "s/^PUERTO_I_MONGO_STORE=.*/PUERTO_I_MONGO_STORE="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

### MIRAM
CONFIG_FILE="$CARPETA"/miram.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_MI_RAM"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"



### STORE
CONFIG_FILE="$CARPETA"/store.cfg

old_ip=`grep "^PUERTO=" "$CONFIG_FILE" | sed "s/PUERTO=//"`
sed "s/^PUERTO=.*/PUERTO="$NUEVO_PUERTO_STORE"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^IP_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/IP_DISCORDIADOR=//"`
sed "s/^IP_DISCORDIADOR=.*/IP_DISCORDIADOR="$NUEVA_IP_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"

old_ip=`grep "^PUERTO_DISCORDIADOR=" "$CONFIG_FILE" | sed "s/PUERTO_DISCORDIADOR=//"`
sed "s/^PUERTO_DISCORDIADOR=.*/PUERTO_DISCORDIADOR="$NUEVO_PUERTO_DISCORDIADOR"/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"



############## FIN
