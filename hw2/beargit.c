#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);

  FILE* fbranches = fopen(".beargit/.branches", "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);

  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  write_string_to_file(".beargit/.current_branch", "master");

  return 0;
}


/* beargit add <filename>
 *
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}


/* beargit rm <filename>
 *
 * See "Step 2" in the homework 1 spec.
 *
 */

 int beargit_rm(const char* filename) {
    FILE* findex = fopen(".beargit/.index", "r");
    FILE *fnewindex = fopen(".beargit/.newindex", "w");
    char line[FILENAME_SIZE];
    int flag = 0;

    while (fgets(line, sizeof(line), findex)) {
       strtok(line, "\n");
       if (strcmp(line, filename) == 0) {
           flag = 1;
       }
       else {
         fprintf(fnewindex, "%s\n", line);
       }
    }

    if (flag != 1) {
      fprintf(stderr, "ERROR: File %s not tracked.\n", filename);
    }
    else {
      printf("\n");
      printf("%s is removed from the index.\n", filename);
      printf("\n");
      fs_rm(".beargit/.index");
      fs_mv(".beargit/.newindex", ".beargit/.index");

    }
    fclose(findex);
    fclose(fnewindex);

   return 0;
  }


/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char* go_bears = "GO BEARS!";

int is_commit_msg_ok(const char* msg) {
  if(strcmp(msg, go_bears) == 0) {
    return 1;
  }
  return 0;
}

void next_commit_id(char* commit_id, const char* msg) {
  char first_line[COMMIT_ID_SIZE] = "0000000000000000000000000000000000000000";
  strtok(commit_id, "\n");
  int id_arr[COMMIT_ID_SIZE];
  int i;
  int index = 0;
  int rmd = 0;
  int first = 0;


      for (i = COMMIT_ID_SIZE - 2; i >= 0; i--) {
          if (commit_id[i] == 'c') {
              id_arr[i] = 0;
          }
          else if (commit_id[i] == '1') {
              id_arr[i] = 1;
          }
          else if (commit_id[i] == '6') {
              id_arr[i] = 2;
          } else {
              id_arr[i] = 0;
          }

          index = index + id_arr[i]*pow(3, COMMIT_ID_SIZE - 2 - i);
          id_arr[i] = id_arr[i]*pow(3, COMMIT_ID_SIZE - 2 - i);

      }


  for (i = 0; i < pow(3, COMMIT_ID_SIZE - 2) - 1; i++) {
      if (index == i) {
        index++;
        break;
      }
  }

  i = COMMIT_ID_SIZE - 2;
  while (index > 0) {
    rmd = index % 3;

    if (rmd == 0) {
        first_line[i] = 'c';
    }
    else if (rmd == 1) {
        first_line[i] = '1';
    }
    else if (rmd == 2) {
        first_line[i] = '6';
    }
    index = index / 3;
    i--;
  }
  printf("This is now first line: %s\n", first_line);


  char bear_dir[10] = ".beargit/";
  char new_dir[1 + 9 + COMMIT_ID_SIZE - 2];


  for (i = 0; i < 1 + 9 + COMMIT_ID_SIZE; i++) {
      if (i < 9) {
          new_dir[i] = bear_dir[i];
      }
      else {
          new_dir[i] = first_line[i - 9];
      }
  }
  printf("%s\n", new_dir);
  printf("Making new directory now.\n");
  fs_mkdir(new_dir);
  char new_index[9 + COMMIT_ID_SIZE - 2 + 7];
  char new_prev[9 + COMMIT_ID_SIZE - 2 + 5];
  char new_msg[9 + COMMIT_ID_SIZE - 2 + 4];
  char index_file[9] = "/.index";
  char prev_file[7] = "/.prev";
  char msg_file[6] = "/.msg";

  for (i = 0; i < 57; i ++) {
    if (i < 1 + 9 + COMMIT_ID_SIZE - 2 ) {
        new_index[i] = new_dir[i];
        new_prev[i] = new_dir[i];
        new_msg[i] = new_dir[i];
    }
    else {
        if (i == 54) {
          new_index[i] = index_file[i - 49];
          new_prev[i] = prev_file[i - 49];
        }
        else if (i == 55){
          new_index[i] = index_file[i - 49];
        }
        else {
          new_msg[i] = msg_file[i - 49];
          new_prev[i] = prev_file[i - 49];
          new_index[i] = index_file[i - 49];
        }

    }
  }


  FILE* new_msg_file = fopen(new_msg, "w");
  FILE* new_index_file = fopen(new_index, "w");
  FILE* new_prev_file = fopen(new_prev, "w");
  fs_cp(".beargit/.index", new_index);

  FILE* findex = fopen(".beargit/.index", "r");
  FILE* fprev = fopen(".beargit/.prev", "r");
  char line[FILENAME_SIZE];
  char new_file[FILENAME_SIZE];
  int j;
  while (fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    fprintf(new_index_file, "%s\n", line);
    for (i = 0; new_dir[i] != '\0'; i++) {
        new_file[i] = new_dir[i];
    }
    new_file[i] = '/';
    i++;
    for (j = 0; line[j] != '\0'; j++, i++) {
        new_file[i] = line[j];
    }
    new_file[i] = '\0';
    fs_cp(line, new_file);
  }

  while (fgets(line, sizeof(line), fprev)) {
    strtok(line, "\n");
    fprintf(new_prev_file, "%s\n", line);
  }

  fclose(findex);
  fclose(fprev);
  fclose(new_prev_file);
  fclose(new_index_file);
  fprintf(new_msg_file, "%s\n", msg);
  fs_cp(".beargit/.prev", new_prev);
  write_string_to_file(".beargit/.prev", first_line);

}


