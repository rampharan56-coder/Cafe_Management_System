/* === src/recipe.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "recipe.h"
#include "menu.h"
#include "ingredient.h"
#include "order.h"
#include "fileio.h"
#include "utils.h"

Recipe recipes[MAX_RECIPES];
int recipeCount = 0;

/* Find a recipe by its menu item ID */
static int findRecipeByMenuItemID(int menuItemID)
{
    for (int i = 0; i < recipeCount; i++) {
        if (recipes[i].menuItemID == menuItemID)
            return i;
    }
    return -1;
}

/* ------- Management ------- */

int recipe_set(int menuItemID, const RecipeItem *items, int itemCount)
{
    if (itemCount < 0 || itemCount > MAX_RECIPE_ITEMS)
        return -1;

    int idx = findRecipeByMenuItemID(menuItemID);

    if (idx == -1) {
        /* New recipe */
        if (recipeCount >= MAX_RECIPES)
            return -1;
        idx = recipeCount;
        recipes[idx].menuItemID = menuItemID;
        recipes[idx].itemCount = 0;
        recipeCount++;
    }

    recipes[idx].itemCount = itemCount;
    for (int i = 0; i < itemCount; i++)
        recipes[idx].items[i] = items[i];

    saveRecipes("data/recipes.txt");
    return 0;
}

int recipe_remove(int menuItemID)
{
    int idx = findRecipeByMenuItemID(menuItemID);
    if (idx == -1)
        return -1;

    /* Compact the array by shifting left */
    for (int i = idx; i < recipeCount - 1; i++)
        recipes[i] = recipes[i + 1];
    recipeCount--;

    saveRecipes("data/recipes.txt");
    return 0;
}

const Recipe* recipe_get(int menuItemID)
{
    int idx = findRecipeByMenuItemID(menuItemID);
    if (idx == -1)
        return NULL;
    return &recipes[idx];
}

int recipe_exists(int menuItemID)
{
    return findRecipeByMenuItemID(menuItemID) != -1;
}

int recipe_getIngredientInfo(int menuItemID, RecipeIngredientInfo *out, int maxCount)
{
    int idx = findRecipeByMenuItemID(menuItemID);
    if (idx == -1)
        return 0;

    int count = recipes[idx].itemCount;
    if (count > maxCount)
        count = maxCount;

    for (int i = 0; i < count; i++) {
        int ingID = recipes[idx].items[i].ingredientID;
        int ingIdx = findIngredientByID(ingID);

        out[i].ingredientID = ingID;
        out[i].quantity = recipes[idx].items[i].quantity;

        if (ingIdx != -1) {
            strncpy(out[i].name, ingredients[ingIdx].name, sizeof(out[i].name) - 1);
            out[i].name[sizeof(out[i].name) - 1] = '\0';
            strncpy(out[i].unit, ingredients[ingIdx].unit, sizeof(out[i].unit) - 1);
            out[i].unit[sizeof(out[i].unit) - 1] = '\0';
            out[i].costPerServing = recipes[idx].items[i].quantity * ingredients[ingIdx].purchasePrice;
        } else {
            out[i].name[0] = '\0';
            out[i].unit[0] = '\0';
            out[i].costPerServing = 0.0f;
        }
    }

    return count;
}

/* ------- Cost ------- */

float recipe_calculateCostPerServing(int menuItemID)
{
    RecipeIngredientInfo info[MAX_RECIPE_ITEMS];
    int count = recipe_getIngredientInfo(menuItemID, info, MAX_RECIPE_ITEMS);

    float total = 0.0f;
    for (int i = 0; i < count; i++)
        total += info[i].costPerServing;

    return total;
}

float recipe_calculateCostForOrder(const Order *order)
{
    float total = 0.0f;

    for (int i = 0; i < order->itemCount; i++) {
        float perServing = recipe_calculateCostPerServing(order->items[i].itemID);
        total += perServing * order->items[i].quantity;
    }

    return total;
}

/* ------- Stock ------- */

