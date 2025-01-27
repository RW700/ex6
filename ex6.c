#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void freeOwnerNode(OwnerNode *owner) {
    free(owner->ownerName);
    owner->ownerName = NULL;
    recursivelyFreePokemonNodes(owner->pokedexRoot);
    owner->pokedexRoot = NULL;
    free(owner);
    owner = NULL;
}

void freeAllOwners() {
    if (ownerHead == NULL) {
        return;
    }
    OwnerNode *current = ownerHead;
    do {
        OwnerNode *temp = current;
        current = current->next;
        freeOwnerNode(temp);
    } while (current != ownerHead);
    ownerHead = NULL;
}

void printOwnersCircular() {
    // first: if no owners, print message and return
    if (ownerHead == NULL) {
        printf("No owners.\n");
        return;
    }

    // get direction from user
    printf("Enter direction (F or B): ");
    OwnerNode *temp = ownerHead;
    char *choice = getDynamicInput();
    if (!choice) {
        printf("Memory allocation failed.\n");
        return;
    }

    // validate input
    while (!strchr(choice, 'F') && !strchr(choice, 'f') &&
            !strchr(choice, 'B') && !strchr(choice, 'b')) {
        printf("Invalid direction, must be F or B.\n");
        printf("Enter direction (F or B): ");
        free(choice);
        choice = getDynamicInput();
        if (!choice) {
            printf("Memory allocation failed.\n");
            return;
        }
    }

    // get number of prints from user
    int numPrints = readIntSafe("How many prints? ");

    // case of less than 1: no prints
    if (numPrints < 1) {
        free(choice);
        return;
    }

    // print in said direction
    if (strchr(choice, 'F') || strchr(choice, 'f')) {
        // print all owners
        for (int i = 0; i < numPrints; i++) {
            printf("[%d] %s\n", i + 1, temp->ownerName);
            temp = temp->next;
        }
    } else {
        // print all owners in reverse
        for (int i = 0; i < numPrints; i++) {
            printf("[%d] %s\n", i + 1, temp->ownerName);
            temp = temp->prev;
        }
    }
    // free choice before exit
    free(choice);
}

void sortOwners() {
    // edge case: 0 or 1 owners:
    if (ownerHead == NULL || ownerHead->next == ownerHead) {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }
    // bubble sort on circular list
    int amountOfOwners = 0;
    OwnerNode *temp = ownerHead;
    while (temp->next != ownerHead) {
        amountOfOwners++;
        temp = temp->next;
    }
    amountOfOwners++;

    // bubble sort on circular list
    for (int i = 0; i < amountOfOwners - 1; i++) {
        OwnerNode *current = ownerHead;
        for (int j = 0; j < amountOfOwners - i - 1; j++) {
            if (strcmp(current->ownerName, current->next->ownerName) > 0) {
                swapOwnerData(current, current->next);
            }
            current = current->next;
        }
    }
    printf("Owners sorted by name.\n");
}

void swapOwnerData(OwnerNode *a, OwnerNode *b) {
    // swap the owner name and pokedexRoot
    a->prev->next = b;
    b->next->prev = a;
    b->prev = a->prev;
    a->next = b->next;
    a->prev = b;
    b->next = a;
}

OwnerNode *findOwnerByName(const char *nameToFind) {
    OwnerNode *temp = ownerHead;
    do {
        if (strcmp(temp->ownerName, nameToFind) == 0) {
            return temp;
        }
        temp = temp->next;
    } while (temp != ownerHead);
    return NULL;
}