int beargit_commit(const char* msg) {
  char line[COMMIT_ID_SIZE];
  char commit_id[COMMIT_ID_SIZE];
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }


  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  printf("%s\n", commit_id);
  next_commit_id(commit_id, msg);



  return 0;
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

 int beargit_status() {
   int count = 0;
   FILE* findex = fopen(".beargit/.index", "r");
   char line[FILENAME_SIZE];

   printf("Tracked files: \n");
   while(fgets(line, sizeof(line), findex) != NULL) {
       strtok(line, "\n");
       count++;
       printf("   %d. %s\n", count, line);
   }
   fclose(findex);

   printf("%d file(s) total.\n", count);
   return 0;
 }

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */

 int beargit_log() {
   char base_case[COMMIT_ID_SIZE] = "0000000000000000000000000000000000000000";
   char commit_id[COMMIT_ID_SIZE];
   read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
   char new_prev[8 + COMMIT_ID_SIZE + 6] = ".beargit/";
   char new_msg[8 + COMMIT_ID_SIZE + 5] = ".beargit/";
   char prev_file[7] = "/.prev";
   char msg_file[6] = "/.msg";
   char commit_msg[MSG_SIZE];
   int i;
   int j;

   if(strcmp(commit_id, base_case) == 0) {
       fprintf(stderr, "ERROR: There are no commits! \n");
       return 1;
   }
   else {
     printf("\n");
     while (strcmp(commit_id, base_case) != 0) {
       for (i = 0; i <  COMMIT_ID_SIZE + 8; i++) {
           new_prev[i + 9] = commit_id[i];
           new_msg[i + 9] = commit_id[i];
       }

       for (j = 0; j < 7; j++, i++) {
         if (j == 6) {
           new_prev[i] = prev_file[j];
         } else {
           new_prev[i] = prev_file[j];
           new_msg[i] = msg_file[j];
         }
       }
       new_msg[i - 1] = '\0';
       new_prev[i] = '\0';

       read_string_from_file(new_msg, commit_msg, MSG_SIZE);
       fprintf(stdout, "Commit id is: %s\n", commit_id);
       fprintf(stdout, "Message is: %s\n", commit_msg);
       read_string_from_file(new_prev, commit_id, COMMIT_ID_SIZE);

     }
   }



   return 0;
 }

