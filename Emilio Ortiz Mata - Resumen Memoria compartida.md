Resumen
Memoria Compartida
  

































Emilio Ortiz Mata
________________


Chequeo de posicionamiento de datos en memoria compartida


Para entender de manera completa el como usar la memoria compartida de forma efectiva, se examinará ejemplos simples del uso de memoria compartida, incluyendo los siguientes temas:


* Arrays Cuadrados contra Rectangulares.


* Acceso de fila mayor contra columna mayor.


* Declaraciones estáticas contra dinámicas de memoria compartida.


* Alcance de archivos contra alcance de kernel en memoria compartida.


* Relleno de memoria contra memoria sin relleno.


Cuando diseñas tu kernel que usa memoria compartida, tu enfoque debe de ser en lo siguiente:


* Mapear elementos de datos a través de los bancos de memoria.
* Mapear los índices de threads al desplazamiento de memoria compartida.


Memoria compartida cuadrada
Puede usar memoria compartida a datos de cache global con dimensiones cuadradas de manera directa. La dimensionalización de un array cuadrado hace fácil el calcular memoria desplazada de 1D a índice de threads 2D. Una memoria compartida, se divide en 32 bancos con múltiples filas, las filas dependen de la cantidad de datos que sean necesarios.


Puedes declarar una variable estática en memoria compartida 2D , de la siguiente forma:
_shared_ int tile [N][N];


Porque esta memoria compartida es cuadrada, puedes escoger el acceder a ella a través de un thread block 2D con threads que lo rodean, accediendo a elementos a su alrededor en la dimensión “x” como en “y”:


tile[threadIDx.y] [threadIDx.x]
tile[threadIDx.x] [threadIDx.y]


Acceso a fila-mayor contra columna-mayor
Considera un ejemplo donde un grid con un bloque 2D contiene 32 threads en cada dimensión es utilizado. Puedes definir las dimensiones del bloque usando lo siguiente macro:


#define BDIMX 32
#define BDIMY 32


Tambien podrias usar el macro para definir la configuración de ejecución para el kernel:


dim3 block (BDIMx, BDIMY);
dim3 grid (1, 1);


El kernel tiene dos simples operaciones:


* Escribir índices de threads globales a un array de memoria compartida 2D en orden de fila mayor.
* Leer los valores de la memoria compartida en orden de fila mayor y guardarlos a memoria global.


Primero puedes declarar un array estático de memoria compartida 2D:
_share_ int tile [BDIMY][BDIMX]


Después, necesitas calcular el índice de thread global por cada thread de su thread 2D ID. Porque solo un thread block va a ser lanzado, la conversión de indice puede ser simple:
        unsigned int idx = threadIdx.y * blockDim.x + threadIdx.x


idx va a ser utilizado para simular el valor leído de la matriz de entrada. Guardando el valor de idx al array de salida permitiendo el visualizar el patrón de acceso del kernel basado en donde los threads escriben su IDs globales.


Escribiendo los índices globales de threads en un espacio de la memoria compartida en orden de fila mayor pueden ser hechas de la siguiente manera:
tile[threadIdx.y] [threadIdx.x] = idx;


Una ves un punto de sincronización es alcanzado, todos los threads deben de guardar los datos al espacio de memoria compartida, para que puedas asignar valores a la memoria global desde memoria compartida en orden de fila mayor como se muestra: 
        out [idx] = tile[threadIdx.y] [threadIdx.x];


De momento tendrás 3 operaciones en el kernel:


* Una operación de guardar en memoria compartida.
* Una operación de cargar en memoria compartida.
* Una operación de guardar en memoria global.


Porque threads en el mismo warp tienen threadIdx.x con valores consecutivos y usa la dimensión más central del array de memoria compartida, el kernel es libre de conflictos en el banco.


Por el otro lado, si tu cambias “threadIdx.y” y  “threadIdx.x” cuando se asignan datos a la memoria compartida, el acceso de memoria de un warp va a ser de orde columna mayor. Cada cargado y guardado en memoria compartida va a causar un conflicto de banco de 32 caminos para un dispositivo Fermi, y un conflicto de 16 caminos para un dispositivo Kepler.


Antes de probar los kernels, debes preparar la Memoria global. Estás motivado a escribir la función main por ti mismo.


Escribiendo fila mayor y leyendo columna mayor
El siguiente kernel implementa memoria compartir escrita en orden de fila mayor, y leída en orden de columna mayor. Escribir a la memoria compartida en orden de fila mayor es implementado al poner la dimensión más central del índice de thread como columna en la memoria compartida 2D:
tile[threadIdx.y] [threadIdx.x] = idx;


