/***************************************************************************
 *            ut_load.c
 *
 *  Thu Dec 23 15:53:42 2004
 *  Copyright  2004  Alliance MCA
 *  Written by : Antoine Calando (antoine@alliancemca.net)
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 

/*!
 * \file ut_load.c 
 * \brief Loading functions
 * \author Antoine Calando (antoine@alliancemca.net)
 */

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "utrac.h"
#include "ut_text.h"

//#undef UT_DEBUG
//#define UT_DEBUG 2
#include "debug.h"


/***************************************************************************/
/*!
 * \brief Load a text in a buffer
 * 	 
 * \param text	UtText structure containing eventually a pointer to a user function
 *				which update a progress bar. File contents will be accessible via 
 *				UtText::data, and size in UtText::size.
 * \param filename	Filename (with path) of the file to load.
 *
 * \return UT_OK on succes, error code otherwise.
 */
UtCode ut_load_file_pass (UtText *text, const char * filename) {
	DBG3 ("Loading file %s...", filename)
	
	int fd = open (filename, O_RDONLY);
	if (fd==-1) return UT_OPEN_FILE_ERROR;

	struct stat f_stat;
	if (fstat (fd, &f_stat)) return UT_FSTAT_FILE_ERROR;
	text->size = f_stat.st_size;
	
	if (!text->size) return UT_EMPTY_DATA_ERROR;

	free (text->data);
	//some space is needed to add an EOL (1 or 2 bytes) if it is missing and an EOF
	text->data = (char*) malloc (text->size + 3);
	if (!text->data) return UT_MALLOC_ERROR;
	
	ulong cumul=0;
	bool cont = true;

	//if progress feature is used, read() is called with UT_LOAD_STEP several times and
	//can overflow the buffer allocated if read() fills it more than the size of the file
	while (cumul<text->size && cont) {
		long code=read (fd, text->data+cumul, (ut_session->progress_function)?UT_LOAD_STEP:(text->size-cumul));
		if (code<=0) return UT_READ_FILE_ERROR;
		cumul += code;
		if (ut_session->progress_function) cont = ut_update_progress (text, cumul, false);
	}

	
	if (!cont) {
		DBG2 ("File loading canceled by user!")
		free (text->data);
		text->data = NULL;
		return UT_INTERRUPTED_BY_USER;
	}

	ASSERT (cumul==text->size)
	DBG2 ("File %s (%lu b / %lu b) loaded!", filename, cumul, text->size)
	
	*(text->data+text->size) = UT_EOF_CHAR;
	*(text->data+text->size+1) = UT_EOF_CHAR;
	*(text->data+text->size+2) = UT_EOF_CHAR;

	if (close(fd)) return UT_CLOSE_FILE_ERROR;
		
	return UT_OK;
}

/***************************************************************************/
/*!
 * \brief Load a text from stdin
 *	 
 * \param text UtText structure used to return the loaded text, which will
 *        be accessible via UtText::data, and size in UtText::size. Progress function not called since
 *        we can't know the size of the input text before it is completely loaded.
 *
 * \return UT_OK on succes, error code otherwise.
 *
 * \note EC le buffer utilisé ici est dynamique, pourquoi ajouter 3 octets à UT_STDIN_BUFFER_SIZE
 *       qui doit être dimensionné à un multiple d'un bloc mémoire du système.
 *       AC Effectivement...
 * \bug  EC read() peut renvoyer 0 sans indiquer une fin de fichier (errno==EAGAIN) et le test
 *       while (code && cont); est donc faux !
 *       AC read() ne peut pas renvoyer EAGAIN si le fichier n'a pas été open() en mode EAGAIN
 *       (norme POSIX)
 */
UtCode ut_load_stdin_pass (UtText *text) {

	//some space is needed to add an EOL (1 or 2 bytes) if it is missing and an EOF
	free(text->data);
	text->data = (char*) malloc (UT_STDIN_BUFFER_SIZE);
	if (!text->data) return UT_MALLOC_ERROR;
		
	struct stat f_stat;
	if (fstat (fileno(stdin), &f_stat)) return UT_FSTAT_FILE_ERROR;
	text->size = f_stat.st_size;
	
	ulong bytes_read = 0, bytes_to_read = 0;
	ulong buffer_size = UT_STDIN_BUFFER_SIZE;
	bool cont = true;
	
	//if progress feature is used, read() is called with UT_LOAD_STEP several times and
	//can overflow the buffer allocated if read() fills it more than the size of the file
	long code;
	do {
		bytes_to_read = buffer_size-bytes_read;
		if (ut_session->progress_function && UT_LOAD_STEP < bytes_to_read ) bytes_to_read = UT_LOAD_STEP;
		code = read (STDIN_FILENO, text->data + bytes_read, bytes_to_read);
		if (code<0) {
			free (text->data);
			text->data = NULL;
			return UT_READ_FILE_ERROR;
		}
		bytes_read += code;

		if (fstat (fileno(stdin), &f_stat)) return UT_FSTAT_FILE_ERROR;
		text->size = f_stat.st_size;
		if (ut_session->progress_function) cont = ut_update_progress (text, bytes_read, false);
	
		//if buffer full, allocate new bigger buffer
		if (bytes_read == buffer_size) {
			buffer_size *= 2;
			text->data =  realloc (text->data, buffer_size + 3);
			if (!text->data) return UT_MALLOC_ERROR;
		}
		
		//if (ut_session->progress_function) cont = ut_update_progress (text, bytes_read, false);
	} while (code && cont);

	if (!cont) {
		DBG2 ("** stdin loading canceled by user! **")
		free (text->data);
		text->data = NULL;
		return UT_INTERRUPTED_BY_USER;
	}

	text->size = bytes_read;
	
	DBG2 ("stdin stream (%lu b) loaded!", text->size)
	
	*(text->data+text->size) = UT_EOF_CHAR;
	*(text->data+text->size+1) = UT_EOF_CHAR;
	*(text->data+text->size+2) = UT_EOF_CHAR;

	return UT_OK;
}