void mergePokedexMenu() {
    // first - if less than two owners
    if (ownerHead == NULL || ownerHead->next == ownerHead) {
        printf("Not enough owners to merge.\n");
        return;
    }

    // ask to enter names
    printf("\n=== Merge Pokedexes ===\n");
    printf("Enter name of first owner: ");
    char *firstUser = getDynamicInput();
    if (!firstUser) {
        printf("Memory allocation failed.\n");
        return;
    }
    printf("Enter name of second owner: ");
    char *secondUser = getDynamicInput();
    if (!secondUser) {
        printf("Memory allocation failed.\n");
        free(firstUser);
        return;
    }
    printf("Merging %s and %s...\n", firstUser, secondUser);

    // find the two owners
    OwnerNode *firstOwner = findOwnerByName(firstUser);
    OwnerNode *secondOwner = findOwnerByName(secondUser);

    free(firstUser);
    free(secondUser);

    if (!firstOwner || !secondOwner) {return;}

    // BFS on second user, for each node add it to first - normal insertion
    // use same queue as evolve
    PokedexQueue *queue = malloc(sizeof(PokedexQueue));
    if (!queue) {
        printf("Memory allocation failed.\n");
        return;
    }
    // put root into queue
    PokedexQueueNode *firstNode = createQueueNode(secondOwner->pokedexRoot);
    queue->front = firstNode;
    queue->rear = firstNode;
    while (queue->front) {
        // add children to queue
        PokedexQueueNode *current = queue->front;
        if (current->data->left) {
            PokedexQueueNode *tempLeft = createQueueNode(current->data->left);
            queue->rear->next = tempLeft;
            queue->rear = tempLeft;
        }
        if (current->data->right) {
            PokedexQueueNode *tempRight = createQueueNode(current->data->right);
            queue->rear->next = tempRight;
            queue->rear = tempRight;
        }
        // add current to first owner
        addPKMNToBST(firstOwner->pokedexRoot, current->data->data->id);
        // remove from queue and free
        PokedexQueueNode *temp = queue->front;
        queue->front = queue->front->next;
        free(temp);
    }
    free(queue);

    // now use delete owner logic to remove second
    // first - free entire pokemon tree
    recursivelyFreePokemonNodes(secondOwner->pokedexRoot);
    // then free owner name
    free(secondOwner->ownerName);
    // then connect two surrounding nodes
    secondOwner->prev->next = secondOwner->next;
    secondOwner->next->prev = secondOwner->prev;
    // account for if this is the global head
    if (secondOwner == ownerHead) {
        ownerHead = secondOwner->next;
    }
    // then free itself
    free(secondOwner);

}

void deletePokedex() {
    printf("\n=== Delete a Pokedex ===\n");

    // print by looping through each owner - while doesn't equal head, print next etc
    printAllOwners();

    // get user input for which owner by list number starting at 1
    int ownerChoice = readIntSafe("Choose a Pokedex to delete by number: ");
    OwnerNode *cur = ownerHead;
    if (ownerChoice > 1) {
        for (int i = 1; i < ownerChoice; i++) {
            cur = cur->next;
        }
    }
    printf("\nDeleting %s's entire Pokedex...\n", cur->ownerName);

    // account for if this is the global head
    if (cur == ownerHead) {
        ownerHead = cur->next;
    }

    // first - free entire pokemon tree
    recursivelyFreePokemonNodes(cur->pokedexRoot);
    recursivelyCleanNullPokemon(&cur->pokedexRoot);
    // then free owner name
    free(cur->ownerName);
    cur->ownerName = NULL;
    // then connect two surrounding nodes
    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;
    // then free itself
    free(cur);
    cur = NULL;

    printf("Pokedex deleted.\n");
}

