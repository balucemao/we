#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estructura que almacena los datos de una pelicula
typedef struct {
  // Guarda el id de la pelicula
  char id[100]; 
  char title[100];
  char director[150];
  // Lista de generos
  List *genres;
  // Anio de lanzamiento de la pelicula
  int year;
  // IMDb rating
  float rating;
  
} Film;

// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("     Base de Datos de Películas");
  puts("========================================");

  puts("1) Cargar Películas");
  puts("2) Buscar por id");
  puts("3) Buscar por genero");
  puts("4) Buscar por director");
  puts("5) Buscar por decada");
  puts("6) Busqueda avanzada (genero y decada)");
  puts("7) Gestionar una WatchList (personas)");
  puts("8) Salir");
}

/**
 * Compara dos claves de tipo string para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo string.
 *
 * @param key1 Primer puntero a la clave string.
 * @param key2 Segundo puntero a la clave string.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_str(void *key1, void *key2) {
  char *clave1 = (char*)key1; 
  char *clave2 = (char*)key2; 
  // Se revisa caracter a caracter convertido a minuscula para ver si coinciden completamente ambas claves
  // Por ejemplo Fantasy = FANTASY = fantasy
  while(*clave1 && *clave2){
    // Aqui se revisa si es que en algun punto los caracteres no coinciden, si no lo hacen se retorna 0;
    if (tolower((unsigned char)*clave1) != tolower((unsigned char)*clave2))return 0;
    // Aqui se avanza en ambas cadenas al siguiente caracter
    clave1++;
    clave2++;
  }
  //aqui se retorna verdaderO si incluso el ultimo caracter de aMbas cadenas son iGuales
  return (*clave1 == *clave2);
  //return strcmp((char *)key1, (char *)key2) == 0; Esta funcion compara si la cadena ingresada es completamente igual a la buscada
  //el problema de eso es que muchas veces uno sabe el nombre pero piensa que no importa como lo escribamos mientras las letras sean iguales
  //pero strcmp solo dice gato = gato, pero Gato != gato
}

/**
 * Compara dos claves de tipo entero para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo entero.
 *
 * @param key1 Primer puntero a la clave entera.
 * @param key2 Segundo puntero a la clave entera.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_int(void *key1, void *key2) {
  return *(int *)key1 == *(int *)key2; // Compara valores enteros directamente
}

// Funcion para mostrar todos los datos de una pelicula
void mostrarPelicula(Film *pelicula){
  // Aqui se muestra el id, anio de lanzamiento, rating segun IMDb y titulo de la pelicula
  printf("ID : %s | AÑO : %d | RATING : %.1f | TITULO : %s\n", pelicula->id, pelicula->year, pelicula->rating, pelicula->title);
  // Aqui se muestra en pantalla el director de la pelicula
  printf("Director : %s\n\n", pelicula->director);
  // Aqui se muestran todos los generos de la pelicula
  printf("Genero(s) : \n");
  for (char *genero = list_first(pelicula->genres); genero != NULL; genero = list_next(pelicula->genres)){
    printf("- [%s]\n", genero);
  }
  printf("==========================================\n\n");
}

// Funcion para mostrar todas las peliculas almacenada en una lista de peliculas
void mostrarListaPeliculas(List *listaPeliculas){
  // Se revisa si la lista de peliculas es valida, y si hay peliculas en la lista
  if (listaPeliculas == NULL || list_size(listaPeliculas) == 0){
    printf("No se encontraron peliculas\n");
    return;
  }
  // Aqui guardo la primera pelicula en la lista para recorrer desde el principio todas las peliculas
  Film *peliculaActual = list_first(listaPeliculas);
  // Se recorre la lista hasta que no queden peliculas en la lista
  while (peliculaActual != NULL){
    // Se llama a la funcion mostrarPelicula para mostrar pelicula por pelicula
    mostrarPelicula(peliculaActual);
    // Este es el paso clave, me permite recorrer la lista de peliculas
    peliculaActual = list_next(listaPeliculas);
  }
  // Se muestran las peliculas procesadas
  printf("Total de peliculas procesadas : %d\n", list_size(listaPeliculas));
}


// =============== Cargue de Peliculas (CSV) ===============
// Esta funcion lee linea por linea el archivo "data/Top1500.csv".
// Copia los datos en una variable de tipo FILM y los ordena en los mapas (garantizar busquedas instantaneas) segun corresponda.
// Mapas por id (clave es el id), genero (clave es el genero), decada (clave es la decada int) y director (clave es el nombre del director)
void cargarPeliculas(Map *pelis_porId, Map *pelis_porGenero, Map *pelis_porDirector, Map *pelis_porDecada) {
  // Intenta abrir el archivo CSV que contiene datos de películas
  FILE *archivo = fopen("data/Top1500.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  // variable donde se almacenaran las lineas con la informacion del csv
  char **campos;
  int cantidadCampos = 0;
  // Leer y parsear una línea del archivo CSV. La función devuelve un array de
  // strings, donde cada elemento representa un campo de la línea CSV procesada.
  campos = leer_linea_csv(archivo, ','); // Lee los encabezados del CSV

  // Lee cada línea del archivo CSV hasta el final
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    // Crea una nueva estructura Film y almacena los datos de cada película
    Film *peli = (Film *)malloc(sizeof(Film));
    strcpy(peli->id, campos[0]);        // Asigna ID
    strcpy(peli->title, campos[5]);     // Asigna título
    strcpy(peli->director, campos[14]);   // Asigna director(es)
    peli->genres = split_string(campos[11], ",");       // Inicializa la lista de géneros
    peli->year = atoi(campos[10]); // Asigna año, convirtiendo de cadena a entero
    peli->rating = atof(campos[8]);   // Inicializa rating, convirtiendo de cadena a float
    
    // Inserta la película en el mapa usando el ID como clave
    map_insert(pelis_porId, strdup(peli->id), peli);

    // =============== Indexacion Por Genero ===============
    // Obtiene el primer género de la lista de géneros de la película
    char *genre = list_first(peli->genres);
    // Itera sobre cada género de la película
    while (genre != NULL) {
        // Busca el género en el mapa pelis_bygenres
        MapPair *genre_pair = map_search(pelis_porGenero, genre);

        // Si el género no existe en el mapa, crea una nueva lista y agrégala al mapa
        if (genre_pair == NULL) {
            List *nuevaLista_gen = list_create();
            list_pushBack(nuevaLista_gen, peli);
            map_insert(pelis_porGenero, strdup(genre), nuevaLista_gen);
        } else {
            // Si el género ya existe en el mapa, obtén la lista y agrega la película
            list_pushBack((List *)genre_pair->value, peli);
        }

        // Avanza al siguiente género en la lista
        genre = list_next(peli->genres);
    }

    // =============== Indexacion Por Directores ===============
    // Aqui separamos la cadena de directores para indexarlos uno a uno (si es que exiten mas de 1 director en la pelicula)
    // Se insertan en una lista
    List *listaDirectoresPeli = split_string(campos[14], ",");
    // Se crea cadena que apunta al primer elmento en la lista de peliculas por director
    char *directorActual = list_first(listaDirectoresPeli);
    // Se recorren todos los directores de la lista de directores
    while(directorActual != NULL){
      // Si el nombre empieza con algun espacio, avanzamos el puntero hasta el caracter siguiente (sirve en esta caso porque IMDb tiene ese formato)
      // Ejemplo: nombre1, nombre2....
      if (directorActual[0] == ' ')directorActual += 1;
      // Se busca en el mapa de directores si existe ya el director actual
      MapPair *pairActualDir = map_search(pelis_porDirector, directorActual);
      // Si no existe
      if (pairActualDir == NULL){
        // Se crea una nuevo sublista de directores
        List *nuevaListaDirectores = list_create();
        // Se inserta la pelicula en el nuevo editor (actual)
        list_pushBack(nuevaListaDirectores, peli);
        // Se inserta la sublista en el mapa de peliculas por director
        map_insert(pelis_porDirector, strdup(directorActual), nuevaListaDirectores);
      }
      else list_pushBack((List*)pairActualDir->value, peli);
      directorActual = list_next(listaDirectoresPeli);
    }
    // Aqui se libera la lista temporal que utilice para guardar los directores.
    // Primero la lista interna y despues por completo.
    list_clean(listaDirectoresPeli);
    free(listaDirectoresPeli);

    // =============== Indexacion Por Decadas ===============
    // Primero se debe truncar el año con la operacion (anio/10)*10, asi obtengo la decada.
    // Por ejemplo 1997, (1997/10)*10 -> 1990.
    int decada = (peli->year/10) * 10;
    // Se obtiene si existe el pair asociado a la decada.
    MapPair *decadaPair = map_search(pelis_porDecada, &decada);
    // Si no existe pair en esa epoca, se debe crear uno.
    if (decadaPair == NULL){
      // Aqui se reserva memoria dinamica para la decada nueva (entero).
      int *nuevaDecada = (int*)malloc(sizeof(int));
      // Se copia la decada en la cadena.
      *nuevaDecada = decada;
      // Se crea lista para guardar la lista de peliculas por decada especifica.
      List *nuevaLista_dec = list_create();
      // Se Inserta la pelicula en la nueva lista.
      list_pushBack(nuevaLista_dec, peli);
      // Se inserta la nueva lista en el mapa por decada.
      map_insert(pelis_porDecada, nuevaDecada, nuevaLista_dec);
    }
    // Si existe simplemente se inserta la pelicula en el pair de decada correspondiente en el mapa
    else list_pushBack((List*)decadaPair->value, peli);
    cantidadCampos++;
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas
  //se muestra cuantas peliculas fueron procesadas
  printf("Se cargo el catalogo, total de peliculas procesadas = %d\n", cantidadCampos);
}

//FUNCION EXTRA: esta funcion busca peliculas por su id, y la muestra (si existe)
void buscarPorId(Map *pelis_porId) {
  char id[100]; // Buffer para almacenar el ID de la película

  // Solicita al usuario el ID de la película
  printf("Ingrese el id de la película: ");
  scanf(" %99s", id); // Lee el ID del teclado

  // Busca el par clave-valor en el mapa usando el ID proporcionado
  MapPair *pair = map_search(pelis_porId, id);

  // Si se encontró el par clave-valor, se extrae y muestra la información de la
  // película
  if (pair != NULL) {
    Film *peli = pair->value; // Obtiene el puntero a la estructura de la película
    // Muestra el título y el año de la película
    mostrarPelicula(peli);
  } else {
    // Si no se encuentra la película, informa al usuario
    printf("La película con id %s no existe\n", id);
  }
}

// =============== Implementacion: Busqueda Por genero ===============
//Esta funcion resive el mapa de peliculas por genero
//Despues se pide un genero al usuario
//Finalmente busca si existe el genero en el mapa de peliculas por genero,
//-- Si existe muestra toda la lista de peliculas de ese genero
//-- Si no existe muestra que no se encontraron peliculas del genero solicitado
void buscarPorGenero(Map *pelis_porGenero){
  char generoActual[100];
  printf("Ingrese el genero que se quiere buscar (drama, romance, fantasy, etc) :\n");
  scanf(" %99[^\n]", generoActual);

  // Aqui busco el pair del genero buscado en el mapa (osea clave : genero y valor : lista de peliculas del genero) y lo copio
  MapPair *pairActual = map_search(pelis_porGenero, generoActual);
  // Si el pair existe significa que existen peliculas de ese genero (osea pairActual != NULL)
  if (pairActual != NULL){
    printf("Las peliculas que son del genero %s son: \n", generoActual);
    mostrarListaPeliculas((List*)pairActual->value);
  }
  // Si el pair no existe entonces significa que no existen peliculas de ese genero
  else printf("No se encontraron peliculas de este genero\n");
}

// =============== Implementacion: Busqueda Por Director ===============
//Esta funcion recibe el mapa de peliculas por director
//Despues se le pide un director al usuario
//Finalmente busca si existen peliculas por parte de  ese director
//-- Si existen muestra toda la lista de peliculas de ese director
//-- Si no existe muestra que el director no tiene peliculas
void buscarPorDirector(Map *pelis_porDirector){
  char directorActual[150];
  printf("Ingrese el nombre del director :\n");
  scanf(" %149[^\n]", directorActual);

  // Aqui busco si existe el pair del director buscado en el mapa por director (osea clave : director y valor : lista de peliculas de ese director) y lo copio
  MapPair *pairActual = map_search(pelis_porDirector, directorActual);
  // Si el pair existe significa que ese autor efectivamente tiene peliculas (osea pairActual != NULL)
  if (pairActual != NULL){
    printf("Las obras del director %s son :\n", directorActual);
    mostrarListaPeliculas((List*) pairActual->value);
  }
  // Si no existe simplemente se muestra que no se encontraron peliculas del director solicitado
  else printf("No se encontraron peliculas para el director %s\n", directorActual);
}

// =============== Implementacion: Busqueda Por Director ===============
//Esta funcion recibe el mapa de peliculas por decada
//Despues se le pide un año al usuario, y con la operacion matematica (año/10)*10 se transforma a decada
//Finalmente busca si existe peliculas de esa decada
//-- Si existen muestra toda la lista de peliculas en esa decada (sublista dentro del mapa)
//-- Si no existen muestra que no hay peliculas ese año
void buscarPorDecada(Map *pelis_porDecada){
  int anio, decada;

  printf("Ingrese el año para filtrar por su decada : \n\n");
  scanf("%d", &anio);
  decada = (anio / 10)*10;

  MapPair *pairActual = map_search(pelis_porDecada, &decada);
  if (pairActual != NULL){
    printf("Peliculas de la decada de %d :\n", decada);
    mostrarListaPeliculas((List*)pairActual->value);
  }
  else printf("No se encontraron peliculas en la decada seleccionada\n", decada);
}

// =============== Implementacion: Busqueda Avanza (Genero y Decada) ===============
//Esta funcion recibe el mapa de peliculas por genero
//despues le pide al usuario un genero
//despues se revisa si existe el genero en el mapa de peliculas por genero
//Si no existe se avisa que no hay peliculas de ese genero
//Si existe se le pide al usuario que ingrese un año para filtrar
//Despues se transforma el anio a la decada con la operacion (anio/10)*10
//Finalmente se revisa si existen peliculas dentro de la lista de peliculas por genero que coincidan con la decada buscada
//Si existen se muestran todas las peliculas que cumplan ambos criterios
//Si no existe se muestra que no existen peliculas que cumplan ambos criterios
void busquedaAvanzada(Map *pelis_PorGenero){
  char generoActual[100];
  int anio, decada;

  printf("Ingrese el genero que desea filtrar :\n");
  // Se solicita genero de pelicula
  scanf(" %99[^\n]", generoActual);
  // Se revisa si existe ese genero en el mapa
  // Si no existen se muestra que no existen peliculas de ese genero
  if (map_search(pelis_PorGenero, generoActual) == NULL){
    printf("No existe peliculas de ese genero\n");
    return;
  }

  // Si existian peliculas, se solicita al usuario el anio
  printf("Ingrese un año para filtrar por esa decada :\n");
  scanf("%d", &anio);
  // Con el anio entregado se realiza la operacion matematica para transformarlo a decada
  decada = (anio / 10) * 10;

  // Se busca el pair del genero buscado en el mapa de generos (clave : genero y valor : lista de peliculas por genero)
  MapPair *pairActual = map_search(pelis_PorGenero, generoActual);
  // Confirmacion de que existe el pair del genero buscado
  if (pairActual != NULL){
    List *listaGeneros = (List*)pairActual->value;
    // Esta es la cantidad de peliculas de un genero en una determinada decada
    int peliculasGeneroDecada = 0;
    
    printf("Peliculas del genero %s en la decada %d :\n", generoActual, decada);
    // Aqui guardo en peliculaActual la primero pelicula de la lista de generos
    // Posteriormente se usara para recorrer todas las peliculas en esa sublista de peliculas por genero
    Film *peliculaActual = list_first(listaGeneros);
    // Aqui empieza el recorrido para mostrar todas las peliculas de ese genero
    while(peliculaActual != NULL){
      // Aqui se revisa si la peliculaActual pertenece a la decada buscada (se transforma al formato decada)
      if ((peliculaActual->year/10)*10 == decada){
        // Aqui se utiliza la funcion mostrarPelicula si cumple con ser del mismo genero y decada
        mostrarPelicula(peliculaActual);
        // Se cuentan la cantidad de peliculas que cumplen los requisitos (mismo genero y decada)
        peliculasGeneroDecada++;
      }
      // Se avanza a la siguiente pelicula para validar si cumple los criterios de genero y decada
      peliculaActual = list_next(listaGeneros);
    }
    // Si no existiero peliculas se muestra 
    if (peliculasGeneroDecada == 0)printf("No se encontraron peliculas que coincidan con el genero y decada solicitada\n");
    // Si existieron se muestra la cantidad de peliculas procesadas
    else printf("El total de peliculas en el genero y decada solicitada son : %d", peliculasGeneroDecada);
  }
}

// =============== Implementacion: Gestionar la watchlist ===============
// Esta funcion recibe una lista watchlist y el mapa de peliculas por id
// despues se muestran opciones para hacer en la watchlist
// despues se revisa si existe el genero en el mapa de peliculas por genero
// Se pide al usuario que opcion ingresar (1, 2 o 3)
// Si elige la opcion 1 (agregar) : 
//- se pedica al usuario el id de la pelicula a buscar
//- Buscara en el mapa de id la pelicula que se requiere anadir a la watchlist
//- Si lo encuentra lo añadira a la watchlist en el caso de que no este anadido. Si ya esta anadido muestra que ya esta en la watchlist
//- Si no, muestra el mensaje que no existe el id de esa pelicula
// Si elige opcion 2 (eliminar) :
//- Primero se pide al usuario el id de la pelicula a eliminar
//- Despues revisa la lista de watchlist si existe el id de esa pelicula
//- Si existe la elimina 
//- Si no existe muestra el mensaje que la pelicula no esta dentro de watchlist
// Si elige opcion 3 (mostrar) :
//- Muestra todas las peliculas dentro de la watchlist
void watchList(List *watchlist, Map *pelis_porId){
  // Opcion que se le pedira al usuario
  int subOpcion;
  // Id relacionado a la eliminacion o insercion de pelicula en la watchlist
  char idBuscar[100];

  // Opciones de gestion de la watchlist
  printf("---- Gestion de WatchList ----\n\n");
  printf("1.- Agregar pelicula a la watchlist (por id)\n");
  printf("2.- Eliminar pelicula de la watchist (por id)\n");
  printf("3.- Mostrar WatchList\n");
  printf("indique que opcion desea realizar (numero)\n");
  
  // Se le solicita al usuario ingresar la opcion que desea
  scanf(" %d", &subOpcion);

  // Opcion 1 : Agregar a watchlist
  if (subOpcion == 1){
    printf("ingrese el id de la pelicula que usted quiere agregar a su WatchList :\n\n");
    // Se solicita el id de la pelicula a agregar
    scanf(" %99s", idBuscar);
    // Se busca en el mapa de id, el pair id de la pelicula que se quiere anadir
    MapPair *pairActual = map_search(pelis_porId, idBuscar);
    // Mientras se encuentre el id de la pelicula
    if (pairActual != NULL){
      // Se accede a la pelicula dentro del pair
      Film *peliculaActual = pairActual->value;
      // Variable para revisar si existe ya la pelicula en mi watchlist
      int existe = 0;
      // Se recorre la watchlist
      for (Film *peliculaWatchlist = list_first(watchlist); peliculaWatchlist != NULL; peliculaWatchlist = list_next(watchlist)){
        // Si existe la pelicula dentro significa que por lo tanto se cambia existe = 1. Salgo del ciclo
        if (strcmp(peliculaWatchlist->id, idBuscar) == 0){
          existe = 1;
          break;
        }
      }
      // Si no existe
      if (existe == 0){
        // Se anade la pelicula a la watchlist
        list_pushBack(watchlist, peliculaActual);
        printf("se ha agregado exitosamente la pelicula : [%s] a la watchlist\n", peliculaActual->title);
      }
      // Si ya existe se muestra que ya esta la pelicula
      else printf("La pelicula ya pertenece a la watchlist\n");
    }
    // Si el id no existe se muestra que no existe pelicula con ese id
    else printf("No existen peliculas con ese id en el catalogo\n");
  }

  // Opcion para eliminar peliculas de la watchlist
  else if (subOpcion == 2){
    printf("Ingrese el id de la pelicula a eliminar en la watchlist :\n");
    // Se pide el id de la pelicula a eliminar de la watchlist
    scanf(" %99s", idBuscar);
    // Se guarda la primera pelicula de la watchlist
    Film *peliculaWatchlist = list_first(watchlist);
    // Aqui se recorre todas la watchlist desde el principio
    while(peliculaWatchlist != NULL){
      // Si se encuentra en la watchlist el id (y por tanto la pelicula)
      if (strcmp(peliculaWatchlist->id, idBuscar) == 0){
        // Se elimina la pelicula de la watchlist
        list_popCurrent(watchlist);
        printf("La pelicula ha sido eliminada correctamente de la watchlist\n\n");
        return;
      }
      // Se avanza a la siguiente pelicula de la watchlist
      peliculaWatchlist = list_next(watchlist);
    }
    printf("No se encontro la pelicula con id [%s] en la watchlist\n", idBuscar);
  }

  // Opcion para mostrar watchlist  
  else if (subOpcion == 3){
    printf("------ WATCHLIST ------\n");
    // Se muestran todas las peliculas dentro de la watchlist, por ende usa la funcion para mostrar listas de peliculas
    mostrarListaPeliculas(watchlist);
  }
  // Si la opcion no es valida se muestra el respectivo mensaje
  else printf("opcion invalida, porfavor ingrese otra\n");
}

void liberarTodo(List *ListaWatchlist, Map *pelis_porId, Map *pelis_porGenero, Map *pelis_porDirector, Map *pelis_porDecada){
  // Primero limpio la watchlist pelicula a pelicula
  list_clean(ListaWatchlist);
  // Finalmente elimino la watchlist como recipiente
  free(ListaWatchlist);

  // ========= Liberar Mapa Por Id =========
  // Aqui accedo al primer pair del mapa de id para recorrer todo el mapa
  MapPair *pair = map_first(pelis_porId);
  // Se recorre hasta el final (osea pair == NULL)
  while(pair != NULL){
    // En cada ciclo se libera la clave dentro del pair actual
    free(pair->key);
    // Se guarda la pelicula dentro del pair actual
    Film *pelicula = (Film*)pair->value;
    // Libero los generos almacenados dentro de la pelicula actual
    list_clean(pelicula->genres);
    // Libero la lista de generos como recipiente de los generos (en su totalidad)
    free(pelicula->genres);
    // Se elimina la pelicula por completo
    free(pelicula);
    // Con esto se avanza pair a pair dentro del mapa id
    pair = map_next(pelis_porId);
  }
  // Elimino todos los elementos (pair) del mapa
  map_clean(pelis_porId);
  // Elimino el mapa por completo
  free(pelis_porId);

  // ========= Liberar Mapa Por genero =========
  // Accedo al primer elemenento dentro del mapa de peliculas por genero
  pair = map_first(pelis_porGenero);
  // Recorro todo el mapa pair a pair
  while(pair != NULL){
    // Se libera la clave del pair actual
    free(pair->key);
    // Se libera la lista de peliculas de generos dentro del valor actual del mapa
    list_clean((List*)pair->value);
    // Se libera la lista de peliculas de generos por completo
    free(pair->value);
    // Se avanza al siguiente pair del mapa
    pair = map_next(pelis_porGenero);
  }
  // Eliminar todas las posiciones (pair) del mapa
  map_clean(pelis_porGenero);
  // Liberar todo el mapa por completo
  free(pelis_porGenero);

  // ========= Liberar Mapa Por Directores =========
  // SIGUE LA MISMA LOGICA ANTERIOR PERO CON EL MAPA DE DIRECTORES
  pair = map_first(pelis_porDirector);
  while(pair != NULL){
    free(pair->key);
    list_clean((List*)pair->value);
    free(pair->value);
    pair = map_next(pelis_porDirector);
  }
  map_clean(pelis_porDirector);
  free(pelis_porDirector);

  // ========= Liberar Mapa Por Decada =========
  // Se accede al primero pair del mapa de edades
  pair = map_first(pelis_porDecada);
  // se recorre todo el mapa por decada desde el principio
  while(pair != NULL){
    // Se libera la clave actual del mapa por decada (la decada)
    free(pair->key);
    // Se libera la lista de peliculas en esa decada que es el valor del mapa
    list_clean((List*)pair->value);
    // Se libera la lista de peliculas por esa decada en general
    free(pair->value);
    // Se avanza al siguiente pair dentro del mapa
    pair = map_next(pelis_porDecada);
  }
  // Se libera el mapa por decada pair a pair
  map_clean(pelis_porDecada);
  // Se libera todo el mapa por completo
  free(pelis_porDecada);
  
}


int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
               // (sin uso en este fragmento)

  // Aqui creo todos los mapas para ordenar peliculas por id, genero, director y decada, utilizando una función de
  // Comparación que trabaja con claves de tipo string e int.
  Map *pelis_porId = map_create(is_equal_str);
  Map *pelis_porGenero = map_create(is_equal_str);
  Map *pelis_porDirector = map_create(is_equal_str);
  Map *pelis_porDecada = map_create(is_equal_int);


  // Lista para almacenar la watchlist
  List *ListaWatchlist = list_create();

  do {
    // Se muestra el menu principal 
    mostrarMenuPrincipal();
    printf("Ingrese su opcion (1-8): ");
    // Se requiere opcion (1-8) para continuar
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      // Cargar peliculas desde el archivo csv
      cargarPeliculas(pelis_porId, pelis_porGenero, pelis_porDirector, pelis_porDecada);
      break;
    case '2':
      // Funcion extra (buscar por id)
      buscarPorId(pelis_porId);
      break;
    case '3':
      // Funcion para filtrar peliculas por genero
      buscarPorGenero(pelis_porGenero);
      break;
    case '4':
      // Funcion para filtrar peliculas por director
      buscarPorDirector(pelis_porDirector);
      break;
    case '5':
      // Funcion para filtrar peliculas por decada
      buscarPorDecada(pelis_porDecada);
      break;
    case '6':
      // Funcion de busqueda avanzada para buscar peliculas por genero y decada
      busquedaAvanzada(pelis_porGenero);
      break;
    case '7':
      // Funcion para gestionar la watchlist
      watchList(ListaWatchlist, pelis_porId);
      break;
    }
    // Opcion para para realizar otras acciones en el programa
    if (opcion != '8'){
      presioneTeclaParaContinuar();
    }
  } while (opcion != '8');
  
  // Se liberan todas los mapas, listas y sublistas de mi programa con la funcion liberar todo
  liberarTodo(ListaWatchlist, pelis_porId, pelis_porGenero, pelis_porDirector, pelis_porDecada);
  return 0;
}
