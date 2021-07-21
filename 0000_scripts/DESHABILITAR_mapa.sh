######  01.estabilidad_FIFO

cd ..
cd miram
cd cfg

CONFIG_FILE=miram.cfg

old_ip=`grep "^MAPA=" "$CONFIG_FILE" | sed "s/MAPA=//"`
sed "s/^MAPA=.*/MAPA=DESHABILITADO/" "$CONFIG_FILE" > /tmp/temp.x
mv /tmp/temp.x "$CONFIG_FILE"