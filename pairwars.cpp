/*
Name: Christian Coulter
Class: Operating Systems CS4328
Date: November 15, 2019
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#define NUM_THREADS 3 // Number of threads due to the 3 test cases.
#define NUM_CARDS 52 // Cards in deck
#define PTHREADM PTHREAD_MUTEX_INITIALIZER
#define PTHREADC PTHREAD_COND_INITIALIZER

// Deck Struct
struct Deck
{
    int card1, card2;
};

// Intialize an empy deck obj
Deck player_hand_1, player_hand_2, player_hand_3;

// Pthread Initialization
pthread_mutex_t mutext_deck_handler = PTHREADM;
pthread_cond_t exclusion_variable = PTHREADC;
pthread_mutex_t mutext_exit_cond = PTHREADM;
pthread_cond_t win_condition = PTHREADC;

// player and dealer init
pthread_t player_threads_handler[NUM_THREADS];
pthread_t dealer_threads_handler;

// output file for the log data
FILE * output_file;

// Declaration of Global Variables
int round_count = 1; //init to 1
int number_of_cases = 3;
int turn_val = 0;

// Other Intit
int deck[NUM_CARDS];
int *beginning_of_deck;
int *end_of_deck;
int seed_value = 0;
bool win_flag = false;

// Generating functions
void arguments(char *[]);
void random_seed_generator();
void generate_deck();
void print_output();

// Player thread functions
void *dealer_thread(void *arg);
void *player_threads(void *current_player_id);

// Deck Logic Handler
void deck_handler(long, Deck);
void shuffle_handler();
void deal_handler();
void round_handler();

// Take in command Arguments
void arguments(char *argv[])
{
   seed_value = atoi(argv[1]);
}

// Generate random value and set to global variable
void random_seed_generator()
{
   srand(seed_value);
}

// Card Logic
void generate_deck()
{
    int card_counter = 0, suit_count = 4, card_at_hand = 0;
        while((card_at_hand < NUM_CARDS) &&
              (card_counter < (NUM_CARDS/suit_count)) )
        {
            for(int i = 0; i < suit_count; i++ )
            {
                deck[card_at_hand] = card_counter;
                card_at_hand++;
            }
            card_counter++;
        }
    beginning_of_deck = deck;
    end_of_deck = deck + 51;
}

// Printing output data for rounds
void round_text()
{
    printf("---------------------------------------------------------------------------------------------------------------\n");
        printf("ROUND: %d\n", round_count);
    printf("---------------------------------------------------------------------------------------------------------------\n");
    fprintf(output_file,"---------------------------------------------------------------------------------------------------------------\n");
        fprintf(output_file, "ROUND: %d\n", round_count);
    fprintf(output_file,"---------------------------------------------------------------------------------------------------------------\n");
}

void round_handler()
{
    // printing out the data
    round_text();

    int round_Player;

    for(long a = 1; a <= NUM_THREADS; a++)
    {
        round_Player = pthread_create(&player_threads_handler[a], NULL, &player_threads, (void *)a);
    }
    // Joining threads
    pthread_join(dealer_threads_handler, NULL);

    for(int b = 0; b < 3; b++)
    {
        pthread_join(player_threads_handler[b], NULL);
    }
}

// Dealer logic
void *dealer_thread(void *arg)
{
    Deck dealerHand;
    long process_id = 0;
    turn_val = 0;
    deck_handler(process_id, dealerHand);

    // Waiting conditions
    pthread_mutex_lock(&mutext_exit_cond);
    while(!win_flag )
    {
        pthread_cond_wait(&win_condition, &mutext_exit_cond);
    }
    pthread_mutex_unlock(&mutext_exit_cond);
    fprintf(output_file, "DEALER: stops round\n");

    // Exit condition
    pthread_exit(NULL);
}


void *player_threads(void *current_player_id)
{
    long process_id = (long)current_player_id;

    Deck players_hand;

// Based upon round counter conditions
    if(round_count == 1)
    {
        if(process_id == 1)
        {
            players_hand = player_hand_1;
        }
        else if(process_id == 2)
        {
            players_hand = player_hand_2;
        }
        else
        {
            players_hand = player_hand_3;
        }
    }
    else if(round_count == 2)
    {
        if(process_id == 2)
        {
            players_hand = player_hand_1;
        }
        else if(process_id == 3)
        {
            players_hand = player_hand_2;
        }
        else
        {
            players_hand = player_hand_3;
        }
    }
    else if(round_count == 3)
    {
        if(process_id == 3)
        {
            players_hand = player_hand_1;
        }
        else if(process_id == 1)
        {
            players_hand = player_hand_2;
        }
        else
        {
            players_hand = player_hand_3;
        }
    }

    // Win flag conditions
    while(win_flag == 0)
    {
        pthread_mutex_lock(&mutext_deck_handler);
        while(process_id != turn_val && win_flag == 0)
        {
            pthread_cond_wait(&exclusion_variable, &mutext_deck_handler);
        }
        if(win_flag == 0)
        {
            deck_handler(process_id, players_hand);
        }
        pthread_mutex_unlock(&mutext_deck_handler);
    }

    fprintf(output_file, "PLAYER %ld: round completed\n", process_id);
    pthread_exit(NULL); //pthread exit condition
}

// Main deck logic in which handles wins and losses
void deck_handler(long process_id, Deck thisHand)
{
    if(process_id == 0)
    {
        fprintf(output_file, "DEALER: shuffle\n"); shuffle_handler();
        fprintf(output_file, "DEALER: deal\n"); deal_handler();
    }
    else
    {
        fprintf(output_file, "PLAYER %ld: hand %d \n", process_id, thisHand.card1);

        thisHand.card2 = *beginning_of_deck;
        beginning_of_deck = beginning_of_deck + 1;
        fprintf(output_file, "PLAYER %ld: draws %d \n", process_id,thisHand.card2);

        printf("HAND %d %d\n", thisHand.card1, thisHand.card2);
        fprintf(output_file, "PLAYER %ld: hand %d %d\n", process_id, thisHand.card1, thisHand.card2);

        if(thisHand.card1 == thisHand.card2)
        {
            printf("PLAYER WIN yes\n");
            fprintf(output_file, "PLAYER %ld: wins\n", process_id);
            win_flag = true;
            pthread_cond_signal(&win_condition);
        }
        else
        {
            printf("WIN no\n");

            beginning_of_deck = beginning_of_deck - 1;
            int *pointer_value = beginning_of_deck;
            while(pointer_value != end_of_deck)
            {
                *pointer_value = *(pointer_value + 1);
                pointer_value = pointer_value + 1;
            }

            int discard_cards = rand()%2;
            if( discard_cards == 0 )
            {
                fprintf(output_file, "PLAYER %ld: discards %d \n", process_id, thisHand.card1);
                *end_of_deck = thisHand.card1;
                thisHand.card1 = thisHand.card2;
            }
            else
            {
                fprintf(output_file, "PLAYER %ld: discards %d \n", process_id, thisHand.card2);
                *end_of_deck = thisHand.card2;
            }

            print_output();
        }
    }

    turn_val ++;
    if(turn_val > NUM_THREADS)
    {
        turn_val = 1;
    }
    pthread_cond_broadcast(&exclusion_variable);
}

// Shuffling logic
void shuffle_handler()
{
    for(int i = 0; i < (NUM_CARDS - 1); i++ )
    {
        int random_position = i + (rand() % (NUM_CARDS - i));
        int temp_val = deck[i];
        deck[i] = deck[random_position];
        deck[random_position] = temp_val;
    }
}

void deal_handler()
{
    player_hand_1.card1 = *beginning_of_deck;
    beginning_of_deck = beginning_of_deck + 1;
    player_hand_2.card1 = *beginning_of_deck;
    beginning_of_deck = beginning_of_deck + 1;
    player_hand_3.card1 = *beginning_of_deck;
    beginning_of_deck = beginning_of_deck + 1;
}

// Output logic per project specs
void print_output()
{
    printf("Deck : ");
    fprintf(output_file, "DECK: ");
    int *pointer_value = beginning_of_deck;
    while(pointer_value != end_of_deck)
    {
        printf("%d ", *pointer_value);
        fprintf(output_file, "%d ", *pointer_value);
        pointer_value++;
        if(pointer_value == end_of_deck)
        {
            printf("%d \n", *pointer_value);
            fprintf(output_file, "%d \n", *pointer_value);
        }
    }
}

// Main function
int main(int argc, char *argv[]){
    output_file = fopen("testcases.txt", "a");

    // Value intialization
    arguments(argv);
    random_seed_generator();
    generate_deck();

    // Round counters
    while(round_count <= number_of_cases)
    {
        round_handler();
        round_count++;
        win_flag = false;
    }

    // Exit conditions
    fclose(output_file);
    exit(EXIT_SUCCESS);
}
