CUDA Execution Model
Vista General de la Arquitectura de la GPU
La arquitectura de una GPU esta Construida alrededor de una matriz escalable de SMs (Streaming Multiprocessors). El paralelismo en el hardware de la GPU es logrado a través de la replicación de esta arquitectura de construcción de bloques. Los componentes principales son:
Núcleos CUDA
Memoria Compartida / Cache L1
Archivo de registro
Unidad de Carga / Gurdado
Unidad de Funciones Especiales
Programador Warp

Cada SM en la GPU está diseñado para apoyar la ejecución de cuentos de threads, y generalmente hay múltiples SMs por GPU, asi que es posible el tener miles de threads ejecutándose continuamente en una sola GPU. Cuando una red kernel es activada, los thread block del kernel son distribuidos entre los SMs disponibles para ejecución. Una vez programado en un SM, los thread de un thread block solo se podrán ejecutar en el SM asignado. Múltiples thread blocks pueden ser asignados al mismo SM a la vez y programados basados en los recursos disponibles del SM.

Cuda utiliza la arquitectura SIMT (Single Instruction Multiple Thread) para manejar y ejecutar threads en grupos de 32 llamados warps. Todos los threads de un warp ejecutan la misma instrucción al mismo tiempo. Cada thread tiene su propio contador de ubicación de instrucciones y registro de estado, y se encarga de las instrucciones actuales en su propia información. Cada SM particiona el thread block que le es asignado a 32 thread warps que son programados para ser ejecutado en los recursos del hardware que estén disponibles.


Un thread block es programado en solo un SM. Una vez está programado en un SM, quedará ahí hasta que la ejecución se complete En SM Puede mantener más de un thread block al mismo tiempo.

Memoria compartida y registros son recursos importantes en un SM. La memoria compartida es particionada entre los tread blocks que residen en el SM y los registros son particionados entre los threads. Threads en un thread block se pueden cooperar y comunicarse entre ellos a través de estos recursos. Aunque todos los threads en un thread block corren lógica paralela, no todos los threads se pueden ejecutar físicamente al mismo tiempo. Debido a esto, diferentes threads en un thread block progresan a diferente velocidad.

Compartir información entre threads paralelos puede causar una condición de carrera: Múltiples threads accediendo a la misma información con un orden indefinido, lo que resulta en un comportamiento impredecible del programa. CUDA provee los métodos para sincronizar los threads en un thread block asegurándose de que todos los threads alcancen ciertos durante su ejecución antes de hacer cualquier progreso.

Mientras los warps de un thread block pueden estar programados en cualquier orden, el número de warps activos está limitado por los recursos del SM. Cuando un warp está inactivo por cualquier razón, el SM es libre de programar otro warp disponible de cualquier thread block que esté en el mismo SM. Cambiar entre warps concurrentes no tiene un suspenso porque los recursos del hardware estan particionados con todos los threads y blocks en un SM, asi que el warp recién programado ya esta guardado en el SM.

Arquitectura de Fermi
Esta arquitectura fue la primera arquitectura computacional de GPU en enviar las características requeridas para las aplicaciones HPC que demandan muchos recursos.

Fermi incluye un caché L2 de 768 KB, compartido por todos los 16 SMs. Cada SM contiene:
Unidades de ejecución (Núcleos CUDA)
unidades de programación y despachante que se encarga de los warps.
Memoria compartida, archivo de registros y cache L1.

Cada multiprocesador tiene 16 unidades de carga/guardado, permitiendo calcular de origen y destinación de direcciones para 16 threads por ciclo de clock. Unidades de función especial (SFU´s) ejecutan instrucciones intrínsecas como seno, coseno, raíz cuadrada e interpolación. Y cada cada SFU´s puede ejecutar una operación intrínseca por thread en un ciclo de clock.

