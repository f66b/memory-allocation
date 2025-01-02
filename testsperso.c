#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "common.h"
#include "mem.h"

// Fonction pour afficher l'état des blocs
void afficher_zone(void *adresse, size_t taille, int libre) {
    printf("Zone %s, Adresse : %p, Taille : %zu\n", libre ? "libre" : "occupée", adresse, taille);
}

int main() {
    // Taille de la mémoire à gérer
    size_t taille_mem = 1024; // 1 Ko
    void *mem = malloc(taille_mem); // Allocation de la mémoire

    // Initialisation de l'allocateur
    mem_init(mem, taille_mem);

    // Affichage de l'état initial de la mémoire
    printf("État initial de la mémoire :\n");
    mem_show(afficher_zone);

    // Tests d'allocation
    printf("\nTests d'allocation :\n");
    void *zone1 = mem_alloc(100);
    printf("Zone 1 allouée à %p\n", zone1);
    mem_show(afficher_zone);

    void *zone2 = mem_alloc(200);
    printf("Zone 2 allouée à %p\n", zone2);
    mem_show(afficher_zone);

    void *zone3 = mem_alloc(50);
    printf("Zone 3 allouée à %p\n", zone3);
    mem_show(afficher_zone);


    // Test d'allocation après libération
    printf("\nNouvelle allocation après libération :\n");
    void *zone4 = mem_alloc(150);
    printf("Zone 4 allouée à %p\n", zone4);
    mem_show(afficher_zone);

    

    // Libération de la mémoire utilisée
    free(mem);
    printf("tests passed\n");
    return 0;
}