void evolvePokemon(OwnerNode *owner) {
    int IDToEvolve = readIntSafe("Enter ID of Pokemon to evolve: ");
    // find ID in BST - if exists, if not, NULL
    PokemonNode *pokemonToEvolve = searchPokemonBFS(owner->pokedexRoot, IDToEvolve);
    // if pokemon not in tree - print message and done
    if (pokemonToEvolve == NULL) {
        printf("No Pokemon with ID %d found.\n", IDToEvolve);
        return;
    }
    // if pokemon in tree, but cant evolve - print message and done
    if (pokemonToEvolve->data->CAN_EVOLVE == CANNOT_EVOLVE) {
        printf("%s (ID %d) cannot evolve.\n", pokemonToEvolve->data->name, IDToEvolve);
        return;
    }
    // otherwise, can evolve, so remove ID from tree, and add ID + 1 to tree
    freePokemonHelper(&(owner->pokedexRoot), IDToEvolve);
    printf("Removing Pokemon %s (ID %d).\n", pokedex[IDToEvolve - 1].name, IDToEvolve);
    addPKMNToBST(owner->pokedexRoot, IDToEvolve + 1);
    printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n", pokedex[IDToEvolve - 1].name, IDToEvolve, pokedex[IDToEvolve].name, IDToEvolve + 1);
}

void pokemonFight(OwnerNode *owner) {
    int ID1 = readIntSafe("Enter ID of the first Pokemon: ");
    int ID2 = readIntSafe("Enter ID of the second Pokemon: ");
    PokemonNode *pokemon1 = searchPokemonBFS(owner->pokedexRoot, ID1);
    PokemonNode *pokemon2 = searchPokemonBFS(owner->pokedexRoot, ID2);
    if (pokemon1 == NULL || pokemon2 == NULL) {
        printf("One or both Pokemon IDs not found.\n");
        return;
    }
    float attack1 = ((float)pokemon1->data->attack * (float)ATTACK_MODIFIER) + ((float)pokemon1->data->hp * (float)HP_MODIFIER);
    float attack2 = ((float)pokemon2->data->attack * (float)ATTACK_MODIFIER) + ((float)pokemon2->data->hp * (float)HP_MODIFIER);
    printf("Pokemon 1: %s (Score = %.2f)\n", pokemon1->data->name, attack1);
    printf("Pokemon 2: %s (Score = %.2f)\n", pokemon2->data->name, attack2);

    if (attack1 > attack2) {
        printf("%s wins!\n", pokemon1->data->name);
        return;
    }
    if (attack2 > attack1) {
        printf("%s wins!\n", pokemon2->data->name);
        return;
    }
    printf("It's a tie!\n");
}

void insertPokemonNode(PokemonNode **root, PokemonNode *newNode) {
    if (newNode == NULL || newNode->data == NULL) {
        return;
    }
    if (*root == NULL) {
        *root = newNode;
        return;
    }
    if (newNode->data->id < (*root)->data->id) {
        insertPokemonNode(&((*root)->left), newNode);
    } else if (newNode->data->id > (*root)->data->id) {
        insertPokemonNode(&((*root)->right), newNode);
    }
}

void recursivelyFreePokemonNodes(PokemonNode *root) {
    if (root == NULL) {
        return;
    }
    // free children first
    if (root->left != NULL) {
        recursivelyFreePokemonNodes(root->left);
    }
    if (root->right != NULL) {
        recursivelyFreePokemonNodes(root->right);
    }
    // free current node
    // at this point, already called recursively, so right and left can be freed too
    free(root->data->name);
    root->data->name = NULL;
    free(root->data);
    root->data = NULL;
    free(root);
    root = NULL;
}

PokemonNode *searchPokemonBFS(PokemonNode *root, int ID) {
    if (root == NULL) {
        return NULL;
    }
    if (ID > root->data->id) {
        return searchPokemonBFS(root->right, ID);
    }
    if (ID < root->data->id) {
        return searchPokemonBFS(root->left, ID);
    }
    if (ID == root->data->id) {
        return root;
    }
    return NULL;
}

