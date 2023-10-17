#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"

/*
 * I've left these definitions in from the
 * solution program. You don't have to
 * use them, but the `dupe_check()` function
 * unit test expects certain values to be
 * returned for certain situations!
 */
#define DUPE -1
#define NO_DUPE -2

/*
 * These are the special strings you need to
 * print in order for the text in the terminal
 * to be bold, italic or normal (end)
 */
#define BOLD "\e[1m"
#define ITALIC "\e[3m"
#define END "\e[0m"

/*
 * You will have to implement all of these functions
 * as they are specifically unit tested by Mimir
 */
int dupe_check(unsigned, char*);
char *fix_text(char*);
void free_card(CARD_T*);
CARD_T *parse_card(char*);
void print_card(CARD_T*);
int comparator(const void *a, const void *b) {
	CARD_T *a_ptr = *((CARD_T**)a);
	CARD_T *b_ptr = *((CARD_T**)b);
	return strcmp(a_ptr -> name, b_ptr -> name);
}
/*
 * We'll make these global again, to make
 * things a bit easier
 */
CARD_T **cards = NULL;
size_t total_cards = 0;

int main(int argc, char ** argv) {
  // TODO: 1. Open the file
  //       2. Read lines from the file...
  //          a. for each line, `parse_card()`
  //          b. add the card to the array
  //       3. Sort the array
  //       4. Print and free the cards
  //       5. Clean up!
  if (argc < 2) {
    fprintf(stderr, "Usage: %s file\n", argv[0]);
    return 1;
  }

  // 1. Open the file
  char * filename = argv[1];
  FILE * file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  // 2. Read lines from the file
  char line[MAX_LINE_LENGTH];
  while (fgets(line, MAX_LINE_LENGTH, file)) {
    // 2a. Parse the card
    CARD_T * card = parse_card(line);

    // 2b. Add the card to the array
    cards = realloc(cards, sizeof(CARD_T * ) * (total_cards + 1));
    cards[total_cards] = card;
    total_cards++;
  }

  // 3. Sort the array (using qsort function)
  qsort(cards, total_cards, sizeof(CARD_T * ), compare_cards);

  // 4. Print and free the cards
  for (int i = 0; i < total_cards; i++) {
    print_card(cards[i]);
    free_card(cards[i]);
  }

  // 5. Clean up
  free(cards);
  fclose(file);
  return 0;
}
/*
 * This function has to return 1 of 3 possible values:
 *     1. NO_DUPE (-2) if the `name` is not found
 *        in the `cards` array
 *     2. DUPE (-1) if the `name` _is_ found in
 *        the `cards` array and the `id` is greater
 *        than the found card's id (keep the lowest one)
 *     3. The last case is when the incoming card has
 *        a lower id than the one that's already in
 *        the array. When that happens, return the
 *        index of the card so it may be removed...
 */
int dupe_check(unsigned id, char* name) {
    for (size_t i = 0; i < total_cards; i++) {
        if (strcmp(cards[i]->name, name) == 0) {
            if (cards[i]->id >= id) {
                return DUPE;
            } else {
                return NO_DUPE;
            }
        }
    }
    return NO_DUPE;
}

/*
 * This function has to do _five_ things:
 *     1. replace every "" with "
 *     2. replace every \n with `\n`
 *     3. replace every </b> and </i> with END
 *     4. replace every <b> with BOLD
 *     5. replace every <i> with ITALIC
 *
 * The first three are not too bad, but 4 and 5
 * are difficult because you are replacing 3 chars
 * with 4! You _must_ `realloc()` the field to
 * be able to insert an additional character else
 * there is the potential for a memory error!
 */
char *fix_text(char *text) {
    // Create an empty string called result
    char *result = malloc(1);
    result[0] = '\0';

    // Iterate through the characters of the text string
    for (int i = 0; i < strlen(text); i++) {
        // Check if the character is ""
        if (text[i] == '"' && text[i + 1] == '"') {
            result = realloc(result, strlen(result) + 2);
            strcat(result, "\"");
            i++;
            continue;
        }

        // Check if the character is \n
        if (text[i] == '\\' && text[i + 1] == 'n') {
            result = realloc(result, strlen(result) + 3);
            strcat(result, "\\n");
            i++;
            continue;
        }

        // Check if the character is </b> or </i>
        if (text[i] == '<' && text[i + 1] == '/' && (text[i + 2] == 'b' || text[i + 2] == 'i') && text[i + 3] == '>') {
            result = realloc(result, strlen(result) + 4);
            strcat(result, "END");
            i += 3;
            continue;
        }

        // Check if the character is <b>
        if (text[i] == '<' && text[i + 1] == 'b' && text[i + 2] == '>') {
            result = realloc(result, strlen(result) + 5);
            strcat(result, "BOLD");
            i += 2;
            continue;
        }

        // Check if the character is <i>
        if (text[i] == '<' && text[i + 1] == 'i' && text[i + 2] == '>') {
            result = realloc(result, strlen(result) + 6);
            strcat(result, "ITALIC");
            i += 2;
            continue;
        }

        // Append the character to the result string as is
        result = realloc(result, strlen(result) + 2);
        result[strlen(result)] = text[i];
        result[strlen(result) + 1] = '\0';
    }

    // Allocate enough memory for the result string
    result = realloc(result, strlen(result) + 1);

    // Return the result string
    return result;
}

