#include "ejecucion_tareas.h"
#include "store.h"

void generarRecursos(tipoRecurso recurso, int cantidad){

    verificarMetadata(recurso); //Verifico si existe la metadata del recurso, sino lo creo. Por ej: Oxigeno.ims


//-------------------------------------------------APERTURA DE ARCHIVOS-----------------------------------------------------------------------------

    int archivoBlocks = open("Blocks.ims",O_RDWR); //abro el archivo blocks en modo lectura/escritura

    if (archivoBlocks == -1) {
        exit(1); //fallo apertura de archivo
    }

    int tamanioDeBlocks = calcularTamanioBlocks(); //Calcula el tamaño de Block.ims, tomando los datos del archivo SuperBloque y haciendo Block_size * Blocks

    int copiaArchivoBlocks = open("copia", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); //creo un archivo nuevo

    if (copiaArchivoBlocks == -1){
       exit(1); //fallo apertura de archivo
    }
    
    if (ftruncate(copiaArchivoBlocks, tamanioDeBlocks) == -1){ //trunco la copia del archivo al tamaño de blocks.
        exit(1); //fallo recorte de archivo copia
    }

    
    int archivoMetadata = open(obtenerMetadata(recurso), O_RDWR); //Abro el archivo de metadata del recurso
    

//-----------------------------------------------MAPEO DE ARCHIVOS A MEMORIA-----------------------------------------------------------------------
    
    char *punteroBloque = mmap(0, tamanioDeBlocks, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBlocks, 0); //mapeo el archivo Blocks a memoria
    //char * mmap (void *address, size_t length, int protect, int flags, int filedes, off_t offset)
    
    if (punteroBloque == MAP_FAILED){
        exit(1); //fallo mapeo de archivo
    }
    
    char *punteroBloqueCopia = mmap(NULL, tamanioDeBlocks, PROT_READ | PROT_WRITE, MAP_SHARED, copiaArchivoBlocks, 0); //mapeo el archivo vacio a memoria
    
    if (punteroBloqueCopia == MAP_FAILED){
        exit(1); //fallo mapeo de archivo
    }

    memcpy(punteroBloqueCopia, punteroBloque, tamanioDeBlocks);  //copio el contenido del archivo Blocks en el archivo vacio creado

            

    //---------------------------------------------------ESCRITURA DE DATOS------------------------------------------------------------------------------------

    int posicionDisponible = buscarposicionDisponible(archivoMetadata); //busco si tengo un lugar disponible y devuelvo la posición
    
    agregarRecurso(punteroBloqueCopia, recurso, posicionDisponible, cantidad, archivoMetadata); //agrego la cantidad de caracteres en el archivo


    //---------------------------------------------CIERRE, LIBERACION DE MEMORIA Y SINCRONIZACION---------------------------------------------------------
    
   
        
    if (msync(punteroBloqueCopia, tamanioDeBlocks, MS_SYNC) == -1){ //Sincronizo la copia del archivo bloque a disco
        exit(1); //Fallo la sincronizacion
    }

    if (munmap(punteroBloqueCopia, tamanioDeBlocks) == -1){ //desmapeo la copia del archivo bloque
        exit(1);//Fallo el desmapeo de copia
    }

    if (munmap(punteroBloque, tamanioDeBlocks) == -1){ //desmapeo el archivo bloque
       exit(1);//Fallo el desmapeo de original
    }

    close(archivoBlocks); 
    close(copiaArchivoBlocks);
    close(archivoMetadata); 
}

void agregarRecurso(char * punteroBloqueCopia, int posicionDisponible, int cantidad, int archivoMetadata, char* cadenaAEscribir){

            llenarDeCaracteres(punteroBloqueCopia, posicionDisponible, cadenaAEscribir, archivoMetadata); 

}

void llenarDeCaracteres(char * punteroBloquedisponible, int posicionDisponible, char * cadenaAEscribir, int archivoMetadata){
    
    /*
    char src[] = "OOO";
    char dest[6] = "OOOO--";

    memcpy(punteroBloquedisponible,src,6);

      printf("dest after first memcpy() => %s\n",dest);

      memcpy(dest+4, src ,2);

      printf("dest after second memcpy() => %s\n",dest);

      return 0;
 
 /*
    char src[] = cadenaAEscribir; //ej "OOOO"
    char dest[]  = punteroBloqueCopia; //ej "OO"
    int cantEspaciosLibresEnBloque = cantPosicionesLibresEn(posicionDisponible); //caulcula cuantos espacios libres tengo en ese bloque
    
    if (string_length(cadenaAEscribir) > cantEspaciosLibresEnBloque){
        
        src =  string_substring_until(cadenaAEscribir, cantEspaciosLibresEnBloque-1); //retorna los primeros caracteres de la cadena a escribir
        
        memcpy(dest, src, strlen(src)+1);
        
        posicionDisponible = buscarposicionDisponible(archivoMetadata);
        
        char * restanteAEscribir =  string_substring_from(cadenaAEscribir, cantEspaciosLibresEnBloque); //retorna los ultimos caracteres desde el indice

        llenarDeCaracteres(punteroBloqueCopia, posicionDisponible, restanteAEscribir, archivoMetadata);
    }

    src = cadenaAEscribir;
    
    memcpy(dest, src, strlen(src)+1);

  /* 
    int cantEspaciosLibresEnBloque = cantPosicionesLibresEn(posicionDisponible);

    for(int i = 1; i<=cantEspaciosLibresEnBloque; i++){

        punteroBloqueCopia[posicionDisponible] = caracterDeLlenado; //Agrega el caracter deseado al lugar apuntado disponible
        posicionDisponible++;

    }

    if(cantidadCaracteres > cantEspaciosLibresEnBloque){
        
        int cantidadCaracteresRestantes = cantidadCaracteres - cantEspaciosLibresEnBloque;

        posicionDisponible = buscarposicionDisponible(archivoMetadata);
        
        llenarDeCaracteres(punteroBloqueCopia, posicionDisponible, caracterDeLlenado, cantidadCaracteresRestantes, archivoMetadata);
    }*/


        
}

void verificarMetadata(tipoRecurso recurso){
    //Verifico si existe el archivo metadata de ese, sino lo creo.
}

int obtenerMetadata(tipoRecurso recurso) {
    //Devuelve el archivo de metadata correspondiente al recurso
}

int calcularTamanioBlocks(){
    //Calcula el tamaño de Blocks.ims
}