void freePokemonNode(PokemonNode **pokemonInTree, int IDToRelease) {
        // create a list of all sub children ID's
        // recursively free everyone
        // then from smallest value of list (which is what should replace current)
        // copy ID to current location
        // then iteratively through the list of ids, add them to the tree standard node creation
        int amountOfPokemon = countNodesInTree((*pokemonInTree));
        int *allIDS = (int *)malloc(amountOfPokemon * sizeof(int));
        if (!allIDS) {
            printf("Memory allocation failed.\n");
            return;
        }
        //make queue to get ID's in proper order
        IDQueue *queue = malloc(sizeof(IDQueue));
        if (!queue) {
            printf("Memory allocation failed.\n");
            return;
        }
        // add all ID's to list and free all nodes
        if ((*pokemonInTree)->left != NULL) {
            IDQueueNode *firstLeft = malloc(sizeof(IDQueueNode));
            if (!firstLeft) {
                printf("Memory allocation failed.\n");
                return;
            }
            firstLeft->data = (*pokemonInTree)->left->data->id;
            queue->front = firstLeft;
            queue->rear = firstLeft;
        }
        if ((*pokemonInTree)->right != NULL) {
            IDQueueNode *firstRight = malloc(sizeof(IDQueueNode));
            if (!firstRight) {
                printf("Memory allocation failed.\n");
                return;
            }
            firstRight->data = (*pokemonInTree)->right->data->id;
            if (!queue->front || !queue->rear) {
                queue->front = firstRight;
                queue->rear = firstRight;
            } else {
                queue->rear->next = firstRight;
                queue->rear = firstRight;
            }
        }
        int locIDAdd = 0;
        // add all ID's to list and free nodes:
        while (queue->front) {
            // add ID to list, add children to queue, free
            IDQueueNode *current = queue->front;
            allIDS[locIDAdd] = current->data;
            locIDAdd++;
            // always add left child first
            PokemonNode *currentNode = searchPokemonBFS((*pokemonInTree), current->data);
            if (currentNode->left != NULL) {
                IDQueueNode *tempLeft = malloc(sizeof(IDQueueNode));
                if (!tempLeft) {
                    printf("Memory allocation failed.\n");
                    return;
                }
                tempLeft->data = currentNode->left->data->id;
                queue->rear->next = tempLeft;
                queue->rear = tempLeft;
            }
            if (currentNode->right != NULL) {
                IDQueueNode *tempRight = malloc(sizeof(IDQueueNode));
                if (!tempRight) {
                    printf("Memory allocation failed.\n");
                    return;
                }
                tempRight->data = currentNode->right->data->id;
                queue->rear->next = tempRight;
                queue->rear = tempRight;
            }
            // free current Queue node
            queue->front = queue->front->next;
            free(current);
        }

        // now all IDs in list, so free all children recursively
        recursivelyFreePokemonNodes((*pokemonInTree));

        // now, find smallest greater ID in list, and copy it to current location
        // first set smallest greater to higher than num pokemon, so we can find smallest
        int smallestGreaterID = AMOUNT_OF_POKEMON;
        for (int i = 0; i < amountOfPokemon; i++) {
            if (allIDS[i] > IDToRelease && allIDS[i] < smallestGreaterID) {
                smallestGreaterID = allIDS[i];
            }
        }
        PokemonNode *newSubTree = NULL;
        // if smallest greater is not AMOUNT_OF_POKEMON, then copy it to current location
        if (smallestGreaterID != AMOUNT_OF_POKEMON) {
            newSubTree = createPokemonNode(copyPokedexEntryByID(smallestGreaterID - 1));
            newSubTree->left = NULL;
            newSubTree->right = NULL;
        }
        for (int i = 0; i < amountOfPokemon - 1; i++) {
            if (allIDS[i] != smallestGreaterID) {
                // add all other ID's to tree
                if (newSubTree == NULL) {
                    newSubTree = createPokemonNode(copyPokedexEntryByID(allIDS[i] - 1));
                    newSubTree->left = NULL;
                    newSubTree->right = NULL;
                } else {
                    insertPokemonNode(&newSubTree, createPokemonNode(copyPokedexEntryByID(allIDS[i] - 1)));
                }
            }
        }

        *pokemonInTree = newSubTree;

        // free used date:
        free(queue);
        free(allIDS);
}

