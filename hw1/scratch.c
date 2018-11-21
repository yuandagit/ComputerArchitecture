// Number of bytes in a commit id
#define COMMIT_ID_BYTES 40


---- ---- ---- ---- ---- ----



- - - - -
'6', '1', 'c'
 2     1   0
a(3^4) + b(3^3) + c(3^2) + d(3^1) + e(3^0)

1*3^4 + 2*(3^2)+1*(3^0)\
100/3 = 33, mod = 1
33/3 = 11, mod = 0
11/3 = 3, mod = 2
3/3 = 1, mod = 0
1/3 = 0, mod = 1
10201
1*(3^4) + 2*(3^2) + 1*(3^0)



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