int recipe_checkAvailabilityForOrder(const Order *order, int *missingIDs, int maxMissing)
{
    int missingCount = 0;

    for (int i = 0; i < order->itemCount; i++) {
        int idx = findRecipeByMenuItemID(order->items[i].itemID);
        if (idx == -1)
            continue;

        for (int j = 0; j < recipes[idx].itemCount; j++) {
            int ingID = recipes[idx].items[j].ingredientID;
            int required = (int)(recipes[idx].items[j].quantity * order->items[i].quantity);

            if (!ingredient_hasStock(ingID, required)) {
                if (missingIDs != NULL && missingCount < maxMissing)
                    missingIDs[missingCount] = ingID;
                missingCount++;
            }
        }
    }

    return missingCount;
}

void recipe_consumeIngredientsForOrder(const Order *order)
{
    for (int i = 0; i < order->itemCount; i++) {
        int idx = findRecipeByMenuItemID(order->items[i].itemID);
        if (idx == -1)
            continue;

        for (int j = 0; j < recipes[idx].itemCount; j++) {
            int ingID = recipes[idx].items[j].ingredientID;
            int required = (int)(recipes[idx].items[j].quantity * order->items[i].quantity);
            ingredient_reduceStock(ingID, required);
        }
    }
}

/* ------- Customization ------- */

int recipe_applyCustomization(int menuItemID, const DrinkCustomization *custom,
                               RecipeItem *outItems, int maxItems)
{
    const Recipe *r = recipe_get(menuItemID);
    if (!r) return 0;
    if (r->itemCount > maxItems) return 0;

    for (int i = 0; i < r->itemCount; i++)
        outItems[i] = r->items[i];
    int count = r->itemCount;

    /* Apply sugar adjustment */
    if (custom->sugarOption >= 0) {
        for (int i = 0; i < count; i++) {
            int idx = findIngredientByID(outItems[i].ingredientID);
            if (idx != -1 && strcmp(ingredients[idx].name, "Sugar") == 0) {
                switch (custom->sugarOption) {
                    case 0: outItems[i].quantity = 0; break;
                    case 1: outItems[i].quantity *= 0.5f; break;
                    case 2: break;
                    case 3: outItems[i].quantity *= 1.5f; break;
                }
                break;
            }
        }
    }

    /* Apply ice adjustment */
    if (custom->iceOption >= 0) {
        for (int i = 0; i < count; i++) {
            int idx = findIngredientByID(outItems[i].ingredientID);
            if (idx != -1 && strcmp(ingredients[idx].name, "Ice") == 0) {
                switch (custom->iceOption) {
                    case 0: outItems[i].quantity = 0; break;
                    case 1: outItems[i].quantity *= 0.5f; break;
                    case 2: break;
                    case 3: outItems[i].quantity *= 1.5f; break;
                }
                break;
            }
        }
    }

    return count;
}

int recipe_checkStock(const RecipeItem *items, int itemCount, int quantity,
                       int *missingIDs, int maxMissing)
{
    int missingCount = 0;

    for (int i = 0; i < itemCount; i++) {
        int required = (int)(items[i].quantity * quantity);
        if (!ingredient_hasStock(items[i].ingredientID, required)) {
            if (missingIDs != NULL && missingCount < maxMissing)
                missingIDs[missingCount] = items[i].ingredientID;
            missingCount++;
        }
    }

    return missingCount;
}

void recipe_consumeStock(const RecipeItem *items, int itemCount, int quantity)
{
    for (int i = 0; i < itemCount; i++) {
        int required = (int)(items[i].quantity * quantity);
        ingredient_reduceStock(items[i].ingredientID, required);
    }
}

/* ------- Display ------- */

