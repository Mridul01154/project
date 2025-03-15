#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>

FILE *log_file = NULL;

typedef struct 
{
  int id;
  char name[100];
  float balance;
}Customer;

typedef struct {
  int id;
  long offset;
} IndexEntry;



void begin_transaction() {
  log_file = fopen("transaction.log", "w");
  if (!log_file) {
      printf("Error creating transaction log.\n");
      return;
  }
  printf("Transaction started.\n");
}

void commit_transaction() {
  if (!log_file) {
      printf("No active transaction.\n");
      return;
  }
  
  fclose(log_file);
  remove("transaction.log"); 
  printf("Transaction committed.\n");
}

void rollback_transaction() {
  if (!log_file) {
      printf("No active transaction.\n");
      return;
  }

  FILE *db = fopen("customer.db", "rb+");
  if (!db) {
      printf("Error opening database file.\n");
      fclose(log_file);
      return;
  }

  fseek(log_file, 0, SEEK_SET);
  char action[10];
  Customer old_data;
  long offset;


  while (fscanf(log_file, "%s %ld %d %s %f\n", action, &offset, &old_data.id, old_data.name, &old_data.balance) != EOF) {
      if (strcmp(action, "UPDATE") == 0) {
          fseek(db, offset, SEEK_SET);
          fwrite(&old_data, sizeof(Customer), 1, db);
      } else if (strcmp(action, "INSERT") == 0) {
          fseek(db, offset, SEEK_SET);
          Customer empty = {0, "", 0.0};
          fwrite(&empty, sizeof(Customer), 1, db);
      }
  }

  fclose(db);
  fclose(log_file);
  remove("transaction.log");
  log_file = NULL;
  printf("Transaction rolled back.\n");
}

void update_index(int id, long offset) {
  FILE *indexFile = fopen("customers.idx", "ab");
  if (!indexFile) {
      printf("Error opening index file.\n");
      return;
  }
  
  IndexEntry entry = {id, offset};
  fwrite(&entry, sizeof(IndexEntry), 1, indexFile);
  fclose(indexFile);
}


void insert_customer(int id, char *name, float balance) {
  if (log_file) {
    fprintf(log_file, "INSERT %d '%s' %.2f\n", id, name, balance);
    fflush(log_file); 
}
  
  
  FILE *db = fopen("customer.db", "ab");
  if (!db) {
      printf("Error opening database file.\n");
      return;
  }

 

  Customer c;
  c.id = id;
  strcpy(c.name, name);
  c.balance = balance;

  fwrite(&c, sizeof(Customer), 1, db);
  long offset = ftell(db); 
  fclose(db);


  if (log_file) {
    fprintf(log_file, "INSERT %ld %d %s %.2f\n", offset, id, name, balance);
    fflush(log_file);
}
  
  printf("Inserted customer: %d, %s, %.2f\n", id, name, balance);
  update_index(id, offset); 
}


void read_customer(){
  FILE *db = fopen("customer.db","rb");
  if (!db)
  {
    printf("Error opening database file,\n");
    return;
  }

  Customer c;

    printf("ID\tName\tBalance\n");
    printf("----------------------\n");
    while (fread(&c, sizeof(Customer), 1, db)) {
        printf("%d\t%-20s\t%.2f\n", c.id, c.name, c.balance);
    }
    fclose(db);
}


void find_customer(int id){
  FILE *indexFile = fopen("customers.idx", "rb");
  if (!indexFile) {
    printf("Error opening index file. No customers found.\n");
    return;
  }

  FILE *db = fopen("customer.db", "rb"); 
  if (!db) {
    printf("Error opening database file.\n");
    fclose(indexFile);
    return;
  }
  IndexEntry entry;
  while (fread(&entry, sizeof(IndexEntry), 1, indexFile)) {
    if (entry.id == id) {
      fseek(db, entry.offset-108, SEEK_SET);
      Customer c;
      fread(&c, sizeof(Customer), 1, db);
      printf("ID: %d, Name: %s, Balance: %.2f\n", c.id, c.name, c.balance);
      fclose(indexFile);
      fclose(db);
      return;
  }
  }
  printf("Customer not found.\n");
    fclose(indexFile);
    fclose(db);
}

