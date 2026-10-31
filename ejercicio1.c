#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_TEXTO 100
#define LIBRE     0
#define OCUPADO   1
#define BORRADO   -1

typedef struct {
    int       clave;               /* 9 ultimas cifras del ID (ID mod 10^9) */
    long long id;                  /* ID de la ejecucion (13 digitos): no cabe en un int */
    char      agente[MAX_TEXTO];
    char      herramienta[MAX_TEXTO];
    int       tokens;
    int       latencia_ms;
    int       estado;
} hash_list;

int calcula_primo (int primo) {

    for (int i = 2; i * i <= primo; i++) {
      if (primo % i == 0)
        return 0;
    }

    return 1;
}

int leerFichero(const char *nombre_fichero, hash_list *ej, int max_ej)
{
    char linea[512];
    FILE *fp = fopen(nombre_fichero, "r");
    int n = 0;

    if (fp == NULL) {
        printf("Error: no se puede abrir %s\n", nombre_fichero);
        return -1;
    }
    if (fgets(linea, sizeof linea, fp) == NULL) {      /* cabecera */
        fclose(fp);
        return 0;
    }
    while (n < max_ej && fgets(linea, sizeof linea, fp) != NULL) {
        char *id     = strtok(linea, ",\r\n");
        char *agente = strtok(NULL, ",\r\n");
        char *herr   = strtok(NULL, ",\r\n");
        char *tokens = strtok(NULL, ",\r\n");
        char *lat    = strtok(NULL, ",\r\n");
        if (!id || !agente || !herr || !tokens || !lat) continue;   /* linea incompleta */

        ej[n].id = strtoll(id, NULL, 10);
        ej[n].clave = (int)(ej[n].id % 1000000000LL);
        strncpy(ej[n].agente, agente, MAX_TEXTO - 1);      ej[n].agente[MAX_TEXTO - 1] = '\0';
        strncpy(ej[n].herramienta, herr, MAX_TEXTO - 1);   ej[n].herramienta[MAX_TEXTO - 1] = '\0';
        ej[n].tokens = atoi(tokens);
        ej[n].latencia_ms = atoi(lat);
        ej[n].estado = OCUPADO;
        n++;
    }
    fclose(fp);
    return n;
}

void calcula_parametros(int *R, int *P, int *Mo, long long *K, int *M_ch) {

    char linea[60];
    int NN, num_sur_uno, num_sur_dos, num_sur_tres;
    char *sur_uno, *sur_dos, *sur_tres, *temp;
    char ini_uno[3], ini_dos[3], ini_tres[3];

    do {
        printf("Escribe el numero del grupo y los apellidos de los integraantes separados por espacio: ");
        fgets(linea, sizeof(linea), stdin);
    } while (linea == NULL || linea == '\0');

    /* Separamos NN y los apellidos */
    NN = atoi(strtok(linea, " "));
    sur_uno = strtok(NULL, " ");
    sur_dos = strtok(NULL, " ");
    sur_tres = strtok(NULL, " ");

    /* Ponemos apellidos en mayusculas y ordenamos alfabeticamente */
    for (int i = 0; sur_uno[i] != '\0'; i++) 
        sur_uno[i] = toupper(sur_uno[i]);

    for (int i = 0; sur_dos[i] != '\0'; i++) 
        sur_dos[i] = toupper(sur_dos[i]);

    for (int i = 0; sur_tres[i] != '\0'; i++) 
        sur_tres[i] = toupper(sur_tres[i]);

    if (strcmp(sur_uno, sur_dos) > 0) {
        temp = sur_uno;
        sur_uno = sur_dos;
        sur_dos = temp;
    }

    if (strcmp(sur_dos, sur_tres) > 0) {
        temp = sur_dos;
        sur_dos = sur_tres;
        sur_tres = temp;
    }

    if (strcmp(sur_uno, sur_dos) > 0) {
        temp = sur_uno;
        sur_uno = sur_dos;
        sur_dos = temp;
    }
    
    /* separar las primeras dos inciales */
    for (int i = 0; i < 2; i++) {
        ini_uno[i] = sur_uno[i];
        ini_dos[i] = sur_dos[i];
        ini_tres[i] = sur_tres[i];

        // printf("Apellido uno: %c\n", ini_uno[i]);
        // printf("Apellido dos: %c\n", ini_dos[i]);
        // printf("Apellido tres: %c\n", ini_tres[i]);
    }

    /* Caracter de fin de cadena */
    ini_uno[2] = '\0';
    ini_dos[2] = '\0';
    ini_tres[2] = '\0';

    /* Calclular los valores numericos */
    num_sur_uno = 26 * (ini_uno[0] - 'A') + (ini_uno[1] - 'A');
    num_sur_dos = 26 * (ini_dos[0] - 'A') + (ini_dos[1] - 'A');
    num_sur_tres = 26 * (ini_tres[0] - 'A') + (ini_tres[1] - 'A');

    /* Calcular la semilla K */
    *K = *K * 676 + num_sur_uno;
    *K = *K * 676 + num_sur_dos;
    *K = *K * 676 + num_sur_tres;
    *K = *K + NN;

    /* Calcular el resto de parametros */
    *R = 400 + 5 * (*K % 20);
    *P = *K % 500;
    *Mo = 300 + (*K % 200); 
    while (!calcula_primo(*Mo)) {
        (*Mo)++;
    } 
    *M_ch = 100 + (*K % 50);

    printf("numero de grupo e iniciales: %lld - %s - %s - %s\n", NN, ini_uno, ini_dos, ini_tres);
    // printf("Valores: %d - %d - %d - K: %lld\n", num_sur_uno, num_sur_dos, num_sur_tres, *K);
    printf("Parametros: K: %lld - R: %d - P: %d - Mo: %d - M_ch: %d\n", *K, *R, *P, *Mo, *M_ch);

    return;
}