void freePokemonHelper(PokemonNode **pokemonInTree, int IDToRelease) {
    // case 1: ID == currentNodeID => got it, free pokemon
    if (IDToRelease == (*pokemonInTree)->data->id) {

        // case: no children to worry about assigning - just free it in place now
        if (((*pokemonInTree)->left == NULL) && ((*pokemonInTree)->right == NULL)) {
            free((*pokemonInTree)->data->name);
            (*pokemonInTree)->data->name = NULL;
            free((*pokemonInTree)->data);
            (*pokemonInTree)->data = NULL;
            free(*pokemonInTree);
            *pokemonInTree = NULL;
            return;
        }

        // otherwise, neeed to handle children, so:
        // call freePokemonNode which has the logic to free it and handle children
        freePokemonNode(pokemonInTree, IDToRelease);

        return;
    }
    // case 2: both children are null => return failure
    if (((*pokemonInTree)->left == NULL) && ((*pokemonInTree)->right == NULL)) {
        printf("No Pokemon with ID %d found.", IDToRelease);
        return;
    }
    // case 3: ID > currentNodeID => go right
    if (IDToRelease > (*pokemonInTree)->data->id) {
        return freePokemonHelper(&((*pokemonInTree)->right), IDToRelease);
    }
    // case 4: ID < currentNodeID => go left
    return freePokemonHelper(&((*pokemonInTree)->left), IDToRelease);

}

void freePokemon(OwnerNode *owner) {
    int choiceOfIDToFree = readIntSafe("Enter Pokemon ID to release: ");
    // call recursive function to find if ID exists in tree and free it
    freePokemonHelper(&owner->pokedexRoot, choiceOfIDToFree);
}

int countNodesInTree(PokemonNode *root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + countNodesInTree(root->left) + countNodesInTree(root->right);
}

void displayAlphabetical(PokemonNode *root, VisitNodeFunc visit) {
    // initialize NodeArray struct
    int numNodes = countNodesInTree(root);
    NodeArray *nodeArray = (NodeArray*)malloc(sizeof(NodeArray));
    if (!nodeArray) {
        printf("Memory allocation failed.\n");
        return;
    }
    initNodeArray(nodeArray, numNodes);
    // add all nodes, regardless of order, into array
    // don't need to worry about reallc, as counted nodes first and made it to size
    collectAll(root, nodeArray);
    // run bubble sort on array
    qsort(nodeArray->nodes, nodeArray->size, sizeof(PokemonNode*), compareByNameNode);

    for (int i = 0; i < nodeArray->size; i++) {
        visit(nodeArray->nodes[i]);
    }

    free(nodeArray->nodes);
    free(nodeArray);
}

int compareByNameNode(const void *a, const void *b) {
    // run strcmp on our two strings for qsort compare
    return strcmp((*(PokemonNode**)a)->data->name, (*(PokemonNode**)b)->data->name);
}

void collectAll(PokemonNode *root, NodeArray *na) {
    // add node to Array, then recursively add left and right children
    na->nodes[na->size] = root;
    na->size++;
    if (root->left) {
        collectAll(root->left, na);
    }
    if (root->right) {
        collectAll(root->right, na);
    }
}

void initNodeArray(NodeArray *na, int cap) {
    // initialize array and set capacity to size of array
    na->nodes = malloc(cap * sizeof(PokemonNode*));
    na->capacity = cap;
    // initialize current size/amount of nodes added to zero
    na->size = 0;
}

void postOrderTraversal(PokemonNode *root, VisitNodeFunc visit) {
    // post order visits bottom node from left path first, then right, then back to root
    // recursively visit left
    if (root->left) {
        postOrderTraversal(root->left, visit);
    }
    // recursively visit right
    if (root->right) {
        postOrderTraversal(root->right, visit);
    }
    // visit root
    visit(root);
}

void inOrderTraversal(PokemonNode *root, VisitNodeFunc visit) {
    // in order visits bottom node from left path first, then back to root, then right, then back
    // recursively visit left
    if (root->left) {
        inOrderTraversal(root->left, visit);
    }
    // visit root
    visit(root);
    // recursively visit right
    if (root->right) {
        inOrderTraversal(root->right, visit);
    }
}

