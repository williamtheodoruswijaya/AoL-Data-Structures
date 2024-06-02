#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

# define SIZE 100

struct User{
	char userID[101];
	char username[101];
	char password[101];
	struct User* prev;
	struct User* next;
}*head[SIZE], *tail[SIZE];

struct Item{
	char userID[101];
	char warehouseID[101];
	char warehouseName[101];
	char warehouseLocation[101];
	char itemName[101];
	char itemCategory[101];
	char itemTypes[101];
	double itemWeight;
	int itemQuantity;
	int height;
	struct Item* left;
	struct Item* right;
}*root = NULL;

struct Item* createItems(char userID[], char warehouseID[], char warehouseName[], char warehouseLocation[], char itemName[], char itemCategory[], char itemTypes[], double itemWeight, int itemQuantity){
	struct Item* node = (struct Item*)malloc(sizeof(struct Item));
	strcpy(node->userID, userID);
	strcpy(node->warehouseID, warehouseID);
	strcpy(node->warehouseName, warehouseName);
	strcpy(node->warehouseLocation, warehouseLocation);
	strcpy(node->itemName, itemName);
	strcpy(node->itemCategory, itemCategory);
	strcpy(node->itemTypes, itemTypes);
	node->itemWeight = itemWeight;
	node->itemQuantity = itemQuantity;
	node->left = NULL;
	node->right = NULL;
	return node;
}

