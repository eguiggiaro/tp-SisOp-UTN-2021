all: 
	-cd discordiador && $(MAKE) all
	-cd miram && $(MAKE) all
	-cd store && $(MAKE) all
	-cd utils && $(MAKE) all

clean:
	-cd discordiador && $(MAKE) clean
	-cd miram && $(MAKE) clean
	-cd store && $(MAKE) clean
	-cd utils && $(MAKE) clean

