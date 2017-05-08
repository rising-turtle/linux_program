/* utmplib.c  - functions to buffer reads from utmp file 
 *
 *      functions are
 *              utmp_open( filename )   - open file
 *                      returns -1 on error
 *              utmp_next( )            - return pointer to next struct
 *                      returns NULL on eof
 *              utmp_close()            - close file
 *
 *      reads NRECS per read and then doles them out from the buffer
 */
#include        <stdio.h>
#include        <fcntl.h>
#include        <sys/types.h>
#include        <utmp.h>
#include	<unistd.h>

#define NRECS   16
#define NULLUT  ((struct utmp *)NULL)
#define UTSIZE  (sizeof(struct utmp))

static  char    utmpbuf[NRECS * UTSIZE];                /* storage      */
static  int     num_recs;                               /* num stored   */
static  int     cur_rec;                                /* next to go   */
static  int     fd_utmp = -1;                           /* read from    */

static  int  utmp_reload();

/*
 * if filename is NULL use UTMP_FILE from utmp.h
 */

int utmp_open( char *filename )
{
	/* opent the file */
        fd_utmp = open( (filename?filename:UTMPFILE), O_RDONLY );
        cur_rec = num_recs = 0;                         /* no recs yet  */
        return fd_utmp;                                 /* report       */
}

struct utmp *utmp_next()
{
        struct utmp *recp;

        if ( fd_utmp == -1 )                            /* error ?      */
                return NULLUT;
        if ( cur_rec==num_recs && utmp_reload()==0 )    /* any more ?   */
                return NULLUT;
                                        /* get address of next record    */
        recp = (struct utmp *) &utmpbuf[cur_rec * UTSIZE];
        cur_rec++;
        return recp;
}

static int utmp_reload()
/*
 *      read next bunch of records into buffer
 */
{
        int     amt_read;
                                                
        amt_read = read(fd_utmp, utmpbuf, NRECS*UTSIZE);   /* read data	*/
	if ( amt_read < 0 )			/* mark errors as EOF   */
		amt_read = -1;
                                                
        num_recs = amt_read/UTSIZE;		/* how many did we get?	*/
        cur_rec  = 0;				/* reset pointer	*/
        return num_recs;			/* report results	*/
}

int utmp_close()
{
	int rv = 0;
        if ( fd_utmp != -1 )                    /* don't close if not   */
                rv = close( fd_utmp );          /* open                 */
	return rv;
}
