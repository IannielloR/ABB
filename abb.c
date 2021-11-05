#include "abb.h"

/* *****************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct nodo
{
    abb_nodo_t *izq;
    abb_nodo_t *der;
    char *clave;
    void *dato;
} abb_nodo_t;

typedef struct abb
{
    abb_nodo_t *raiz;
    abb_destruir_dato_t destruit_dato;
    abb_comparar_clave_t comparar_clave;
} abb_t;

/* *****************************************************************
 *                    PRIMITIVAS DEL ABB
 * *****************************************************************/

abb_t *abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato)
{
    abb_t *arbol = malloc(sizeof(abb_t));
    if (!arbol)
        return NULL;

    arbol->comparar_clave = cmp;
    arbol->destruit_dato = destruir_dato;
    arbol->raiz = NULL;
    return arbol;
}

/*abb_nodo_t *guardar_nodo(abb_nodo_t *nodo, const char* clave){
    if(!nodo)return NULL;

    if(strcmp(nodo->clave, clave) == 0 )return nodo;

    abb_nodo_t *nodo_izq = obtener_nodo(nodo->izq, clave);
    if(nodo_izq) return nodo_izq;

    abb_nodo_t *nodo_der = obtener_nodo(nodo->der, clave);
    if(nodo_der) return nodo_der;

    return NULL;
} */

abb_nodo_t *crear_nodo(const char *clave, void *dato)
{
    abb_nodo_t *nodo = malloc(sizeof(abb_nodo_t));
    if (!nodo)
        return false;
    nodo->clave = strdup(clave);
    nodo->dato = dato;
    nodo->izq = NULL;
    nodo->der = NULL;
    return nodo;
}

/*abb_nodo_t *obtener_nodo(abb_nodo_t *nodo, const char *clave)
{ //Fijarse por que vino
    if (!nodo)
        return NULL;

    if (strcmp(nodo->clave, clave) == 0)
    {
        return nodo;
    }

    abb_nodo_t *nodo_izq = obtener_nodo(nodo->izq, clave);
    if (nodo_izq)
        return nodo_izq;

    abb_nodo_t *nodo_der = obtener_nodo(nodo->der, clave);
    if (nodo_der)
        return nodo_der;

    return NULL;
}*/

bool ubicar_nodo(abb_nodo_t *nodo_actual, abb_nodo_t *nodo_padre, const char *clave, void *dato, abb_t *arbol)
{
    if (nodo_actual == NULL)
        return false;

    int resultado = arbol->comparar_clave(nodo_actual->clave, clave);

    if (resultado == 0)
    {
        void *aux_dato = nodo_actual->dato;
        nodo_actual->dato = dato;
        return true;
    }

    if (resultado < 0)
    {
        bool encontrado = (ubicar_nodo(nodo_actual->izq, nodo_actual, clave, dato, arbol) == false);
        if (encontrado)
        {
            abb_nodo_t *nodo = crear_nodo(clave, dato);
            if (!nodo)
                return false;
            nodo_padre->izq = nodo;
            return true;
        }
    }

    if (resultado > 0)
    {
        bool encontrado = (ubicar_nodo(nodo_actual->der, nodo_actual, clave, dato, arbol) == false);
        if (encontrado)
        {
            abb_nodo_t *nodo = crear_nodo(clave, dato);
            if (!nodo)
                return false;
            nodo_padre->der = nodo;
            return true;
        }
    }

    return false;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato)
{
    if (!arbol->raiz)
    {
        abb_nodo_t *nodo = crear_nodo(clave, dato);
        if (!nodo)
            return false;
        arbol->raiz = nodo;
    }
    ubicar_nodo(arbol->raiz, NULL, clave, dato, arbol);
}

int calcular_hijos(abb_nodo_t *nodo)
{
    int cantidad = 0;
    if (nodo->izq != NULL)
        cantidad++;
    if (nodo->der != NULL)
        cantidad++;
    return cantidad;
}

void borrar_0_1(abb_nodo_t *nodo_actual, abb_nodo_t *nodo_padre, abb_t *arbol, char dir)
{
    abb_nodo_t *nodo = nodo_actual->izq;
    if (nodo == NULL)
        nodo = nodo_actual->der;
    if (dir == 'i')
        nodo_padre->izq = nodo;
    if (dir == 'd')
        nodo_padre->der = nodo;
    free(nodo_actual->clave);
    free(nodo_actual);
}

void borrar_2(abb_nodo_t *nodo_actual, abb_nodo_t *nodo_padre, abb_t *arbol, char dir){
    abb_nodo_t *reemplazante = nodo_actual->izq;
    while (reemplazante->der != NULL)reemplazante = reemplazante->der;

    char *aux_clave = strdup(reemplazante->clave);
    void *aux_dato = reemplazante->dato;

    abb_borrar(arbol, reemplazante->clave);

    free(nodo_actual->clave);
    nodo_actual->clave = aux_clave;
    nodo_actual->dato = aux_dato;
}

