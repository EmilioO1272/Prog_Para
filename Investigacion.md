CUDA core
Los CUDA Cores son procesadores paralelos que se encargan de procesar todos los datos que entran y salen de la GPU, 
Estos se especializan en cálculos gráficos, los cuales se ven reflejados al usuario final. Estos colores son exclusivos de 
GPUs creadas por NVIDIA.

Su objetivo es el coordinar la renderización de Objetos 3D, Iluminación, dibujar modelos y sombreado de una escena.

Esto es logrado mediante la computación paralela, donde los trabajos de fórmulas son divididos entre los miles 
de cores por los que está compuesta la GPU, haciendo el trabajo mas rapido, a comparacion que si la CPU los hiciera.

Thread
Los Threads son grupos de 32 Lanes que son las que la conforman, cada Thread tiene su propio registro e id en un threadblock.
Esto hace que pueda designar qué partes de un problema van a ser designadas a que Thread mediante el uso del global.

Block
Blocks son grupos de Threads los cuales pueden ser desde 512 o hasta 1024 en tamaño.

Grid
Un Grid es el conjunto de Blocks o threadblocks los cuales están conformados por 65535 x 65535 x 65535. Aunque los Blocks dentro
de una Grid pueden llegar a ser asignados diferentes SM (Shared Memory). Para maximizar el poder del hardware el SM puede correr
Threads de diferentes Blocks al mismo tiempo.

Bibliografía


A. Aller. “Qué son los Nvidia CUDA Cores y cuál es su importancia”. Profesional Review. Accedido el 6 de octubre de 2023. [En línea]. 
Disponible: https://www.profesionalreview.com/2018/10/09/que-son-nvidia-cuda-core/

N. Wilt, CUDA Handbook: A Comprehensive Guide to GPU Programming. Pearson Educ. Ltd.