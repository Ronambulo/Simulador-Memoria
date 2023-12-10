#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//----------------------------------------------VGLOBALES---------------------------------------------------

#define TAM_LINEA 16
#define NUM_FILAS 8

//----------------------------------------------ESTRUCTURA---------------------------------------------------

typedef struct{
	unsigned char ETQ;
	unsigned char Data[TAM_LINEA];
}T_CACHE_LINE;

//----------------------------------------------FUNCIONES---------------------------------------------------

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int*palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ,int linea, int bloque);

char guardarValor(T_CACHE_LINE *tbl, int palabra, int linea);
void MeterEnRAM(FILE* ficheroContRAM, unsigned char Simul_RAM[4096]);
int FicherosExisten(FILE* f1, FILE* f2, FILE* f3);
void CerrarFicheros(FILE* f1, FILE* f2, FILE* f3);

//----------------------------------------------MAIN---------------------------------------------------


int main(){
	int numFallos = 0;
	int globalTime = 0;

	unsigned int Hex;
	int ETQ=0, palabra=0, linea=0, bloque=0;
	int contAccesos = 0, index = 0;
	char textoRamPalabra[16];

	//Aquí se guarda lo que hay dentro del fichero .bin

	FILE* ficheroDirMemoria = fopen("dirs_memoria.txt", "r");
	FILE* ficheroContRAM = fopen("CONTENTS_RAM.bin", "r");
	FILE* ficheroAcessoMemoria = fopen("accesos_memoria.txt", "r");



	T_CACHE_LINE lineaCache[NUM_FILAS]; //tbl
	unsigned char Simul_RAM[4096]; //char *MRAM

	if(FicherosExisten(ficheroAcessoMemoria, ficheroContRAM, ficheroDirMemoria) == -1)
		return(-1);

	MeterEnRAM(ficheroContRAM, Simul_RAM);
	ficheroAcessoMemoria = fopen("accesos_memoria.txt", "r");

	fopen("CONTENTS_RAM.bin", "r");


	LimpiarCACHE(lineaCache);
	while((fscanf(ficheroAcessoMemoria,"%X",&Hex)!=EOF)){
		ParsearDireccion(Hex, &ETQ, &palabra, &linea, &bloque);
		if(ETQ != lineaCache[linea].ETQ){
			numFallos++;
			globalTime += 20;
			printf("\n\nHa habido un fallo: fallo n: %d\n", numFallos);
			printf("T: %d, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X\n\n", globalTime, numFallos, Hex, ETQ, linea, palabra, bloque);
			TratarFallo(lineaCache, Simul_RAM, ETQ, linea, bloque);
		}

		//hacer lo del array
		textoRamPalabra[index] = guardarValor(lineaCache, palabra, linea);
		index++;
		printf("\nT: %d, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X\n\n", globalTime, Hex, ETQ, linea, palabra, bloque);
		for(int i = 0; i < NUM_FILAS; i++){
			printf("%X\tDatos: ", lineaCache[i].ETQ);
			for(int j = 0; j < TAM_LINEA; j++){
				printf("%X ", lineaCache[i].Data[j]);
			}
			printf("\n");
		}
		contAccesos++;
		sleep(1);


	}
	printf("\n\n\n");
	printf("Accesos totales: %d; Fallos %d; Tiempo Medio %f\n\n", contAccesos, numFallos, (float)(globalTime/contAccesos));
	VolcarCACHE(lineaCache);
	for(int i = 0; i <= index; i++){
		printf("%c", textoRamPalabra[i]);
	}
	printf("\n");
	fclose(ficheroContRAM);

}

//----------------------------------------------FUNCIONES---------------------------------------------------

// Esta función sirve para limpiar todas los valores de la cache, tanto de las etiquetas como de los datos.
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){
	for(int i =0; i<NUM_FILAS;i++){

		tbl[i].ETQ = 0xFF;

		for(int j = 0; j < TAM_LINEA; j++)
		{
			tbl[i].Data[j] = 0x23;
		}
	}
}


//Antes de acabar el programa, vuelca los contenidos dentro del fichero .bin
void VolcarCACHE(T_CACHE_LINE *tbl) {

	FILE* ficheroContentCache = fopen("CONTENTS_CACHE.bin", "w+");

    for (int i = 0; i < NUM_FILAS; i++) {
   	for (int j = 0; j < TAM_LINEA; j++) {
        	fprintf(ficheroContentCache, "%c", tbl[i].Data[j]);
    	}
        fprintf(ficheroContentCache, "\n");
    }

	fclose(ficheroContentCache);

}


void ParsearDireccion(unsigned int addr, int *ETQ, int*palabra, int *linea, int *bloque){

	//Utilizamos & con la máscara binaria 0b1111 para obtener los últimos 4 bits de la dirección. Estos 4 bits representarán la
	//posición de la palabra dentro del bloque de memoria.
	*palabra = addr & 0b1111;

	//El operador >>, lo utilizamos para realizar desplazamiento de bits de la dirección addr 4 lugares hacia la derecha.
	//(Esto es como dvidir la dirección entre 16(2⁴=16)).
    	*bloque = addr >> 4;

	//Extraemos los últimos 3 bits del bloque
    	*linea = (*bloque & 0b111);

	//Extraemmos los primeros 5 bits del bloque y desplazamos a la derecha 3 posiciones para ajustar estos 5 bits (para colocarlo adecuadamente)
    	*ETQ = (*bloque & 0b11111000)>>3;
	//printf("%i, %i, %i, %i\n", *palabra, *bloque, *linea, *ETQ);

	*bloque = *ETQ * NUM_FILAS;
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ,int linea, int bloque){
	int j = bloque * TAM_LINEA; //tam linea == tam bloque

	tbl[linea].ETQ = ETQ;

	for(int i = 0; i < TAM_LINEA; i++)
		tbl[linea].Data[i] = MRAM[i + j];
}

char guardarValor(T_CACHE_LINE *tbl, int palabra, int linea){
	return tbl[linea].Data[palabra];
}

void MeterEnRAM(FILE* ficheroContRAM, unsigned char Simul_RAM[4096]){
	char input = '\0';
	int cont = 0;

	fopen("CONTENTS_RAM.bin", "rb");
	while((input = getc(ficheroContRAM)) != '\0' && !(feof(ficheroContRAM))){
		Simul_RAM[cont] = input;
		cont++;
		//printf("%c", Simul_RAM[cont]);
	}
	fclose(ficheroContRAM);
}

//Comprobación del ficheros
int FicherosExisten(FILE* f1, FILE* f2, FILE* f3){

	if((f1 == NULL)  || (f2 == NULL) || (f3 == NULL))
	{
		printf("Error: algun fichero no existe");
		return(-1);
	}
	return 0;
	CerrarFicheros(f1,f2,f3);
}

//Cierre de ficheros
void CerrarFicheros(FILE* f1, FILE* f2, FILE* f3){
	fclose(f1);
	fclose(f2);
	fclose(f3);
}
