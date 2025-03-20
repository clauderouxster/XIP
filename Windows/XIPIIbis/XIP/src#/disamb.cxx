/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the XIP library, 
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : XIP
Version    : 9.60
filename   : disamb.cxx
Date       : 10/01/1999
Purpose    : HMM C++ encapsulation
Programmer : Claude ROUX
Reviewer   : 
*/


/* $Id: disamb.cxx,v 1.22 2008/09/29 15:28:29 roux Exp $ */

/*************************************************************
*
*  FILE  "disamb.cxx"
*
*      DISAMBIGUATION OF ALTERNATIVE TAGS
*
*  Andre Kempe, RANK XEROX, Grenoble, France
*  Claude ROUX, XEROX, for a C++ version without any global variable
*  30/01/96 - ...25/02/2002
*  
*
*************************************************************/


#include "parsbin.h"
#include "ntmhmm.h"
      
/*===========================================================*/
//Partie memory.c
void *mem_alloc_vit (long size)
{
/* Allocates the memory, or prints an error message and exits if failed.
    Fills the memory with 0. */
    
    void *mem;
    
    if (! (mem = (void *) malloc (size))) {
        printf("%s","memory allocation failed");
        exit(-1);
    }
    
    memset (mem, 0, size);
    return (mem);
}

void *mem_realloc_vit (void *old, long size)
{
/* Allocates the memory, or prints an error message and exits if failed.
    Fills the memory with 0. */
    
    void *nouveau;
    
    if (! (nouveau = (void *) realloc (old, size))) {
        printf("%s","memory reallocation failed");
        exit(-1);
    }
    
    
    return (nouveau);
}

char* strdup_vit (const char* ch) {
	char* val= (char*)mem_alloc_vit(strlen(ch)+1);
	strcpy(val,ch);
	return val;
}

char* strdup_vit (char* ch) {
	char* val= (char*)mem_alloc_vit(strlen(ch)+1);
	strcpy(val,ch);
	return val;
}

/*===========================================================
TAGS AND CLASSES
===========================================================*/


short *alloc_tag_vector (int num_tags)
{
    short *vec;
    
    vec = (short *) mem_alloc_vit ((num_tags + 1) * sizeof (short));
    vec [0] = CLASS_END;
    
    return (vec);
}

void put_tag_to_sorted_vector (short tag, short *vec)
{
    register short *pt, buffer;
    
    /* search for tag or position */
    
    for (pt = vec;  *pt >= 0;  pt ++)
    {
        if (*pt == tag)    /* tag found in vector */
            return;
        
        if (*pt > tag)     /* tag not found in vector */
            break;
    }
    
    /* insert tag */
    
    for (;;)
    {
        buffer = *pt;
        
        if ((*pt ++ = tag) == CLASS_END)
            return;
        
        tag = buffer;
    }
}

/*===========================================================
BASIC HMM MEMORY
===========================================================*/



/*===========================================================
GENERAL PURPOSE HEAPS CONTAINING ITEMS WITH EQUAL LENGTH
===========================================================*/

static EIBLOCKptr alloc_equal_item_block (long size)
{
/* Allocates and returns a block of equal-length items,
    or NULL on failure. */
    
    EIBLOCKptr block;
    
    if (! (block = (EIBLOCKptr) mem_alloc_vit (sizeof (EIBLOCKtype))))
        return (NULL);
    
    if (! (block->items = (void *) mem_alloc_vit (size)))
        return (NULL);
    
    block->used = 0;
    block->next = NULL;
    
    return (block);
}

static EIHEAPptr alloc_equal_item_heap(const char *heap_name, int bytes_per_item, int items_per_block)
{
/* Allocates and returns a heap of equal-length items,
    or NULL on failure. */
    
    EIHEAPptr heap;
    
    if (! (heap = (EIHEAPptr) mem_alloc_vit (sizeof (EIHEAPtype))))
        return (NULL);
    
    heap->name = strdup_vit (heap_name);
    heap->bytes_per_item = bytes_per_item;
    heap->items_per_block = items_per_block;
    heap->bytes_per_block = bytes_per_item * items_per_block;
    heap->blocks_in_heap = 0;
    heap->bytes_in_heap = 0;
    heap->blocks = NULL;
    heap->free_list = NULL;
    
    return (heap);
}

static void clean_equal_item_heap (EIHEAPptr heap)
{
    /* Cleans a heap of equal-length items. Frees all blocks. */
    
    EIBLOCKptr block, nextblock;
    
    for (block = heap->blocks;  block;  block = nextblock)
    {
        nextblock = block->next;
        free (block->items);
        free (block);
    }
    
    heap->blocks_in_heap = 0;
    heap->bytes_in_heap = 0;
    heap->blocks = NULL;
    heap->free_list = NULL;
}

static void free_equal_item_heap (EIHEAPptr heap)
{
    /* Frees a heap of equal-length items. */
    
    clean_equal_item_heap (heap);   /* free blocks */
    free (heap->name);
    free (heap);
}

void *get_equal_item_from_heap (EIHEAPptr heap)
{
    /* Returns a clean item from the heap, or NULL on failure. */
    
    EIBLOCKptr block;  void *item;
    
    if (heap->free_list)   /* take item from free-list */
    {
        item = heap->free_list;
        heap->free_list = *((void **) item);
    }
    else          /* take item from 1st block */
    {
        block = heap->blocks;
        if ((! block) || (block->used == heap->items_per_block))
        {                                      /* new 1st block required */
            if (! (block = alloc_equal_item_block (heap->bytes_per_block)))
                return (NULL);   /* failure */
            
            block->next = heap->blocks;
            heap->blocks = block;
            
            heap->blocks_in_heap ++;
            heap->bytes_in_heap += heap->bytes_per_block;
        }
        
        item = ((char*)block->items) + (block->used * heap->bytes_per_item);
        block->used ++;
    }
    
    memset (item, 0, heap->bytes_per_item);
    return (item);
}

void put_equal_item_to_heap (EIHEAPptr heap, void *item)
{
/* Puts the item into the free-list of the heap if the item has
    at least 4 bytes. */
    
    if (heap->bytes_per_item >= sizeof (void *))
    {
        *((void **) item) = heap->free_list;
        heap->free_list = item;
    }
}


/*===========================================================
VITERBI TREE
===========================================================*/

ATGCELLptr clear_alltag_cell (ATGCELLptr cell)
{
    cell->tag = NO_INDEX;
    cell->score = 0.0;
    cell->less = NULL;
    
    return (cell);
}


VCELLptr clear_vit_cell (VCELLptr cell)
{
    cell->prev = NULL;
    cell->tag = NO_INDEX;
    cell->in_count = 0;
    
    return (cell);
}

/*-----------------------------------------------------------*/
static VITPOSptr alloc_vit_vector (int max)
{
    VITPOSptr vec;
    
    vec = (VITPOSptr) mem_alloc_vit (sizeof (VITPOStype));
    
    vec->max = max;
    vec->len = 0;
    
    vec->alt = (VITVECTORptr) mem_alloc_vit (max * sizeof (VITVECTORtype));
    vec->tagseq = alloc_tag_vector (max);
    
    return (vec);
}

static void free_vit_vector (VITPOSptr vec)
{
    free (vec->alt);
    free (vec->tagseq);
    free (vec);
}

VTREEptr alloc_viterbi (int max)
{
    /* Allocates the Viterbi vectors, heap, blocks, etc. */
    
    VTREEptr vtree;
    
    vtree = (VTREEptr) mem_alloc_vit (sizeof (VTREEtype));
    
    vtree->cur = alloc_vit_vector (max);
    vtree->prev = alloc_vit_vector (max);
    
    vtree->heap.vcells = alloc_equal_item_heap
        ("Viterbi tree cells", sizeof (VCELLtype), VIT_BLOCK_LENGTH);
    
    vtree->heap.atgcells = alloc_equal_item_heap
        ("Tag list cells", sizeof (ATGCELLtype), ATG_BLOCK_LENGTH);
    
    return (vtree);
}

