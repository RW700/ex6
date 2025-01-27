#ifndef EX6_H
#define EX6_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// custom defines to avoid magic numbers and have clearer code
// display menu options
#define DISP_BFS_OPT 1
#define DISP_PRE_ORD_OPT 2
#define DISP_IN_ORD_OPT 3
#define DISP_POST_ORD_OPT 4
#define DISP_ALPH_ORD_OPT 5

// owner actions menu options
#define OWN_ADD_OPT 1
#define OWN_DISP_OPT 2
#define OWN_FREE_OPT 3
#define OWN_FIGHT_OPT 4
#define OWN_EVOLVE_OPT 5
#define OWN_BACK_OPT 6

// main menu options
#define MAIN_OPEN_POKEDEX_OPT 1
#define MAIN_ENTER_POKEDEX_OPT 2
#define MAIN_DELETE_OPT 3
#define MAIN_MERGE_OPT 4
#define MAIN_SORT_OPT 5
#define MAIN_PRINT_OPT 6
#define MAIN_EXIT_OPT 7

// starter pokemon options and IDs
#define BULBASAUR_OPT 1
#define BULBASAUR_ID 1
#define CHARMANDER_OPT 2
#define CHARMANDER_ID 4
#define SQUIRTLE_OPT 3
#define SQUIRTLE_ID 7

#define AMOUNT_OF_POKEMON 151

// Attack modifiers
#define ATTACK_MODIFIER 1.5
#define HP_MODIFIER 1.2

typedef enum
{
    GRASS,
    FIRE,
    WATER,
    BUG,
    NORMAL,
    POISON,
    ELECTRIC,
    GROUND,
    FAIRY,
    FIGHTING,
    PSYCHIC,
    ROCK,
    GHOST,
    DRAGON,
    ICE
} PokemonType;

typedef enum
{
    CANNOT_EVOLVE,
    CAN_EVOLVE
} EvolutionStatus;

typedef struct PokemonData
{
    int id;
    char *name;
    PokemonType TYPE;
    int hp;
    int attack;
    EvolutionStatus CAN_EVOLVE;
} PokemonData;

// Binary Tree Node (for Pokédex)
typedef struct PokemonNode
{
    PokemonData *data;
    struct PokemonNode *left;
    struct PokemonNode *right;
} PokemonNode;

// Linked List Node (for Owners)
typedef struct OwnerNode
{
    char *ownerName;          // Owner's name
    PokemonNode *pokedexRoot; // Pointer to the root of the owner's Pokédex
    struct OwnerNode *next;   // Next owner in the linked list
    struct OwnerNode *prev;   // Previous owner in the linked list
} OwnerNode;

// Global head pointer for the linked list of owners
OwnerNode *ownerHead = NULL;

// Queue node for BFS printing
typedef struct PokedexQueueNode {
   PokemonNode *data;
   struct PokedexQueueNode *next;
} PokedexQueueNode;

// Queue for BFS printing
typedef struct PokedexQueue {
   PokedexQueueNode *front;
   PokedexQueueNode *rear;
} PokedexQueue;

// Queue node for BFS printing
typedef struct IDQueueNode {
   int data;
   struct IDQueueNode *next;
} IDQueueNode;

// Queue for BFS printing
typedef struct IDQueue {
   IDQueueNode *front;
   IDQueueNode *rear;
} IDQueue;


/* ------------------------------------------------------------
   1) Safe Input + Utility
   ------------------------------------------------------------ */

/**
 * @brief Remove leading/trailing whitespace (including '\r').
 * @param str modifiable string
 * Why we made it: We must handle CR/LF or random spaces in user input.
 */
void trimWhitespace(char *str);

/**
 * @brief C99-friendly strdup replacement.
 * @param src source string
 * @return newly allocated copy of src
 * Why we made it: Some old systems lack strdup; we do it ourselves.
 */
char *myStrdup(const char *src);

