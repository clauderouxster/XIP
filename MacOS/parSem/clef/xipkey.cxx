/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003, Xerox Corporation, XRCE-Grenoble.
 * This file can only be used with the XIP library,
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 3.16
   filename   : clef.cxx
   Date       : 10/01/1999
   Purpose    : Compute the value of the encryption key
   Programmer : Claude ROUX
   Reviewer   :
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vecta.h"
#include <iostream>
#include <fstream>

using namespace std;
#define strcpy_s(a,c,b) strncpy(a,b,c)

int LINES;
int ERROR_LINE;
#define YES 1
#define NO 0

void display_error(string message) {
    cerr << message << endl;
    exit(-1);
}
/*-------------------------------------------------------------------------*/
void Trim(char* str) {
    unsigned char* buf = (unsigned char*)str;
    size_t l, i;
    l = strlen(str);
    i = l - 1;
    while (i >= 0 && buf[i] <= 32)
        buf[i--] = 0;
    i = 0;
    while (buf[i] <= 32 && buf[i] != 0)
        i++;
    if (i)
        strcpy(str, str + i);
}

/*-------------------------------------------------------------------------*/
char* DuplicateString(char* m) {
    char* c;
    size_t l = strlen(m);
    c = new char[l + 1];
    memcpy(c, m, l);
    c[l] = 0;
    return c;
}

/*--------------------------------------------------------------------------*/
void freeMemory(char* c) {
    delete[] c;
}

/*-------------------------------------------------------------------------*/
void VerifyAndCompare(char* buffer) {
    struct tm *newtime;
    time_t long_time;

    time(&long_time);                /* Get time as long integer. */
    newtime = localtime(&long_time); /* Convert to local time. */

    int year;
    int month;
    int day;
    // Find the first digit
    int i = 0;
    while (buffer[i] < '0' || buffer[i] > '9') i++;
    char* pt = strchr(buffer + i, '/');
    if (pt == NULL)
        display_error("Bad date format: error");
    year = atoi(buffer + i) - 1900; // the year value is 100 for 2000...
    pt++;
    char* suite = strchr(pt, '/');
    *suite = 0;
    month = atoi(pt) - 1;
    day = atoi(suite + 1);

    if (year > newtime->tm_year)
        return;
    if (year == newtime->tm_year) {
        if (month > newtime->tm_mon)
            return;
        if (month == newtime->tm_mon) {
            cerr << "License will expire in: " << day - newtime->tm_mday << ". Please contact Xerox to renew..." << endl;
            if (day >= newtime->tm_mday)
                return;
        }
    }
    cerr << "License has Expired..." << endl;
}

/****************************************************************************************************/
#define ReadChar(x) x = grmfile.get(); if (x == '\n') LINES++;
/****************************************************************************************************************/
char isSpace(unsigned char c) {
    if (c == ' ' || c == '\t' || c == '\n')
        return YES;
    return NO;
}

/*****************************************************************************************************/
char isLetterBis(unsigned char c) {
    if (c <= 32 || strchr("<>{}[](),;:.&|!/\\=~#@^?+-*$%'", c))
        return NO;

    return c;
}

/*****************************************************************************************************/
char isLetter(unsigned char c) {
    if (c <= 32 || strchr("<>{}[](),;:.&|!/\\=~#@^", c))
        return NO;

    return tolower(c);
}
/****************************************************************************************/

/* This function reads comments...*/
char ReadComments(ifstream& grmfile, unsigned char cc, unsigned char* r) {
    char c = cc;

    if (c == '/') {
        ReadChar(c);

        // Read a line
        if (c == '/') {
            while (!grmfile.eof() && c != '\n') { ReadChar(c); }
        }
        else
            while (!grmfile.eof() && c != '\\') { ReadChar(c); }

        ReadChar(c);
        while (!grmfile.eof() && isSpace(c)) { ReadChar(c); }
        *r = c;
        return YES;
    }

    *r = c;
    return NO;
}

