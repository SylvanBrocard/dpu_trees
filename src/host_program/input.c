/**
 * @file input.c
 * @author Sylvan Brocard (sbrocard@upmem.com)
 * @brief Utility functions for input parsing
 *
 */

#include "../trees.h"

/**
 * @brief Removes the extension from a file name.
 *
 * @param fname [in] The file name string.
 */
static void strip_ext(char *fname)
{
    char *end = fname + strlen(fname);

    while (end > fname && *end != '.')
        --end;

    if (end > fname)
        *end = '\0';
}

/**
 * @brief Allocates memory for input data.
 */
static void alloc_features_and_int_targets(
    Params *p,             /**< Algorithm parameters */
    float ***features_out, /**< [out] Vector of features. */
    int **targets_out)     /**< [out] Vector of targets. */
{
    float **features;
    int *targets;

    /* rounding the size of the input to the smallest multiple of 8*ndpu larger
     * than npoints */
    p->npadded = ((p->npoints + 8 * p->ndpu - 1) / (8 * p->ndpu)) * 8 * p->ndpu;

    /* allocate space for features[][] */
    features = (float **)malloc(p->npadded * sizeof(*features));
    features[0] =
        (float *)malloc(p->npadded * p->nfeatures * sizeof(**features));
    for (int ipoint = 1; ipoint < p->npadded; ipoint++)
        features[ipoint] = features[ipoint - 1] + p->nfeatures;

    /* checking that we managed to assign enough memory */
    if (!features[0]) {
        perror("malloc features[0]");
        exit(EXIT_FAILURE);
    }

    /* allocate space for targets[] */
    targets = (int *)malloc(p->npadded * sizeof(*targets));

    /* checking that we managed to assign enough memory */
    if (!targets[0]) {
        perror("malloc targets");
        exit(EXIT_FAILURE);
    }

    *features_out = features;
    *targets_out = targets;
}

/**
 * @brief Reads a binary input file with integer targets from disk.
 */
void read_bin_input_with_int_targets(
    Params *p,             /**< Algorithm parameters */
    const char *filename,  /**< [in] The file name. */
    float ***features_out, /**< [out] Vector of features. */
    int **targets_out)     /**< [out] Vector of targets. */
{
    float **features;
    int *targets;

    FILE *infile;
    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Error: no such file (%s)\n", filename);
        exit(1);
    }

    /* get nfeatures and npoints */
    fread(&p->npoints, sizeof(uint64_t), 1, infile);
    fread(&p->nfeatures, sizeof(int), 1, infile);

    alloc_features_and_int_targets(p, &features, &targets);

    fread(features[0], sizeof(**features), p->npoints * p->nfeatures, infile);
    fread(targets, sizeof(*targets), p->npoints, infile);

    fclose(infile);

    *features_out = features;
    *targets_out = targets;
}

/**
 * @brief Reads a text input file with int targets from disk.
 */
void read_txt_input_with_int_target(
    Params *p,             /**< Algorithm parameters */
    const char *filename,  /**< [in] The file name. */
    float ***features_out, /**< [out] Vector of features. */
    int **targets_out)     /**< [out] Vector of targets. */
{
    char line[1024];
    float **features;
    int *targets;

    FILE *infile;
    if ((infile = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Error: no such file (%s)\n", filename);
        exit(1);
    }
    while (fgets(line, 1024, infile) != NULL)
        if (strtok(line, " \t\n") != 0)
            p->npoints++;
    rewind(infile);
    while (fgets(line, 1024, infile) != NULL) {
        if (strtok(line, " \t\n") != 0) {
            /* ignore the id (first attribute): nfeatures = 1; */
            while (strtok(NULL, " ,\t\n") != NULL)
                p->nfeatures++;
            break;
        }
    }
    p->nfeatures--;

    alloc_features_and_int_targets(p, &features, &targets);

    rewind(infile);
    {
        uint64_t ifeature_global = 0;
        uint64_t itarget = 0;
        while (fgets(line, 1024, infile) != NULL) {
            if (strtok(line, " \t\n") == NULL)
                continue;
            for (int ifeature = 0; ifeature < p->nfeatures; ifeature++) {
                features[0][ifeature_global] = atof(strtok(NULL, " ,\t\n"));
                ifeature_global++;
            }
            targets[itarget] = atof(strtok(NULL, " ,\t\n"));
            itarget++;
        }
    }
    fclose(infile);

    *features_out = features;
    *targets_out = targets;
}

/**
 * @brief Saves the input data in a binary file for faster access next time.
 *
 * @param p Algorithm parameters.
 * @param filename_in [in] Name of the input text file.
 * @param features [npoints][nfeatures] Feature array.
 */
void save_dat_file_with_int_target(
    Params *p,               /**< Algorithm parameters */
    const char *filename_in, /**< [in] Name of the input text file. */
    float **features,        /**< [in] Feature array. */
    int *targets)            /**< Targets array. */
{
    char *filename = strdup(filename_in);
    char suffix[] = ".dat";

    int n = strlen(filename) + strlen(suffix);
    char *dat_name = (char *)malloc(n * sizeof(*dat_name));

    strcpy(dat_name, filename);
    strip_ext(dat_name);
    strcat(dat_name, ".dat");

    printf("Writing points in binary format to %s\n", dat_name);

    FILE *binfile;
    binfile = fopen(dat_name, "wb");
    fwrite(&p->npoints, sizeof(p->npoints), 1, binfile);
    fwrite(&p->nfeatures, sizeof(p->nfeatures), 1, binfile);
    fwrite(features[0], sizeof(*features[0]), p->npoints * p->nfeatures,
           binfile);
    fwrite(targets, sizeof(*targets), p->npoints, binfile);
    fclose(binfile);

    free(filename);
    free(dat_name);
}

/**
 * @brief Formats a flat array into a bidimensional representation
 */
void format_array_input(
    Params *p,             /**< Algorithm parameters. */
    float *data,           /**< [in] The data as a flat table */
    float ***features_out) /**< [out] The data as two dimensional table */
{
    // uint64_t npadded;
    p->npadded = ((p->npoints + 8 * p->ndpu - 1) / (8 * p->ndpu)) * 8 * p->ndpu;

    float **features = (float **)malloc(p->npadded * sizeof(*features));
    features[0] = data;
    for (int ipoint = 1; ipoint < p->npadded; ipoint++)
        features[ipoint] = features[ipoint - 1] + p->nfeatures;

    *features_out = features;
}
