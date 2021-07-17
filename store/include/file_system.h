#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "store.h"

#define SUPERBLOQUE_FILE "Superbloques.ims"
#define BLOCKS_FILE "Blocks.ims"

//Paths
char* puntoMontaje;
char* pathSuperbloque;
char* pathBlocks;
char* pathFiles;
char* pathBitacoras;

//Superbloque
char* punteroSuperbloque;
uint32_t tamanioBloque; 
uint32_t cantidadBloques;
t_bitarray* bitmap;

//Blocks
char* punteroBlocks;

//Semáforos
pthread_mutex_t mutex_bitmap;
pthread_mutex_t mutex_bloques;
pthread_mutex_t mutex_escribirBloques;
pthread_mutex_t mutex_metadataRecurso;
pthread_mutex_t mutex_metadataBitacora;


/**** MANEJO FILE SYSTEM ****/
bool verificarFS();
bool verificarExistenciaPuntoMontaje(char*);
void crearSuperbloque();
void crearBlocks();
void crearDirectorio(char*);
void crearArbolDirectorios();
void leerSuperbloque();
void subirBlocksAMemoria();
void finalizarFS();

/**** MANEJO BLOQUES ****/
t_list* escribirBloquesNuevo(char*);
int buscarYAsignarProximoBloqueLibre();
int calcularCantBloques(int);
int verificarQueTengoLosBloquesNecesarios(int);
char* leerBloque(int , int);
char* leerBloques(t_list*, int);
void liberarBloque(int);
void escribirBloque(int, int, char*);
void escribirBloqueUsado(int, int , char*);

/**** MANEJO METADATA ****/
MetadataBitacora* leerMetadataBitacora(char*);
MetadataRecurso* leerMetadataRecurso(tipoRecurso);
char* obtenerDireccionDeMetadataRecurso(tipoRecurso);
char* obtenerDireccionDeMetadataBitacora(char*);
int modificarMetadataRecurso(MetadataRecurso*, tipoRecurso);
int modificarMetadataBitacora(MetadataBitacora*, char*);
int crearMetadataRecurso(tipoRecurso);
int crearMetadataBitacora(char*);
int eliminarArchivoBasura();
void freeMetadataRecurso(MetadataRecurso*);
void freeMetadataBitacora(MetadataBitacora*);

/********** PROTOCOLO SABOTAJE **********/
int verificarCantidadBloques();
int verificarBitmap();
int verificarSizeEnFile();
int verificarBlockCount();
int verificarBlocks();

/**** SOPORTE ****/
t_list* listaFromArray(char**);
t_list* verificarQueArchivosDeRecursosHay();
int verificarExistenciaFile(char*);
int tamanioArchivo(char*);
int cantidadArchivosEnDirectorio(char*);
int tamanioOcupadoRecursoEnBloque(int, tipoRecurso);
char* stringFromList(t_list*);
char* generarMd5(t_list*, int);
bool compararMd5(MetadataRecurso*);
void borrarTodosLosArchivos(char*);
void liberar_array(char**);

/**** DEṔRECADAS ****/
void leerSuperBloqueSinMapear(void);

#endif