/* === include/ingredient.h === */
#ifndef INGREDIENT_H
#define INGREDIENT_H

#define MAX_INGREDIENTS        30
#define INGREDIENT_NAME_BUFFER 50

typedef struct {
    int ingredientID;
    char name[INGREDIENT_NAME_BUFFER];
    char unit[8];          /* "g", "mL", "piece" */
    float purchasePrice;    /* Purchase price per unit */
    int currentStock;     /* Current stock quantity */
    int minStock;         /* Minimum stock threshold */
} Ingredient;

extern Ingredient ingredients[MAX_INGREDIENTS];
extern int ingredientCount;
extern int nextIngredientID;

void initializeIngredients(void);
void viewIngredients(void);
void addIngredient(void);
void updateIngredient(void);
void deleteIngredient(void);
int findIngredientByID(int id);
int isValidIngredientName(const char *name);
int isValidUnit(const char *unit);

/* Stock operations (used by recipe system) */
void ingredient_reduceStock(int ingredientID, int quantity);
int ingredient_hasStock(int ingredientID, int quantity);
void restockIngredient(void);

#endif /* INGREDIENT_H */
