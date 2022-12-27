#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "projekti.h"


// Ohjelman pääfunktio
int main(void)
{
    // Varataan taistelijoiden listalle muistia ja asetetaan ensimmäisen taistelijan nimeksi NULL
    Fighter *allfighters;
    allfighters = malloc(sizeof(Fighter));
    if(!allfighters){
        printf("Error in memory allocation, program shutting down.\n");
        exit(-1);
    }
    allfighters[0].name = NULL;
    char buffer[80];
    int exit = 0;
    // Pyydetään käyttäjältä syötettä, kunnes annetaan komento 'Q'
    while (!exit)
    {
        // Tarkistetaan että syöte ei pääty ennenaikaisesti
        if(!fgets(buffer, 80, stdin) || feof(stdin) || ferror(stdin)){
            // Vapautetaan muisti ja poistutaan ohjelmasta
            free_memory(allfighters);
            exit = 1;
        }
        else{
   
            switch(buffer[0]){

            case 'A':
                // Lisätään taistelija
                allfighters = add_fighter(buffer, allfighters);
                break;

            case 'H':
                // Taistelija hyökkää toiseen taistelijaan
                allfighters = attack(buffer, allfighters);
                break;

            case 'L':
                // Tulostetaan taistelijat kokemuspisteiden mukaisessa järjestyksessä
                print_fighters(allfighters);
                break;

            case 'W':
                // Kirjoitetaan taistelijat tiedostoon
                write_fighters(buffer, allfighters);
                break;

            case 'O':
                // Luetaan taistelijat tiedostosta ja korvataan aikaisempi lista uusilla hahmoilla
                allfighters = read_fighters(buffer, allfighters);
                break;

            case 'Q':
                // Vapautetaan muisti ja poistutaan ohjelmasta
                free_memory(allfighters);
                exit = 1;
                break;

            default:
                printf("ERROR: Invalid command entered.\n");
        
            }
    }
    }
    printf("Program shutting down...\n");
    return 0;   
}



// Lisää uuden taistelijan structiin fighters ja varaa tälle muistia
Fighter* add_fighter(char *buffer, Fighter *allfighters)
{
    char name[80];
    int hp;
    char weapon[80];
    int weapon_damage;
    // Tarkistetaan syöte
    if(sscanf(buffer, "A %s %d %s %d", name, &hp, weapon, &weapon_damage) != 4){
        printf("ERROR: Invalid input for command 'A'.\n");
        return allfighters;
    }
    // Tarkistetaan, että taistelijan ja aseen nimikentät eivät ole tyhjiä eivätkä elämäpisteet tai aseen vahinkopisteet ole alle 1
    if(strlen(name) < 1){
        printf("ERROR: The fighter must have a name.\n");
        return allfighters;
    }
    if(strlen(weapon) < 1){
        printf("ERROR: The fighter's weapon must have a name.\n");
        return allfighters;
    }
    if(hp <= 0){
        printf("ERROR: The fighter must have more than 0 hp.\n");
        return allfighters;
    }
    if(weapon_damage <= 0){
        printf("ERROR: The fighter's weapon must do more than 0 damage.\n");
        return allfighters;
    }
    // Tarkistetaan, onko samannimistä taistelijaa jo olemassa
    int i = 0;
    while(allfighters[i].name != NULL){
        if(strcmp(allfighters[i].name, name) == 0){
            printf("ERROR: There is already a fighter with the same name!\n");
            return allfighters;
        }
        i++;
    }
    // Lisätään uusi taistelija, reallocataan listalle lisää muistia ja asetetaan viimeisen taistelijan nimen ensimmäiseksi merkiksi NULL
    Fighter* newfighters = realloc(allfighters, (i+2)*sizeof(Fighter));
    if(!newfighters){
        printf("ERROR: Couldn't allocate memory.\n");
        return allfighters;
    }
    newfighters[i].name = calloc(strlen(name) +1,sizeof(char));
    strcpy(newfighters[i].name, name);
    newfighters[i].hp = hp;
    newfighters[i].exp = 0;
    newfighters[i].weapon = calloc(strlen(weapon) +1, sizeof(char));
    strcpy(newfighters[i].weapon, weapon);
    newfighters[i].weapon_damage = weapon_damage;
    newfighters[i+1].name = NULL;
    printf("Successfully added a new character %s.\n", name);
    return newfighters;
}