/****************************************************************************************************/
/* Read certain character strings that will be used to
construct
attributes, values, etc...*/
char read(ifstream& grmfile, char *buffer) {
    int i = 0;
    unsigned char c, k;

    ReadChar(c);

    while (!grmfile.eof() && isSpace(c)) { ReadChar(c); }

    unsigned char cc = c;
    while (ReadComments(grmfile, c, &cc)) {
        c = cc;
    }

    ERROR_LINE = LINES + 1;

    // Escape sequence
    if (cc == '\\') {
        ReadChar(k);
        buffer[i++] = k;
        ReadChar(cc);
    }
    k = isLetter(cc);

    while (!grmfile.eof() && k) {
        buffer[i] = k;
        ReadChar(c);
        // Escape sequence
        if (c == '\\') {
            ReadChar(k);
            buffer[++i] = k;
            ReadChar(c);
        }

        k = isLetter(c);
        i++;
    }
    buffer[i] = '\0';
    if (c == 32 || c == '\n')
        while (!grmfile.eof() && isSpace(c)) {
            ReadChar(c);
        }

    if (grmfile.eof()) return ':';
    return c;
}
/****************************************************************************************************/
char isLetterA(unsigned char c) {
    if (c <= 32 || strchr("<>{}[](),;:.&|!/\\=~#@^$%", c))
        return NO;

    return c;
}

/****************************************************************************************************/
char readWithout(ifstream& grmfile, char *buffer) {
    int i = 0;
    unsigned char c, k;

    ReadChar(c);

    while (!grmfile.eof() && isSpace(c)) { ReadChar(c); }

    unsigned char cc = c;
    while (ReadComments(grmfile, c, &cc)) {
        c = cc;
    }

    ERROR_LINE = LINES + 1;

    // Escape sequence
    if (cc == '\\') {
        ReadChar(k);
        buffer[i++] = k;
        ReadChar(cc);
    }

    k = isLetterA(cc);

    while (!grmfile.eof() && k) {
        buffer[i] = k;
        ReadChar(c);
        // Escape sequence

        if (c == '\\') {
            ReadChar(k);
            buffer[++i] = k;
            ReadChar(c);
        }

        k = isLetterA(c);
        i++;
    }

    buffer[i] = '\0';
    if (c == 32 || c == '\n')
        while (!grmfile.eof() && isSpace(c)) { ReadChar(c); }
    if (grmfile.eof()) return ':';
    return c;
}
/****************************************************************************************************/
char Lread(ifstream& grmfile, char *buffer) {
    int i = 0;
    unsigned char c, k;

    ReadChar(c);

    while (!grmfile.eof() && isSpace(c)) { ReadChar(c); }

    unsigned char cc = c;
    while (ReadComments(grmfile, c, &cc)) {
        c = cc;
    }

    ERROR_LINE = LINES + 1;

    // Escape sequence
    if (cc == '\\') {
        ReadChar(k);
        buffer[i++] = k;
        ReadChar(cc);
    }

    k = isLetter(cc);

    while (!grmfile.eof() && k) {
        buffer[i] = k;
        ReadChar(c);
        // Escape sequence
        if (c == '\\') {
            ReadChar(k);
            buffer[++i] = k;
            ReadChar(c);
        }

        k = isLetter(c);
        i++;
    }

    buffer[i] = '\0';
    if (grmfile.eof())
        return ':';
    return c;
}

/****************************************************************************************************/

char Letter(unsigned char c) {
    if (c <= 32 || strchr("<>{}[](),;:.&|!/\\=~#@^?+*%", c))
        return NO;

    return tolower(c);
}

/****************************************************************************************************/
/* Read the next word in a string */
char read(char *buffer, char* phrase, int* i) {
    int j = 0;
    char k;

    while (phrase[*i] != '\x0' && (!Letter(phrase[*i]) || isSpace(phrase[*i]))) ++*i;
    k = Letter(phrase[*i]);

    while (phrase[*i] != '\x0' && phrase[*i] != '.' && k) {
        buffer[j] = k;
        j++;
        ++*i;
        k = Letter(phrase[*i]);
    }

    buffer[j] = '\0';

    if (phrase[*i] == '\0')
        return YES;

    return NO;
}

