#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct bank
{
  char *name;
  int money;
  struct bank *next;
};
#define TABLE_SIZE 20
struct bank *hashTable[TABLE_SIZE];
int hashFunction(char *name) {
  unsigned long hash = 5381; 
    while (*name) {
        hash = ((hash << 5) + hash) + (*name); 
        name++;
    }
  return hash % TABLE_SIZE; 
}

void Add_Account(char *arr,int val){
  int index=hashFunction(arr);
  struct bank *temp=(struct bank *)malloc(sizeof(struct bank));
  temp->name=(char *)malloc(strlen(arr) + 1);
  strcpy(temp->name,arr);
  temp->money=val;
  temp->next=NULL;
  if (hashTable[index] == NULL) {
    hashTable[index] = temp;
} else {
    temp->next = hashTable[index];
    hashTable[index] = temp;
}
}

void Delete_Account(char *arr){
  int index=hashFunction(arr);
  struct bank *temp=hashTable[index];
  struct bank *prev=NULL;

  while (temp != NULL) {
    if (strcmp(temp->name, arr) == 0) {
        if (prev == NULL) {
            hashTable[index] = temp->next;
        } else {
            prev->next = temp->next;
        }
        printf("%s please withdraw you %d rs.\n",temp->name,temp->money);
        free(temp->name);
        free(temp);
        printf("Account %s deleted successfully.\n", arr);
        return;
    }
    prev = temp;
    temp = temp->next;
}
printf("Account %s not found.\n", arr);
}


void Display(){
  for (size_t i = 0; i < TABLE_SIZE; i++)
  {
    printf("[%d] ",i);
    struct bank *temp=hashTable[i];
    while (temp!=NULL)
    {
      printf("-> [Name: %s, Balance: %d] ",temp->name,temp->money);
      temp=temp->next;
    }
    printf("->NULL\n");
  }
  
}

struct bank *search(char *arr){
  int A=hashFunction(arr);
  struct bank *temp=hashTable[A];
  while (temp!=NULL)
  {
    if (strcmp(temp->name,arr)==0)
    {
      return temp;
    }
    temp=temp->next;
  }
  return NULL;
}

void Transaction(char *arr1,char *arr2,int money){
  int A=hashFunction(arr1);
  struct bank *temp1=hashTable[A];
  int B=hashFunction(arr2);
  struct bank *temp2=hashTable[B];


  while (temp1 != NULL && strcmp(temp1->name, arr1) != 0) {
    temp1 = temp1->next;
}
  while (temp2 != NULL && strcmp(temp2->name, arr2) != 0) {
  temp2 = temp2->next;
}

if (temp1 == NULL) {
  printf("Error: The sender account does not exist.\n");
  return;
}

if (temp2 == NULL) {
  printf("Error: The recever account does not exist.\n");
  return;
}

if (temp1->money<money)
{
  printf("Error: The sender does not have enough money in the account.\n");
  return;
}


    temp1->money -= money;
    temp2->money += money;
    printf("Transaction successful: %d transferred from %s to %s.\n", money, arr1, arr2);
}

void SaveAccountsToFile() {
  FILE *file = fopen("accounts.dat", "wb");
  if (!file) {
      printf("Error saving accounts.\n");
      return;
  }
  for (int i = 0; i < TABLE_SIZE; i++) {
      struct bank *temp = hashTable[i];
      while (temp) {
          fwrite(temp, sizeof(struct bank), 1, file);
          temp = temp->next;
      }
  }
  fclose(file);
  printf("Accounts saved successfully!\n");
}


void Deposits(char *arr,int val){
  int A=hashFunction(arr);
  struct bank *temp=hashTable[A];
  if (val <= 0) {
    printf("Error: Deposit amount must be positive.\n");
    return;
}
  while (temp!=NULL)
  {
    if (strcmp(temp->name,arr)==0)
    {
      temp->money+=val;
      printf("Deposit successful: %d added to %s's account.\n", val, arr);
      return;
    }
    temp=temp->next;
  }
  printf("TError: he Account does not exist\n");
}

void Withdrawal(char *arr,int val){
  int A=hashFunction(arr);
  struct bank *temp=hashTable[A];
  while (temp!=NULL)
  {
    if (strcmp(temp->name,arr)==0)
    {
      if (temp->money>=val)
      {
        temp->money-=val;
        printf("Withdrawal successful: %d withdrawn from %s's account.\n", val, arr);
        return;
      }
      else
      {
        printf("Error: The account have Insufficient funds\n");
        return;
      }
      
    }
    temp=temp->next;
  }
  printf("Error: The Account does not exist\n");
}