/**
 * @brief Read an integer safely, re-prompt if invalid.
 * @param prompt text to display
 * @return valid integer from user
 * Why we made it: We want robust menu/ID input handling.
 */
int readIntSafe(const char *prompt);

/**
 * @brief Read a line from stdin, store in malloc'd buffer, trim whitespace.
 * @return pointer to the newly allocated string (caller frees)
 * Why we made it: We need flexible name input that handles CR/LF etc.
 */
char *getDynamicInput(void);

/**
 * @brief Return a string for a given PokemonType enum.
 * @param type the enum
 * @return string like "GRASS", "FIRE", etc.
 * Why we made it: So we can print readable type names.
 */
const char *getTypeName(PokemonType type);

/* ------------------------------------------------------------
   2) Creating & Freeing Nodes
   ------------------------------------------------------------ */

/**
 * @brief Create a BST node with a copy of the given PokemonData.
 * @param data pointer to PokemonData (like from the global pokedex)
 * @return newly allocated PokemonNode*
 * Why we made it: We need a standard way to allocate BST nodes.
 */
PokemonNode *createPokemonNode(const PokemonData *data);

/**
 * @brief Create an OwnerNode for the circular owners list.
 * @param ownerName the dynamically allocated name
 * @param starter BST root for the starter Pokemon
 * @return newly allocated OwnerNode*
 * Why we made it: Each user is represented as an OwnerNode.
 */
OwnerNode *createOwner(char *ownerName, PokemonNode *starter);

/**
 * @brief Free one PokemonNode (including name).
 * @param pokemonInTree pointer to pointer to node tree
 * @param IDToRelease ID of pokemon we want to release
 * Why we made it: Avoid memory leaks for single nodes.
 */
void freePokemonNode(PokemonNode **pokemonInTree, int IDToRelease);
/**
 * @brief Recursively free a BST of PokemonNodes.
 * @param root BST root
 * Why we made it: Clearing a user’s entire Pokedex means freeing a tree.
 */
void freePokemonTree(PokemonNode *root);

/**
 * @brief Free an OwnerNode (including name and entire Pokedex BST).
 * @param owner pointer to the owner
 * Why we made it: Deleting an owner also frees their Pokedex & name.
 */
void freeOwnerNode(OwnerNode *owner);

/**
 * @brief Recursively free all PokemonNodes from a given root (can be sub-tree).
 * Why we made it: Useful for cleaning tree for end of program or for restructure.
 */
void recursivelyFreePokemonNodes(PokemonNode *root);

/* ------------------------------------------------------------
   3) BST Insert, Search, Remove
   ------------------------------------------------------------ */

/**
 * @brief Insert a PokemonNode into BST by ID; duplicates freed.
 * @param root pointer to pointer to BST root
 * @param newNode node to insert
 * @return updated BST root
 * Why we made it: Standard BST insertion ignoring duplicates.
 */
void insertPokemonNode(PokemonNode **root, PokemonNode *newNode);

/**
 * @brief BFS search for a Pokemon by ID in the BST.
 * @param root BST root
 * @param id ID to find
 * @return pointer to found node or NULL
 * Why we made it: BFS ensures we find nodes even in an unbalanced tree.
 */
PokemonNode *searchPokemonBFS(PokemonNode *root, int id);

/**
 * @brief Remove node from BST by ID if found (BST removal logic).
 * @param root BST root
 * @param id ID to remove
 * @return updated BST root
 * Why we made it: We handle special cases of a BST remove (0,1,2 children).
 */
PokemonNode *removeNodeBST(PokemonNode *root, int id);

/**
 * @brief Combine BFS search + BST removal to remove Pokemon by ID.
 * @param root BST root
 * @param id the ID to remove
 * @return updated BST root
 * Why we made it: BFS confirms existence, then removeNodeBST does the removal.
 */
PokemonNode *removePokemonByID(PokemonNode *root, int id);

/**
 * @brief Add a Pokemon to the BST by ID, avoiding duplicates.
 * @param root BST root
 * @param ID ID to add
 * @return 1 if added, 0 if already present
 * Why we made it: Simplifies adding a Pokemon to a BST.
 */