/****************************************************************************************************/
/****************************************************************************************************/
void ReadLine(ifstream& grmfile, char *buffer) {
    int i = 0;
    unsigned char c, k;

    ReadChar(c);

    while (!grmfile.eof() && isSpace(c)) { ReadChar(c); }

    unsigned char cc = c;
    while (ReadComments(grmfile, c, &cc)) {
        c = cc;
    }

    ERROR_LINE = LINES + 1;

    k = cc;
    while (!grmfile.eof() && k != '\n') {
        buffer[i] = k;
        ReadChar(k);
        i++;
    }

    buffer[i] = '\0';
}
/****************************************************************************************************/
inline void CheckCRC(VECTA<char*>& contents) {
    const size_t size = 16;

    const size_t contents_length = contents.dernier;
    const size_t number_of_row = (contents_length % size);

    unsigned char colBuffer[size] = "INTERNAL ERROR ";

    unsigned long rowBuffer = 0;
    // the columns
    size_t num_row = 0;
    size_t i;

    for (i = 0; i < contents_length; i++) {
        for (size_t letter = 0; letter < strlen(contents[i]); letter++) {
            size_t i_col = (i + letter) % size;

            if (i_col == 0 && i > 0) {
                unsigned char sum_line = (rowBuffer << 4) + (rowBuffer >> 4);
                // start of a new line
                for (size_t j = 0; j < size; j++)
                    colBuffer[j] += sum_line;
                rowBuffer = 0;
            }

            colBuffer[i_col] += contents[i][letter];
            rowBuffer += contents[i][letter];
        }
    }

    char res[1000];
    res[0] = 0;
    for (i = 0; i < 8; i++) {
        const size_t base = 41;
        unsigned short s = (((unsigned short)colBuffer[2 * i]) << 8) | colBuffer[(2 * i) + 1];
        char conv[4] = "000";
        size_t j = 2;
        while (s != 0) {
            char c = s % base;
            if (c < 26)
                c += 'A';
            else
                c += 'a' - 26;
            conv[j] = c;
            j--;
            s /= base;
        }
        conv[3] = '\0';
        strcat(res, conv);
    }

    cout << "Code= " << res << endl;
}

void ChainMultiple(ifstream& grmfile,
    const char* ChainTest,
    char* buffer,
    char& k,
    char& addFile,
    VECTA<char*>& names) {

    char fileName[255];

    if (buffer[0] == 0)
        k = Lread(grmfile, buffer);

    if (buffer[0] == 0 && k == ':')
        return;

    if (strcmp(buffer, ChainTest) || k != ':')
        display_error(buffer);

    // A list ends with a ";"

    while (k != ';') {
        k = read(grmfile, buffer);

        strcpy_s(fileName, 255, buffer);
        if (!memcmp(fileName, (char*)"limited", 7) || !strcmp(fileName, "noparameter"))
            addFile = YES;

        Trim(fileName);
        names.ajoute(DuplicateString(fileName));
        unsigned char cc = k;
        while (ReadComments(grmfile, k, &cc)) {
            k = cc;
        }

        if (k == ':') {
            buffer[0] = 0;
            return;
        }

        if (k != ',') {
            buffer[0] = tolower(k);
            k = Lread(grmfile, buffer + 1);
            return;
        }
    }
    buffer[0] = 0;
}

int main(int argc, char *argv[]) {

/*-------------------------------------------------------------------------*/
    char k, buffer[500];

    if (argc < 2) {
        cerr << "Error no filename on the command line..." << endl;
        exit(-1);
    }
    ifstream grmfile(argv[1], ios::in | ios::binary);

    LINES = 0;
    ERROR_LINE = 0;

    if (grmfile.fail()) {
        strcpy_s(buffer, 500, argv[1]);
        display_error(buffer);
    }
    else {
        VECTA<char*> CalculateKey;

        char UseLimit[300];
        int rec = strlen(argv[1]) - 1;
        while (rec >= 0 && argv[1][rec] != '/' && argv[1][rec] != '\\') rec--;
        strcpy_s(UseLimit, 300, argv[1] + rec + 1);

        k = Lread(grmfile, buffer);
        if (strcmp(buffer, "license"))
            display_error("No license found... Error..");
        // Read the rest of the line
        ReadLine(grmfile, buffer);
        Trim(buffer);
        CalculateKey.ajoute(DuplicateString(buffer));

        buffer[0] = 0;
        k = 0;
        char add = NO;
        ChainMultiple(grmfile, "modules", buffer, k, add, CalculateKey);
        if (add == YES) {
            cerr << "Adding:" << UseLimit << endl;
            Trim(UseLimit);
            CalculateKey.ajoute(DuplicateString(UseLimit));
        }

        int imo;

        // Now read the expiration date in American format: Year/month/day
        if (strcmp(buffer, "expiration"))
            display_error("No expiration date found... Error..");
        ReadLine(grmfile, buffer);
        Trim(buffer);
        CalculateKey.ajoute(DuplicateString(buffer));
        VerifyAndCompare(buffer);
        CheckCRC(CalculateKey);

        // Clean up this structure...
        for (imo = 0; imo < CalculateKey.dernier; imo++)
            freeMemory(CalculateKey[imo]);
        CalculateKey.raz();
    }
    return 0;
}

