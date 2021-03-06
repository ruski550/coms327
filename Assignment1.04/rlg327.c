#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h> 

/* Very slow seed: 686846853 */

#include "dungeon.h"
#include "path.h"

//Pointer of monster structures 
monster_t *monsters_pointer;

srand(time(NULL));

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
          "          [-p|--pc <y> <x>]\n",
          name);

  exit(-1);
}
//Initializes the monster structs in the struct array that holds the monsters 
//TODO add a current turn int for each person
//make the pc a monster. Just don't initialize the monster specific stuff for the pc
void make_monsters(monster_t &monster_pointer, int nummons){ 
	for (int monster_number =0; i < nummons; i++){
	monster_pointer[monster_number].speed = rand() % 16 + 5;
	setMonsterAbilities(monster_pointer[monster_number])
	}
}



//TODO create a function to manipulate  the abilities aspect of the monster mash struct. 
//TODO does this need to a pointer?
void setMonsterAbilities(monster_t monster){
	for (int num =0; num < 3; num++){

		monster.abilities[num] = rand % 2;
	}
}
	


//Creates a monster using a hex value to deal with it's different attributes.
//TODO generate monster
//TODO generate way to hold the monsters
//Creates the monsters, randomly assigns the attirubtes needed. 50% chance of each.
typedef struct monsterMash{

	//TODO might be worth looking into making these into variables as opposed to array
	unsigned int abilities[4];
	char symbol;
	int speed;
	pair_t pc_location;//Last known location of the player
	pair_t self_location;//Location of the monster
	int turn_num;//Holds the number turn it is
	int speed;
	
}monster_t;


//Moves the player position a max of 1 unit in any given direction if the terrain is a room or corridor
void movePC(pc_t player, dungeon_t *d){
	
	srand(time(NULL));
	//Randomly decides whether the player is going to take a step in a direction
	yrand = rand() % 3 -1;
	xrand = rand() % 3 -1;
	//Makes sure that step is valid by checking terrain vlaue and making sure it is not a rock or object that the player cannot go on if it allowed, changed the values in the position array
	//TODO check if pc in dungeon is what is being used for the pc.
	
	if((d->map[player.position[dim_x] + xrand][player.position[dim_y]+yrand]) >= ter_floor ){
	
		player.position[dim_x] = player.position[dim_x] + xrand;
		player.position[dim_y] = player.position[dim_y] + yrand;
	}

}


int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed,
           do_save_image, do_place_pc;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;
  
  //Hold the number of monsters
	monsters_pointer = malloc(sizeOf(monster)*nummons);
	monsters_pointer[2].turns = 0;


  //Gives space to the pointer that holds the monsters
  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed =
    do_save_image = do_place_pc = 0;
  do_seed = 1;
  save_file = load_file = NULL;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
    * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */
 
 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
	//Reads in the number of monsters to create 
	case 'n':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
	      //TODO the the variables to the final location of the monster pointer
              !sscanf(argv[i], "%d", nummons ) ) {
            usage(argv[0]);
          }
          break;

        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll save to it.  If it is "seed", we'll save to    *
	     * <the current seed>.rlg327.  If it is "image", we'll  *
	     * save to <the current image>.rlg327.                  */
	    if (!strcmp(argv[++i], "seed")) {
	      do_save_seed = 1;
	      do_save_image = 0;
	    } else if (!strcmp(argv[i], "image")) {
	      do_save_image = 1;
	      do_save_seed = 0;
	    } else {
	      save_file = argv[i];
	    }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
        case 'p':
          /* PC placement makes no effort to avoid placing *
           * the PC inside solid rock.                     */
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-pc"))) {
            usage(argv[0]);
          }
          if ((d.pc.position[dim_y] = atoi(argv[++i])) < 1 ||
              d.pc.position[dim_y] > DUNGEON_Y - 2         ||
              (d.pc.position[dim_x] = atoi(argv[++i])) < 1 ||
              d.pc.position[dim_x] > DUNGEON_X - 2)         {
            fprintf(stderr, "Invalid PC position.\n");
            usage(argv[0]);
          }
          do_place_pc = 1;
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }
 
	//Holds the number of monsters
	monsters_pointer = malloc(sizeOf(monster)*nummons);
	monsters_pointer[2].turns = 0;



  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  if (!do_load && !do_image) {
    printf("Seed is %ld.\n", seed);
  }
  srand(seed);

  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  if (!do_place_pc) {
    i = rand() % d.num_rooms;
    d.pc.position[dim_x] = (d.rooms[i].position[dim_x] +
                            (rand() % d.rooms[i].size[dim_x]));
    d.pc.position[dim_y] = (d.rooms[i].position[dim_y] +
                            (rand() % d.rooms[i].size[dim_y]));
  }

  printf("PC is at (y, x): %d, %d\n",
         d.pc.position[dim_y], d.pc.position[dim_x]);

  render_dungeon(&d);

  dijkstra(&d);
  dijkstra_tunnel(&d);
  render_distance_map(&d);
  render_tunnel_distance_map(&d);

  if (do_save) {
    if (do_save_seed) {
       /* 10 bytes for number, please dot, extention and null terminator. */
      save_file = malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image) {
      if (!pgm_file) {
	fprintf(stderr, "No image file was loaded.  Using default.\n");
	do_save_image = 0;
      } else {
	/* Extension of 3 characters longer than image extension + null. */
	save_file = malloc(strlen(pgm_file) + 4);
	strcpy(save_file, pgm_file);
	strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);

    if (do_save_seed || do_save_image) {
      free(save_file);
    }
  }

  delete_dungeon(&d);

  return 0;
}