Asignar valores a la memoria global desde la memoria compartida en orden de columna mayor es implementado al cambiar los índices de thread cuando se referencia a la memoria compartida:
        out [idx] = tile[threadIdx.x] [threadIdx.y];


Memoria compartida dinámica
Puedes declarar los kernel de ciertas formas al declarar la memoria compartida como dinámica. Puedes hacer la declaración global de la memoria compartida para hacerla global, o dentro del kernel para restringirlo al kernel. Memoria compartida dinámica debe ser declarada como un array sin tamaño de 1D; por lo que, necesitar calcular el índice de acceso de memoria basado en índices de threads 2D. Porque se va a escribir como orden de fila mayor y leer como orden de columna mayor, necesitas el mantener dos índices:


* row_idx: 1D fila mayor calculado desde índices de threads 2D.
* col_idx: 1D columna mayor calculado desde índices de threads 2D.


Escribes a la memoria compartida en orden de fila mayor usando el calculado row_idx como se muestra:
        tile [row_idx] = row_idx;


Usando sincronización apropiada después de que el espacio de memoria compartida haya sido llenado, entonces lees en orden de columna mayor y asigna memoria global como se muestra:
        out [row_idx] = tile [col_idx];


Porque out es guardado en memoria global y los threads estan organizados en orden de fila mayor dentro de un thread block, quieres el escribir a out en orden de fila mayor mediante coordenadas del thread para asegurar su guardado.


__global__ void setRowReadColDyn(int *out) {


// dynamic shared memory
extern __shared__ int tile[];


// mapping from thread index to global memory index
unsigned int row_idx = threadIdx.y * blockDim.x + threadIdx.x;
unsigned int col_idx = threadIdx.x * blockDim.y + threadIdx.y;


// shared memory store operation
tile[row_idx] = row_idx;


// wait for all threads to complete
__syncthreads();


// shared memory load operation
out[row_idx] = tile[col_idx];
}


Se debe definir el tamaño de la memoria compartida cuando se lanza el kernel, de la siguiente forma:
        setRowReadColDyn<<<grid, block, BDIMX * BDIMY * sizeof (int)>>>(d_C);


Padding estático
Como se describe “Memory Padding” arrays de padding es una forma de evitar conflictos de banco. Padding estático declarado en memoria compartida es muy directo. Simplemente agrega una columna a la memoria compartida 2D de la siguiente forma:
        _shared_ int tile[BDIMY][BDIMX + 1];


Para dispositivos Fermi, necesito un pad de una columna para resolver el conflicto de banco. Para dispositivos Kepler, no siempre es verdad. El número de elementos de datos necesarios para un pad en cada fila para dispositivos Kepler depende del tamaño de la memoria compartida 2D. Por lo que deberías hacer más pruebas para determinar el número apropiado de paddings para modo de acceso de 64-bit.


Padding dinámico
Ponerle padding a una memoria compartida dinámica es más complejo. debes de saltarte un espacio de memoria con padding por cada fila cuando se realiza la conversión de índice de índices de thread 2D a índices de memoria 1D, como se muestra:
        unsigned int row_idx = threadIdx.y * (blockDim.x + 1) + threadIdx.x;
        unsigned int col_idx = threadIdx.x * (blockDim.x + 1) + threadIdx.y;


Porque la memoria global usada para guardar datos en el siguiente kernel es mas chico que la memoria compartida con padding, necesitaria 3 índices: un índice para la escritura de fila mayor en memoria compartida, un índice para lectura en columna mayor en memoria compartida, y un índice para fusionar accesos a memoria global sin padding, como se muestra:
__global__ void setRowReadColDynPad(int *out) {


// dynamic shared memory
extern __shared__ int tile[];


// mapping from thread index to global memory index
unsigned int row_idx = threadIdx.y * (blockDim.x + IPAD) + threadIdx.x;
unsigned int col_idx = threadIdx.x * (blockDim.x + IPAD) + threadIdx.y;
unsigned int g_idx = threadIdx.y * blockDim.x + threadIdx.x;


// shared memory store operation
tile[row_idx] = g_idx;


// wait for all threads to complete
__syncthreads();


// shared memory load operation
out[g_idx] = tile[col_idx];
}


 Para especificar el tamaño de memoria compartida con padding lanzando el kernel, es la siguiente: 
        setRowReadColDynPad<<<grid,block,(BDIMX+1)*BDIMY*sizeof(int)>>>(d_C);


Memoria compartida rectangular
Memoria compartida rectangular es un caso más general de memoria compartida 2D, donde el número de filas y columnas no son iguales.
        _shared_ int tile [Row][Col];


No es posible el cambiar simplemente las coordenadas del thread usadas para referenciar un array rectangular cuando se hace la operación de transporte, como en la implementación de memoria compartida cuadrada. Hacer eso causaría una violacion al acceso de la memoria cuando se usa memoria compartida rectangular. Necesitarás re-implementar todos los kernels recalculando índices de acceso basados en matrices dimensionales. 


