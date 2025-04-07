# Simulador de Memoria a Bajo Nivel

Este proyecto es un simulador de memoria de bajo nivel implementado en C. La aplicación emula el funcionamiento de una caché de memoria, gestionando accesos, detección de fallos y volcado de datos mediante la interacción con archivos que representan la memoria principal y los accesos realizados.

## Características

- **Simulación de caché:** Implementa una estructura para la caché compuesta por 8 líneas, cada una con 16 bytes.
- **Parseo de direcciones:** Descompone una dirección en sus componentes (etiqueta, línea, palabra y bloque) utilizando operaciones a nivel de bits.
- **Manejo de fallos:** Detecta fallos de caché y actualiza la línea correspondiente con datos extraídos de la memoria simulada.
- **Volcado de datos:** Al finalizar, el contenido de la caché se guarda en un archivo binario.
- **Interacción con archivos:** Utiliza archivos externos para configurar la memoria y registrar los accesos.

## Requisitos

- **Compilador de C:** Se recomienda utilizar GCC.
- **Librerías estándar:** `stdio.h`, `stdlib.h` y `unistd.h`.
- **Sistema operativo:** Linux, macOS o cualquier sistema compatible con compilación en C.

## Compilación

Para compilar el proyecto, ejecuta el siguiente comando en el directorio del proyecto:

```bash
gcc -o simulador main.c
```

Esto generará un ejecutable llamado `simulador`.

## Ejecución

Antes de ejecutar el simulador, asegúrate de contar con los archivos necesarios en el mismo directorio (por ejemplo, `dirs_memoria.txt`, `accesos_memoria.txt` y `CONTENTS_RAM.bin`).

Luego, ejecuta el simulador con:

```bash
./simulador
```

## Licencia

Este proyecto se distribuye bajo la [Licencia MIT](LICENSE) (o la licencia que prefieras).

## Realizado por

- Saray Hershkovich [@sasahershko](https://github.com/sasahershko)
- Enrique Rodríguez [@ronambulo](https://github.com/ronambulo)