hash_list* crea_lista (hash_list *lista_vieja, int *tam_viejo, int tam_nuevo) {

    hash_list *lista_nueva = malloc (tam_nuevo * sizeof(hash_list));

    for (int i = 0; i < tam_nuevo; i++) {
        lista_nueva[i].estado = LIBRE;
    }

    for (int i = 0; i < *tam_viejo; i++) {
        strcpy(lista_nueva[i].agente, lista_vieja[i].agente);
        strcpy(lista_nueva[i].herramienta, lista_vieja[i].herramienta);
        lista_nueva[i].clave = lista_vieja[i].clave;
        lista_nueva[i].id = lista_vieja[i].id;
        lista_nueva[i].latencia_ms = lista_vieja[i].latencia_ms;
        lista_nueva[i].tokens = lista_vieja[i].tokens;
        lista_nueva[i].estado = lista_vieja[i].estado;
    }

    // printf("Se creo una nueva lista\n");
    return lista_nueva;
}

hash_list* inserta_registros_lineal(int *R, int *P, int *Mo, long long *K, int *tam_actual) {
    
    int colisiones = 0, pos_actual, pos_original, tam_nuevo, espacios_libres = 0;
    static hash_list lista_original[500];
    hash_list *lista_ordenanda = malloc(*Mo * sizeof(hash_list));
    float  espacios_ocupados = 0;

    *tam_actual = *Mo;

    int n = leerFichero("trazas.csv", lista_original, 500);
    if (n < 0) {
        printf("no se pudo leer el fichero\n");
        return NULL;
    }

    /* inicializar la lista ordenanda */
    for (int i = 0; i < *tam_actual; i++) {
        lista_ordenanda[i].estado = LIBRE;
        espacios_libres++;
    }

    /* Insertar registros en la lista nueva */
    for (int i = 0; i < *R; i++) {

        /* Se ve que no este mas del 50% ocupado, si lo esta se redimensiona */
        if ((espacios_ocupados + 1) / (float)(*tam_actual) > 0.5) {
            tam_nuevo = *tam_actual * 2;
            while (!calcula_primo(tam_nuevo)) {
                tam_nuevo++;
            } 
            lista_ordenanda = crea_lista(lista_ordenanda, tam_actual, tam_nuevo);
            *tam_actual = tam_nuevo;
        }

        /* Calculo de la posicion */
        pos_original = (*P + i) % 500;
        int j = 1;

        pos_actual = lista_original[pos_original].clave % *tam_actual;

        /* Verifica si esta ocupado ese lugar, en caso de estarlo prueba otro con prueba lineal */
        while (lista_ordenanda[pos_actual].estado == OCUPADO) {
            colisiones++;
            pos_actual = (pos_original + j) % *tam_actual;
            j++;

            if (j > *tam_actual) {
                tam_nuevo = *tam_actual * 2;
                while (!calcula_primo(tam_nuevo)) {
                    tam_nuevo++;
                } 
                lista_ordenanda = crea_lista(lista_ordenanda, tam_actual, tam_nuevo);
                *tam_actual = tam_nuevo;
            }
        }

        /* Se insertan los valores al encontrar hueco */
        strcpy(lista_ordenanda[pos_actual].agente, lista_original[pos_original].agente);
        strcpy(lista_ordenanda[pos_actual].herramienta, lista_original[pos_original].herramienta);
        lista_ordenanda[pos_actual].clave = lista_original[pos_original].clave;
        lista_ordenanda[pos_actual].id = lista_original[pos_original].id;
        lista_ordenanda[pos_actual].latencia_ms = lista_original[pos_original].latencia_ms; 
        lista_ordenanda[pos_actual].tokens = lista_original[pos_original].tokens;
        lista_ordenanda[pos_actual].estado = lista_original[pos_original].estado;

        espacios_ocupados++;
    }

    espacios_libres = *tam_actual - espacios_ocupados;

    return lista_ordenanda;
}