void preOrderTraversal(PokemonNode *root, VisitNodeFunc visit) {
    // pre order visits everything from left first:
    // first visit root/current node
    visit(root);
    // then recursively visit left
    if (root->left) {
        preOrderTraversal(root->left, visit);
    }
    // then recursively visit right
    if (root->right) {
        preOrderTraversal(root->right, visit);
    }
}

PokedexQueueNode* createQueueNode(PokemonNode *data) {
    PokedexQueueNode *newNode = (PokedexQueueNode*)malloc(sizeof(PokedexQueueNode));
    if (!newNode) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

void displayBFS(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        // printf("Pokedex is empty.\n");
        return;
    }
    if (root->data == NULL) {
        // printf("Pokedex is empty.\n");
        return;
    }
    if (root->data->name == NULL) {
        return;
    }

    //first create the queue
    PokedexQueue *queue = malloc(sizeof(PokedexQueue));
    if (!queue) {
        printf("Memory allocation failed.\n");
        return;
    }
    // put root into queue
    PokedexQueueNode *firstNode = createQueueNode(root);
    queue->front = firstNode;
    queue->rear = firstNode;

    // while queue is not empty, print the front, then enqueue the left and right children
    while (queue->front) {
        PokedexQueueNode *current = queue->front;
        visit(current->data);
        if (current->data->left) {
            PokedexQueueNode *tempLeft = createQueueNode(current->data->left);
            queue->rear->next = tempLeft;
            queue->rear = tempLeft;
        }
        if (current->data->right) {
            PokedexQueueNode *tempRight = createQueueNode(current->data->right);
            queue->rear->next = tempRight;
            queue->rear = tempRight;
        }
        queue->front = queue->front->next;
        free(current);
    }
    free(queue);
}

PokemonNode *createPokemonNode(const PokemonData *data) {
    PokemonNode *newPokemon = malloc(sizeof(PokemonNode));
    if (!newPokemon) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    // created the new node, now set all it's values
    newPokemon->data = malloc(sizeof(PokemonData));
    if (!newPokemon->data) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    newPokemon->data->id = data->id;
    newPokemon->data->name = myStrdup(data->name);
    newPokemon->data->TYPE = data->TYPE;
    newPokemon->data->hp = data->hp;
    newPokemon->data->attack = data->attack;
    newPokemon->data->CAN_EVOLVE = data->CAN_EVOLVE;
    newPokemon->left = NULL;
    newPokemon->right = NULL;
    return newPokemon;
}

int addPKMNToBST(PokemonNode *root, int ID) {
    // edge case: root / data is NULL
    if (root == NULL) {
        return -1;
    }
    if (root->data == NULL) {
        return -1;
    }
    if (!root->data->id) {
        return -1;
    }
    if (root->data->id <= 0 || root->data->id > AMOUNT_OF_POKEMON) {
        return -1;
    }

    // case 0: ID is already in tree
    if (ID == root->data->id) {
        // case: pokemon already in tree -> return 0
        return 0;
    }
    // case 1: ID goes to the right (is greater than current)
    if (ID > root->data->id) {
        // if no pokemon to right, add right there
        // case: successfully put into tree, return 1
        if (root->right == NULL) {
            // when getting Pokedex from array, need to minus 1 from ID for index
            root->right = createPokemonNode(&pokedex[ID - 1]);
            return 1;
        }
        // otherwise, return recursively in next place to the right
        return addPKMNToBST(root->right, ID);
    }
    // otherwise, case 2, it goes to left, and same thing
    // case: successfully put into tree, return 1
    if (root->left == NULL) {
        //when getting Pokedex from array, need to minus 1 from ID for index
        root->left = createPokemonNode(&pokedex[ID - 1]);
        return 1;
    }
    // otherwise return recursively to the left
    return addPKMNToBST(root->left, ID);
}

