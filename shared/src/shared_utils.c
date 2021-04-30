#include "shared_utils.h"



void leer_consola(t_log* logger){
	char* leido;
	leido = readline(">");
	while(*leido != '\0')
	{
		log_info(logger,leido);
		free(leido);
		leido = readline(">");
	}
	free(leido);
}

t_buffer* serializar_lista_strings(t_list* lista){

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) //cantidad de elementos de la lista
			+ peso_de_una_lista(lista); //peso de los elemetos de la lista

	uint32_t cantElementos,tamanio;
	char* char_auxiliar;
	cantElementos = list_size(lista);

	void* stream = malloc(buffer->size);
	int offset = 0;

	// memcpy
	memcpy(stream + offset, &cantElementos, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for(int i = 0; i < cantElementos; i++){
		char_auxiliar = list_get(lista,i);
		tamanio = strlen(char_auxiliar);

		memcpy(stream + offset, &tamanio, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(stream + offset, char_auxiliar, tamanio+1);
		offset += tamanio+1;
	}

 	buffer->stream = stream;

 	return buffer;
}

t_list* deserializar_lista_strings(t_buffer* buffer){

	uint32_t cantElementos, tamanio;
	char* char_auxiliar;
	t_list* lista = list_create();

 	void* stream = buffer->stream;

 	memcpy(&cantElementos, stream, sizeof(uint32_t));
 	stream += sizeof(uint32_t);

 	for(int i = 0; i < cantElementos; i++){
 		memcpy(&tamanio, stream, sizeof(uint32_t));
 		stream += sizeof(uint32_t);
 		char_auxiliar = malloc(tamanio+1);
 		memcpy(char_auxiliar, stream, tamanio+1);
 		stream += tamanio+1;

 		list_add(lista,char_auxiliar);
 	}
	return lista;
}

t_buffer* serializar_numero(uint32_t numero){

 	t_buffer* buffer = malloc(sizeof(t_buffer));
 	buffer->size = sizeof(uint32_t);

 	void* stream = malloc(buffer->size);
 	int offset = 0;

 	memcpy(stream + offset, &numero, sizeof(uint32_t));
 	offset += sizeof(uint32_t);

 	buffer->stream = stream;

 	return buffer;
 }

uint32_t deserializar_numero(t_buffer* buffer){
 	uint32_t numero;

 	void* stream = buffer->stream;

 	memcpy(&numero, stream, sizeof(uint32_t));

	return numero;
 }


t_buffer* serializar_string(char* nombre){

 	t_buffer* buffer = malloc(sizeof(t_buffer));
 	buffer->size = sizeof(uint32_t) + strlen(nombre) + 1;
 	uint32_t tamanio = strlen(nombre);

 	void* stream = malloc(buffer->size);
 	int offset = 0;

 	memcpy(stream + offset, &tamanio, sizeof(uint32_t));
 	offset += sizeof(uint32_t);
 	memcpy(stream + offset, nombre, strlen(nombre)+1);
 	offset += strlen(nombre)+1;

 	buffer->stream = stream;

 	return buffer;
 }

char* deserializar_string(t_buffer* buffer){
 	uint32_t tamanio;
 	char* nombre;

 	void* stream = buffer->stream;

 	memcpy(&tamanio, stream, sizeof(uint32_t));
 	stream += sizeof(uint32_t);

 	nombre = malloc(tamanio);

 	memcpy(nombre, stream, tamanio);

	return nombre;
 }



size_t peso_de_una_lista(t_list* lista){

	t_list* lista_auxiliar;
	char* char_auxiliar;
	size_t size = 0;

	lista_auxiliar = list_duplicate(lista);

	for(int i = 0 ; i < list_size(lista_auxiliar); i++){

		char_auxiliar = list_get(lista_auxiliar,i);
		size = size + (strlen(char_auxiliar)+1);

	}

	size = size + (sizeof(uint32_t)*list_size(lista));

	return size;
}