void clearScreen(){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void clearInputBuffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

int hash(char username[]){
	int sum = 0;
	for(int i = 0; username[i] != '\0'; i++){
		sum += username[i];
	}
	return sum % SIZE;
}

int getMax(int a, int b){
	if(a > b){
		return a;
	}
	return b;
}

int getHeight(struct Item* root){
	if(root == NULL){
		return 0;
	}
	return root->height;
}

int getBalance(struct Item* root){
	if(root == NULL){
		return 0;
	}
	return getHeight(root->left) - getHeight(root->right);
}

struct Item* rightRotate(struct Item* root){
	struct Item* x = root->left;
	struct Item* subX = x->right;

	x->right = root;
	root->left = subX;

	root->height = 1 + getMax(getHeight(root->left), getHeight(root->right));
	x->height = 1 + getMax(getHeight(x->left), getHeight(x->right));
	return x;
}

struct Item* leftRotate(struct Item* root){
	struct Item* x = root->right;
	struct Item* subX = x->left;
	
	x->left = root;
	root->right = subX;
	
	root->height = 1 + getMax(getHeight(root->left), getHeight(root->right));
	x->height = 1 + getMax(getHeight(x->left), getHeight(x->right));
	return x;
}

struct Item* insertItems(struct Item* root, char userID[], char warehouseID[], char warehouseName[], char warehouseLocation[], char itemName[], char itemCategory[], char itemTypes[], double itemWeight, int itemQuantity){
	if(root == NULL){
		return createItems(userID, warehouseID, warehouseName, warehouseLocation, itemName, itemCategory, itemTypes, itemWeight, itemQuantity);
	}
	else{
		int key = hash(itemName);
		if(key < hash(root->itemName)){
			root->left = insertItems(root->left, userID, warehouseID, warehouseName, warehouseLocation, itemName, itemCategory, itemTypes, itemWeight, itemQuantity);
		}
		else if(key > hash(root->itemName)){
			root->right = insertItems(root->right, userID, warehouseID, warehouseName, warehouseLocation, itemName, itemCategory, itemTypes, itemWeight, itemQuantity);
		}
		else{
			// Add the item quantity
			root->itemQuantity += itemQuantity;
		}
	}
	root->height = 1 + getMax(getHeight(root->left), getHeight(root->right));
	int balance = getBalance(root);
	if(balance > 1 && hash(itemName) < hash(root->left->itemName)){
		return rightRotate(root);
	}
	if(balance < -1 && hash(itemName) > hash(root->right->itemName)){
		return leftRotate(root);
	}
	if(balance > 1 && hash(itemName) > hash(root->left->itemName)){
		root->left = leftRotate(root->left);
		return rightRotate(root);
	}
	if(balance < -1 && hash(itemName) < hash(root->right->itemName)){
		root->right = rightRotate(root->right);
		return leftRotate(root);
	}
	return root;
}

void readMsItem(char userID[], char warehouseID[]){
	FILE* outputFile = fopen("MsItem.txt", "r");
	if(outputFile == NULL){
		printf("WARNING! ERROR IN FILE!");
		printf("\n");
		exit(1);
	}
	char FILEuserID[101];
	char FILEwarehouseID[101];
	char warehouseName[101];
	char warehouseLocation[101];
	char itemName[101];
	char itemCategory[101];
	char itemTypes[101];
	double itemWeight;
	int itemQuantity;
	while(fscanf(outputFile, "%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%lf#%d\n", FILEuserID, FILEwarehouseID, warehouseName, warehouseLocation, itemName, itemCategory, itemTypes, &itemWeight, &itemQuantity) != EOF){
		if(strcmp(userID, FILEuserID) == 0){
			if(strcmp(warehouseID, FILEwarehouseID) == 0){
				root = insertItems(root, userID, warehouseID, warehouseName, warehouseLocation, itemName, itemCategory, itemTypes, itemWeight, itemQuantity);
			}
		}
	}
	fclose(outputFile);
	return;
}

void inOrder(struct Item* root, int count){
	count++;
	if(root == NULL){
		return;
	}
	else{
		// Ini pre-order traversal biar si countnya ngurut
		printf("%d. %s - %s\n", count, root->itemCategory, root->itemName);
		inOrder(root->left, count);
		inOrder(root->right, count);
	}
}

struct Item* searchItems(struct Item* root, char itemName[]){
	if(root == NULL){
		return NULL;
	}
	else{
		int key = hash(itemName);
		if(key < hash(root->itemName)){
			return searchItems(root->left, itemName);
		}
		else if(key > hash(root->itemName)){
			return searchItems(root->right, itemName);
		}
		else{
			return root;
		}
	}
}

void viewIteminWarehouse(char userID[], char warehouseID[]){
	back:
	clearScreen();
	readMsItem(userID, warehouseID);
	if(strcmp(warehouseID, "YPD01") == 0){
		printf("== Yuripedia - Bekasi ==\n");
	}
	else if(strcmp(warehouseID, "YSK02") == 0){
		printf("== Yurisekai - Pompom Nation ==\n");
	}
	else if(strcmp(warehouseID, "YRH03") == 0){
		printf("== Yurihime - Takaoka ==\n");
	}
	else if(strcmp(warehouseID, "YYR04") == 0){
		printf("== Yuriyuri - Rhodes Island ==\n");
	}
	else if(strcmp(warehouseID, "DGA05") == 0){
		printf("== Digital Anomaly - Lungmen ==\n");
	}
	printf("\n");
	if(root == NULL){
		printf("There's no item in this warehouse!\n");
		printf("\n");
		printf("Press Enter to go back...");
	}
	else{
		inOrder(root, 0);
		printf("\nEnter items name to see more details (0 to go back)\n");
		printf(">> ");
		char itemName[101];
		fgets(itemName, sizeof(itemName), stdin);
		itemName[strcspn(itemName, "\n")] = '\0';
		if(strcmp(itemName, "0") == 0){
			return;
		}
		struct Item* item = searchItems(root, itemName);
		if(item == NULL){
			clearScreen();
			printf("Item not found!\n");
			printf("\n");
			printf("Press Enter to go back...");
			getchar();
			goto back;
		}
		else{
			clearScreen();
			printf("== Item Details ==\n");
			printf("Warehouse ID: %s\n", item->warehouseID);
			printf("Warehouse Name: %s\n", item->warehouseName);
			printf("Warehouse Location: %s\n", item->warehouseLocation);
			printf("Item Name: %s\n", item->itemName);
			printf("Item Category: %s\n", item->itemCategory);
			printf("Item Types: %s\n", item->itemTypes);
			printf("Item Weight: %.2lf kg\n", item->itemWeight);
			printf("Item Quantity: %d\n", item->itemQuantity);
			printf("\n");
			printf("Press Enter to go back...");
			getchar();
			goto back;
		}
	}
	return;
}

struct Item* deleteItem(struct Item* root, char itemName[]){
	if(root == NULL){
		return root;
	}
	else{
		int key = hash(itemName);
		if(key < hash(root->itemName)){
			root->left = deleteItem(root->left, itemName);
		}
		else if(key > hash(root->itemName)){
			root->right = deleteItem(root->right, itemName);
		}
		else{
			if(root->left == NULL && root->right == NULL){
				struct Item* temp = root;
				root = NULL;
				free(temp);
			}
			else if(root->left == NULL && root->right != NULL){
				struct Item* temp = root;
				root = root->right;
				free(temp);
			}
			else if(root->left != NULL && root->right == NULL){
				struct Item* temp = root;
				root = root->left;
				free(temp);
			}
			else{
				struct Item* temp = root->right;
				while(temp->left != NULL){
					temp = temp->left;
				}
				strcpy(root->itemName, temp->itemName);
				root->right = deleteItem(root->right, temp->itemName);
			}
		}
	}
	root->height = 1 + getMax(getHeight(root->left), getHeight(root->right));
	int balance = getBalance(root);

	if(balance > 1 && getBalance(root->left) >= 0){
		return rightRotate(root);
	}
	if(balance > 1 && getBalance(root->left) < 0){
		root->left = leftRotate(root->left);
		return rightRotate(root);
	}
	if(balance < -1 && getBalance(root->right) <= 0){
		return leftRotate(root);
	}
	if(balance < -1 && getBalance(root->right) > 0){
		root->right = rightRotate(root->right);
		return leftRotate(root);
	}
	return root;
}

void deleteAllItem(struct Item* root){
    if(root == NULL){
        return;
    }
    deleteAllItem(root->left);
    deleteAllItem(root->right);
    free(root);
}

void clearTree(struct Item** root){
    deleteAllItem(*root);
    *root = NULL;
}

void viewingItems(char userID[]){
	char warehouseID[101];
	int select = 0;
	while(true){
		clearTree(&root);
		clearScreen();
		puts("== Warehouse List ==");
		puts("Choose Warehouse:");
		puts("1. Yuripedia - Bekasi");
		puts("2. Yurisekai - Pompom Nation");
		puts("3. Yurihime - Takaoka");
		puts("4. Yuriyuri - Rhodes Island");
		puts("5. Digital Anomaly - Lungmen");
		puts("0. Exit");
		printf(">> ");
		scanf("%d", &select);
		clearInputBuffer();
		switch(select){
			case 1:
				strcpy(warehouseID, "YPD01");
				viewIteminWarehouse(userID, warehouseID);
				getchar();
				break;
			case 2:
				strcpy(warehouseID, "YSK02");
				viewIteminWarehouse(userID, warehouseID);
				getchar();
				break;
			case 3:
				strcpy(warehouseID, "YRH03");
				viewIteminWarehouse(userID, warehouseID);
				getchar();
				break;
			case 4:
				strcpy(warehouseID, "YYR04");
				viewIteminWarehouse(userID, warehouseID);
				getchar();
				break;
			case 5:
				strcpy(warehouseID, "DGA05");
				viewIteminWarehouse(userID, warehouseID);
				getchar();
				break;
			case 0:
				return;
			default:
				printf("Invalid selection, Please choose again!\n");
				getchar();
				continue;
		}
	}
	return;
}

void writeItemToFile(char userID[], char warehouseID[], char warehouseName[], char warehouseLocation[], char itemName[], char itemCategory[], char itemTypes[], double itemWeight, int itemQuantity){
	FILE* inputFile = fopen("MsItem.txt", "a");
	if(inputFile == NULL){
		printf("WARNING! ERROR IN FILE!");
		printf("\n");
		exit(1);
	}
	fprintf(inputFile, "%s#%s#%s#%s#%s#%s#%s#%.2lf#%d\n", userID, warehouseID, warehouseName, warehouseLocation, itemName, itemCategory, itemTypes, itemWeight, itemQuantity);
	fclose(inputFile);
	printf("Item added successfully!\n");
	return;
}

void addingItem(char userID[]){
	int select = 0;
	char warehouseID[101];
	char warehouseName[101];
	char warehouseLocation[101];
	char itemName[101];
	char itemCategory[101];
	char itemTypes[101];
	double itemWeight;
	int itemQuantity;

	do{
		clearScreen();
		puts("== Warehouse List ==");
		puts("Choose Warehouse:");
		puts("1. Yuripedia - Bekasi");
		puts("2. Yurisekai - Pompom Nation");
		puts("3. Yurihime - Takaoka");
		puts("4. Yuriyuri - Rhodes Island");
		puts("5. Digital Anomaly - Lungmen");
		puts("0. Exit");
		printf(">> ");
		scanf("%d", &select);
		clearInputBuffer();

		switch(select){
			case 1:
				strcpy(warehouseID, "YPD01");
				strcpy(warehouseName, "Yuripedia");
				strcpy(warehouseLocation, "Bekasi");
				break;
			case 2:
				strcpy(warehouseID, "YSK02");
				strcpy(warehouseName, "Yurisekai");
				strcpy(warehouseLocation, "Pompom Nation");
				break;
			case 3:
				strcpy(warehouseID, "YRH03");
				strcpy(warehouseName, "Yurihime");
				strcpy(warehouseLocation, "Takaoka");
				break;
			case 4:
				strcpy(warehouseID, "YYR04");
				strcpy(warehouseName, "Yuriyuri");
				strcpy(warehouseLocation, "Rhodes Island");
				break;
			case 5:
				strcpy(warehouseID, "DGA05");
				strcpy(warehouseName, "Digital Anomaly");
				strcpy(warehouseLocation, "Lungmen");
				break;
			case 0:
				return;
			default:
				printf("Invalid selection, Please choose again!\n");
				getchar();
				continue;
		}
	}while(select == 0);

	int valid = 0;
	do{
		valid = 1;
		printf("Enter item name (5-100 characters): ");
		fgets(itemName, sizeof(itemName), stdin);
		itemName[strcspn(itemName, "\n")] = '\0';

		// Check if the item name is empty
		if(strlen(itemName) == 0){
			valid = 0;
			printf("\nWarning!, Item name cannot be empty!\n\n");
		}
		// Check if the item name is between 5 and 100 characters
		else if(strlen(itemName) < 5 || strlen(itemName) > 100){
			valid = 0;
			printf("\nWarning! Item name must be between 5 and 100 characters.\n\n");
		}
	}while(valid == 0);

	do{
		valid = 1;
		printf("Enter item category (5-100 characters): ");
		fgets(itemCategory, sizeof(itemCategory), stdin);
		itemCategory[strcspn(itemCategory, "\n")] = '\0';

		// Check if the item category is empty
		if(strlen(itemCategory) == 0){
			valid = 0;
			printf("\nWarning!, Item category cannot be empty!\n\n");
		}
		// Check if the item category is between 5 and 100 characters
		else if(strlen(itemCategory) < 5 || strlen(itemCategory) > 100){
			valid = 0;
			printf("\nWarning! Item category must be between 5 and 100 characters.\n\n");
		}
	}while(valid == 0);

	do{
		valid = 1;
		printf("Enter item types (5-100 characters): ");
		fgets(itemTypes, sizeof(itemTypes), stdin);
		itemTypes[strcspn(itemTypes, "\n")] = '\0';

		// Check if the item types is empty
		if(strlen(itemTypes) == 0){
			valid = 0;
			printf("\nWarning!, Item types cannot be empty!\n\n");
		}
		// Check if the item types is between 5 and 100 characters
		else if(strlen(itemTypes) < 5 || strlen(itemTypes) > 100){
			valid = 0;
			printf("\nWarning! Item types must be between 5 and 100 characters.\n\n");
		}
	}while(valid == 0);

	do{
		valid = 1;
		printf("Enter item weight (in kg): ");
		scanf("%lf", &itemWeight);
		clearInputBuffer();

		// Check if the item weight is less than 0
		if(itemWeight < 0){
			valid = 0;
			printf("\nWarning! Item weight must be greater than 0.\n\n");
		}
	}while(valid == 0);

	do{
		valid = 1;
		printf("Enter item quantity: ");
		scanf("%d", &itemQuantity);
		clearInputBuffer();

		// Check if the item quantity is less than 0
		if(itemQuantity < 0){
			valid = 0;
			printf("\nWarning! Item quantity must be greater than 0.\n\n");
		}
	}while(valid == 0);
	writeItemToFile(userID, warehouseID, warehouseName, warehouseLocation, itemName, itemCategory, itemTypes, itemWeight, itemQuantity);
	return;
}

void moveMsItemtoTree(char userID[]){
	FILE* outputFile = fopen("MsItem.txt", "r");
	if(outputFile == NULL){
		printf("WARNING! ERROR IN FILE!");
		printf("\n");
		exit(1);
	}
	char FILEuserID[101];
	char FILEwarehouseID[101];
	char warehouseID[101];
	char warehouseName[101];
	char warehouseLocation[101];
	char itemName[101];
	char itemCategory[101];
	char itemTypes[101];
	double itemWeight;
	int itemQuantity;
	while(fscanf(outputFile, "%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%lf#%d\n", FILEuserID, FILEwarehouseID, warehouseName, warehouseLocation, itemName, itemCategory, itemTypes, &itemWeight, &itemQuantity) != EOF){
		if(strcmp(userID, FILEuserID) == 0){
			root = insertItems(root, userID, warehouseID, warehouseName, warehouseLocation, itemName, itemCategory, itemTypes, itemWeight, itemQuantity);
		}
	}
	fclose(outputFile);
	return;
}

void removeItemMenu(char userID[]){
	
}

void mainMenu(char userID[], char username[], char password[]){
	int select = 0;
	do{
		clearScreen();
		puts(" __      __                    ___________           .__");
		puts("/  \\    /  \\_____ _______   ___\\__    ___/___   ____ |  |__  ");
		puts("\\   \\/\\/   /\\__  \\\\_  __ \\_/ __ \\|    |_/ __ \\_/ ___\\|  |  \\ ");
		puts(" \\        /  / __ \\|  | \\/\\  ___/|    |\\  ___/\\  \\___|   Y  \\\\");
		puts("  \\__/\\  /  (____  /__|    \\___  >____| \\___  >\\___  >___|  /");
		puts("       \\/        \\/            \\/           \\/     \\/     \\/ ");
		printf("Welcome, %s\n", username);
		puts("1. View Items");
		puts("2. Add Items");
		puts("3. Remove Items");
		puts("0. Exit");
		printf(">> ");
		scanf("%d", &select);
		clearInputBuffer();
		switch(select){
			case 1:
				viewingItems(userID);
				break;
			case 2:
				addingItem(userID);
				getchar();
				break;
			case 3:
				removeItemMenu(userID);
				break;
			case 0:
				break;
			default:
				break;
		}
	}while(select != 0);
	return;
}

void insertToHashTables(char userID[], char username[], char password[]){
	struct User* node = (struct User*)malloc(sizeof(struct User));
	strcpy(node->userID, userID);
	strcpy(node->username, username);
	strcpy(node->password, password);
	node->prev = NULL;
	node->next = NULL;
	
	int index = hash(username);
	if(head[index] == NULL){
		head[index] = node;
		tail[index] = node;
	}
	else{
		node->prev = tail[index];
		tail[index]->next = node;
		tail[index] = node;
	}
	return;
}

void readFile(){
	// ini masukin data dari file ke hash table
	FILE* outputFile = fopen("MsUser.txt", "r");
	if(outputFile == NULL){
		printf("WARNING! ERROR IN FILE!");
		printf("\n");
		exit(1);
	}

	char userID[101];
	char username[101];
	char password[101];
	while(fscanf(outputFile, "%s %s %s\n", userID, username, password) != EOF){
		insertToHashTables(userID, username, password);
	}
	fclose(outputFile);
	return;
}

void popHashTables(){
	for(int i = 0; i < SIZE; i++){
		while(head[i] != NULL){
			if(head[i] == tail[i]){
				struct User* temp = head[i];
				head[i] = NULL;
				tail[i] = NULL;
				free(temp);
			}
			else{
				struct User* temp = head[i];
				head[i] = head[i]->next;
				temp->next = NULL;
				temp->prev = NULL;
				free(temp);
			}
		}
	}
	return;
}

void loginPage(){
	clearScreen();
	int valid = 0;
	printf("== WareTech == \n");
	char username[101], password[101], userID[101], userPassword[101];
	printf("Enter 0 to return...\n\n");
	do{
		printf("Username: ");
		scanf(" %[^\n]", username);
		if(strcmp(username, "0") == 0){
			return;
		}
		readFile();
		int index = hash(username);
		struct User* curr = head[index];
		valid = 0;
		while(curr){
			if(strcmp(username, curr->username) == 0){
				strcpy(userPassword, curr->password);
				strcpy(userID, curr->userID);
				valid = 1;
				break;
			}
			curr = curr->next;
		}
		if(valid == 0){
			popHashTables();
			printf("Username is not available!\n");
		}
	}while(valid == 0);
	
	do{
		printf("Password: ");
		scanf(" %[^\n]", password);
		if(strcmp(password, "0") == 0){
			return;
		}
		else if(strcmp(password, userPassword) != 0){
			printf("Wrong password!");
			printf("\n");
			valid = 0;
		}
		else{
			valid = 1;
		}
	}while(valid == 0);
	printf("Login successful! Welcome, %s\n", username);
	getchar();
	popHashTables();
	mainMenu(userID, username, password);
	printf("\n\nPress Enter\n");
	getchar();
	return;
}

void writeFile(char userID[], char username[], char password[]){
	FILE* inputFile = fopen("MsUser.txt", "a");
	if(inputFile == NULL){
		printf("WARNING! ERROR IN FILE!");
		printf("\n");
		exit(1);
	}
	fprintf(inputFile, "%s %s %s\n", userID, username, password);
	fclose(inputFile);
	printf("Register process success\n");
	return;
}

void registerPage(){
	clearScreen();
	char username[101], password[101], userID[101];
	printf("== WareTech == \n\n");
	int valid = 0;
    do{
    	valid = 1; 
        printf("Enter new username (5-100 characters): ");
        fgets(username, sizeof(username), stdin);
        username[strcspn(username, "\n")] = '\0';
        int length = strlen(username); 
        if(length == 0){
        	valid = 0;
        	printf("\nWarning!, Username cannot be empty!\n\n");
		}
        else if (length < 5 || length > 100) {
            valid = 0; 
            printf("\nWarning! Username must be between 5 and 100 characters.\n\n");
        }
        else{	
        	for(int i = 0; username[i] != '\0'; i++){
        		if(username[i] == ' '){
        			valid = 0;
        			printf("\nUsername must not contains any space!\n\n");
					break;
				}
			}
		}
	}while(valid == 0);
	int valid2, length2;
	do{
		valid2 = 1;
		printf("Enter password (5-20 characters): ");
		fgets(password, sizeof(password), stdin);
		password[strcspn(password, "\n")] = '\0';
		length2 = strlen(password);
		if(length2 == 0){
			valid2 = 0;
        	printf("\nWarning!, Password cannot be empty!\n\n");
		}
		else if(length2 < 5 || length2 > 20){
			valid2 = 0; 
            printf("\nWarning! Password must be between 5 and 20 characters.\n\n");
		}
		else{	
        	for(int i = 0; password[i] != '\0'; i++){
        		if(password[i] == ' '){
        			valid2 = 0;
        			printf("\nPassword must not contains any space!\n\n");
					break;
				}
			}
		}
	}while(valid2 == 0);
	
	int registers = 0;
	do{
		valid = 1;
		printf("Are you sure to register as '%s'? [Yes | No] [Case sensitive]\n", username);
		printf(">> ");
		char signupValidation[10];
		scanf("%s", signupValidation);
		if(strcmp(signupValidation, "Yes") == 0){
			registers = 1;
		}
		else if(strcmp(signupValidation, "No") == 0){
			registers = 0;
		}
		else{
			valid = 0;
		}
	}while(valid == 0);
	
	if(registers == 1){
		srand(time(NULL));
		for(int i = 0; i < 3; i++){
			userID[i] = 'A' + rand() % 26;
		}
		for(int i = 3; i < 5; i++){
			userID[i] = '0' + rand() % 10;
		}
		userID[5] = '\0';
		// Saving the registered user into a file
		writeFile(userID, username, password);
	}
	else{
		printf("Register cancelled!\n");
	}
	printf("\n\nPress Enter\n");
	getchar();
	return;
}

void landingPage(){
	int select = 0;
	do{
		clearScreen();
		puts(" __      __                    ___________           .__");
		puts("/  \\    /  \\_____ _______   ___\\__    ___/___   ____ |  |__  ");
		puts("\\   \\/\\/   /\\__  \\\\_  __ \\_/ __ \\|    |_/ __ \\_/ ___\\|  |  \\ ");
		puts(" \\        /  / __ \\|  | \\/\\  ___/|    |\\  ___/\\  \\___|   Y  \\\\");
		puts("  \\__/\\  /  (____  /__|    \\___  >____| \\___  >\\___  >___|  /");
		puts("       \\/        \\/            \\/           \\/     \\/     \\/ ");
		puts("1. Login");
		puts("2. Register");
		puts("3. Exit");
		printf(">> ");
		scanf("%d", &select);
		clearInputBuffer();
		switch(select){
			case 1:
				loginPage();
				break;
			case 2:
				registerPage();
				getchar();
				break;
			case 3:
				break;
			default:
				break;
		}
	}while(select != 3);
	if(select == 3){
		clearScreen();
		printf("\n\n\n");
		char exitMessage[] = "Thank you for using WareTech, Where technologies rests...\n";
		for(int i = 0; exitMessage[i] != '\0'; i++){
			printf("%c", exitMessage[i]);
			Sleep(100);
		}
	}
}

int main(){
	keybd_event(VK_F11, 0, 0, 0); // enter fullscreen mode
	keybd_event(VK_F11, 0, KEYEVENTF_KEYUP, 0);
	landingPage();
	return 0;
}