void recipe_display(int menuItemID)
{
    RecipeIngredientInfo info[MAX_RECIPE_ITEMS];
    int count = recipe_getIngredientInfo(menuItemID, info, MAX_RECIPE_ITEMS);

    if (count == 0) {
        printf("  No recipe defined for this item.\n");
        return;
    }

    printf("  Recipe Ingredients:\n");
    printf("  -------------------------------------\n");

    float totalCost = 0.0f;
    for (int i = 0; i < count; i++) {
        printf("  %-25s %6.1f %-6s  $%.2f\n",
               info[i].name,
               (double)info[i].quantity,
               info[i].unit,
               (double)info[i].costPerServing);
        totalCost += info[i].costPerServing;
    }

    printf("  -------------------------------------\n");
    printf("  Total ingredient cost:        $%.2f\n", (double)totalCost);
}

/* ------- Lifecycle ------- */

void recipe_reset(void)
{
    recipeCount = 0;
    loadRecipes("data/recipes.txt");
}

void initializeDefaultRecipes(void)
{
    /* Americano (ID 1): Coffee Beans 18g */
    {
        RecipeItem items[1];
        items[0].ingredientID = 1; items[0].quantity = 18.0f;
        recipe_set(1, items, 1);
    }

    /* Espresso (ID 2): Coffee Beans 18g */
    {
        RecipeItem items[1];
        items[0].ingredientID = 1; items[0].quantity = 18.0f;
        recipe_set(2, items, 1);
    }

    /* Cappuccino (ID 3): Coffee Beans 18g, Milk 120mL, Cream 20mL */
    {
        RecipeItem items[3];
        items[0].ingredientID = 1; items[0].quantity = 18.0f;
        items[1].ingredientID = 2; items[1].quantity = 120.0f;
        items[2].ingredientID = 8; items[2].quantity = 20.0f;
        recipe_set(3, items, 3);
    }

    /* Mocha (ID 5): Coffee Beans 18g, Milk 150mL, Choc Syrup 30mL, Sugar 8g */
    {
        RecipeItem items[4];
        items[0].ingredientID = 1; items[0].quantity = 18.0f;
        items[1].ingredientID = 2; items[1].quantity = 150.0f;
        items[2].ingredientID = 4; items[2].quantity = 30.0f;
        items[3].ingredientID = 3; items[3].quantity = 8.0f;
        recipe_set(5, items, 4);
    }

    /* Matcha Latte (ID 6): Matcha Powder 10g, Milk 180mL, Sugar 8g */
    {
        RecipeItem items[3];
        items[0].ingredientID = 6; items[0].quantity = 10.0f;
        items[1].ingredientID = 2; items[1].quantity = 180.0f;
        items[2].ingredientID = 3; items[2].quantity = 8.0f;
        recipe_set(6, items, 3);
    }

    /* Coffee Milk (ID 7): Coffee Beans 18g, Milk 150mL, Sugar 5g */
    {
        RecipeItem items[3];
        items[0].ingredientID = 1; items[0].quantity = 18.0f;
        items[1].ingredientID = 2; items[1].quantity = 150.0f;
        items[2].ingredientID = 3; items[2].quantity = 5.0f;
        recipe_set(7, items, 3);
    }

    /* Latte (ID 4): Coffee Beans 18g, Milk 180mL */
    {
        RecipeItem items[2];
        items[0].ingredientID = 1; items[0].quantity = 18.0f;
        items[1].ingredientID = 2; items[1].quantity = 180.0f;
        recipe_set(4, items, 2);
    }
}

/* Display available ingredients for selection */
static void listIngredientsForRecipe(void)
{
    printf("\nAvailable Ingredients:\n");
    printf(" ID  %-24s %-6s %-8s %-8s\n", "Name", "Unit", "Price", "Stock");
    printf(" -----------------------------------------------\n");
    for (int i = 0; i < ingredientCount; i++) {
        printf(" [%2d] %-24s %-6s $%-7.4f %-8d\n",
               ingredients[i].ingredientID,
               ingredients[i].name,
               ingredients[i].unit,
               (double)ingredients[i].purchasePrice,
               ingredients[i].currentStock);
    }
}