void delete(int id) {
  FILE *indexFile = fopen("customers.idx", "rb");
  if (!indexFile) {
      printf("Error opening index file. No customers found.\n");
      return;
  }

  FILE *db = fopen("customer.db", "rb+");
  if (!db) {
      printf("Error opening database file.\n");
      fclose(indexFile);
      return;
  }

  IndexEntry entry;
  FILE *tempIndex = fopen("temp.idx", "wb");
  int found = 0;

  while (fread(&entry, sizeof(IndexEntry), 1, indexFile)) {
      if (entry.id == id) {
          found = 1;

          fseek(db, entry.offset - sizeof(Customer), SEEK_SET);
          Customer empty = {0, "", 0.0};  
          fwrite(&empty, sizeof(Customer), 1, db);

          if (log_file) {
              fprintf(log_file, "DELETE %ld %d\n", entry.offset, id);
              fflush(log_file);
          }

          printf("Deleted Customer ID: %d\n", id);
      } else {
          fwrite(&entry, sizeof(IndexEntry), 1, tempIndex); 
      }
  }

  fclose(db);
  fclose(indexFile);
  fclose(tempIndex);

  remove("customers.idx");  
  rename("temp.idx", "customers.idx");

  if (!found) {
      printf("Customer ID %d not found.\n", id);
  }
}


void update_balance(int id, float balance){
  FILE *indexFile = fopen("customers.idx", "rb");
  if (!indexFile) {
    printf("Error opening index file. No customers found.\n");
    return;
  }

  FILE *db = fopen("customer.db", "rb+"); 
  if (!db) {
    printf("Error opening database file.\n");
    fclose(indexFile);
    return;
  }
  IndexEntry entry;
  while (fread(&entry, sizeof(IndexEntry), 1, indexFile)) {
    if (entry.id == id) {
      fseek(db, entry.offset-108, SEEK_SET);
      Customer c;
      if (log_file) {
        
        fprintf(log_file, "UPDATE %ld %d %s %.2f\n", entry.offset, c.id, c.name, c.balance);
        fflush(log_file);
    }

      fread(&c, sizeof(Customer), 1, db);
      c.balance=balance;
      fseek(db, entry.offset-108, SEEK_SET);
      fwrite(&c, sizeof(Customer), 1, db);
      printf("Updated balance: %d, New Balance: %.2f\n", id, balance);
      fclose(indexFile);
      fclose(db);
      return;
  }
  }
  printf("Customer not found.\n");
    fclose(indexFile);
    fclose(db);
}

void update_name(int id, char *name){
  FILE *indexFile = fopen("customers.idx", "rb");
  if (!indexFile) {
    printf("Error opening index file. No customers found.\n");
    return;
  }

  FILE *db = fopen("customer.db", "rb+"); 
  if (!db) {
    printf("Error opening database file.\n");
    fclose(indexFile);
    return;
  }
  IndexEntry entry;
  while (fread(&entry, sizeof(IndexEntry), 1, indexFile)) {
    if (entry.id == id) {
      fseek(db, entry.offset-108, SEEK_SET);
      Customer c;
      if (log_file) {
        
        fprintf(log_file, "UPDATE %ld %d %s %.2f\n", entry.offset, c.id, c.name, c.balance);
        fflush(log_file);
    }

      fread(&c, sizeof(Customer), 1, db);
      strcpy(c.name,name);
      fseek(db, entry.offset-108, SEEK_SET);
      fwrite(&c, sizeof(Customer), 1, db);
      printf("Updated balance: %d, New Name: %s\n", id, name);
      fclose(indexFile);
      fclose(db);
      return;
  }
  }
  printf("Customer not found.\n");
    fclose(indexFile);
    fclose(db);
}

void parse_query(char *query) {
  int id;
  char name[100];
  float balance;
  
  if (strncmp(query, "SELECT * FROM customers", 22) == 0) {
      read_customer();
  }
  else if (sscanf(query, "INSERT INTO customers VALUES (%d, '%99[^']', %f);", &id, name, &balance) == 3) {
      insert_customer(id, name, balance);
  }
  else if (sscanf(query, "UPDATE customers SET balance = %f WHERE id = %d;", &balance, &id) == 2) {
      update_balance(id, balance);
  }
  else if (sscanf(query, "UPDATE customers SET name = '%99[^']' WHERE id = %d;", name, &id) == 2) {
      update_name(id, name);
  }
  else if (sscanf(query, "DELETE FROM customers WHERE id = %d;", &id) == 1) {
      delete(id);
  }
  else if (strcmp(query, "BEGIN TRANSACTION;") == 0) {
    begin_transaction();
}
  else if (strcmp(query, "COMMIT;") == 0) {
    commit_transaction();
}
else if (strcmp(query, "ROLLBACK;") == 0) {
    rollback_transaction();
}
  else {
      printf("Invalid query format.\n");
  }
}


int main(){
  
  char query[200];

    printf("Simple Query Processor\n");
    printf("Enter SQL Query (or 'exit' to quit):\n");

    while (1) {
        printf("> ");
        fgets(query, sizeof(query), stdin);
        
        
        query[strcspn(query, "\n")] = '\0';

        if (strcmp(query, "exit") == 0) {
            break;
        }

        
    parse_query(query);
    }
    return 0;
}
