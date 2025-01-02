/* On inclut l'interface publique */
#include "mem.h"
/* ainsi que les détails d'implémentation locaux */
#include "common.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

/* Définition de l'alignement recherché
 * Avec gcc, on peut utiliser __BIGGEST_ALIGNMENT__
 * sinon, on utilise 16 qui conviendra aux plateformes qu'on cible
 */
#ifdef __BIGGEST_ALIGNMENT__
#define ALIGNMENT __BIGGEST_ALIGNMENT__
#else
#define ALIGNMENT 16
#endif

/* structure placée au début de la zone de l'allocateur

   Elle contient toutes les variables globales nécessaires au
   fonctionnement de l'allocateur

   Elle peut bien évidemment être complétée
*/
struct allocator_header {
    size_t memory_size;
    mem_fit_function_t *fit;
};

/* La seule variable globale autorisée
 * On trouve à cette adresse le début de la zone à gérer
 * (et une structure 'struct allocator_header)
 */
static void *memory_addr;

static inline void *get_system_memory_addr() {
    return memory_addr;
}

static inline struct allocator_header *get_header() {
    struct allocator_header *h;
    h = get_system_memory_addr();
    return h;
}

static inline size_t get_system_memory_size() {
    return get_header()->memory_size;
}

struct fb {
    // Taille, entête compris
    size_t size;
    struct fb *next;
       /* ... */
};


void mem_init(void *mem, size_t taille) {
    memory_addr = mem;
    /* On vérifie qu'on a bien enregistré les infos et qu'on
     * sera capable de les récupérer par la suite
     */
    assert(mem == get_system_memory_addr());
    struct allocator_header *header = get_header();
    header->memory_size=taille;
    header->fit= &mem_fit_first;
    assert(taille == get_system_memory_size());
    struct fb *first_block = (struct fb *)((char *)mem + sizeof(struct allocator_header));
    first_block->size=taille-sizeof(struct allocator_header);
    first_block->next=NULL;

    /* On enregistre une fonction de recherche par défaut */
    mem_fit(&mem_fit_first);
}

void mem_show(void (*print)(void *zone, size_t size, int free)) {
    struct fb *current = (struct fb *)((char *)memory_addr+ sizeof(struct allocator_header));
    while (current) {
        /* ... */
        print((char *)current + sizeof(struct fb), current->size, 1);
        current = current->next;
        /* ... */
    }
}

void mem_fit(mem_fit_function_t *f) {
     get_header()->fit = f;
}

void *mem_alloc(size_t taille) {
    if (taille==0) {
        return NULL;
    }
    taille = (taille + sizeof(size_t)+ ALIGNMENT - 1)& ~(ALIGNMENT - 1);
    struct fb *block = get_header()->fit((struct fb *)((char *)memory_addr + sizeof(struct allocator_header)),taille);
    if (!block) {
        return NULL ;
    }
    if(block->size >= taille + sizeof(struct fb)) {
        struct fb *new_block = (struct fb *)((char *)block + sizeof(struct fb) + taille);
        new_block->size = block->size - taille - sizeof(struct fb);
        new_block->next = block->next;
        block->size = taille;
        block->next = new_block;
    }
    
    return (char *)block + sizeof(struct fb);
    
}


void mem_free(void *zone) {
    if (!zone) {
        return;
    }
    struct fb *block = (struct fb *)((char *)zone - sizeof(struct fb));
    block->next = (struct fb *)((char *)memory_addr + sizeof(struct allocator_header));
    struct fb *current = (struct fb *)((char *)memory_addr + sizeof(struct allocator_header));
    while (current){
        if((char *)current + sizeof(struct fb) + current->size == (char *)block){
            current->size += sizeof(struct fb) + block->size;
            block = current;
        }
        current=current->next;
    }
}

struct fb *mem_fit_first(struct fb *list, size_t size) {
    while(list){
        if (list->size >= size){
            return list;
        }
        list =list->next; 
    }
    return NULL;
}

/* Fonction à faire dans un second temps
 * - utilisée par realloc() dans malloc_stub.c
 * - nécessaire pour remplacer l'allocateur de la libc
 * - donc nécessaire pour 'make test_ls'
 * Lire malloc_stub.c pour comprendre son utilisation
 * (ou en discuter avec l'enseignant)
 */
size_t mem_get_size(void *zone) {
    /* zone est une adresse qui a été retournée par mem_alloc() */

    /* la valeur retournée doit être la taille maximale que
     * l'utilisateur peut utiliser dans cette zone */
    if (!zone) {
        return 0;
    }struct fb *block=(struct fb *)((char *)zone - sizeof(struct fb));
    return block->size;
}

/* Fonctions facultatives
 * autres stratégies d'allocation
 */
struct fb *mem_fit_best(struct fb *list, size_t size) {
    return NULL;
}

struct fb *mem_fit_worst(struct fb *list, size_t size) {
    return NULL;
}