int addPKMNToBST(PokemonNode *root, int ID);

/**
 * @brief Recursively clean all NULL PokemonNode pointers from a BST.
 * @param root pointer to pointer to BST root
 * Why we made it: Avoid memory leaks from freeing NULL nodes.
 */
void recursivelyCleanNullPokemon(PokemonNode **root);

/* ------------------------------------------------------------
   4) Generic BST Traversals (Function Pointers)
   ------------------------------------------------------------ */

/**
 * @brief Create a copy of the PokemonData from the Pokedex Array
 * @param id Pokedex Pokemon ID
 * @return pointer to a copy of the PokemonData
 */
PokemonData *copyPokedexEntryByID(int id);

// Please notice, it's not really generic, it's just a demonstration of function pointers.
// so don't be confused by the name, but please remember that you must use it.

typedef void (*VisitNodeFunc)(PokemonNode *);

/**
 * @brief Generic BFS traversal: call visit() on each node (level-order).
 * @param root BST root
 * @param visit function pointer for what to do with each node
 * Why we made it: BFS plus function pointers => flexible traversal.
 */
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit);

/**
 * @brief Create a new queue node for BFS traversal.
 * @param data pointer to PokemonNode
 * @return newly allocated PokedexQueueNode
 * Why we made it: We need a standard way to allocate queue nodes.
 */
PokedexQueueNode *createQueueNode(PokemonNode *data);

/**
 * @brief A generic pre-order traversal (Root-Left-Right).
 * @param root BST root
 * @param visit function pointer
 * Why we made it: Another demonstration of function-pointer-based traversal.
 */
void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit);

/**
 * @brief A generic in-order traversal (Left-Root-Right).
 * @param root BST root
 * @param visit function pointer
 * Why we made it: Great for seeing sorted order if BST is sorted by ID.
 */
void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit);

/**
 * @brief A generic post-order traversal (Left-Right-Root).
 * @param root BST root
 * @param visit function pointer
 * Why we made it: Another standard traversal pattern.
 */
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit);

/**
 * @brief Print one PokemonNode’s data: ID, Name, Type, HP, Attack, Evolve?
 * @param node pointer to the node
 * Why we made it: We can pass this to BFSGeneric or others to quickly print.
 */
void printPokemonNode(PokemonNode *node);

/* ------------------------------------------------------------
   5) Display Methods (BFS, Pre, In, Post, Alphabetical)
   ------------------------------------------------------------ */

typedef struct
{
    PokemonNode **nodes;
    int size;
    int capacity;
} NodeArray;

/**
 * @brief Count the number of nodes in a BST.
 * @param root BST root
 * @return number of nodes
 * Why we made it: Useful for pre-allocating an array for sorting.
 */
int countNodesInTree(PokemonNode *root);

/**
 * @brief Initialize a NodeArray with given capacity.
 * @param na pointer to NodeArray
 * @param cap initial capacity
 * Why we made it: We store pointers to PokemonNodes for alphabetical sorting.
 */
void initNodeArray(NodeArray *na, int cap);

/**
 * @brief Recursively collect all nodes from the BST into a NodeArray.
 * @param root BST root
 * @param na pointer to NodeArray
 * Why we made it: We gather everything for qsort.
 */
void collectAll(PokemonNode *root, NodeArray *na);

/**
 * @brief Compare function for qsort (alphabetical by node->data->name).
 * @param a pointer to a pointer to PokemonNode
 * @param b pointer to a pointer to PokemonNode
 * @return -1, 0, or +1
 * Why we made it: Sorting by name for alphabetical display.
 */
int compareByNameNode(const void *a, const void *b);

/**
 * @brief BFS is nice, but alphabetical means we gather all nodes, sort by name, then print.
 * @param root BST root
 * Why we made it: Provide user the option to see Pokemon sorted by name.
 */