/*
 * This short function simply frees both fields
 * and then the card itself
 */
void free_card(CARD_T *card) {
    // Check if the card pointer is NULL
    //if (card == NULL) return;

    // Free the card structure itself
    //free(card);
    
}
Note: Make sure to include the necessary header files for the CARD_T structure and the free() funct

/*
 * This is the tough one. There will be a lot of
 * logic in this function. Once you have the incoming
 * card's id and name, you should call `dupe_check()`
 * because if the card is a duplicate you have to
 * either abort parsing this one or remove the one
 * from the array so that this one can go at the end.
 *
 * To successfully parse the card text field, you
 * can either go through it (and remember not to stop
 * when you see two double-quotes "") or you can
 * parse backwards from the end of the line to locate
 * the _fifth_ comma.
 *
 * For the fields that are enum values, you have to
 * parse the field and then figure out which one of the
 * values it needs to be. Enums are just numbers!
 */
CARD_T *parse_card(char *line) {
  // allocate memory for the card and initialize string parser
  CARD_T *card = malloc(sizeof(CARD_T));
  char *stringp = strdup(line);

  // parse id
  char *token = strsep(&stringp,",");
  card->id = atoi(token);

  // parse name
  token = strsep(&stringp, "\"");
  card->name = strdup(token);
  strsep(&stringp, "\""); // skip the closing double quote

  // parse cost
  token = strsep(&stringp, ",");
  card->cost = atoi(token);

  // parse text
  token = strsep(&stringp,"\"");
  if(token[0] == ','){
    card->text = strdup("");
  }
  else {
    // concatenate the text field until we find the closing double quote
    char temp[50000] = "";
    strcat(temp, token);
    strcat(temp, "\""); // add the opening double quote that was removed by strsep
    while (true) {
      token = strsep(&stringp, "\"");
      strcat(temp, token);
      if (strlen(token) > 0) { // found the closing double quote
        break;
      }
      // if we didn't find the closing double quote, add a space and continue
      strcat(temp, " ");
    }
    card->text = strdup(temp);
  }

  // parse attack
  token = strsep(&stringp, ",");
  card->attack = atoi(token);

  // parse health
  token = strsep(&stringp, ",");
  card->health = atoi(token);
  
  
  // parse type
  token = strsep(&stringp, ",");
  if (strcmp(token, "MINION") == 0) {
    card->type = MINION;
  } else if (strcmp(token, "SPELL") == 0) {
    card->type = SPELL;
  } else if (strcmp(token, "WEAPON") == 0) {
    card->type = WEAPON;
  } else if (strcmp(token, "HERO") == 0) {
    card->type = HERO;
  } else {
    card->type = INVALID;
  }
  
  // parse class
  token = strsep(&stringp, ",");
  if (strcmp(token, "DEMONHUNTER") == 0) {
    card->class = DEMONHUNTER;
  } else if (strcmp(token, "DRUID") == 0) {
    card->class = DRUID;
  } else if (strcmp(token, "HUNTER") == 0) {
    card->class = HUNTER;
  } else if (strcmp(token, "MAGE") == 0) {
    card->class = MAGE;
  } else if (strcmp(token, "NEUTRAL") == 0) {
    card->class = NEUTRAL;
  } else if (strcmp(token, "PALADIN") == 0) {
    card->class = PALADIN;
  } else if (strcmp(token, "PRIEST") == 0) {
    card->class = PRIEST;
  } else if (strcmp(token, "ROGUE") == 0) {
    card->class = ROGUE;
  } else if (strcmp(token, "SHAMAN") == 0) {
    card->class = SHAMAN;
  } else if (strcmp(token, "WARLOCK") == 0) {
    card->class = WARLOCK;
  } else if (strcmp(token, "WARRIOR") == 0) {
    card->class = WARRIOR;
  }
  
  //parse rarity
  token = strsep(&stringp, ",");
  if (strcmp(token, "FREE") == 0) {
    card->rarity = FREE;
  } else if (strcmp(token, "COMMON") == 0) {
    card->rarity = COMMON;
  } else if (strcmp(token, "EPIC") == 0) {
    card->rarity = EPIC;
  } else if (strcmp(token, "RARE") == 0) {
    card->rarity = RARE;
  } else if (strcmp(token, "LEGENDARY") == 0) {
    card->rarity = LEGENDARY;
  }
    
    
/*
 * Because getting the card class centered is such
 * a chore, you can have this function for free :)
 */
void print_card(CARD_T *card) {
	printf("%-29s %2d\n", card->name, card->cost);
	unsigned length = 15 - strlen(class_str[card->class]);
	unsigned remainder = length % 2;
	unsigned margins = length / 2;
	unsigned left = 0;
	unsigned right = 0;
	if (remainder) {
		left = margins + 2;
		right = margins - 1;
	} else {
		left = margins + 1;
		right = margins - 1;
	}
	printf("%-6s %*s%s%*s %9s\n", type_str[card->type], left, "", class_str[card->class], right, "", rarity_str[card->rarity]);
	printf("--------------------------------\n");
	printf("%s\n", card->text);
	printf("--------------------------------\n");
	printf("%-16d%16d\n\n", card->attack, card->health);
}