Un SM contiene dos “programadores de warp” y dos “unidades de despacho de instrucciones”. Cuando un thread block es asignado a un SM, todos los threads de ese thread block son divididos entre warps. Los dos programadores de warp eligen dos warps y les dan una instrucción por cada warp a un grupo de 16 núcleos CUDA, 16 unidades de carga/guardado, o 4 SFUs. La arquitectura Fermi, puede manejar simultáneamente 48 warps por SM para un total de 1,536 threads residiendo en un solo SM a la vez.

Arquitectura de Kepler

La arquitectura Kepler de GPU, es rápida y altamente eficiente, arquitectura de alto rendimiento en computado. Las características de Kepler hacen que el computador híbrido sea más accesible, conteniendo 15 SM´s y seis controladores de memoria de 64 bit. Las tres innovaciones importantes en la arquitectura de Kepler son:

Mejores SM´s: Tiene Kepler K20X, siendo una nueva unidad SM, la cual tiene varias innovaciones en la arquitectura mejorando el poder y eficiencia. Cada SM Kepler consiste de 192 núcleos CUDA de precisión única, 64 unidades de precisión doble, 32 SFU, y 32 unidades de carga/guardado. Cada SM incluye también 4 programadores de warp y 8 despachadores de instrucciones, permitiendo el tener 4 warps el ser expedidos y ejecutados a la vez en un solo SM. La arquitectura del Kepler K20X puede programar 64 warps por SM para un total de 2,048 threads residiendo en un solo SM a la vez. Incrementó el tamaño del archivo de registro a 64K, comparado a los 32K de en Ferni. También permite más particiones entre la memoria compartida y el caché L1.

Paralelismo Dinámico: Permite de manera dinámica hacer nuevos grids. Con esto es posible que cualquier kernel pueda lanzar otro kernel y manejar cualquier dependencia inter-kernel necesitada para hacer correctamente trabajo adicional. Esta característica hace más fácil el crear y optimizar patrones recursivos y datos dependientes.

Hyper-Q: Agrega mas conecciones de hardware simultáneas entre la CPU y GPU, permitiendo que los núcleos de la CPU hagan más tareas simultáneamente en la GPU. Resultando en un incremento de la utilidad de la GPU y una reducción del tiempo inactivo de la CPU al usar GPUs con Kepler.

Profile-Driven Optimization
Profiling es la acción de analizar el rendimiento de un programa midiendo:

El espacio de memoria o el tiempo de  complejidad del código de la aplicación.

El uso de instrucciones particulares.

La frecuencia y duración de llamadas de funciones.

Profiling es un paso crítico en desarrollo de programas, especialmente en optimizando código de aplicación HPC. El Profiling usualmente requiere un básico entendimiento del modelo de ejecución de una plataforma para ayudar a tomar decisiones para la optimización. Desarrollar un aplicación HPC usualmente involucra dos pasos importantes:
Desarrollo del código de corrección.

Mejorar el código para rendimiento.

Es natural usar un acercamiento profile-driven para el segundo paso. El desarrollo con profile-driven es particularmente importante en la programación de CUDA por las siguientes razones:

Una implementación de nivel kernel generalmente no da el mejor rendimiento. Herramientas de profiling ayudan a encontrar regiones críticas en tu código que están teniendo cuellos de botella.

CUDA participa los recursos de computado en una SM entre muchos threads block residentes. Esta partición causa que algunos recursos sean de rendimiento limitado. Herramientas de profiling pueden ayudarte a entender cómo los recursos de computación están siendo utilizados.

CUDA provee una abstracción en la arquitectura de hardware permitiendo el control sobre los threads concurrentes. Herramientas de profiling te ayudan a medir, visualizar y guiar tu optimización.

Las herramientas de profiling proveen una vista profunda al rendimiento de kernel y ayuda a identificar cuellos de botella en los kernels. CUDa provee dos herramientas de profiling primarios, un profiler visual; y nvprof, una línea de comando profile.