void displayAlphabetical(PokemonNode *root, VisitNodeFunc visit);

/**
 * @brief BFS user-friendly display (level-order).
 * @param root BST root
 * Why we made it: Quick listing in BFS order.
 */
void displayBFS(PokemonNode *root, VisitNodeFunc visit);

/**
 * @brief Pre-order user-friendly display (Root->Left->Right).
 * @param root BST root
 * Why we made it: Another standard traversal for demonstration.
 */
void preOrderTraversal(PokemonNode *root, VisitNodeFunc visit);

/**
 * @brief In-order user-friendly display (Left->Root->Right).
 * @param root BST root
 * Why we made it: Good for sorted output by ID if the tree is a BST.
 */
void inOrderTraversal(PokemonNode *root, VisitNodeFunc visit);

/**
 * @brief Post-order user-friendly display (Left->Right->Root).
 * @param root BST root
 * Why we made it: Another standard traversal pattern.
 */
void postOrderTraversal(PokemonNode *root, VisitNodeFunc visit);

/* ------------------------------------------------------------
   6) Pokemon-Specific
   ------------------------------------------------------------ */

/**
 * @brief Let user pick two Pokemon by ID in the same Pokedex to fight.
 * @param owner pointer to the Owner
 * Why we made it: Fun demonstration of BFS and custom formula for battles.
 */
void pokemonFight(OwnerNode *owner);

/**
 * @brief Evolve a Pokemon (ID -> ID+1) if allowed.
 * @param owner pointer to the Owner
 * Why we made it: Demonstrates removing an old ID, inserting the next ID.
 */
void evolvePokemon(OwnerNode *owner);

/**
 * @brief Prompt for an ID, BFS-check duplicates, then insert into BST.
 * @param owner pointer to the Owner
 * Why we made it: Primary user function for adding new Pokemon to an owner’s Pokedex.
 */
void addPokemon(OwnerNode *owner);

/**
 * @brief Recursive Helper to free a Pokemon by ID in the BST.
 * @param pokemonInTree pointer to the current PokemonNode
 * @param IDToRelease ID to remove
 * Why we made it: Recursive helper for freeing a Pokemon by ID.
 */
void freePokemonHelper(PokemonNode **pokemonInTree, int IDToRelease);

/**
 * @brief Prompt for ID, remove that Pokemon from BST by ID.
 * @param owner pointer to the Owner
 * Why we made it: Another user function for releasing a Pokemon.
 */
void freePokemon(OwnerNode *owner);

/* ------------------------------------------------------------
   7) Display Menu for a Pokedex
   ------------------------------------------------------------ */

/**
 * @brief Show sub-menu to let user pick BFS, Pre, In, Post, or alphabetical.
 * @param owner pointer to Owner
 * Why we made it: We want a simple menu that picks from various traversals.
 */
void displayMenu(OwnerNode *owner);

/* ------------------------------------------------------------
   8) Sorting Owners (Bubble Sort on Circular List)
   ------------------------------------------------------------ */

/**
 * @brief Sort the circular owners list by name.
 * Why we made it: Another demonstration of pointer manipulation + sorting logic.
 */
void sortOwners(void);

/**
 * @brief Helper to swap name & pokedexRoot in two OwnerNode.
 * @param a pointer to first owner
 * @param b pointer to second owner
 * Why we made it: Used internally by bubble sort to swap data.
 */
void swapOwnerData(OwnerNode *a, OwnerNode *b);

/* ------------------------------------------------------------
   9) Circular List Linking & Searching
   ------------------------------------------------------------ */

/**
 * @brief Insert a new owner into the circular list. If none exist, it's alone.
 * @param newOwner pointer to newly created OwnerNode
 * Why we made it: We need a standard approach to keep the list circular.
 */
void linkOwnerInCircularList(OwnerNode *newOwner);

/**
 * @brief Remove a specific OwnerNode from the circular list, possibly updating head.
 * @param target pointer to the OwnerNode
 * Why we made it: Deleting or merging owners requires removing them from the ring.
 */