/* Shared ingredient selection loop for create and edit */
static int buildRecipeFromInput(RecipeItem *items, int maxItems)
{
    int count = 0;

    while (count < maxItems) {
        listIngredientsForRecipe();

        int ingID = readInt("\n> enter ingredient ID (0 to finish): ", 0, 99999);
        if (ingID == 0)
            break;

        // Validate ingredient exists
        if (findIngredientByID(ingID) == -1) {
            printf("  Error: No ingredient with ID %d.\n", ingID);
            continue;
        }

        // Check for duplicate
        int dup = 0;
        for (int i = 0; i < count; i++) {
            if (items[i].ingredientID == ingID) {
                printf("  Error: Ingredient already added to this recipe.\n");
                dup = 1;
                break;
            }
        }
        if (dup)
            continue;

        float qty = readFloat("> enter quantity: ", 0.01f, 99999.0f);

        items[count].ingredientID = ingID;
        items[count].quantity = qty;
        count++;

        printf("  Added. (%d / %d ingredients)\n", count, maxItems);
    }

    return count;
}

void recipe_viewAll(void)
{
    CLEAR_SCREEN();
    printf("================ RECIPES ================\n");
    printf("=========================================\n");

    if (menuCount == 0) {
        printf("  No menu items available.\n");
        waitKey(NULL);
        return;
    }

    for (int i = 0; i < menuCount; i++) {
        int id = menuItems[i].itemID;
        printf("\nItem ID %d: %s  ($%.2f)\n", id, menuItems[i].itemName,
               (double)menuItems[i].price);

        if (!recipe_exists(id)) {
            printf("  No recipe defined.\n");
            continue;
        }

        RecipeIngredientInfo info[MAX_RECIPE_ITEMS];
        int n = recipe_getIngredientInfo(id, info, MAX_RECIPE_ITEMS);
        float totalCost = 0.0f;

        printf("  %-24s %6s %8s\n", "Ingredient", "Qty", "Cost");
        printf("  ---------------------------------------\n");
        for (int j = 0; j < n; j++) {
            printf("  %-24s %5.1f %-4s $%7.4f\n",
                   info[j].name,
                   (double)info[j].quantity,
                   info[j].unit,
                   (double)info[j].costPerServing);
            totalCost += info[j].costPerServing;
        }
        printf("  ---------------------------------------\n");
        printf("  Ingredient cost: $%.4f | Sell price: $%.2f | Profit: $%.2f\n",
               totalCost, (double)menuItems[i].price,
               (double)(menuItems[i].price - totalCost));
    }

    waitKey(NULL);
}

void recipe_createFlow(void)
{
    CLEAR_SCREEN();
    printf("========== CREATE RECIPE ==========\n");

    if (menuCount == 0) {
        printf("  No menu items available.\n");
        waitKey(NULL);
        return;
    }

    // Choose menu item
    viewMenuAdmin();

    int menuID = readInt("\n> enter menu item ID to create recipe for (0 to cancel): ", 0, 99999);
    if (menuID == 0) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    if (findItemByID(menuID) == -1) {
        printf("  Error: No menu item with ID %d.\n", menuID);
        waitKey(NULL);
        return;
    }

    if (recipe_exists(menuID)) {
        int yn = readYesNo("  Recipe already exists. Replace it? (Y/N): ");
        if (yn != 1) {
            printf("Canceled.\n");
            waitKey(NULL);
            return;
        }
    }

    if (ingredientCount == 0) {
        printf("  Error: No ingredients available. Add ingredients first.\n");
        waitKey(NULL);
        return;
    }

    // Build recipe
    RecipeItem items[MAX_RECIPE_ITEMS];
    int count = buildRecipeFromInput(items, MAX_RECIPE_ITEMS);

    if (count == 0) {
        printf("No ingredients added. Recipe not saved.\n");
        waitKey(NULL);
        return;
    }

    int r = recipe_set(menuID, items, count);
    if (r == 0) {
        printf("\nRecipe saved for '%s' (%d ingredients, cost $%.4f).\n",
               menuItems[findItemByID(menuID)].itemName, count,
               (double)recipe_calculateCostPerServing(menuID));
    } else {
        printf("  Error: Could not save recipe.\n");
    }

    waitKey(NULL);
}