void addPokemon(OwnerNode *owner) {
    int IDToAdd = readIntSafe("Enter ID to add: ");

    // edge case(s), root or roots data is empty, in which case, instantialize root to this pokemon
    if (owner->pokedexRoot == NULL) {
        owner->pokedexRoot = malloc(sizeof(PokemonNode));
        if (owner->pokedexRoot == NULL) {
            printf("Memory allocation failed.\n");
            return;
        }
        owner->pokedexRoot->data = copyPokedexEntryByID(IDToAdd - 1);
        printf("Pokemon %s (ID %d) added.\n", pokedex[IDToAdd - 1].name, IDToAdd);
        return;
    }
    if (owner->pokedexRoot->data == NULL) {
        owner->pokedexRoot->data = copyPokedexEntryByID(IDToAdd - 1);
        printf("Pokemon %s (ID %d) added.\n", pokedex[IDToAdd - 1].name, IDToAdd);
        return;
    }


    // based on ID given, call addPKMNToBST with the root of the BST and the ID wanted
    int returnStatus = addPKMNToBST(owner->pokedexRoot, IDToAdd);

    if (returnStatus == 0) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", IDToAdd);
        return;
    }
    printf("Pokemon %s (ID %d) added.\n", pokedex[IDToAdd - 1].name, IDToAdd);

}

void printAllOwners() {
    OwnerNode *temp = ownerHead;
    int upTo = 1;
    do {
        printf("%d. %s\n", upTo, temp->ownerName);
        upTo++;
        temp = temp->next;
    } while (temp != ownerHead);
}

PokemonData *copyPokedexEntryByID(int id) {
    PokemonData *newData = malloc(sizeof(PokemonData));
    if (!newData) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    newData->id = pokedex[id].id;
    newData->name = myStrdup(pokedex[id].name);
    newData->attack = pokedex[id].attack;
    newData->hp = pokedex[id].hp;
    newData->TYPE = pokedex[id].TYPE;
    newData->CAN_EVOLVE = pokedex[id].CAN_EVOLVE;

    return newData;
}