void removeOwnerFromCircularList(OwnerNode *target);

/**
 * @brief Find an owner by name in the circular list.
 * @param name string to match
 * @return pointer to the matching OwnerNode or NULL
 * Why we made it: We often need to locate an owner quickly.
 */
OwnerNode *findOwnerByName(const char *name);

/* ------------------------------------------------------------
   10) Owner Menus
   ------------------------------------------------------------ */

/**
 * @brief Let user pick an existing Pokedex (owner) by number, then sub-menu.
 * Why we made it: This is the main interface for adding/fighting/evolving, etc.
 */
void enterExistingPokedexMenu(void);

/**
 * @brief Creates a new Pokedex (prompt for name, check uniqueness, choose starter).
 * Why we made it: The main entry for building a brand-new Pokedex.
 */
void openPokedexMenu(void);

/**
 * @brief Delete an entire Pokedex (owner) from the list.
 * Why we made it: Let user pick which Pokedex to remove and free everything.
 */
void deletePokedex(void);

/**
 * @brief Merge the second owner's Pokedex into the first, then remove the second owner.
 * Why we made it: BFS copy demonstration plus removing an owner.
 */
void mergePokedexMenu(void);

/* ------------------------------------------------------------
   11) Printing Owners in a Circle
   ------------------------------------------------------------ */

/**
 * @brief Print owners left or right from head, repeating as many times as user wants.
 * Why we made it: Demonstrates stepping through a circular list in a chosen direction.
 */
void printOwnersCircular(void);

/* ------------------------------------------------------------
   11.b) Printing All Owners
   ------------------------------------------------------------ */
/**
 * @brief Print all owners from first element to last element.
 * Why we made it: Utilized for printing all Owner options for menus
 */
void printAllOwners(void);

/* ------------------------------------------------------------
   12) Cleanup All Owners at Program End
   ------------------------------------------------------------ */

/**
 * @brief Frees every remaining owner in the circular list, setting ownerHead = NULL.
 * Why we made it: Ensures a squeaky-clean exit with no leftover memory.
 */
void freeAllOwners(void);

/* ------------------------------------------------------------
   13) The Main Menu
   ------------------------------------------------------------ */

/**
 * @brief The main driver loop for the program (new pokedex, merge, fight, etc.).
 * Why we made it: Our top-level UI that keeps the user engaged until they exit.
 */
void mainMenu(void);