void recipe_editFlow(void)
{
    CLEAR_SCREEN();
    printf("========== EDIT RECIPE ==========\n");

    if (recipeCount == 0) {
        printf("  No recipes to edit.\n");
        waitKey(NULL);
        return;
    }

    // Show items that have recipes
    printf("Items with recipes:\n");
    for (int i = 0; i < menuCount; i++) {
        int id = menuItems[i].itemID;
        if (recipe_exists(id)) {
            printf("  [ID %d] %s\n", id, menuItems[i].itemName);
        }
    }

    int menuID = readInt("\n> enter menu item ID to edit recipe for (0 to cancel): ", 0, 99999);
    if (menuID == 0) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    if (!recipe_exists(menuID)) {
        printf("  Error: No recipe exists for item ID %d.\n", menuID);
        waitKey(NULL);
        return;
    }

    // Show current recipe
    printf("\nCurrent recipe for '%s':\n", menuItems[findItemByID(menuID)].itemName);
    recipe_display(menuID);

    int yn = readYesNo("\nReplace this recipe with a new one? (Y/N): ");
    if (yn != 1) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    if (ingredientCount == 0) {
        printf("  Error: No ingredients available.\n");
        waitKey(NULL);
        return;
    }

    // Build new recipe
    RecipeItem items[MAX_RECIPE_ITEMS];
    int count = buildRecipeFromInput(items, MAX_RECIPE_ITEMS);

    if (count == 0) {
        printf("No ingredients added. Recipe not saved.\n");
        waitKey(NULL);
        return;
    }

    int r = recipe_set(menuID, items, count);
    if (r == 0) {
        printf("\nRecipe updated for '%s' (%d ingredients, cost $%.4f).\n",
               menuItems[findItemByID(menuID)].itemName, count,
               (double)recipe_calculateCostPerServing(menuID));
    } else {
        printf("  Error: Could not update recipe.\n");
    }

    waitKey(NULL);
}

void recipe_deleteFlow(void)
{
    CLEAR_SCREEN();
    printf("========== DELETE RECIPE ==========\n");

    if (recipeCount == 0) {
        printf("  No recipes to delete.\n");
        waitKey(NULL);
        return;
    }

    // Show items with recipes
    printf("Items with recipes:\n");
    for (int i = 0; i < menuCount; i++) {
        int id = menuItems[i].itemID;
        if (recipe_exists(id)) {
            printf("  [ID %d] %s\n", id, menuItems[i].itemName);
        }
    }

    int menuID = readInt("\n> enter menu item ID to delete recipe for (0 to cancel): ", 0, 99999);
    if (menuID == 0) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    if (!recipe_exists(menuID)) {
        printf("  Error: No recipe exists for item ID %d.\n", menuID);
        waitKey(NULL);
        return;
    }

    // Show the recipe before confirming
    printf("\nRecipe to delete:\n");
    recipe_display(menuID);

    int yn = readYesNo("\nDelete this recipe? (Y/N): ");
    if (yn != 1) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    int r = recipe_remove(menuID);
    if (r == 0)
        printf("\nRecipe deleted successfully.\n");
    else
        printf("  Error: Could not delete recipe.\n");

    waitKey(NULL);
}

void recipe_manageMenu(void)
{
    while (1) {
        CLEAR_SCREEN();
        printf("============= RECIPE MANAGEMENT =============\n");
        printf("=============================================\n");
        printf("[1]. VIEW RECIPES\n");
        printf("[2]. CREATE RECIPE\n");
        printf("[3]. EDIT RECIPE\n");
        printf("[4]. DELETE RECIPE\n");
        printf("[0]. BACK\n");
        printf("=============================================\n");

        int choice = readInt("> enter choice: ", 0, 4);

        switch (choice) {
        case 0:
            return;
        case 1:
            recipe_viewAll();
            break;
        case 2:
            recipe_createFlow();
            break;
        case 3:
            recipe_editFlow();
            break;
        case 4:
            recipe_deleteFlow();
            break;
        default:
            printf("\nInvalid choice.\n");
            waitKey(NULL);
        }
    }
}