void openPokedexMenu() {
    // create the new node
    OwnerNode* newOwner = malloc(sizeof(OwnerNode));
    if (newOwner == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // get the new node's data
    printf("Your name: ");
    newOwner->ownerName = getDynamicInput();
    if (findOwnerByName(newOwner->ownerName) != NULL) {
        printf("Owner already exists.\n");
        free(newOwner->ownerName);
        free(newOwner);
        return;
    }
    printf("Choose Starter:\n1. Bulbasaur\n2. Charmander\n3. Squirtle\n");
    int choice = readIntSafe("Your choice: ");
    if (choice < BULBASAUR_OPT || choice > SQUIRTLE_OPT) {
        printf("Invalid choice.\n");
        return;
    }
    newOwner->pokedexRoot = malloc(sizeof(PokemonNode));
    if (newOwner->pokedexRoot == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    int idOfChoice;
    if (choice == BULBASAUR_OPT) {idOfChoice = BULBASAUR_ID;}
    else if (choice == CHARMANDER_OPT) {idOfChoice = CHARMANDER_ID;}
    else {idOfChoice = SQUIRTLE_ID;}
    newOwner->pokedexRoot->data = copyPokedexEntryByID(idOfChoice - 1);
    printf("New Pokedex created for %s with starter %s.\n", newOwner->ownerName, newOwner->pokedexRoot->data->name);

    // assign this new node a location in the node circle
    // if it's the first, set its location to itself
    if (ownerHead == NULL) {
        ownerHead = newOwner;
        ownerHead->next = ownerHead;
        ownerHead->prev = ownerHead;
    } else {
    // otherwise slip it in before the head / in last place in the circle
        OwnerNode *lastNode = ownerHead->prev;
        lastNode->next = newOwner;
        newOwner->prev = lastNode;
        newOwner->next = ownerHead;
        ownerHead->prev = newOwner;
    }

}

void recursivelyCleanNullPokemon(PokemonNode **root) {
    if (root == NULL || *root == NULL) {
        return;
    }
    if ((*root)->data == NULL || (*root)->data->name == NULL) {
        *root = NULL;
        return;
    }
    recursivelyCleanNullPokemon(&((*root)->left));
    recursivelyCleanNullPokemon(&((*root)->right));
}

void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (node == NULL) {
        return;
    }
    if (node->data == NULL) {
        return;
    }
    if (node->data->name == NULL) {
        return;
    }
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    if (!owner->pokedexRoot || !owner->pokedexRoot->data || !owner->pokedexRoot->data->name)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    // create function pointer as requirement
    void (*printFunction)(PokemonNode *root) = printPokemonNode;

    switch (choice)
    {
    case DISP_BFS_OPT:
        displayBFS(owner->pokedexRoot, printFunction);
        break;
    case DISP_PRE_ORD_OPT:
        preOrderTraversal(owner->pokedexRoot, printFunction);
        break;
    case DISP_IN_ORD_OPT:
        inOrderTraversal(owner->pokedexRoot, printFunction);
        break;
    case DISP_POST_ORD_OPT:
        postOrderTraversal(owner->pokedexRoot, printFunction);
        break;
    case DISP_ALPH_ORD_OPT:
        displayAlphabetical(owner->pokedexRoot, printFunction);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    // list owners
    printf("\nExisting Pokedexes (circular list):\n");

    // print by looping through each owner - while doesn't equal head, print next etc
    printAllOwners();

    // get user input for which owner by list number starting at 1
    int ownerChoice = readIntSafe("Choose a Pokedex by number: ");
    OwnerNode *cur = ownerHead;
    if (ownerChoice > 1) {
        for (int i = 1; i < ownerChoice; i++) {
            cur = cur->next;
        }
    }


    printf("\nEntering %s's Pokedex...\n", cur->ownerName);

    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case OWN_ADD_OPT:
            addPokemon(cur);
            break;
        case OWN_DISP_OPT:
            displayMenu(cur);
            break;
        case OWN_FREE_OPT:
            if (!cur->pokedexRoot || !cur->pokedexRoot->data) {
                printf("No Pokemon to release.\n");
                break;
            }
            freePokemon(cur);
            // if we removed the only node, name should set to NULL, so check for that here
            // data is already freed, so just set the pointer to NULL, so we don't later try to reference a freed data point
            if (cur->pokedexRoot->data->name == NULL) {
                cur->pokedexRoot = NULL;
            }
            // edge case where there is remnant pointer left, this finds it and sets it to null
            recursivelyCleanNullPokemon(&(cur->pokedexRoot));
            break;
        case OWN_FIGHT_OPT:
            if (cur->pokedexRoot == NULL) {
                printf("Pokedex is empty.\n");
                break;
            }
            pokemonFight(cur);
            break;
        case OWN_EVOLVE_OPT:
            if (cur->pokedexRoot == NULL) {
                printf("Pokedex is empty.\n");
                break;
            }
            evolvePokemon(cur);
            break;
        case OWN_BACK_OPT:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice)
        {
        case MAIN_OPEN_POKEDEX_OPT:
            openPokedexMenu();
            break;
        case MAIN_ENTER_POKEDEX_OPT:
            if (ownerHead == NULL) {
                printf("No existing Pokedexes.\n");
                break;
            }
            enterExistingPokedexMenu();
            break;
        case MAIN_DELETE_OPT:
            deletePokedex();
            break;
        case MAIN_MERGE_OPT:
            mergePokedexMenu();
            break;
        case MAIN_SORT_OPT:
            sortOwners();
            break;
        case MAIN_PRINT_OPT:
            printOwnersCircular();
            break;
        case MAIN_EXIT_OPT:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    mainMenu();
    freeAllOwners();
    return 0;
}