// Array of Pokemon data
static const PokemonData pokedex[] = {
    {1, "Bulbasaur", GRASS, 45, 49, CAN_EVOLVE},
    {2, "Ivysaur", GRASS, 60, 62, CAN_EVOLVE},
    {3, "Venusaur", GRASS, 80, 82, CANNOT_EVOLVE},
    {4, "Charmander", FIRE, 39, 52, CAN_EVOLVE},
    {5, "Charmeleon", FIRE, 58, 64, CAN_EVOLVE},
    {6, "Charizard", FIRE, 78, 84, CANNOT_EVOLVE},
    {7, "Squirtle", WATER, 44, 48, CAN_EVOLVE},
    {8, "Wartortle", WATER, 59, 63, CAN_EVOLVE},
    {9, "Blastoise", WATER, 79, 83, CANNOT_EVOLVE},
    {10, "Caterpie", BUG, 45, 30, CAN_EVOLVE},
    {11, "Metapod", BUG, 50, 20, CAN_EVOLVE},
    {12, "Butterfree", BUG, 60, 45, CANNOT_EVOLVE},
    {13, "Weedle", BUG, 40, 35, CAN_EVOLVE},
    {14, "Kakuna", BUG, 45, 25, CAN_EVOLVE},
    {15, "Beedrill", BUG, 65, 90, CANNOT_EVOLVE},
    {16, "Pidgey", NORMAL, 40, 45, CAN_EVOLVE},
    {17, "Pidgeotto", NORMAL, 63, 60, CAN_EVOLVE},
    {18, "Pidgeot", NORMAL, 83, 80, CANNOT_EVOLVE},
    {19, "Rattata", NORMAL, 30, 56, CAN_EVOLVE},
    {20, "Raticate", NORMAL, 55, 81, CANNOT_EVOLVE},
    {21, "Spearow", NORMAL, 40, 60, CAN_EVOLVE},
    {22, "Fearow", NORMAL, 65, 90, CANNOT_EVOLVE},
    {23, "Ekans", POISON, 35, 60, CAN_EVOLVE},
    {24, "Arbok", POISON, 60, 85, CANNOT_EVOLVE},
    {25, "Pikachu", ELECTRIC, 35, 55, CAN_EVOLVE},
    {26, "Raichu", ELECTRIC, 60, 90, CANNOT_EVOLVE},
    {27, "Sandshrew", GROUND, 50, 75, CAN_EVOLVE},
    {28, "Sandslash", GROUND, 75, 100, CANNOT_EVOLVE},
    {29, "NidoranF", POISON, 55, 47, CAN_EVOLVE},
    {30, "Nidorina", POISON, 70, 62, CAN_EVOLVE},
    {31, "Nidoqueen", POISON, 90, 92, CANNOT_EVOLVE},
    {32, "NidoranM", POISON, 46, 57, CAN_EVOLVE},
    {33, "Nidorino", POISON, 61, 72, CAN_EVOLVE},
    {34, "Nidoking", POISON, 81, 102, CANNOT_EVOLVE},
    {35, "Clefairy", FAIRY, 70, 45, CAN_EVOLVE},
    {36, "Clefable", FAIRY, 95, 70, CANNOT_EVOLVE},
    {37, "Vulpix", FIRE, 38, 41, CAN_EVOLVE},
    {38, "Ninetales", FIRE, 73, 76, CANNOT_EVOLVE},
    {39, "Jigglypuff", NORMAL, 115, 45, CAN_EVOLVE},
    {40, "Wigglytuff", NORMAL, 140, 70, CANNOT_EVOLVE},
    {41, "Zubat", POISON, 40, 45, CAN_EVOLVE},
    {42, "Golbat", POISON, 75, 80, CAN_EVOLVE},
    {43, "Oddish", GRASS, 45, 50, CAN_EVOLVE},
    {44, "Gloom", GRASS, 60, 65, CAN_EVOLVE},
    {45, "Vileplume", GRASS, 75, 80, CANNOT_EVOLVE},
    {46, "Paras", BUG, 35, 70, CAN_EVOLVE},
    {47, "Parasect", BUG, 60, 95, CANNOT_EVOLVE},
    {48, "Venonat", BUG, 60, 55, CAN_EVOLVE},
    {49, "Venomoth", BUG, 70, 65, CANNOT_EVOLVE},
    {50, "Diglett", GROUND, 10, 55, CAN_EVOLVE},
    {51, "Dugtrio", GROUND, 35, 80, CANNOT_EVOLVE},
    {52, "Meowth", NORMAL, 40, 45, CAN_EVOLVE},
    {53, "Persian", NORMAL, 65, 70, CANNOT_EVOLVE},
    {54, "Psyduck", WATER, 50, 52, CAN_EVOLVE},
    {55, "Golduck", WATER, 80, 82, CANNOT_EVOLVE},
    {56, "Mankey", FIGHTING, 40, 80, CAN_EVOLVE},
    {57, "Primeape", FIGHTING, 65, 105, CANNOT_EVOLVE},
    {58, "Growlithe", FIRE, 55, 70, CAN_EVOLVE},
    {59, "Arcanine", FIRE, 90, 110, CANNOT_EVOLVE},
    {60, "Poliwag", WATER, 40, 50, CAN_EVOLVE},
    {61, "Poliwhirl", WATER, 65, 65, CAN_EVOLVE},
    {62, "Poliwrath", WATER, 90, 95, CANNOT_EVOLVE},
    {63, "Abra", PSYCHIC, 25, 20, CAN_EVOLVE},
    {64, "Kadabra", PSYCHIC, 40, 35, CAN_EVOLVE},
    {65, "Alakazam", PSYCHIC, 55, 50, CANNOT_EVOLVE},
    {66, "Machop", FIGHTING, 70, 80, CAN_EVOLVE},
    {67, "Machoke", FIGHTING, 80, 100, CAN_EVOLVE},
    {68, "Machamp", FIGHTING, 90, 130, CANNOT_EVOLVE},
    {69, "Bellsprout", GRASS, 50, 75, CAN_EVOLVE},
    {70, "Weepinbell", GRASS, 65, 90, CAN_EVOLVE},
    {71, "Victreebel", GRASS, 80, 105, CANNOT_EVOLVE},
    {72, "Tentacool", WATER, 40, 40, CAN_EVOLVE},
    {73, "Tentacruel", WATER, 80, 70, CANNOT_EVOLVE},
    {74, "Geodude", ROCK, 40, 80, CAN_EVOLVE},
    {75, "Graveler", ROCK, 55, 95, CAN_EVOLVE},
    {76, "Golem", ROCK, 80, 120, CANNOT_EVOLVE},
    {77, "Ponyta", FIRE, 50, 85, CAN_EVOLVE},
    {78, "Rapidash", FIRE, 65, 100, CANNOT_EVOLVE},
    {79, "Slowpoke", WATER, 90, 65, CAN_EVOLVE},
    {80, "Slowbro", WATER, 95, 75, CANNOT_EVOLVE},
    {81, "Magnemite", ELECTRIC, 25, 35, CAN_EVOLVE},
    {82, "Magneton", ELECTRIC, 50, 60, CANNOT_EVOLVE},
    {83, "Farfetch'd", NORMAL, 52, 65, CANNOT_EVOLVE},
    {84, "Doduo", NORMAL, 35, 85, CAN_EVOLVE},
    {85, "Dodrio", NORMAL, 60, 110, CANNOT_EVOLVE},
    {86, "Seel", WATER, 65, 45, CAN_EVOLVE},
    {87, "Dewgong", WATER, 90, 70, CANNOT_EVOLVE},
    {88, "Grimer", POISON, 80, 80, CAN_EVOLVE},
    {89, "Muk", POISON, 105, 105, CANNOT_EVOLVE},
    {90, "Shellder", WATER, 30, 65, CAN_EVOLVE},
    {91, "Cloyster", WATER, 50, 95, CANNOT_EVOLVE},
    {92, "Gastly", GHOST, 30, 35, CAN_EVOLVE},
    {93, "Haunter", GHOST, 45, 50, CAN_EVOLVE},
    {94, "Gengar", GHOST, 60, 65, CANNOT_EVOLVE},
    {95, "Onix", ROCK, 35, 45, CANNOT_EVOLVE},
    {96, "Drowzee", PSYCHIC, 60, 48, CAN_EVOLVE},
    {97, "Hypno", PSYCHIC, 85, 73, CANNOT_EVOLVE},
    {98, "Krabby", WATER, 30, 105, CAN_EVOLVE},
    {99, "Kingler", WATER, 55, 130, CANNOT_EVOLVE},
    {100, "Voltorb", ELECTRIC, 40, 30, CAN_EVOLVE},
    {101, "Electrode", ELECTRIC, 60, 50, CANNOT_EVOLVE},
    {102, "Exeggcute", GRASS, 60, 40, CAN_EVOLVE},
    {103, "Exeggutor", GRASS, 95, 95, CANNOT_EVOLVE},
    {104, "Cubone", GROUND, 50, 50, CAN_EVOLVE},
    {105, "Marowak", GROUND, 60, 80, CANNOT_EVOLVE},
    {106, "Hitmonlee", FIGHTING, 50, 120, CANNOT_EVOLVE},
    {107, "Hitmonchan", FIGHTING, 50, 105, CANNOT_EVOLVE},
    {108, "Lickitung", NORMAL, 90, 55, CANNOT_EVOLVE},
    {109, "Koffing", POISON, 40, 65, CAN_EVOLVE},
    {110, "Weezing", POISON, 65, 90, CANNOT_EVOLVE},
    {111, "Rhyhorn", GROUND, 80, 85, CAN_EVOLVE},
    {112, "Rhydon", GROUND, 105, 130, CANNOT_EVOLVE},
    {113, "Chansey", NORMAL, 250, 5, CANNOT_EVOLVE},
    {114, "Tangela", GRASS, 65, 55, CANNOT_EVOLVE},
    {115, "Kangaskhan", NORMAL, 105, 95, CANNOT_EVOLVE},
    {116, "Horsea", WATER, 30, 40, CAN_EVOLVE},
    {117, "Seadra", WATER, 55, 65, CANNOT_EVOLVE},
    {118, "Goldeen", WATER, 45, 67, CAN_EVOLVE},
    {119, "Seaking", WATER, 80, 92, CANNOT_EVOLVE},
    {120, "Staryu", WATER, 30, 45, CAN_EVOLVE},
    {121, "Starmie", WATER, 60, 75, CANNOT_EVOLVE},
    {122, "Mr. Mime", PSYCHIC, 40, 45, CANNOT_EVOLVE},
    {123, "Scyther", BUG, 70, 110, CANNOT_EVOLVE},
    {124, "Jynx", ICE, 65, 50, CANNOT_EVOLVE},
    {125, "Electabuzz", ELECTRIC, 65, 83, CANNOT_EVOLVE},
    {126, "Magmar", FIRE, 65, 95, CANNOT_EVOLVE},
    {127, "Pinsir", BUG, 65, 125, CANNOT_EVOLVE},
    {128, "Tauros", NORMAL, 75, 100, CANNOT_EVOLVE},
    {129, "Magikarp", WATER, 20, 10, CAN_EVOLVE},
    {130, "Gyarados", WATER, 95, 125, CANNOT_EVOLVE},
    {131, "Lapras", WATER, 130, 85, CANNOT_EVOLVE},
    {132, "Ditto", NORMAL, 48, 48, CANNOT_EVOLVE},
    {133, "Eevee", NORMAL, 55, 55, CAN_EVOLVE},
    {134, "Vaporeon", WATER, 130, 65, CANNOT_EVOLVE},
    {135, "Jolteon", ELECTRIC, 65, 65, CANNOT_EVOLVE},
    {136, "Flareon", FIRE, 65, 130, CANNOT_EVOLVE},
    {137, "Porygon", NORMAL, 65, 60, CANNOT_EVOLVE},
    {138, "Omanyte", ROCK, 35, 40, CAN_EVOLVE},
    {139, "Omastar", ROCK, 70, 60, CANNOT_EVOLVE},
    {140, "Kabuto", ROCK, 30, 80, CAN_EVOLVE},
    {141, "Kabutops", ROCK, 60, 115, CANNOT_EVOLVE},
    {142, "Aerodactyl", ROCK, 80, 105, CANNOT_EVOLVE},
    {143, "Snorlax", NORMAL, 160, 110, CANNOT_EVOLVE},
    {144, "Articuno", ICE, 90, 85, CANNOT_EVOLVE},
    {145, "Zapdos", ELECTRIC, 90, 90, CANNOT_EVOLVE},
    {146, "Moltres", FIRE, 90, 100, CANNOT_EVOLVE},
    {147, "Dratini", DRAGON, 41, 64, CAN_EVOLVE},
    {148, "Dragonair", DRAGON, 61, 84, CAN_EVOLVE},
    {149, "Dragonite", DRAGON, 91, 134, CANNOT_EVOLVE},
    {150, "Mewtwo", PSYCHIC, 106, 110, CANNOT_EVOLVE},
    {151, "Mew", PSYCHIC, 100, 100, CANNOT_EVOLVE}};

#endif // EX6_H