void free_viterbi (VTREEptr vtree)
{
    /* Frees the Viterbi vectors, heap, blocks, etc. */
    
    free_vit_vector (vtree->cur);
    free_vit_vector (vtree->prev);
    
    free_equal_item_heap (vtree->heap.vcells);
    free_equal_item_heap (vtree->heap.atgcells);
    
    free (vtree);
}

/*===========================================================
INPUT WORDS, LEMMAS, TAGS
===========================================================*/

WORDBFptr alloc_word_buffer (long max_words, long max_altbf, long max_linebf)
{
    WORDBFptr wbf;
    
    wbf = (WORDBFptr) mem_alloc_vit (sizeof (WORDBFtype));
    
    wbf->max = max_words;
    wbf->num = 0;
    wbf->word = (INPWORDptr) mem_alloc_vit (max_words * sizeof (INPWORDtype));
    
    wbf->buffer.alt.max = max_altbf;
    wbf->buffer.alt.len = 0;
    wbf->buffer.alt.bf = (INPLINEptr) mem_alloc_vit (max_altbf * sizeof (INPWORDtype));
    
    wbf->buffer.line.max = max_linebf;
    wbf->buffer.line.len = 0;
    wbf->buffer.line.bf = (char *) mem_alloc_vit (max_linebf * sizeof (char));
    
    wbf->words_total = 0;
    
    return (wbf);
}

void adjust_word_buffer
(WORDBFptr wbf, long max_words, long max_altbf, long max_linebf)
{
    INPLINEptr old_altbf;  char *old_linebf;  long i, offset;
    
    if (wbf->max < max_words)
    {
        wbf->max = max_words;
        wbf->word = (INPWORDptr)
            mem_realloc_vit (wbf->word, max_words * sizeof (INPWORDtype));
    }
    
    if (wbf->buffer.alt.max < max_altbf)
    {
        wbf->buffer.alt.max = max_altbf;
        old_altbf = wbf->buffer.alt.bf;
        
        wbf->buffer.alt.bf = (INPLINEptr)
            mem_realloc_vit (wbf->buffer.alt.bf, max_altbf * sizeof (INPWORDtype));
        offset = wbf->buffer.alt.bf - old_altbf;
        
        for (i=0;  i< wbf->num;  i++)
            wbf->word [i].alt += offset;
    }
    
    if (wbf->buffer.line.max < max_linebf)
    {
        wbf->buffer.line.max = max_linebf;
        old_linebf = wbf->buffer.line.bf;
        
        wbf->buffer.line.bf = (char *)
            mem_realloc_vit (wbf->buffer.line.bf, max_linebf * sizeof (char));
        offset = wbf->buffer.line.bf - old_linebf;
        
        for (i=0;  i< wbf->buffer.alt.len;  i++)
            wbf->buffer.alt.bf [i].line += offset;
    }
}

void free_word_buffer (WORDBFptr wbf)
{
    free (wbf->word);
    free (wbf->buffer.alt.bf);
    free (wbf->buffer.line.bf);
    free (wbf);
}

extern "C" {
    int compare_tags (const void* va, const void* vb) {
        VITVECTORptr a= (VITVECTORptr)va;
        VITVECTORptr b = (VITVECTORptr)vb;
        
        if (a->evalu < b->evalu)
            return (-1);
        
        if (a->evalu > b->evalu)
            return (1);
        
        return (0);
    }
}