// ---------------------------------------
// HOMEWORK 2
// ---------------------------------------

// This adds a check to beargit_commit that ensures we are on the HEAD of a branch.
int beargit_commit(const char* msg) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  if (strlen(current_branch) == 0) {
    fprintf(stderr, "ERROR: Need to be on HEAD of a branch to commit\n");
    return 1;
  }

  return beargit_commit_hw1(msg);
}

const char* digits = "61c";

void next_commit_id(char* commit_id) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  // The first COMMIT_ID_BRANCH_BYTES=10 characters of the commit ID will
  // be used to encode the current branch number. This is necessary to avoid
  // duplicate IDs in different branches, as they can have the same pre-
  // decessor (so next_commit_id has to depend on something else).
  int n = get_branch_number(current_branch);
  for (int i = 0; i < COMMIT_ID_BRANCH_BYTES; i++) {
    // This translates the branch number into base 3 and substitutes 0 for
    // 6, 1 for 1 and c for 2.
    commit_id[i] = digits[n%3];
    n /= 3;
  }

  // Use next_commit_id to fill in the rest of the commit ID.
  next_commit_id_hw1(commit_id + COMMIT_ID_BRANCH_BYTES);
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");

  int branch_index = -1;
  int counter = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, branch_name) == 0) {
      branch_index = counter;
    }
    counter++;
  }

  fclose(fbranches);

  return branch_index;
}

/* beargit branch
 *
 * See "Step 6" in the homework 1 spec.
 *
 */

int beargit_branch() {
  /* COMPLETE THE REST */

  return 0;
}

/* beargit checkout
 *
 * See "Step 7" in the homework 1 spec.
 *
 */

int checkout_commit(const char* commit_id) {
  /* COMPLETE THE REST */
  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  /* COMPLETE THE REST */
  return 1;
}

int beargit_checkout(const char* arg, int new_branch) {
  // Get the current branch
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", "current_branch", BRANCHNAME_SIZE);

  // If not detached, update the current branch by storing the current HEAD into that branch's file...
  // Even if we cancel later, this is still ok.
  if (strlen(current_branch)) {
    char current_branch_file[BRANCHNAME_SIZE+50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(".beargit/.prev", current_branch_file);
  }

  // Check whether the argument is a commit ID. If yes, we just stay in detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE] = ".beargit/";
    strcat(commit_dir, arg);
    if (!fs_check_dir_exists(commit_dir)) {
      fprintf(stderr, "ERROR: Commit %s does not exist\n", arg);
      return 1;
    }

    // Set the current branch to none (i.e., detached).
    write_string_to_file(".beargit/.current_branch", "");

    return checkout_commit(arg);
  }

  // Just a better name, since we now know the argument is a branch name.
  const char* branch_name = arg;

  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(branch_name) >= 0);

  // Check for errors.
  if (!(!branch_exists || !new_branch)) {
    fprintf(stderr, "ERROR: A branch named %s already exists\n", branch_name);
    return 1;
  } else if (!branch_exists && new_branch) {
    fprintf(stderr, "ERROR: No branch %s exists\n", branch_name);
    return 1;
  }

  // File for the branch we are changing into.
  char* branch_file = ".beargit/.branch_";
  strcat(branch_file, branch_name);

  // Update the branch file if new branch is created (now it can't go wrong anymore)
  if (new_branch) {
    FILE* fbranches = fopen(".beargit/.branches", "a");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    fs_cp(".beargit/.prev", branch_file);
  }

  write_string_to_file(".beargit/.current_branch", branch_name);

  // Read the head commit ID of this branch.
  char branch_head_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);

  // Check out the actual commit.
  return checkout_commit(branch_head_commit_id);
}
