/* === include/recipe.h === */
#ifndef RECIPE_H
#define RECIPE_H

#include "order.h"

#define MAX_RECIPE_ITEMS 20
#define MAX_RECIPES      10

typedef struct {
    int ingredientID;
    float quantity;
} RecipeItem;

typedef struct {
    int menuItemID;
    RecipeItem items[MAX_RECIPE_ITEMS];
    int itemCount;
} Recipe;

extern Recipe recipes[MAX_RECIPES];
extern int recipeCount;

typedef struct {
    int ingredientID;
    char name[50];
    char unit[8];
    float quantity;
    float costPerServing;
} RecipeIngredientInfo;

/* ------- Management ------- */

int recipe_set(int menuItemID, const RecipeItem *items, int itemCount);
int recipe_remove(int menuItemID);
const Recipe* recipe_get(int menuItemID);
int recipe_exists(int menuItemID);
int recipe_getIngredientInfo(int menuItemID, RecipeIngredientInfo *out, int maxCount);

/* ------- Cost ------- */

float recipe_calculateCostPerServing(int menuItemID);
float recipe_calculateCostForOrder(const Order *order);

/* ------- Stock ------- */

#define SUGAR_NONE 0
#define SUGAR_LESS 1
#define SUGAR_NORM 2
#define SUGAR_EXTRA 3

#define ICE_NONE 0
#define ICE_LESS 1
#define ICE_NORM 2
#define ICE_EXTRA 3

typedef struct {
    int sugarOption;
    int iceOption;
} DrinkCustomization;

int  recipe_checkAvailabilityForOrder(const Order *order, int *missingIDs, int maxMissing);
void recipe_consumeIngredientsForOrder(const Order *order);

/* Customization*/
int recipe_applyCustomization(int menuItemID, const DrinkCustomization *custom,
                               RecipeItem *outItems, int maxItems);

/* Check availability / consume using a pre-built ingredient list */
int  recipe_checkStock(const RecipeItem *items, int itemCount, int quantity,
                        int *missingIDs, int maxMissing);
void recipe_consumeStock(const RecipeItem *items, int itemCount, int quantity);

/* ------- Display ------- */

void recipe_display(int menuItemID);
void recipe_viewAll(void);
void recipe_manageMenu(void);

/* ------- Admin workflows ------- */

void recipe_createFlow(void);
void recipe_editFlow(void);
void recipe_deleteFlow(void);

/* ------- Lifecycle ------- */

void recipe_reset(void);
void initializeDefaultRecipes(void);

#endif /* RECIPE_H */