void HighBalanceReport(int threshold) {
  printf("Accounts with balance greater than %d:\n", threshold);
  for (int i = 0; i < TABLE_SIZE; i++) {
      struct bank *temp = hashTable[i];
      while (temp != NULL) {
          if (temp->money > threshold) {
              printf("Name: %s, Balance: %d\n", temp->name, temp->money);
          }
          temp = temp->next;
      }
  }
}

void FreeMemory() {
  for (int i = 0; i < TABLE_SIZE; i++) {
      struct bank *temp = hashTable[i];
      while (temp != NULL) {
          struct bank *toDelete = temp;
          temp = temp->next;
          free(toDelete->name);
          free(toDelete);
      }
      hashTable[i] = NULL;
  }
}



void printHeader() {
    printf("\n");
    printf("====================================\n");
    printf("        BANKING SYSTEM MENU         \n");
    printf("====================================\n");
}

void printMenu() {
    printf("\n");
    printf("------------------------------------\n");
    printf("|  1. Add Account                  |\n");
    printf("|  2. Delete Account               |\n");
    printf("|  3. Deposit Money                |\n");
    printf("|  4. Withdraw Money               |\n");
    printf("|  5. Transfer Money               |\n");
    printf("|  6. Display All Accounts         |\n");
    printf("|  7. Search Account               |\n");
    printf("|  8. High Balance Report          |\n");
    printf("|  9. Exit                         |\n");
    printf("------------------------------------\n");
    printf("Enter your choice: ");
}

int main() {
    int choice;
    char name[100], receiver[100];
    int amount, threshold;
    struct bank *account;

    while (1) {
        printHeader();
        printMenu();

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1:
                while (getchar() != '\n'); // Clear buffer
                printf("\nEnter Account Holder's Name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0; // Remove newline character

                printf("Enter Initial Deposit Amount: ");
                if (scanf("%d", &amount) != 1 || amount < 0) {
                    printf("Invalid amount! Try again.\n");
                    while (getchar() != '\n');
                    break;
                }
                Add_Account(name, amount);
                printf("Account successfully created for %s!\n", name);
                break;

            case 2:
                while (getchar() != '\n');
                printf("\nEnter Account Holder's Name to Delete: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                Delete_Account(name);
                break;

            case 3:
                while (getchar() != '\n');
                printf("\nEnter Account Holder's Name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                printf("Enter Deposit Amount: ");
                if (scanf("%d", &amount) != 1 || amount <= 0) {
                    printf("Invalid amount! Try again.\n");
                    while (getchar() != '\n');
                    break;
                }
                Deposits(name, amount);
                break;

            case 4:
                while (getchar() != '\n');
                printf("\nEnter Account Holder's Name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                printf("Enter Withdrawal Amount: ");
                if (scanf("%d", &amount) != 1 || amount <= 0) {
                    printf("Invalid amount! Try again.\n");
                    while (getchar() != '\n');
                    break;
                }
                Withdrawal(name, amount);
                break;

            case 5:
                while (getchar() != '\n');
                printf("\nEnter Sender's Name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                printf("Enter Receiver's Name: ");
                fgets(receiver, sizeof(receiver), stdin);
                receiver[strcspn(receiver, "\n")] = 0;

                printf("Enter Transfer Amount: ");
                if (scanf("%d", &amount) != 1 || amount <= 0) {
                    printf("Invalid amount! Try again.\n");
                    while (getchar() != '\n');
                    break;
                }
                Transaction(name, receiver, amount);
                break;

            case 6:
                printf("\nDisplaying All Accounts:\n");
                Display();
                break;

            case 7:
                while (getchar() != '\n');
                printf("\nEnter Account Holder's Name to Search: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                account = search(name);
                if (account != NULL) {
                    printf("Account Found!\n");
                    printf("Name: %s\n", account->name);
                    printf("Balance: %d\n", account->money);
                } else {
                    printf("Error: Account not found.\n");
                }
                break;

            case 8:
                printf("\nEnter Balance Threshold: ");
                if (scanf("%d", &threshold) != 1 || threshold < 0) {
                    printf("Invalid amount! Try again.\n");
                    while (getchar() != '\n');
                    break;
                }
                HighBalanceReport(threshold);
                break;

            case 9:
                printf("\nExiting the Banking System. Goodbye!\n");
                SaveAccountsToFile();
                FreeMemory();
                return 0;

            default:
                printf("Invalid choice! Please select a valid option.\n");
        }
    }
}