// Tulostaa taistelijat kokemuspisteiden mukaisessa järjestyksessä, kuolleet taistelijat viimeisenä
void print_fighters(Fighter *allfighters)
{
    int i = 0;
    while(allfighters[i].name != NULL){
        i++;
    }
    // Tarkistetaan onko lista tyhjä
    if(i == 0){
        printf("ERROR: There are no fighters!\n");
    }
    else{

    qsort(allfighters, i, sizeof(Fighter), compare_exp);
    while(allfighters[0].name != NULL){
        printf("%s   Hp: %d   Exp: %d   Weapon: %s   Weapon damage: %d\n", allfighters[0].name, allfighters[0].hp, allfighters[0].exp, allfighters[0].weapon, allfighters[0].weapon_damage);
        printf("------------------------------------------------------------\n");
        allfighters++;
    }
    }
}


// Kokemuspisteiden vertailufunktio
int compare_exp(const void* a, const void* b)
{
	const Fighter * a_temp = a;
	const Fighter * b_temp = b;
    // Tarkistetaan, onko toinen vertailtavista taistelijoista kuollut, jolloin tämä siirtyy elävän hahmon alapuolelle
    if(a_temp->hp == 0 && b_temp->hp != 0){
        return 1;
    }
    else if(a_temp->hp != 0 && b_temp->hp == 0){
        return -1;
    }
    // Mikäli molemmat taistelijat ovat kuolleet tai elossa, järjestyksen määrää kokemuspisteiden määrä
	int res = b_temp->exp - a_temp->exp;
    // Mikäli kokemuspisteitä on yhtä paljon, järjestyksen määrää taistelijoiden nimi 
	if(res == 0){
		return strcmp(a,b);
	}
	return res;
}


// Taistelija 1 hyökkää taistelijaan 2
Fighter* attack(char *buffer, Fighter *allfighters)
{
    int attackerindex = -1;
    int defenderindex = -1;
    char attackername[31];
    char defendername[31];
    // Tarkistetaan syöte
    if(sscanf(buffer, "H %s %s", attackername, defendername) != 2){
        printf("ERROR: Invalid input for command 'H'.\n");
        return allfighters;
    }
    // Tarkistetaan, ovatko taistelijat listassa ja onko taistelijoilla eri nimet
    int i = 0;
    while (allfighters[i].name != NULL)
    {
        if(strcmp(allfighters[i].name, attackername) == 0){
            attackerindex = i;
        }
        if(strcmp(allfighters[i].name, defendername) == 0){
            defenderindex = i;
        }
        i++;
    }
    if(attackerindex == defenderindex && attackerindex != -1){
        printf("ERROR: The fighter can't attack itself.\n");
        return allfighters;
    }
    if(attackerindex < 0 || defenderindex < 0){
        printf("ERROR: Did not find a fighter with the entered name.\n");
        return allfighters;
    }
    // Tarkastetaan, onko hyökkääjä tai puolustaja jo kuollut
    else if(allfighters[attackerindex].hp == 0){
        printf("ERROR: The attacker is already dead!\n");
        return allfighters;
    }
    else if(allfighters[defenderindex].hp == 0){
        printf("ERROR: The defender is already dead, that wouldn't be very nice.\n");
        return allfighters;
    }
    else{
        /* Arvotaan vahinkopisteiden määrä välillä [0, hyökkääjän asevahinko], vähennetään tämä puolustajan elämäpisteistä ja annetaan hyökkääjälle 
        kokemuspisteitä vahinkopisteiden verran, puolustajan tappamisesta +5 pistettä lisää */
        int damagedealt = rand() % (allfighters[attackerindex].weapon_damage +1);
        int expgained = damagedealt;
        printf("%s attacked %s with %s by %d damage.\n", allfighters[attackerindex].name, allfighters[defenderindex].name, allfighters[attackerindex].weapon, damagedealt);
        if(damagedealt >= allfighters[defenderindex].hp){
            printf("%s has died!\n", allfighters[defenderindex].name);
            expgained = expgained + 5;
            allfighters[defenderindex].hp = 0;
        } 
        else{
            allfighters[defenderindex].hp = allfighters[defenderindex].hp - damagedealt;
            printf("%s has %d hit points remaining.\n", allfighters[defenderindex].name,allfighters[defenderindex].hp);
        }
        if(expgained > 0){
            printf("%s has gained %d experience points.\n", allfighters[attackerindex].name, expgained);
            allfighters[attackerindex].exp = allfighters[attackerindex].exp + expgained;
        }
        return allfighters;
    }
}