hash_list* inserta_registros_cuadratica(int *R, int *P, int *Mo, long long *K, int *tam_actual) {
    
    int colisiones = 0, pos_lista_origianl, pos_actual, pos_original, tam_nuevo, espacios_libres = 0;
    static hash_list lista_original[500];
    hash_list *lista_ordenanda = malloc(*Mo * sizeof(hash_list));
    float  espacios_ocupados = 0;

    *tam_actual = *Mo;

    int n = leerFichero("trazas.csv", lista_original, 500);
    if (n < 0) {
        printf("no se pudo leer el fichero\n");
        return NULL;
    }

    /* inicializar la lista ordenanda */
    for (int i = 0; i < *tam_actual; i++) {
        lista_ordenanda[i].estado = LIBRE;
        espacios_libres++;
    }

    /* Insertar registros en la lista nueva */
    for (int i = 0; i < *R; i++) {

        /* Se ve que no este mas del 50% ocupado, si lo esta se redimensiona */
        if ((espacios_ocupados + 1) / (float)(*tam_actual) > 0.5) {
            tam_nuevo = *tam_actual * 2;
            while (!calcula_primo(tam_nuevo)) {
                tam_nuevo++;
            } 
            lista_ordenanda = crea_lista(lista_ordenanda, tam_actual, tam_nuevo);
            *tam_actual = tam_nuevo;
        }

        /* Calculo de la posicion */
        pos_original = (*P + i) % 500;
        int j = 1;

        pos_actual = lista_original[pos_original].clave % *tam_actual;

        /* Verifica si esta ocupado ese lugar, en caso de estarlo prueba otro con prueba lineal */
        while (lista_ordenanda[pos_actual].estado == OCUPADO) {
            colisiones++;
            pos_actual = (pos_original + (j * j)) % *tam_actual;
            j++;

            if (j > *tam_actual) {
                tam_nuevo = *tam_actual * 2;
                while (!calcula_primo(tam_nuevo)) {
                    tam_nuevo++;
                } 
                lista_ordenanda = crea_lista(lista_ordenanda, tam_actual, tam_nuevo);
                *tam_actual = tam_nuevo;
            }
        }

        /* Se insertan los valores al encontrar hueco */
        strcpy(lista_ordenanda[pos_actual].agente, lista_original[pos_original].agente);
        strcpy(lista_ordenanda[pos_actual].herramienta, lista_original[pos_original].herramienta);
        lista_ordenanda[pos_actual].clave = lista_original[pos_original].clave;
        lista_ordenanda[pos_actual].id = lista_original[pos_original].id;
        lista_ordenanda[pos_actual].latencia_ms = lista_original[pos_original].latencia_ms; 
        lista_ordenanda[pos_actual].tokens = lista_original[pos_original].tokens;
        lista_ordenanda[pos_actual].estado = lista_original[pos_original].estado;

        espacios_ocupados++;
    }

    espacios_libres = *tam_actual - espacios_ocupados;

    return lista_ordenanda;
}

