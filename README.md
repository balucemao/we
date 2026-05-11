# Tarea2

### Nombre: Benjamin Alucema
### Fecha: 10-05-2026
### Rut: 22122629-1

# StreamFinder- Gestor y Explorador de Contenido Multimedia
### Importante: Ingresar generos en ingles

- Este proyecto consiste en una herramienta escrita en codigo en lenguaje C. Su principal objetivo es la gestio eficiente de un catalogo de peliculas y serie, permitiendo realizar tareas de forma eficiente y muy rapido

- Para lograr estas caracteristicas de rendimiento optimo, en mi proyecto utilizare principalmente mapas (TDAs) para indexar los datos, y listas (TDAs) para el agrupamiento de categorias y una watchlist. Esto evita busquedas ineficientes y lineales, permitiendo obtener resultador "instantaneos" debido a la caracteristica de orden por key de los mapas. Esto es independiente del tamaño de los datos.

# Estructuras y caracteristicas necesarias para ejecutar el programa

- TDAs requeridos: list.c, map.c, extra.c (OBLIGATORIOS)
- Cabeceras: list.h, map.h, extra.h (OBLIGATORIOS)

# Instrucciones de compilación:

## Cargar aplicación 

```bash
gcc tarea2.c tdas/*.c -Wno-unused-result -o StreamFinder
```

## Ejecutar aplicación

```bash
./StreamFinder
```

# Funcionalidad principales:

- cargarPeliculas: Este es el paso mas importante en mi programa, se encarga de leer un csv y poblar simultaneamente cuatro mapas distintos: por id, genero, director y decada. Esta "Multi indexación" garantiza las busquedas casi instantaneas.
- buscarPorId: Permite identificar una pelicula especifica en el mapa de IDs, utilizando su id asociado. Es muy eficiente gracias a utilizar mapas.
- buscarPorGenero: Despliega todos los titulos asociados a un genero especifico (drama, fantasy, etc). Si la pelicula tiene varios generos, aparecera en la busqueda de cada uno.
- buscarPorDirector: Lista todas las obras de un director especifico, incluso si la pelicula fue dirigida por más de una persona (separados por "," en csv).
- buscarPorDecada: Agrupa las peliculas por periodos de 10 años. Se utiliza un comparados de enteros en este mapa. Por ejemplo el usuario ingresa 1994, el programa lo convierte a la decada de 1990-1999.
- busquedaAvanzada: Cruza las peliculas por 2 criterios (género y década), para así obtener resultados más específicos, optimizando la busqueda para solo buscar en una sublista del género solicitado.
- watchList: Permite al usuario gestionar (agregar, eliminar y mostrar peliculas) una lista de favoritos personal. Incluye la característica de evitar duplicados, y validar si existe el id de la pelicula buscada en el mapa de IDs.
- liberarTodo: Función importante para la estabilidad y seguridad del proyecto. Realiza un limpiado de todos los mapas, listas, liberando toda la memoria dinamica para prevenir memory leaks.
- is_equal_str: Funcion personalizada que compara cadenas de texto ignorando mayusculas o minusculas. Permite que por ejemplo Drama = drama = DRAMA.
==================================================================================================
- Detección de espacios: Al indexar directores o generos, el codigo limpia automaticamente los espacios despues de una coma (separacion de generos por ejemplo) drama, fantasy.



# Ejemplo de funcionamiento:

1) usuario ingresa opcion 1: Carga data/top1500.csv (automaticamente al seleccionar opción 1), el sistema indexa 1500 peliculas.
2) El usuario ingresa la opción 7->1 (agregar): ingresa id 1. El sistema verifica que la pelicula es "The Godfather" y la agrega a su lista personal (watchlist).
3) El usuario selecciona la opcion 4: ingresa el nombre "Roman Polanski". El sistema muestra todas sus peliculas cargadas.
4) el usuario ingresa la opcion 6: ingresa el genero "fantasy" e ingresa el año "1993". Se muestran las peliculas de fantasia de esa decada.
5) El usuario ingresa opcion 2: ingresa el id "2". El sistema busca en el mapa, encuentra la pelicula "casablanca" y muestra toda su informacion.
6) El usuario ingresa opcion 3: ingresa el genero "sci-fi". El sistema muestra una lista de todas las peliculas de ese genero.
7) El usuario ingresa opcion 5: ingresa el año 2005. El sistema busca en el mapa_PorDecada y muestra todas las peliculas estrenadas en la decada (2000-2009).
8) El usuario ingresa opcion 7->3 (mostrar): se muestra la pelicula "The Godfather".
9) El usuario ingresa opcion 7->2 (eliminar): ingresa id 1. El sistema verifica que la pelicula es "The Godfather" y la elimina.
10) El usuario ingresa opcion 7->3 (mostrar): se muestra que la watchlist esta vacia.
11) El usuario ingresa opcion 8: Se libera todo con la función limpiarTodo, y se cierra el sistema.