// Tallentaa taistelijat tiedostoon
void write_fighters(char *buffer, Fighter* allfighters)
{
    char filename[31];
    int fighters = 0;
    while(allfighters[fighters].name != NULL){
        fighters++;
    }
    // Tarkistetaan syöte ja että listassa on taistelijoita
    if(sscanf(buffer,"W %s",filename) != 1){
        printf("ERROR: Invalid input for command 'W'.\n");
    }
    else if(fighters == 0){
        printf("ERROR: There are no fighters to save!\n");
    }
    else{
        // Tarkistetaan, että annettu tiedosto on olemassa
        FILE *f = fopen(filename, "w");
        if(!f){
            printf("ERROR: Could not find a file with the name of '%s'.\n", filename);    
        }
        else{
            int i = 0;
            while (allfighters[i].name != NULL)
            {
                fprintf(f, "%s ", allfighters[i].name);
                fprintf(f, "%d ", allfighters[i].hp);
                fprintf(f, "%d ", allfighters[i].exp);
                fprintf(f, "%s ", allfighters[i].weapon);
                fprintf(f, "%d\n",allfighters[i].weapon_damage);

                i++;
            }
            fclose(f);
            printf("Fighters successfully saved to file '%s'.\n", filename);
        }
    }
}


// Lukee taistelijat tiedostosta uuteen listaan structeja, vapauttaa vanhan listan
Fighter* read_fighters(char *buffer, Fighter* allfighters)
{
    char filename[31];
    // Tarkistetaan syöte
    if(sscanf(buffer,"O %s",filename) != 1){
        printf("ERROR: Invalid input for command 'W'.\n");
        return allfighters;
    }
    // Tarkistetaan, että annettu tiedosto on olemassa ja että se ei ole tyhjä
    FILE *f = fopen(filename, "r");
    if(!f){
        printf("ERROR: Could not find a file with the name of '%s'.\n", filename);
        return allfighters;
    }
    else if(f == NULL){
        printf("ERROR: The file contains no fighters.\n");
        fclose(f);
        return allfighters;
    }
    // Lasketaan montako taistelijaa tiedostossa on ja varataan uudelle taistelijoiden listalle muistia
    int c;
	int length = 0;
	while ((c = fgetc(f)) != EOF)
	{
		if (c == '\n')
		{
			length++;
		}
	}
    fseek(f, 0, SEEK_SET);
    Fighter* newfighters = calloc((length +1),  sizeof(Fighter));
    if(!newfighters){
        printf("ERROR: Couldn't allocate memory.\n");
        return allfighters;
    }
    char name[80];
    char weapon[80];
    int i = 0;
    while(feof(f) == 0){
        fscanf(f, "%s", name);
        newfighters[i].name = calloc(strlen(name) +1, sizeof(char));
        strcpy(newfighters[i].name, name);
        fscanf(f, "%d",&newfighters[i].hp);
        fscanf(f, "%d",&newfighters[i].exp);
        fscanf(f, "%s", weapon);
        newfighters[i].weapon = calloc(strlen(weapon) +1, sizeof(char));
        strcpy(newfighters[i].weapon, weapon);
        fscanf(f, "%d\n",&newfighters[i].weapon_damage);
        
        i++;
    }
    // Vapautetaan vanhan listan muisti
    newfighters[i].name = NULL;
    
    fclose(f);
    free_memory(allfighters);
    printf("Fighters successfully read from file '%s'.\n", filename);
    return newfighters;
}


// Tyhjentää sille annetun listan muistin
void free_memory(Fighter * allfighters)
{
    int i = 0;
	while (allfighters[i].name != NULL)
	{
		free(allfighters[i].name);
		free(allfighters[i].weapon);
		i++;
	}
	free(allfighters[i].name);
    free(allfighters);
}