hash_list* inserta_registros_dependiente_clave(int *R, int *P, long long *K, int *tam_actual) {

    float alpha = 1;
    int colisiones = 0, pos_original, pos_actual, espacios_libres = 0, espacios_ocupados = 0, paso;
    static hash_list lista_original[500];

    int n = leerFichero("trazas.csv", lista_original, 500);
    if (n < 0) {
        printf("no se pudo leer el fichero\n");
        return NULL;
    }

    *tam_actual = *R; // el tamaño inicial no va a ser mas pequeño que la cantidad de registros a meter

    /* Hcaemos que el tamaño sea primo */
    while (!calcula_primo(*tam_actual)) {
        (*tam_actual)++;
    } 

    /* el factor de carga (alpha) tiene que ser menor a 0.5 para comenzar a inisertar */
    while (alpha >= 0.5) {

        (*tam_actual)++; // aumentamos el tamaño para buscar el siguiente primo

        while (!calcula_primo(*tam_actual)) {
            (*tam_actual)++;
        }
        alpha = (float)(*R) / (float)(*tam_actual); // actualizamos alpha
    }


    hash_list *lista_ordenanda = malloc(*tam_actual * sizeof(hash_list));

    /* inicializar la lista ordenanda */
    for (int i = 0; i < *tam_actual; i++) {
        lista_ordenanda[i].estado = LIBRE;
        espacios_libres++;
    }


    paso = 1 + (*K % (*tam_actual - 2)); // como el tamaño no cambia podemos calcular el paso antes de empezar a recorrer

    for (int i = 0; i < *R; i++) {

        pos_original = (*P + i) % 500;
        int j = 1;

        pos_actual = lista_original[pos_original].clave % *tam_actual;

        while (lista_ordenanda[pos_actual].estado == OCUPADO) {
            colisiones++;
            pos_actual = (pos_original + j * paso) % *tam_actual;
            j++;

            // (H(k) + i · paso(k)) mod M, con paso(k) = 1 + (k mod (M − 2)).

            if (j > *tam_actual) {
                printf("SIN ESPACIO\n");
                break;
            }            
        }

        /* Se insertan los valores al encontrar hueco */
        strcpy(lista_ordenanda[pos_actual].agente, lista_original[pos_original].agente);
        strcpy(lista_ordenanda[pos_actual].herramienta, lista_original[pos_original].herramienta);
        lista_ordenanda[pos_actual].clave = lista_original[pos_original].clave;
        lista_ordenanda[pos_actual].id = lista_original[pos_original].id;
        lista_ordenanda[pos_actual].latencia_ms = lista_original[pos_original].latencia_ms; 
        lista_ordenanda[pos_actual].tokens = lista_original[pos_original].tokens;
        lista_ordenanda[pos_actual].estado = lista_original[pos_original].estado;
    }

    espacios_libres = *tam_actual - espacios_ocupados;

    return lista_ordenanda;

}

void muestra_lista(hash_list *lista, int *tam_lista) {

    if (lista == NULL)
        printf("La lista no existe\n");

    for (int i = 0; i < *tam_lista; i++) {
        if (lista[i].estado == OCUPADO) 
            printf("ID: %lld\t", lista[i].id);
    }

    return;
}

void consulta_factor_carga(hash_list *lista, int *tam_tabla) {

    float alpha = 0, registros_ocupados = 0;

    if (lista == NULL)
        printf("La lista no existe\n");

    for (int i = 0; i < *tam_tabla; i++) {
        if (lista[i].estado == OCUPADO)
            registros_ocupados++;
    }

    alpha = registros_ocupados / (float)(*tam_tabla);

    printf("El factor de carga es: %.4f\n", alpha);

    return;
}

void consulta_por_id(hash_list *lista, long long id) {

    int i = 0;

    if (lista == NULL)
        printf("La lista no existe\n");

    while (lista[i].id != id) {
        i++;
    }

    printf("Agente: %s\n", lista[i].agente);

    return;
}

void borrar_por_id(hash_list *lista, long long id) {

    int i = 0;

    if (lista == NULL)
        printf("La lista no existe\n");

    while (lista[i].id != id) {
        i++;
    }

    lista[i].estado = BORRADO;

    return;
}

int main () {

    int opcion, tam_tabla = 0;
    /* parametros del grupo */
    long long K = 0, id;
    int R = 0, P = 0, Mo = 0, M_ch = 0;
    hash_list *lista;

    do {
        printf("1- Calcular los parametros del grupo\n"
            "2- Prueba lineal con redispersion\n"
            "3- Prueba cuadratica con redispersion\n"
            "4- Exploracion dependiente de la clave\n"
            "5- Mostrar lista\n"
            "6- Mostrar factor de carga\n"
            "7- Consultar informacion a partir de ID\n"
            "Elige una opcion o presiona 9 para salir: ");

        scanf("%d", &opcion);
        getchar();  // eliminar el '\0' del scanf

        switch (opcion) {
            case 1:
                calcula_parametros(&R, &P, &Mo, &K, &M_ch);
                break;

            case 2:
                lista = inserta_registros_lineal(&R, &P, &Mo, &K, &tam_tabla);
                break;

            case 3:
                lista = inserta_registros_cuadratica(&R, &P, &Mo, &K, &tam_tabla);
                break;

            case 4:
                lista = inserta_registros_dependiente_clave(&R, &P, &K, &tam_tabla);
                break;

            case 5:
                muestra_lista(lista, &tam_tabla);
                break;

            case 6:
                consulta_factor_carga(lista, &tam_tabla);
                break;

            case 7:
                printf("Introduce el ID a consultar: ");
                scanf("%lld", &id);
                consulta_por_id(lista, id);
                break;

            case 9: break;

            default: printf("Opcion no valida!\n");
                break;
        }
    } while (opcion != 9);
    
    printf("Programa finalizado");

    return 0;
}