void sort_evaluated_tags (VITPOSptr vec)
{
    /* Sorts all tags in the vector, based on their scores. */
    
    if (vec->len > 1)
        qsort (vec->alt, vec->len, sizeof (VITVECTORtype), compare_tags);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//Methodes GlobalParseurHMM
void GlobalParseurHMM::error_message (const char *msg, ...)
{
    
    if (AFFICHE_ERREUR==NON)
        return;

    va_list ap;  
    const char *section;
    char MessageErreur[1000];
    
    if (DISAMB_FLAGS.system.reading_hmm)
        section = ", in HMM";
    else if (DISAMB_FLAGS.system.reading_input)
        section = ", in input";
    else
        section = "";
    
    sprintf (MessageErreur, "ERROR in '%s'%s", PROGRAM_NAME_HMM, section);
    
    if (DISAMB_FLAGS.system.cur_line_num > 0)
        sprintf (MessageErreur, ", line %li", DISAMB_FLAGS.system.cur_line_num);
    
    sprintf (MessageErreur, " :\n");
    
    va_start (ap, msg);
    vsprintf (MessageErreur, msg, ap);  
    NOM="HMM MODULE";
    erreur(MessageErreur);
}

void  GlobalParseurHMM::work_message (char *msg, ...)
{
    va_list ap;
    
    va_start (ap, msg);
    vfprintf (stderr, msg, ap);
    
    fflush (stderr);
}

void  GlobalParseurHMM::set_disamb_flags (char *argflags)
{
    char* flags;
    char* pt;
    
    memset (&DISAMB_FLAGS.user, 0, sizeof (DISAMB_FLAGS.user));
    
    if (argflags)
        flags = argflags;
    else if (! (flags = getenv (ENV_DISAMB_FLAGS)))
        flags = (char*)DEF_DISAMB_FLAGS;
    
    DISAMB_FLAGS.user.flag_string = flags;
    
    /* flag for printing all tags */
    
    if (strchr (flags, FLAG_PRINT_ALL_TAGS))
        DISAMB_FLAGS.user.print_all_tags = TRUE;
    
    /* flag for printing comments */
    
    if (strchr (flags, FLAG_SHOW_COMMENTS))
        DISAMB_FLAGS.user.show_comments = TRUE;
    
    /* flag for fflush() after fprintf() */
    
    if (strchr (flags, FLAG_DONT_BUFFER))
        DISAMB_FLAGS.user.dont_buffer = TRUE;
    
    /* flags for tag cutting */
    
    if ((pt = strchr (flags, FLAG_TAGCUT_BACKWARD)))
        DISAMB_FLAGS.user.tagcut_backward = TRUE;
    else if ((pt = strchr (flags, FLAG_TAGCUT_FORWARD)))
        DISAMB_FLAGS.user.tagcut_forward = TRUE;
    
    if (pt)    /* tag cut required */
    {
        pt++;
        if (*pt == FLAG_TAGCUT_INCLUSIVE)
        { DISAMB_FLAGS.user.tagcut_inclusive = TRUE;  pt++; }
        
        sscanf (pt, "%hd%c",
            &(DISAMB_FLAGS.user.tagcut_num), &(DISAMB_FLAGS.user.tagcut_sym));
        
        if (! (DISAMB_FLAGS.user.tagcut_num && DISAMB_FLAGS.user.tagcut_sym))
            error_message ("wrong tag cut symbol or number '%s'", pt);
    }
}



/*===========================================================*/
//Ces methodes proviennent de hash.c

int GlobalParseurHMM::calculate_tag_hash_index (unsigned char *key)
{
    unsigned char *pt;  long index = 1, index2 = 0;
    
    for (pt = key; *pt; pt++)
    { index *= *pt; index2 += *pt; }
    
    index += 17* index2;    
	int val=index % TAG.hash.len;
    return abs(val);
}

void GlobalParseurHMM::hash_insert_tags ()
{
    /* Hash inserts all tags. */
    
    HASH_HMMptr *hash, cell;  char **tag;  long i;
    
    for (i=0; i< TAG.num; i++)
    {
        tag = TAG.tag + i;
        cell = TAG.hash.list + i;
        
        hash = TAG.hash.tab + calculate_tag_hash_index ((unsigned char*)*tag);
        
        cell->item = (void *) tag;
        cell->next = *hash;
        *hash = cell;
    }
}

short GlobalParseurHMM::hash_find_tag (const char *key)
{
    HASH_HMMptr cell;  short idx;
    
    for (cell = TAG.hash.tab [calculate_tag_hash_index ((unsigned char*)key)];
    cell;
    cell = cell->next)
        if (! strcmp (*((char **) cell->item), key))
            break;           /* item found */
        
        if (cell)
            idx = cell - TAG.hash.list;
        else
            idx = NO_INDEX;
        
        return (idx);
}

/*-----------------------------------------------------------*/
int GlobalParseurHMM::calculate_class_hash_index (short *key)
{
    short *pt;  long index = 1, index2 = 0;
    
    for (pt = key; *pt != CLASS_END; pt++)
    { index *= *pt; index2 += *pt; }
    
    index += 17* index2;
    int val=index % CLASS.hash.len;
    return abs(val);
}

void GlobalParseurHMM::hash_insert_classes ()
{
    /* Hash inserts all classs. */
    
    HASH_HMMptr *hash, cell;  short **classe;  long i;

    for (i=0; i< CLASS.num; i++)
    {
        classe = CLASS.classe + i;
        cell = CLASS.hash.list + i;
        hash = CLASS.hash.tab + calculate_class_hash_index (*classe);
        
        cell->item = (void *) classe;
        cell->next = *hash;
        *hash = cell;
    }
}

int class_cmp (short *seq1, short *seq2)
{
/* Compares two sequences of short (end is -1).
Return:
1 if seq1 > seq2,
0 if seq1 = seq2,
    -1 if seq1 < seq2  */
    
    while (TRUE)
    {
        if (*seq1 > *seq2)  return (1);
        if (*seq1 < *seq2)  return (-1);
        if (*seq1 == CLASS_END)  return (0);
        
        seq1++;
        seq2++;
    }
}

short GlobalParseurHMM::hash_find_class (short *key)
{
    HASH_HMMptr cell;  short idx;
    
    for (cell = CLASS.hash.tab [calculate_class_hash_index (key)];
    cell;
    cell = cell->next)
        if (! class_cmp (*((short **) cell->item), key))
            break;           /* item found */
        
        if (cell)
            idx = cell - CLASS.hash.list;
        else
            idx = NO_INDEX;
        
        return (idx);
}



/*===========================================================*/
//Ces methodes proviennent de hmmread.c

void GlobalParseurHMM::read_hmm_file_header (FILE *file,short show_comments)
{
    /* Reads header and copyright information of the HMM file. */
    
    HMM_HEADERtype header;
    char copyright [MAX_COPYRIGHT_SIZE];
    
    if (fread (&header, sizeof (header), 1, file) != 1)
        error_message ("wrong header in HMM file");
    
    if (header.file_type != TRAINER_MODEL_FILETYPE)
        error_message ("HMM file of wrong type");
    
    fgets (copyright, MAX_COPYRIGHT_SIZE, file);
    
    if (show_comments)
    {
        fprintf (stderr, "Copyright of the HMM data file:\n\t%s\n", copyright);
        
        /*      if (DISAMB_FLAGS.user.dont_buffer)
        fflush (file); */
    }
}

void GlobalParseurHMM::delete_words (WORDBFptr lwbf, long num)
{
    /* Deletes a number of words from buffer. */
    
    long i, offset;
    
    /* cases: delete all or no words */
    
    if (num < 1)   /* nothing to do */
        return;
    
    if (num >= lwbf->num)
    {
        lwbf->num = 0;
        lwbf->buffer.alt.len = 0;
        lwbf->buffer.line.len = 0;
        return;
    }
    
    /* case: delete some words */
    
    memmove (lwbf->word, lwbf->word + num, (lwbf->num - num) * sizeof (INPWORDtype));
    lwbf->num -= num;
    
    offset = lwbf->word [0].alt - lwbf->buffer.alt.bf;
    memmove (lwbf->buffer.alt.bf, lwbf->buffer.alt.bf + offset,
        (lwbf->buffer.alt.len - offset) * sizeof (INPLINEtype));
    lwbf->buffer.alt.len -= offset;
    for (i=0;  i< lwbf->num;  i++)
        lwbf->word [i].alt -= offset;
    
    offset = lwbf->word [0].alt [0].line - lwbf->buffer.line.bf;
    memmove (lwbf->buffer.line.bf, lwbf->buffer.line.bf + offset,
        (lwbf->buffer.line.len - offset) * sizeof (char));
    lwbf->buffer.line.len -= offset;
    for (i=0;  i< lwbf->buffer.alt.len;  i++)
        lwbf->buffer.alt.bf [i].line -= offset;
}

void GlobalParseurHMM::HMM_write_long (FILE* file, long val)
{
    if (fprintf (file, "%li\n", val))
    {
    /* if (DISAMB_FLAGS.user.dont_buffer)
        fflush (file)  */ ;
    }
    else
        error_message ("long integer could not be written");
}

long  GlobalParseurHMM::HMM_read_long (FILE* file)
{
    long n;  unsigned char line [L_LINE_HMM+2];
    
    DISAMB_FLAGS.system.cur_line_num++;
    
    if ((! fgets ((char*)line, L_LINE_HMM, file)) ||
        (sscanf ((char*)line, "%li", &n) != 1))
        error_message ("long integer could not be read from HMM file");
    
    return n;
}

double  GlobalParseurHMM::HMM_read_double (FILE* file)
{
    double f;  unsigned char line [L_LINE_HMM+2];
    
    DISAMB_FLAGS.system.cur_line_num++;
    
    if ((! fgets ((char*)line, L_LINE_HMM, file)) ||
        (sscanf ((char*)line, "%le", &f) != 1))
        error_message ("double float could not be read from HMM file");
    
    return f;
}

char*  GlobalParseurHMM::HMM_read_line (char *line, FILE* file)
{
    int len;
    
    DISAMB_FLAGS.system.cur_line_num++;
    
    if (! fgets (line, L_LINE_HMM-1, file))
        return (NULL);
    
    len = strlen (line);
    
    if (line [len - 1] == '\n')
        line [len - 1] = '\0';
    
    return (line);
}

int HMM_tokenize_line (char *line, char **token, const char *sep)
{
/* Cuts a line into tokens where-ever any 'sep' character is found.
    Returns the number of tokens. */
    
    char *pt;  int toknum;
    
    for (pt = line, toknum = 0; ; )
    {
        while ((*pt) && (strchr (sep, *pt)))  *pt++='\0';
        if (! *pt)  return (toknum);
        
        token [toknum++] = pt;
        while ((*pt) && (! strchr (sep, *pt)))  pt++;
    }
    
    return toknum;
}

/*===========================================================
READ TAGS AND CLASSES
===========================================================*/

char*  GlobalParseurHMM::put_tags_to_buffer (FILE *file, int num_tags)
{
    char *tagbf, line [L_LINE_HMM+2], buffer [L_LINE_HMM+2], *tag [MAX_TOKENS_HMM];
    int i, max, len, slen;
    
    max = L_LINE_HMM;
    len = 0;
    tagbf = (char *) mem_alloc_vit (max * sizeof (char));
    
    for (i=0; i< num_tags; i++)
    {
        HMM_read_line (line, file);
        strcpy (buffer, line);
        
        if (HMM_tokenize_line (buffer, tag, TAG_SEPARATORS) != 1)
            error_message ("more than one token in tag section of the HMM file\nin line <%s>", line);
        
        slen = strlen (tag [0]);
        
        if (len + slen + 2 > max)  /* 2 more chars for str end and buffer end */
        {
            while (len + slen + 2 > max)
                max *= REALLOC_FACTOR_HMM;
            
            tagbf = (char *) mem_realloc_vit (tagbf, max * sizeof (char));
        }
        
        strcpy (tagbf + len, tag [0]);
        len += slen + 1;
    }
    
    tagbf [len] = '\0';    /* buffer end */
    return (tagbf);
}

void  GlobalParseurHMM::store_tags (int num_tags)
{
    char *pt;  int i;
    
    for (i = 0, pt = TAG.buffer;
    (i < num_tags) && (*pt);
    i ++, pt += strlen (pt) + 1)
        TAG.tag [i] = pt;
    
    if (i < num_tags)
        error_message ("%d tags announced, %d tags found\n", num_tags, i);
    
    if (*pt)
        error_message ("%d tags announced, more found\n", num_tags);
}

void  GlobalParseurHMM::read_tags (FILE *file, int num_tags)
{
    alloc_tags (num_tags);
    TAG.buffer = put_tags_to_buffer (file, num_tags);
    store_tags (num_tags);
    hash_insert_tags ();
}

/*-----------------------------------------------------------*/
char*  GlobalParseurHMM::put_out_tags_to_buffer (FILE *file)
{
    char *tagbf, *t1, *t2,
        line [L_LINE_HMM+2], buffer [L_LINE_HMM+2], *tag [MAX_TOKENS_HMM];
    int max, len, slen;
    
    DISAMB_FLAGS.system.cur_line_num = 0;
    
    max = L_LINE_HMM;
    len = 0;
    tagbf = (char *) mem_alloc_vit (max * sizeof (char));
    
    while (HMM_read_line(line, file))
    {
        strcpy (buffer, line);
        
        switch (HMM_tokenize_line (buffer, tag, TAG_SEPARATORS))
        {
        case 0:
            continue;    /* ignore empty line */
        case 1:
            t1 = t2 = tag [0];
            break;
        case 2:
            t1 = tag [0];
            t2 = tag [1];
            break;
        default:
        error_message ("too many tags on a line in the tag recode file\n\
            in line <%s>", line);
        }
        
        slen = strlen (t1) + 1 + strlen (t2);
        
        if (len + slen + 2 > max)  /* 2 more chars for str end and buffer end */
        {
            while (len + slen + 2 > max)
                max *= REALLOC_FACTOR_HMM;
            
            tagbf = (char *) mem_realloc_vit (tagbf, max * sizeof (char));
        }
        
        sprintf (tagbf + len, "%s%c%s", t1, '\0', t2);
        len += slen + 1;
    }
    
    tagbf [len] = '\0';    /* buffer end */
    return (tagbf);
}

void  GlobalParseurHMM::store_out_tags (int num_tags)
{
    char *pt;  int i, idx;
    
    for (i = 0;  i < num_tags ;  i ++)
        TAG.out_tag [i] = NULL;
    
    for (pt = TAG.out_buffer;  *pt;  )
    {
        if ((idx = hash_find_tag (pt)) == NO_INDEX)
            error_message ("Unknown tag '%s' in the recode file\n", pt);
        
        if (TAG.out_tag [idx])
            error_message ("Tag '%s' more than once recoded\n", pt);
        
        pt += strlen (pt) + 1;
        TAG.out_tag [idx] = pt;
        pt += strlen (pt) + 1;
    }
}

void  GlobalParseurHMM::read_out_tags (char *filename, int num_tags)
{
    FILE *file;
    
    DISAMB_FLAGS.system.cur_line_num = 0;
    
    if (! (file = fopen (filename, "r")))
        error_message ("Tag recode file could not be opened");
    
    TAG.out_buffer = put_out_tags_to_buffer (file);
    fclose(file);
    
    store_out_tags (num_tags);
    
    DISAMB_FLAGS.system.cur_line_num = 0;
}

/*-----------------------------------------------------------*/
short*  GlobalParseurHMM::put_classes_to_buffer (FILE *file, int num_tags, int num_classes)
{
    unsigned char line [L_LINE_HMM+2], buffer [L_LINE_HMM+2], *tag [MAX_TOKENS_HMM];
    short i, n, num, idx, slen, *tagseq, *tagpt, *classbf;
    int max, len;
    
    max = L_LINE_HMM;
    len = 0;
    classbf = (short *) mem_alloc_vit (max * sizeof (short));
    
    tagseq = alloc_tag_vector (num_tags);
    
    for (i=0; i< num_classes; i++)
    {
        HMM_read_line ((char*)line, file);
        strcpy ((char*)buffer, (char*)line);
        
        if ((num = HMM_tokenize_line ((char*)buffer, (char**)tag, TAG_SEPARATORS)) < 1)
            error_message ("class without tags in the HMM file");
        
        tagseq [0] = CLASS_END;
        for (n=0; n< num; n++)
        {
            if ((idx = hash_find_tag ((char*)tag [n])) == NO_INDEX)
                error_message ("undeclared tag <%s> in class <%s>", tag [n], line);
            
            put_tag_to_sorted_vector (idx, tagseq);
        }
        
        for (tagpt = tagseq;  *tagpt != CLASS_END;  tagpt ++)
            ;
        
        slen = tagpt - tagseq;
        
        if (len + slen + 2 > max)  /* 2 more elem for seq end and buffer end */
        {
            while (len + slen + 2 > max)
                max *= REALLOC_FACTOR_HMM;
            
            classbf = (short *) mem_realloc_vit (classbf, max * sizeof (short));
        }
        
        memcpy (classbf + len, tagseq, (slen + 1) * sizeof (short));
        len += slen + 1;
    }
    
    free(tagseq);
    classbf [len] = CLASS_END;    /* buffer end */
    return (classbf);
}

void GlobalParseurHMM::store_classes (int num_classes)
{
    short *tagseq;  int i;
    
    for (i = 0, tagseq = CLASS.buffer;
    (i < num_classes) && (*tagseq != CLASS_END);
    i ++)
    {
        CLASS.classe [i] = tagseq;
        
        while (*tagseq != CLASS_END)
            tagseq ++;
        
        tagseq ++;
    }
    
    if (i < num_classes)
        error_message ("%d classes announced, %d classes found\n", num_classes, i);
    
    if (*tagseq != CLASS_END)
        error_message ("%d classes announced, more found\n", num_classes);
}

void  GlobalParseurHMM::read_classes (FILE *file, int num_tags, int num_classes)
{
    alloc_classes (num_classes);
    CLASS.buffer = put_classes_to_buffer (file, num_tags, num_classes);
    store_classes (num_classes);
    hash_insert_classes ();
}

/*===========================================================
READ AND STORE THE BASIC HMM DATA
===========================================================*/

double hmm_log_score (double prob)
{
    if (! prob)
        return (NO_LOG_SCORE);
    else
        return (- log10 (prob));
}

void  GlobalParseurHMM::read_store_Pi (FILE *file)
{
    /* Reads and stores the initial probabilities. */
    
    long i;  double pi;
    
    if ((i = HMM_read_long (file)) != TAG.num)
        error_message ("%li initial probabilities for %li tags declared",
        i, TAG.num);
    
    for (i=0; i< TAG.num; i++)
    {
        pi = HMM_read_double (file);
        if ((pi < 0.0) || (pi > 1.0))
        error_message ("initial probability number %li is %.3f\n\
        and out of the range 0...1", i, pi);
        
        HMM.Pi [i] = hmm_log_score (pi);
    }
}

void  GlobalParseurHMM::read_store_A (FILE *file)
{
    /* Reads and stores the transition probabilities A (cur | prev). */
    
    long n, prev, num_prev, cur;  double a;
    
    if ((n = HMM_read_long (file)) != TAG.num)
        error_message ("%li transition probability vectors for %li tags declared",
        n, TAG.num);
    
    for (cur = 0; cur < TAG.num; cur++)
    {
        num_prev = HMM_read_long (file);
        if ((num_prev < 0) || (num_prev > TAG.num))
            error_message ("%li transition probabilities for %li tags declared",
            num_prev, TAG.num);
        
        for (prev = 0; prev < num_prev; prev++)
        {
            a = HMM_read_double (file);
            if ((a < 0.0) || (a > 1.0))
            error_message ("transition probability form %li to %li is %.3f\n\
            and out of range 0...1", prev, cur, a);
            
            HMM.A [prev] [cur] = hmm_log_score (a);
        }
        
        for (prev = num_prev; prev < TAG.num; prev++)
            HMM.A [prev] [cur] = NO_LOG_SCORE;
    }
}

void  GlobalParseurHMM::read_store_B (FILE *file)
{
    /* Reads and stores the class probabilities B (class | tag). */
    
    long i, num, tag, classe;
    double b;
    
    if ((i = HMM_read_long (file)) != CLASS.num)
        error_message ("%li probability vectors for %li classes declared",
        i, CLASS.num);
    
    for (classe = 0; classe < CLASS.num; classe++)
    {
        num = HMM_read_long (file);
        if ((num <=0) || (num > TAG.num))
        error_message ("wrong size (%li) of a class set declared\n\
        (there are %li tags in the tag set)", num, TAG.num);
        
        for (tag = 0; tag < TAG.num; tag++)
            HMM.B [tag] [classe] = NO_LOG_SCORE;
        
        for (i=0; i< num; i++)
        {
            tag = HMM_read_long (file);
            if ((tag < 0) || (tag > TAG.num))
                error_message ("wrong tag index in a class set declared");
            
            b = HMM_read_double (file);
            if ((b < 0.0) || (b > 1.0))
            error_message ("probability of class %li with tag %li is %.3f\n\
            and out of the range 0...1", classe, tag, b);
            
            HMM.B [tag] [classe] = hmm_log_score (b);
        }
    }
}

/*===========================================================
MAIN HMM READ FUNCTIONS
===========================================================*/

void GlobalParseurHMM::read_hmm_file (char *filename)
{
/* Reads the HMM file.
    Allocates the required memory and stores there the HMM data. */
    
    FILE *file;  int num_tags, num_classes;
    
    DISAMB_FLAGS.system.cur_line_num = 0;
    DISAMB_FLAGS.system.reading_hmm = TRUE;
    DISAMB_FLAGS.system.reading_input = FALSE;
    
    /* open file */
    
    if (! (file = fopen (filename, "rb")))
        error_message ("HMM file could not be opened");
    
    /* read HMM file until: #tags, #classes */
    
    read_hmm_file_header (file,DISAMB_FLAGS.user.show_comments);
    HMM_read_long (file);    /* maximal class set size; not needed */
    
    if ((num_tags = HMM_read_long (file)) <= 0)
        error_message ("wrong number of tags declared : %li", num_tags);
    
    if ((num_classes = HMM_read_long (file)) <= 0)
        error_message ("wrong number of classes declared : %li", num_classes);
    
    /* read and store tags and classes */
    
    read_tags (file, num_tags);
    read_classes (file, num_tags, num_classes);
    
    /* read and store basic HMM data */
    
    alloc_hmm_basic ();
    
    read_store_Pi (file);
    read_store_A (file);
    read_store_B (file);
    
    fclose (file);
    
    if ((TAG.sent_end = hash_find_tag (SENT_END_TAG)) == NO_INDEX)
        error_message("Sentence end tag %s does not occur in HMM data\n",
        SENT_END_TAG);
    
    /* finish */
    
    DISAMB_FLAGS.system.cur_line_num = 0;
    DISAMB_FLAGS.system.reading_hmm = FALSE;
    DISAMB_FLAGS.system.reading_input = FALSE;
}


//Ces methodes proviennent de viterbi.c
/*===========================================================
BASIC FUNCTIONS
===========================================================*/

void GlobalParseurHMM::delete_alltag_list (ATGCELLptr atgcell, VTREEptr lvtree)
{
    ATGCELLptr prev;
    
    for ( ; atgcell; atgcell = prev)
    {
        prev = atgcell->less;
        free_alltag_cell (atgcell, lvtree);
    }
}

void GlobalParseurHMM::delete_vcell_list (VCELLptr vcell, VTREEptr lvtree)
{
    VCELLptr prev;
    
    for ( ; vcell; vcell = prev)
    {
        if (vcell->in_count > 1)
        {
            vcell->in_count --;
            return;
        }
        else
        {
            if (vcell->all_tags)
                delete_alltag_list (vcell->all_tags, lvtree);
            
            prev = vcell->prev;
            free_vit_cell (vcell, lvtree);
        }
    }
}

void GlobalParseurHMM::clear_vit_vector_and_lists (VITPOSptr vec, VTREEptr lvtree)
{
    /* Clears a Viterbi vector and deletes all appended lists. */
    
    int i;
    
    for (i=0; i< vec->len; i++)
        delete_vcell_list (vec->alt [i].prev, lvtree);
    
    vec->len = 0;
    vec->tagseq [0] = CLASS_END;
    vec->classe = NO_INDEX;
    
    vec->flags.sent_begin = FALSE;
    vec->flags.sent_end = FALSE;
    vec->flags.sent_bound = FALSE;
}

void GlobalParseurHMM::exchange_vit_vectors (VTREEptr lvtree)
{
    VITPOSptr vec;
    
    vec = lvtree->cur;
    lvtree->cur = lvtree->prev;
    lvtree->prev = vec;
}

/*===========================================================
COPY DATA FROM WORD BUFFER TO VITERBI TREE
===========================================================*/

void GlobalParseurHMM::set_current_class_data (WORDBFptr lwbf, VTREEptr lvtree)
{
    INPWORDptr word;  VITVECTORptr alt;  short i, *tagseq,*pt;
    
    if (lwbf->num <= 0)
        return;     /* no input stored --> no update possible */
    
    word = lwbf->word + (lwbf->num - 1);  /* last word in buffer */
    
    /* make sorted tag sequence and class */
    
    tagseq = lvtree->cur->tagseq;
    tagseq [0] = CLASS_END;
    
    for (i = 0;  i < word->num;  i ++)
        put_tag_to_sorted_vector (word->alt [i].tag, tagseq);
    
    /*CR Small particular case, vhen the tagseq vector is empty, we then put an empty value in it*/
    if (*tagseq == CLASS_END) {
        pt=tagseq;
        *pt++=0;
        *pt=CLASS_END;
    }


    lvtree->cur->classe = hash_find_class (tagseq);
    
    /* store alternative tags */
    
    for (alt = lvtree->cur->alt;  *tagseq != CLASS_END;  alt ++, tagseq ++)
    {
        alt->tag = *tagseq;
        alt->prev = NULL;
        alt->score = 0.0;
        alt->evalu = 0.0;
    }
    
    lvtree->cur->len = tagseq - lvtree->cur->tagseq;
    
    /* determine current flags */
    
    lvtree->cur->flags.sent_begin =
        ((lvtree->prev->flags.sent_bound) || (lwbf->words_total == 1));
    
    lvtree->cur->flags.sent_end =
        (word->alt [0].tag == TAG.sent_end);
    
    lvtree->cur->flags.sent_bound = FALSE;
    
    if ((lvtree->cur->flags.sent_end) && (word->num != 1))
        error_message ("Ambiguous sentence end token encountered.\n");
}


/*===========================================================
AUXILIARY VITERBI FUNCTIONS
===========================================================*/

double GlobalParseurHMM::calculate_path_score (VTREEptr lvtree, short prev, short cur)
{
    double score, sc0, a, b;  short t0, t1;
    
    t0 = lvtree->prev->alt [prev].tag;
    sc0 = lvtree->prev->alt [prev].score;
    
    t1 = lvtree->cur->alt [cur].tag;
    
    if (lvtree->cur->flags.sent_bound)
    {
        a = b = 0.0;
    }
    else
    {
        a = HMM.A [t0] [t1];
        
        if (lvtree->cur->classe == NO_INDEX)   /* unknown class */
            b = hmm_log_score (1.0 / lvtree->cur->len); /*assume all B[][] to be equal*/
        else
            b = HMM.B [t1] [lvtree->cur->classe];
    }
    
    score = sc0 + a + b;
    return (score);
}

void GlobalParseurHMM::connect_cur_to_prev(VTREEptr lvtree, short prev_pos, short cur_pos, double score)
{
    VITVECTORptr prev, cur;  VCELLptr cell;
    
    prev = lvtree->prev->alt + prev_pos;
    cur = lvtree->cur->alt + cur_pos;
    cell = alloc_vit_cell (lvtree);
    
    cell->tag = prev->tag;
    cell->all_tags = NULL;
    
    cur->prev = cell;
    cell->in_count = 1;
    
    if ((cell->prev = prev->prev))
        cell->prev->in_count ++;
    
    cur->score = score;
}

/*-----------------------------------------------------------*/
ATGCELLptr GlobalParseurHMM::list_all_tags (VTREEptr lvtree, VITPOSptr vec)
{
    /* Returns a list of all tags in the vector. */
    
    int i;  ATGCELLptr cell, tail = NULL;
    
    for (i = vec->len - 1;  i >= 0;  i--)
    {
        cell = alloc_alltag_cell (lvtree);
        cell->tag = vec->alt [i].tag;
        cell->score = vec->alt [i].evalu;
        cell->less = tail;
        
        tail = cell;
    }
    
    return (tail);
}

/*===========================================================
ONE VITERBI STEP
===========================================================*/

void GlobalParseurHMM::viterbi_start (VTREEptr lvtree)
{
/* Handles the 1st word of a sentence.
    Prob (tag) = Pi (tag) * B (class|tag)   */
    
    short i, tag, classe;  double score;
    
    classe = lvtree->cur->classe;
    
    for (i=0;  i< lvtree->cur->len;  i++)
    {
        tag = lvtree->cur->alt [i].tag;
        
        if (classe == NO_INDEX)   /* unknown class */
            score = HMM.Pi [tag];    /*assume all B[][] to be equal*/
        else
            score = HMM.Pi [tag] + HMM.B [tag][classe];
        
        lvtree->cur->alt [i].score = score;
    }
}

short GlobalParseurHMM::evaluate_prev_tags (VTREEptr lvtree, short cur)
{
    short i, best_prev;  double score, best_score=0;
    long fbest=0;
    long fscore;
    best_prev=0;
    for (i=0;  i< lvtree->prev->len;  i++)
    {
        lvtree->prev->alt [i].evalu = score = calculate_path_score (lvtree, i, cur);
        
        fscore=(long)(score*100000);
        if ((i == 0) || (fbest>fscore))
        {

            best_prev = i;
            best_score = score;
            fbest=(long)(best_score*100000);
        }
    }
    
    return (best_prev);
}

void GlobalParseurHMM::viterbi_step_best_tag (VTREEptr lvtree)
{
/* Handles 2nd and later words of a sentence.
Retains only the most likely tag.
    Prob (tag) = A (tag|prev) * B (class|tag)   */
    
    short best_prev, i;
    double best_score;
    
    for (i=0;  i< lvtree->cur->len;  i++)
    {
        best_prev = evaluate_prev_tags (lvtree, i);        
        best_score = lvtree->prev->alt [best_prev].evalu;
        connect_cur_to_prev (lvtree, best_prev, i, best_score);
    }
}

void GlobalParseurHMM::viterbi_step_all_tags (VTREEptr lvtree)
{
/* Handles 2nd and later words of a sentence.
Retains all tags with the current probabilities.
    Prob (tag) = A (tag|prev) * B (class|tag)   */
    
    short best_prev, i;
    double best_score;
    
    for (i=0;  i< lvtree->cur->len;  i++)
    {
        evaluate_prev_tags (lvtree, i);
        sort_evaluated_tags (lvtree->prev);
        
        best_prev = 0;
        best_score = lvtree->prev->alt [0].evalu;
        connect_cur_to_prev (lvtree, best_prev, i, best_score);
        
        lvtree->cur->alt [i].prev->all_tags = list_all_tags (lvtree, lvtree->prev);
    }
}

void GlobalParseurHMM::viterbi_step (VTREEptr lvtree)
{
    if (DISAMB_FLAGS.user.print_all_tags)
        viterbi_step_all_tags (lvtree);
    else
        viterbi_step_best_tag (lvtree);
}

void GlobalParseurHMM::viterbi_end (VTREEptr lvtree)
{
/* Selects the best of all current pathes, and deletes the other ones,
    by appending a boundary symbol. */
    
    exchange_vit_vectors (lvtree);   /* prev <--> cur */
    clear_vit_vector_and_lists (lvtree->cur, lvtree);
    
    lvtree->cur->alt [0].tag = SENT_BOUND;
    lvtree->cur->len = 1;
    lvtree->cur->flags.sent_bound = TRUE;
    
    viterbi_step (lvtree);
}

///////////////////////////////////////////////////
///Methodes provenant de memory.c


void GlobalParseurHMM::alloc_tags (int num_tags)
{
    /* Allocates the content of the TAG structure. */
    
    TAG.num = num_tags;
    
    TAG.tag = (char **) mem_alloc_vit (num_tags * sizeof (char *));
    TAG.buffer = NULL;
    
    TAG.out_tag = (char **) mem_alloc_vit (num_tags * sizeof (char *));
    TAG.out_buffer = NULL;
    
    TAG.hash.len = num_tags * HASH_LEN_FACTOR;
    TAG.hash.tab = (HASH_HMMptr *) mem_alloc_vit (TAG.hash.len * sizeof (HASH_HMMptr));
    TAG.hash.list = (HASH_HMMptr) mem_alloc_vit (num_tags * sizeof (HASH_HMMtype));
}

void GlobalParseurHMM::free_tags ()
{
    /* Frees the content of the TAG structure. */
    
    free (TAG.buffer);
    free (TAG.tag);
    free (TAG.out_tag);
    free(TAG.out_buffer);
    free (TAG.hash.tab);
    free (TAG.hash.list);
}

void GlobalParseurHMM::alloc_classes (int num_classes)
{
    /* Allocates the content of the CLASS structure. */
    
    CLASS.num = num_classes;
    CLASS.classe = (short **) mem_alloc_vit (num_classes * sizeof (short *));
    CLASS.buffer = NULL;
    
    CLASS.hash.len = num_classes * HASH_LEN_FACTOR;
    CLASS.hash.tab =
        (HASH_HMMptr *) mem_alloc_vit (CLASS.hash.len * sizeof (HASH_HMMptr));
    CLASS.hash.list = (HASH_HMMptr) mem_alloc_vit (num_classes * sizeof (HASH_HMMtype));
}

void GlobalParseurHMM::free_classes ()
{
    /* Frees the content of the CLASS structure. */
    
    free (CLASS.classe);
    free (CLASS.buffer);
    free (CLASS.hash.tab);
    free (CLASS.hash.list);
}

void GlobalParseurHMM::alloc_hmm_basic ()
{
    long i, n;
    
    HMM.Pi = (double *) mem_alloc_vit (TAG.num * sizeof (double));
    
    HMM.A = (double **) mem_alloc_vit (TAG.num * sizeof (double *));
    HMM.A_buffer =
        (double *) mem_alloc_vit (TAG.num * TAG.num * sizeof (double));
    
    for (i=0, n=0; i< TAG.num; i++, n+= TAG.num)
        HMM.A [i] = HMM.A_buffer + n;
    
    HMM.B = (double **) mem_alloc_vit (TAG.num * sizeof (double *));
    HMM.B_buffer =
        (double *) mem_alloc_vit (TAG.num * CLASS.num * sizeof (double));
    
    for (i=0, n=0; i< TAG.num; i++, n+= CLASS.num)
        HMM.B [i] = HMM.B_buffer + n;
}

void GlobalParseurHMM::free_hmm_basic ()
{
    free (HMM.Pi);
    free (HMM.A);
    free (HMM.A_buffer);
    free (HMM.B);
    free (HMM.B_buffer);
}

char* GlobalParseurHMM::get_tag_string (char *full_tag)
{
/* Selects a section from a complete tag according to DISAMB_FLAGS,
    e.g.: '+Verb' from '+SG+P1P2+SubjP+Verb' with flags 'bi1+' */
    
    char *pt;  int i;
    
    if (! DISAMB_FLAGS.user.tagcut_sym)
        return (full_tag);
    
    if (DISAMB_FLAGS.user.tagcut_backward)
    {
        for (i = DISAMB_FLAGS.user.tagcut_num, pt = full_tag + strlen (full_tag);
        (i) && (pt);  i--)
        {
            for (pt-- ; pt >= full_tag; pt--)
                if (*pt == DISAMB_FLAGS.user.tagcut_sym)
                    break;
                
                if (pt < full_tag)
                    return (NULL);
        }
        
        if (! DISAMB_FLAGS.user.tagcut_inclusive)
            pt++;
        
        if (! *pt)
            return (NULL);
        else
            return (pt);
    }
    else    /* DISAMB_FLAGS.user.tagcut_forward */
    {
        for (i = DISAMB_FLAGS.user.tagcut_num, pt = full_tag;
        (i) && (pt);
        i--, pt = strchr (pt, DISAMB_FLAGS.user.tagcut_sym))  ;
        
        if (! pt)
            return (NULL);
        
        if (DISAMB_FLAGS.user.tagcut_inclusive)
            pt++;
        
        *pt = '\0';
        
        if (! *full_tag)
            return (NULL);
        else
            return (full_tag);
    }
    
    return (NULL);
}


void delete_words (WORDBFptr wbf, long num)
{
    /* Deletes a number of words from buffer. */
    
    long i, offset;
    
    /* cases: delete all or no words */
    
    if (num < 1)   /* nothing to do */
        return;
    
    if (num >= wbf->num)
    {
        wbf->num = 0;
        wbf->buffer.alt.len = 0;
        wbf->buffer.line.len = 0;
        return;
    }
    
    /* case: delete some words */
    
    memmove (wbf->word, wbf->word + num, (wbf->num - num) * sizeof (INPWORDtype));
    wbf->num -= num;
    
    offset = wbf->word [0].alt - wbf->buffer.alt.bf;
    memmove (wbf->buffer.alt.bf, wbf->buffer.alt.bf + offset,
        (wbf->buffer.alt.len - offset) * sizeof (INPLINEtype));
    wbf->buffer.alt.len -= offset;
    for (i=0;  i< wbf->num;  i++)
        wbf->word [i].alt -= offset;
    
    offset = wbf->word [0].alt [0].line - wbf->buffer.line.bf;
    memmove (wbf->buffer.line.bf, wbf->buffer.line.bf + offset,
        (wbf->buffer.line.len - offset) * sizeof (char));
    wbf->buffer.line.len -= offset;
    for (i=0;  i< wbf->buffer.alt.len;  i++)
        wbf->buffer.alt.bf [i].line -= offset;
}


/*===========================================================
PUT WORDS FROM FILE TO BUFFER
===========================================================*/

static int is_word_buffer_to_adjust(WORDBFptr wbf, char *line, int new_word_p,
                                    int *max_words, int *max_altbf, int *max_linebf)
{
    int need, max, adjust_p;
    
    *max_words = *max_altbf = *max_linebf = 0;
    adjust_p = FALSE;
    
    if ((wbf->max <= wbf->num) && (new_word_p))
    {
        *max_words = wbf->max * REALLOC_FACTOR_HMM;
        adjust_p = TRUE;
    }
    
    if (wbf->buffer.alt.max <= wbf->buffer.alt.len)
    {
        *max_altbf = wbf->buffer.alt.max * REALLOC_FACTOR_HMM;
        adjust_p = TRUE;
    }
    
    need = wbf->buffer.line.len + 1 + strlen (line);
    max = wbf->buffer.line.max;
    
    while (max < need)
    {
        max *= REALLOC_FACTOR_HMM;
        adjust_p = TRUE;
    }
    
    *max_linebf = max;
    
    return (adjust_p);
}

void GlobalParseurHMM::store_to_word_buffer(WORDBFptr lwbf, char *line, int tag, int new_word_p)
{
    INPWORDptr word;  INPLINEptr alt;
    int max_words, max_altbf, max_linebf;
    
    if (is_word_buffer_to_adjust
        (lwbf, line, new_word_p, &max_words, &max_altbf, &max_linebf))
        adjust_word_buffer (lwbf, max_words, max_altbf, max_linebf);
    
    if (new_word_p)
    {
        lwbf->word [lwbf->num].num = 0;
        lwbf->word [lwbf->num].alt = lwbf->buffer.alt.bf + lwbf->buffer.alt.len;
        
        lwbf->num ++;
        lwbf->words_total ++;
    }
    
    word = lwbf->word + (lwbf->num - 1);
    alt = word->alt + (word->num);
    word->num ++;
#ifdef XIPLIBXML
	word->utilisateur=noeudxmltokencourrant;
#else
	word->utilisateur=NULL;
#endif
    lwbf->buffer.alt.len ++;
    
    alt->line = lwbf->buffer.line.bf + lwbf->buffer.line.len;
    strcpy (alt->line, line);
    lwbf->buffer.line.len += strlen (line) + 1;

    alt->tag = tag;
}

/*-----------------------------------------------------------*/


void GlobalParseurHMM::analyse_input_line (char *line, int *num_tokens, int *tag)
{
/* Anylyzes the number of tokens in the input line and the index
    of the tag. */
    
    char buffer [L_LINE_HMM+2], *tagstr, *token [MAX_TOKENS_HMM];
    
    strcpy (buffer, line);
    *num_tokens = HMM_tokenize_line (buffer, token, INPLINE_SEPARATORS);
    
    if (! *num_tokens)    /* empty line */
    {
        *tag = NO_INDEX;
        return;
    }
    
    if (*num_tokens <= TAG_POSITION)
        error_message ("illformed input line:\n'%s'\n", line);
    
    if (! (tagstr = get_tag_string (token [TAG_POSITION])))
    error_message ("tag '%s' in line '%s'\n\
    cannot be cut as defined by flags '%s'",
    token [TAG_POSITION], line, DISAMB_FLAGS.user.flag_string);
    
    if ((*tag = hash_find_tag (tagstr)) == NO_INDEX)
        error_message ("unknown tag <%s>\nin input line <%s>",
        tagstr, line);
}

int GlobalParseurHMM::get_next_word (FILE *file, WORDBFptr lwbf)
{
/* Reads all lines for one word from file and puts them to
the word buffer.
    Returns the number of non-empty line read. */
    
    char line [L_LINE_HMM+2];  int num_lines, num_tokens, tag;
    
    for (num_lines = 0;  ;  )
    {
        if (! HMM_read_line (line, file))  /* file end encountered */
            break;
        
        analyse_input_line (line, &num_tokens, &tag);
        
        if (! num_tokens)    /* empty line */
        {
            if (! num_lines)
                continue;    /* ignore preceeding empty lines */
            else
                break;       /* end of word information */
        }
        
        /* put data to buffers */
        
        store_to_word_buffer (lwbf, line, tag, (! num_lines));
        num_lines ++;
    }
    
    return (num_lines);
}

void GlobalParseurHMM::free_all_mem ()
{
    free_tags ();
    free_classes ();
    free_hmm_basic ();
}



int GlobalParseurHMM::InitDisambText (char *hmmdata) {
    
    char *flags,  *recode;  
    
    /* handle arguments */
    
    flags = NULL;
    recode=NULL;
    
    /* set flags */
    
    set_disamb_flags (flags);
    
    /* initializations */
    
    read_hmm_file (hmmdata);
    
    return 0;
}



void GlobalParseurHMM::CloseHMM() {
    if (hmmFileName!=NULL && hmmFileName[0]!=0)
        free_all_mem ();
    
}

char GlobalParseurHMM::ParseUneLigne(char* ligne,ListeVoc** voc,char supplementaire) {
    
    char test=NON;
    
    if (appliqueGrammaire==NON && xipresult==NULL) {
        if (OS!=NULL)
            *OS<<ligne;
        else
            cout<<ligne;
        return NON;
    }
    
    
    test=EndOfSentence(ligne,NON);
    char* copieChaine;
    
    LexiconEntryParsing(ligne,&copieChaine);
	if (supplementaire==OUI) {
		for (int i=0;i<listedehmmcat.dernier;i++)
			ListeTagsAjouter(listedehmmcat[i]);
        ListeTagsAjouter((char*)"hmmselection");
	}
    
    if (*voc==NULL) {
        *voc=ConstruitForme(copieChaine,num_mot);
        if (*voc!=NULL) {
            (*voc)->motGauche=num_mot;
            (*voc)->motDroit=num_mot;
            num_mot++;
            nombre_mots++;
        }
    }
    else
        FusionneForme(*voc,copieChaine);
    
    return test;    
}

void GlobalParseurHMM::ListeCatHMMAjouter(char* entree) {

    if (listedehmmcat.dernier >= listedehmmcat.taille)
        listedehmmcat.ajuste(listedehmmcat.taille+10);

	listedehmmcat.dernier++;
	char* v=listedehmmcat.fin();
	if (v==NULL) {
		v=new char[50];
		listedehmmcat.dernier--;
		listedehmmcat.ajoute(v);
	}
	strcpy(v,"hmm_");
    strcat(v,entree);   
	MetEnMinuscule((uchar*)(v+4));
}

void GlobalParseurHMM::print_one_word_one_tag(INPWORDptr word, 
                                              short tag) {
    
    /* Prints all stored input lines with the given idx and tag. */
    
    int i;
    ListeVoc* voc=NULL;
    char testFinDePhrase=NON;
    
    auto_vocabulaire* trad=pileDeMots.retireElement(0);
    char nettoieBalise=NON;
    
    //On active toutes les balises qui sont en attente...
    while (trad!=NULL) {
        AffecteBalise(trad);
        nettoieBalise=OUI;
        trad=pileDeMots.retireElement(0);
    }
    
    unsigned char test;
    listedehmmcat.dernier=0;
    if (pretraitement==OUI) {
        for (i=0;  i< word->num;  i++) {			
            if (word->alt [i].tag != tag) {				
				if (word->alt[i].tag!=-1)
					ListeCatHMMAjouter(TAG.tag[word->alt[i].tag]+1);                
                else
                    ListeCatHMMAjouter("?");				
            }						
        }            
    }
    
    for (i=0;  i< word->num;  i++) {
        if (word->alt [i].tag == tag)
			test=ParseUneLigne(word->alt[i].line,&voc,OUI);
        else
            test=ParseUneLigne(word->alt[i].line,&voc,NON);
        if (test==OUI)
            testFinDePhrase=OUI;            
    }

#ifdef XIPLIBXML
		if (voc!=NULL)
			voc->xmlnoeud=(xmlNodePtr)word->utilisateur;
#endif
    
    if (nettoieBalise==OUI)
        gBaliseDebut->raz();
    
    if (testFinDePhrase==OUI || EndOfSentence(NULL,UTILISE)) {
		DeclencheAjusteoffset(true);
        Parse(OUI);
        Detruit();
        num_phrase++;
		if (renumerotation==OUI) {
            num_mot=0;
			num_token=0;
		}
    }
}



void GlobalParseurHMM::print_one_word (long idx, 
                                       VCELLptr vcell)
{
    /* Prints all stored input lines with the given idx and tag. */
    
    INPWORDptr word;
    
    if ((idx >= wbf->num) || (idx < 0))
        error_message ((char*)"word index %li out of range 0..%li", idx, wbf->num - 1);
    
    word = wbf->word + idx;
    
    print_one_word_one_tag(word, vcell->tag);
}

void GlobalParseurHMM::output_all_prev_words (long idx, VCELLptr vcell)
{
    /* Writes all previous words and tags recursively to the output. */
    
    if (! vcell)   /* no previous tags stored */
    {
        if (idx == -1)
            return;      /* correct value */
        else
            error_message ((char*)"there are %li more words than tags stored", idx+1);
    }
    
    output_all_prev_words (idx-1, vcell->prev);  
    print_one_word (idx, vcell);
}


void GlobalParseurHMM::viterbi_next_word(int next_p)
{
/* Includes the last word to the Viterbi tree if next_p==TRUE.
    Writes as much words as possible to the file. */
    
    VITVECTORptr vit_root;  long words_out;
    
    if (next_p)
    {
        if (vtree->cur->flags.sent_begin)
            viterbi_start (vtree);
        else
            viterbi_step (vtree);
    }
    
    if (vtree->cur->flags.sent_end)      /* keep only best of all pathes */
        viterbi_end (vtree);
    
    if (vtree->cur->len == 1)    /* output possible */
    {
        vit_root = vtree->cur->alt;
        words_out = (vtree->cur->flags.sent_bound) ? wbf->num : wbf->num - 1;
        
        if (words_out > 0)
        {
            output_all_prev_words (words_out - 1, vit_root->prev);
			if (wbf!=NULL) {
				delete_words(wbf, words_out);            
				delete_vcell_list (vit_root->prev, vtree);
			}
            vit_root->prev = NULL;
            vit_root->score = 0.0;  /* score can be reset */
        }
    }
}


void trimall(unsigned char* buff) {
	int sz=strlen((char*)buff)-1;
	while (sz>=0 && buff[sz]<=32) sz--;
	buff[sz+1]=0;
	int i=0;
	while(i<=sz && buff[i]<=32) i++;
	if (i)
		strcpy((char*)buff,(char*)buff+i);
}


int GlobalParseurHMM::mot_suivant() {
    
    if (fichierDonnees!=NULL)
        return get_next_word (fichierDonnees, wbf);
    
    if (finLecture())
        return 0;  
    
    char ligne[1000];
    ligne[0]=0;
    LitDoubleTab(ligne,ligne,999,OUI);
	trimall((uchar*)ligne);
    int tag,num_tokens;
    char premiereLigne=OUI;
    char res=NON;

    while (!finLecture() && ligne[0]!=0) {
		res=OUI;
        analyse_input_line (ligne, &num_tokens, &tag);
        store_to_word_buffer (wbf, ligne, tag, premiereLigne);
        ligne[0]=0;
        LitDoubleTab(ligne,ligne,999,OUI);
		trimall((uchar*)ligne);
        premiereLigne=NON;
    }
    
    return res;
}

void GlobalParseurHMM::SentenceAnalysis(char renumerote,char arbre) {

    wbf=NULL; 
    baseOffset=0;
	baseOffsetCar=0;
    num_phrase=0;
    
    renumerotation=renumerote;
    
    DISAMB_FLAGS.system.cur_line_num = 0;
    DISAMB_FLAGS.system.reading_hmm = FALSE;
    DISAMB_FLAGS.system.reading_input = TRUE;
    
    wbf = alloc_word_buffer (MAX_WORDS, MAX_ALTBF, MAX_LINEBF);
    vtree = alloc_viterbi (TAG.num);
    
    while (mot_suivant()) 
    {
        exchange_vit_vectors (vtree);   /* prev <--> cur */
        clear_vit_vector_and_lists (vtree->cur, vtree);
        
        set_current_class_data (wbf, vtree);
        viterbi_next_word (TRUE);
    }
    
    
    if (wbf->num)   /* still words in buffer */
    {
        vtree->cur->flags.sent_end = TRUE;
        viterbi_next_word (FALSE);
    }
    
    FinAnalyse=OUI;
	DeclencheAjusteoffset(true);
    while (phrase.dernier!=0) {
        Parse(OUI);
        Detruit();
		DeclencheAjusteoffset(false);
    }

    FinAnalyse=NON;
    //HMM lu
    words_total = wbf->words_total;
    
    free_word_buffer (wbf);
    free_viterbi (vtree);
    
    DISAMB_FLAGS.system.cur_line_num = 0;
    DISAMB_FLAGS.system.reading_hmm = FALSE;
    DISAMB_FLAGS.system.reading_input = FALSE;
    
    //  return (words_total);
}


GlobalParseurHMM::GlobalParseurHMM(char* hmm,char* filename,char* path) : GlobalParseur(filename,path) {
    hmmFileName=NULL;
	if (hmm!=NULL && hmm[0]!=0) {
        hmmFileName=strdup_vit(hmm);
		Nom_divers.ajoute(DupliqueChaine(hmm));
	}
    pretraitement=NON;
    renumerotation=NON;
}

GlobalParseurHMM::~GlobalParseurHMM() {
    CloseHMM();
	listedehmmcat.nettoietable();   
    if (hmmFileName!=NULL)
        free(hmmFileName);
}




void GlobalParseurHMM::libereWord(word_offset* w) {
	if (w->pos+1==lectures.dernier)
		lectures.dernier--;
	w->raz();
}

void GlobalParseurHMM::ParseFinal() {
    if (wbf!=NULL && wbf->num)   /* still words in buffer */
    {
        vtree->cur->flags.sent_end = TRUE;
        viterbi_next_word (FALSE);
    }

    FinAnalyse=OUI;
	DeclencheAjusteoffset(true);
    while (phrase.dernier!=0) {       
        Parse(OUI);
        Detruit();
        num_phrase++;
		DeclencheAjusteoffset(false);
    }
    FinAnalyse=NON;
}