Sin perder generalidad, vas a examinar un array de memoria compartida rectangular con 32 elementos por fila y 16 por columna. Siendo definidas de la siguiente forma:
        #define BDIMX 32
#define BDIMY 16


El asignar un espacio de la memoria compartida rectangular es:
        _shared_ int tile[BDIMY] [BDIMX];


Accediendo fila mayor contra columna mayor
Necesitas poner atención a la declaración del array de memoria compartida rectangular en cada kernel. En el kernel setRowReadRow, la longitud de la dimensión central del array de memoria tile es puesto en la misma dimensión a la dimensión central del thread block 2D.


En el kernel setColReadCol, la longitud de la dimensión central del array de memoria es puesto en la misma dimensión a la dimensión más lejana del thread block 2D.


El guardado y cargado de pedidos a la memoria compartida son servidos por un kernel de transición setRowReadRow. Los mismos pedidos son servidos por 8 transiciones en el kernel setColReadCol.


El tamaño de banco del Kepler K40 es de 8 palabras, transponiendo una matriz usando memoria compartida en 8 bancos, lo que responde con un conflicto de 8 caminos.


Escribiendo fila mayor contra columna mayor
El kernel tiene 3 operaciones de memoria:
* Escribir en una fila de memoria compartida con cada warp para prevenir conflictos.


* Leer de una columna de memoria compartida con cada war haciendo una matriz transpuesta.


* Escribir a una fila de memoria global donde para cada warp con acceso fusionado.


El proceso para calcular la memoria compartida y global correcta es de la siguiente forma. Primero el índice de thread 2D del thread actual es convertido en una ID global del thread en 1D. 
        unsigned int irow = idx / blockDim.y;
unsigned int icol = idx % blockDim.y;


Inicializando el espacio de memoria compartida al guardar el Id global del thread al espacio 2D del espacio de la memoria compartida:
        tile[threadIdx.y][threadIdx.x] = idx;


En este punto, el dato en memoria compartida es guardado linealmente de 0 a BDIMXxBDIMY-1. Ya que cada warp hace escritura de fila mayor a la memoria compartida, no hay conflictos durante la operación de escritura. 


Ahora puedes acceder a los datos de la memoria compartida para ser transportados con las coordenadas calculadas anteriormente. Al acceder a la memoria compartida usando filas y columnas cambiadas, puedes escribir los datos transportados a la memoria global usando el ID del thread en 1D.


Declarar dinámicamente memoria compartida
Porqué memoria compartida dinámica sólo puede ser declarada como array de 1D, un nuevo índice es requerido a convertirse de coordenadas de thread 2D a índice de memoria compartida 1D cuando escribiendo por filas y leyendo por columnas.


Porque la columna corresponde a la dimensión central del thread block, esta conversión produce acceso de columna mayor a memoria compartida, resultando en conflictos de banco.


El tamaño de la memoria compartida debe ser especificado como parte del kernel.


Padding declarado estáticamente en memoria compartida
También puedes usar padding de memoria compartida para resolver conflictos de banco para memoria rectangular. Aunque, para dispositivos Kepler debes de calcular cuantos paddings son necesarios.


Cambiar el número de paddings de un kepler puede permitir el evitar conflictos de banco.


Padding declarado dinámicamente en memoria compartida
Las técnicas de padding pueden ser aplicadas a memoria compartida dinámica de kernel que usa regiones de memoria rectangular. Porque la memoria compartida con padding y la memoria global tendrán diferentes tamaños, 3 índices por thread deben de ser mantenidos en el kepler:
* row_idx: Un índice de fila mayor a la memoria compartida con padding.


* col_idx: Un índice de columna mayor a la memoria compartida con padding.


* g_idx: Un índice a memoria global linear.


El código para el kernel es el siguiente:


__global__ void setRowReadColDynPad(int *out) {


// dynamic shared memory
extern __shared__ int tile[];


// mapping from thread index to global memory index
unsigned int g_idx = threadIdx.y * blockDim.x + threadIdx.x;


// convert idx to transposed (row, col)
unsigned int irow = g_idx / blockDim.y;
unsigned int icol = g_idx % blockDim.y;
unsigned int row_idx = threadIdx.y * (blockDim.x + IPAD) + threadIdx.x;


// convert back to smem idx to access the transposed element
unsigned int col_idx = icol * (blockDim.x + IPAD) + irow;


// shared memory store operation
tile[row_idx] = g_idx;


// wait for all threads to complete
__syncthreads();


// shared memory load operation
out[g_idx] = tile[col_idx];
}