void *borrar(abb_nodo_t *nodo_actual, abb_nodo_t *nodo_padre, const char *clave, abb_t *arbol)
{
    if (nodo_actual == NULL)
        return NULL;

    int resultado = arbol->comparar_clave(nodo_actual->clave, clave);

    if (resultado == 0)
        return true;

    if (resultado < 0)
    {
        bool encontrado = (borrar(nodo_actual->izq, nodo_actual, clave, arbol) == NULL);
        if (encontrado)
        {
            void *aux_dato = nodo_actual->dato;
            int hijos = calcular_hijos(nodo_actual);
            char dir = 'i';
            if (hijos == 2)
            {
                borrar_2(nodo_actual, nodo_padre, arbol, dir);
            }
            else
            {
                borrar_0_1(nodo_actual, nodo_padre, arbol, dir);
            }
            return aux_dato;
        }
    }

    if (resultado > 0)
    {
        bool encontrado = (borrar(nodo_actual->der, nodo_actual, clave, arbol) == NULL);
        if (encontrado)
        {
            void *aux_dato = nodo_actual->dato;
            int hijos = calcular_hijos(nodo_actual);
            char dir = 'd';
            if (hijos == 2)
            {
                borrar_2(nodo_actual, nodo_padre, arbol, dir);
            }
            else
            {
                borrar_0_1(nodo_actual, nodo_padre, arbol, dir);
            }
            return aux_dato;
        }
    }

    return NULL;
}

void *abb_borrar(abb_t *arbol, const char *clave)
{
    abb_nodo_t *nodo = borrar(arbol->raiz, NULL, clave, arbol);
}

void *abb_obtener(const abb_t *arbol, const char *clave)
{
    abb_nodo_t *nodo = obtener_nodo(arbol->raiz, clave);
    return nodo->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave)
{
    return (obtener_nodo(arbol->raiz, clave) != NULL);
}

int contar_nodos(abb_nodo_t *nodo)
{
    if (!nodo)
        return 0;

    int actual = 1;

    int cant_izq = contar_nodos(nodo->izq);
    int cant_der = contar_nodos(nodo->der);

    return (actual + cant_izq + cant_der);
}

size_t abb_cantidad(const abb_t *arbol)
{
    return contar_nodos(arbol->raiz);
}

void destruccion(abb_t *arbol, abb_nodo_t *nodo)
{
    if (nodo == NULL)
        return;

    destruccion(arbol, nodo->izq);
    destruccion(arbol, nodo->der);

    free(nodo->clave);
    arbol->destruit_dato(nodo->dato);
}

void abb_destruir(abb_t *arbol)
{
    destruccion(arbol, arbol->raiz);
}

/* *****************************************************************
 *                    AUXILIARES DEL ITERADOR
 * *****************************************************************/
 void pila_apilar_iterador(pila_t* pila, abb_nodo_t* nodo){

     if(!nodo) return NULL;

     pila_apilar(pila, nodo);
     pila_apilar_iterador(pila, nodo->izq)

 }

/* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

void abb_iterar(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra, abb_nodo_t* nodo_actual){
   
    if(!nodo_actual || !visitar(nodo_actual->clave, nodo_actual->dato, extra)) return;

    abb_iterar(visitar, extra, nodo_actual->izq);

    if(!visitar(nodo_actual->clave, nodo_actual->dato, extra) return;

    abb_iterar(visitar, extra, nodo_actual->der);

}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){

    abb_iterar(visitar, extra, arbol->raiz);
}


 /* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/


abb_iter_t *abb_iter_in_crear(const abb_t *arbol){

    abb_iter_t* iter = malloc(sizeof(abb_iter_t));
    if(!iter){
        return NULL;
    }

    pila_t* pila = pila_crear();
    if(!pila){
        free(iter);
        return NULL;
    }

    iter->pila = pila;
    iter->arbol = arbol;
    pila_apilar_iterador(pila, arbol->raiz);

    return iter;
}

bool abb_iter_in_al_final(const abb_iter_t *iter){

    return pila_esta_vacia(iter->pila);
}

bool abb_iter_in_avanzar(abb_iter_t *iter){

    if(abb_iter_in_al_final(iter)){
        return false;
    }

    abb_nodo_t* nodo_actual = pila_desapilar(i->pila);
    pila_apilar_iterador(iter->pila, nodo_actual->der);

    return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter){

    if(abb_iter_in_al_final(iter)){
        return NULL;
    }

    abb_nodo_t* nodo_actual = pila_ver_tope(iter->pila);
    return nodo_actual->clave;
}

void abb_iter_in_destruir(abb_iter_t* iter){
    pila_destruir(iter->pila);
    free(iter);
}
