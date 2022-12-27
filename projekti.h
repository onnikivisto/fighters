// Taistelijan määrittely, muisti nimelle ja aseelle varataan dynaamisesti
typedef struct
{
    char *name;
    int hp;
    int exp;
    char *weapon;
    int weapon_damage;
} Fighter;

int main(void);


Fighter* add_fighter(char *buffer, Fighter *allfighters);

void print_fighters(Fighter *allfighters);

int compare_exp(const void* a, const void* b);

Fighter* attack(char *buffer, Fighter *allfighters);

void write_fighters(char *buffer, Fighter* allfighters);

Fighter* read_fighters(char *buffer, Fighter* allfighters);

void free_memory(Fighter * allfighters);








